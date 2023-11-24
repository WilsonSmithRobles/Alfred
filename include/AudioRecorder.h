#include<AudioDefs.h>
#include <vector>

extern "C" {
#include "portaudio.h"
}

#ifndef PA_AUDIODATA
#define PA_AUDIODATA

struct paAudioData
{
    int                 frameIndex = 0;  /* Index into sample array. */
    int                 maxFrameIndex = 0;
    std::vector<SAMPLE> recordedSamples;
};

#endif  // PA_AUDIODATA

#ifndef AUDIO_RECORDER_H
#define AUDIO_RECORDER_H

#include <iostream>
#include <cmath>

static int recordCallback( const void *inputBuffer, void *outputBuffer,
                        unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo* timeInfo,
                        PaStreamCallbackFlags statusFlags,
                        void *userData );

static int playCallback( const void *inputBuffer, void *outputBuffer,
                        unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo* timeInfo,
                        PaStreamCallbackFlags statusFlags,
                        void *userData );
                        
std::vector<SAMPLE> sample_function(void);

double get_RMS_volume(std::vector<SAMPLE> buffer);


#endif // AUDIO_RECORDER_H