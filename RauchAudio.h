#pragma once
#include "soundio/soundio.h"
#include "AudioFile.h"
#include <vector>
#include <string>
#include <iostream>
#define MINIMP3_FLOAT_OUTPUT
#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"
#include "minimp3_ex.h"
#include "sstream"
#define DR_FLAC_IMPLEMENTATION
#include "dr_flac.h"

using namespace std;

#define UPDATESOUND soundio_flush_events(soundio)



std::string GetFileExtension(const std::string& FileName)
{
    if (FileName.find_last_of(".") != std::string::npos)
        return FileName.substr(FileName.find_last_of(".") + 1);
    return "";
}

struct SoundIoOutStream* outstream;
struct Audio
{
	unsigned int SamplesPerChannel = 0;
	unsigned int SampleRate = 44100;
    unsigned int ChannelCount = 0;
    vector<vector<float>> Samples;
    unsigned int currentSample = 0;
    bool Paused = false;
    bool Looping = false;
    bool Completed = false;

    unsigned int GetSampleRate()
    {
        return SampleRate;
    }

    int Load(const char* path)
    {
        const std::string pathstring(path);
        std::string pathextension = GetFileExtension(pathstring);

        if (pathextension == "wav")
        {
            AudioFile<float> file;
            file.load(path);
            Samples = file.samples;
            SampleRate = file.getSampleRate();
            SamplesPerChannel = file.getNumSamplesPerChannel();
            outstream->layout.channel_count = file.getNumChannels();
            ChannelCount = file.getNumChannels();
            if (!SampleRate)
            {
                cout << "failed to load wav file" << endl;
                return -1;
            }
        }
        else if (pathextension == "mp3")
        {
            mp3dec_t mp3d;
            mp3dec_file_info_t info;
            
            std::ifstream file(pathstring, std::ios::binary);

            file.unsetf(std::ios::skipws);
            std::istream_iterator<uint8_t> begin(file), end;
            std::vector<uint8_t> fileData(begin, end);
            cout << "decoding file..." << endl;
            int decodeerror = mp3dec_load_buf(&mp3d, (const uint8_t*)fileData.data(), fileData.size(), &info, 0, 0);
            if (decodeerror != 0)
            {
                cout << "failed to decode file" << endl;
                return -1;
            }
            cout << "done decoding file" << endl;
            file.close();
           // cout << info.hz;
            
            SampleRate = info.hz;
            outstream->layout.channel_count = info.channels;
            cout << "Number of Samples: " <<info.samples << endl
            << "Sample Rate: " << info.hz << "hz" << endl
            << "Number of Channels: " << info.channels << endl
            << "loading samples..." << endl;
            for (int c = 0; c < info.channels;c++)
            {
                vector<float> empty;
                Samples.push_back(empty);
                
                for (int s = c; s < info.samples;s = s + info.channels)
                {
                    Samples[c].push_back(info.buffer[s]);
                }
            }
            cout << "done loading samples" << endl;
            SamplesPerChannel = Samples[0].size();
            ChannelCount = Samples.size();
            free(info.buffer);
        }
        else if (pathextension == "flac")
        {
            unsigned int channels;
            unsigned int sampleRate;
            drflac_uint64 totalPCMFrameCount;
            float* pSampleData = drflac_open_file_and_read_pcm_frames_f32(pathstring.c_str(), &channels, &sampleRate, &totalPCMFrameCount, NULL);
            if (pSampleData == NULL) {
                // Failed to open and decode FLAC file.
                return 0;
            }
            cout << "total flac frames: "<< totalPCMFrameCount  << ". Number of Channels: "<< channels << endl;
            for (int c = 0; c < channels;c++)
            {
                vector<float> empty;
                Samples.push_back(empty);

                for (int s = c; s < totalPCMFrameCount * channels;s = s + channels)
                {
                    
                    //cout << "loading frame" << s << endl;
                    Samples[c].push_back(pSampleData[s]);
                }  
            }
            SampleRate = sampleRate;
            outstream->layout.channel_count = channels;
            SamplesPerChannel = totalPCMFrameCount;
            ChannelCount = Samples.size();
            free(pSampleData);
        }
        else
        {
            cout << "File extension " << pathextension << " not supported" << endl;
            return -1;
        }
        return 0;
    }
    Audio(const char* path)
    {
        Load(path);
    }
    Audio() = default;
};

Audio ActiveAudio;

static void write_callback(struct SoundIoOutStream* outstream,
    int frame_count_min, int frame_count_max)
{
    const struct SoundIoChannelLayout* layout = &outstream->layout;
    struct SoundIoChannelArea* areas;
    int frames_left = frame_count_max;
    int err;
    while (frames_left > 0) {
        int frame_count = frames_left;

        if ((err = soundio_outstream_begin_write(outstream, &areas, &frame_count))) {
            fprintf(stderr, "%s\n", soundio_strerror(err));
            exit(1);
        }
        int framestofill = frame_count;
        if (ActiveAudio.Samples.size() <= 0)
        {
            framestofill = 0;
        }

        if (!frame_count)
            break;
        for (int frame = 0; frame < framestofill; frame += 1) {
            for (int channel = 0; channel < layout->channel_count; channel += 1) {
                float sample = ActiveAudio.Samples[channel][ActiveAudio.currentSample];
                float* ptr = (float*)(areas[channel].ptr + areas[channel].step * frame);
                *ptr = sample;
                
            }
            if (ActiveAudio.currentSample + 1 < ActiveAudio.SamplesPerChannel && !ActiveAudio.Paused)
            {
                ActiveAudio.currentSample++;
            }
            else if (ActiveAudio.Looping && !ActiveAudio.Paused)
            {
                ActiveAudio.currentSample = 0;
                
            }
            else if(!ActiveAudio.Looping && !ActiveAudio.Paused)
            {
                ActiveAudio.Completed = true;
            }
        }

        if ((err = soundio_outstream_end_write(outstream))) {
            fprintf(stderr, "%s\n", soundio_strerror(err));
            exit(1);
        }

        frames_left -= frame_count;
    }
}


struct SoundIoDevice* device;
SoundIo* soundio;
void InitializeAudio()
{
    soundio = soundio_create();

    soundio_connect(soundio);

    soundio_flush_events(soundio);

    int default_out_device_index = soundio_default_output_device_index(soundio);
    device = soundio_get_output_device(soundio, default_out_device_index);

    outstream = soundio_outstream_create(device);

    outstream->format = SoundIoFormatFloat32NE;
    outstream->write_callback = write_callback;
    outstream->sample_rate = ActiveAudio.GetSampleRate();

    soundio_outstream_open(outstream);

    outstream->layout_error;

    soundio_outstream_start(outstream);
}

void StartAudio(Audio audio)
{
    ActiveAudio = audio;
    outstream->sample_rate = ActiveAudio.GetSampleRate();
}

void DeinitializeAudio()
{
    soundio_outstream_destroy(outstream);
    soundio_device_unref(device);
    soundio_destroy(soundio);
}