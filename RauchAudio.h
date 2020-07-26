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

#define UPDATESOUND soundio_flush_events(soundio)

using namespace std;

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
    unsigned int BufferSize = 0;
    unsigned int currentSample = 0;
    bool Paused = false;
    bool Looping = true;

    unsigned int GetSampleRate()
    {
        return SampleRate;
    }

    void Load(const char* path)
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
        else
        {
            cout << "File extension " << pathextension << " not supported";
        }
    }
    Audio(const char* path)
    {
        Load(path);
        BufferSize = (SampleRate / ChannelCount) * (outstream->software_latency);
    }
    Audio() = default;
};

Audio ActiveAudio;

static float seconds_offset = 0.0f;
static void write_callback(struct SoundIoOutStream* outstream,
    int frame_count_min, int frame_count_max)
{
    
    const struct SoundIoChannelLayout* layout = &outstream->layout;
    float float_sample_rate = outstream->sample_rate;
    float seconds_per_frame = 1.0f / float_sample_rate;
    struct SoundIoChannelArea* areas;
    int frames_left = frame_count_max;
    int err;
    while (frames_left > 0) {
        int frame_count = frames_left;

        if ((err = soundio_outstream_begin_write(outstream, &areas, &frame_count))) {
            fprintf(stderr, "%s\n", soundio_strerror(err));
            exit(1);
        }

        if (!frame_count)
            break;
        int fileframes = ActiveAudio.BufferSize;
        //cout << "current sample: " << ActiveAudio.currentSample << endl << "fileframes: " << fileframes << endl;
        for (int frame = 0; frame < fileframes; frame += 1) {
           // cout << "frame: " << frame << endl;
            for (int channel = 0; channel < layout->channel_count; channel += 1) {
                float sample = ActiveAudio.Samples[channel][ActiveAudio.currentSample];
                float* ptr = (float*)(areas[channel].ptr + areas[channel].step * frame);
                *ptr = sample;
                
            }
            if (ActiveAudio.currentSample + 1 < ActiveAudio.SamplesPerChannel && !ActiveAudio.Paused)
            {
                ActiveAudio.currentSample++;
            }
            else if (ActiveAudio.Looping)
            {
                ActiveAudio.currentSample = 0;
            }
        }
        //ActiveAudio.currentSample += ActiveAudio.BufferSize;
        seconds_offset = fmod(seconds_offset + seconds_per_frame * frame_count, 1.0);

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

void End()
{
    soundio_outstream_destroy(outstream);
    soundio_device_unref(device);
    soundio_destroy(soundio);
}