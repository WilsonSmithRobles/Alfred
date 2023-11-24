#include <AudioRecorder.h>

/*
In order for a stream to maintain glitch-free operation the callback must consume and return audio data faster than it is recorded and/or played.
PortAudio anticipates that each callback invocation may execute for a duration approaching the duration of frameCount audio frames at the stream sample rate.
It is reasonable to expect to be able to utilise 70% or more of the available CPU time in the PortAudio callback. However, due to buffer size adaption and other
factors, not all host APIs are able to guarantee audio stability under heavy CPU load with arbitrary fixed callback buffer sizes. When high callback CPU 
utilisation is required the most robust behavior can be achieved by using paFramesPerBufferUnspecified as the Pa_OpenStream() framesPerBuffer parameter.
*/
static int recordCallback( const void *inputBuffer, void *outputBuffer,
                           size_t framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    paAudioData *data = (paAudioData*)userData;
    const SAMPLE *rptr = (const SAMPLE*)inputBuffer;
    SAMPLE *wptr = &data->recordedSamples[data->frameIndex * NUM_CHANNELS];
    size_t framesToCalc = framesPerBuffer;
    int finished = paContinue;
    size_t framesLeft = data->maxFrameIndex - data->frameIndex;

    (void) outputBuffer; /* Prevent unused variable warnings. */
    (void) timeInfo;
    (void) statusFlags;
    (void) userData;

    if( framesLeft < framesPerBuffer )
    {
        framesToCalc = framesLeft;
        finished = paComplete;
    }

    size_t i;
    if( inputBuffer == NULL )
    {
        for( i=0; i<framesToCalc; i++ )
        {
            *wptr++ = SAMPLE_SILENCE;  /* left */
            if( NUM_CHANNELS == 2 ) *wptr++ = SAMPLE_SILENCE;  /* right */
        }
    }
    else
    {
        for( i=0; i<framesToCalc; i++ )
        {
            *wptr++ = *rptr++;  /* left */
            if( NUM_CHANNELS == 2 ) *wptr++ = *rptr++;  /* right */
        }
    }
    data->frameIndex += framesToCalc;
    return finished;
};

/*******************************************************************/
std::vector<SAMPLE> sample_function(void)
{
    PaStreamParameters  inputParameters,
                        outputParameters;
    PaStream*           stream;
    PaError             err = paNoError;
    paAudioData         data;
    std::vector<SAMPLE> recording_buffer;
    bool                recording = false;
    int                 i;
    int                 totalFrames;
    int                 numSamples;
    int                 numBytes;
    SAMPLE              max, val;
    double              average, rms;

    std::cout << "Inicializando PortAudio" << std::endl;

    data.maxFrameIndex = totalFrames = NUM_SECONDS_RMS * SAMPLE_RATE; /* Record for a few seconds. */
    numSamples = totalFrames * NUM_CHANNELS;
    numBytes = numSamples * sizeof(SAMPLE);
    data.recordedSamples.resize(numSamples); /* From now on, recordedSamples is initialised. */
    
    for( i=0; i<numSamples; i++ ) data.recordedSamples[i] = 0;

    err = Pa_Initialize();
    if( err != paNoError ) goto done;

    inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    if (inputParameters.device == paNoDevice) {
        std::cerr << "Error: No default input device." << std::endl;
        goto done;
    }
    inputParameters.channelCount = NUM_CHANNELS;
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    std::cout << "Grabando."  << std::endl;

    while (true)
    {
        data.frameIndex = 0;
        /* Record some audio. -------------------------------------------- */
        err = Pa_OpenStream(
                &stream,
                &inputParameters,
                NULL,                  /* &outputParameters, */
                SAMPLE_RATE,
                FRAMES_PER_BUFFER,
                paClipOff,      /* we won't output out of range samples so don't bother clipping them */
                recordCallback,
                &data );
        if( err != paNoError ) goto done;
        err = Pa_StartStream( stream );
        if( err != paNoError ) goto done;

        while( ( err = Pa_IsStreamActive( stream ) ) == 1 ) Pa_Sleep(50);
        
        if( err < 0 ) goto done;
        err = Pa_CloseStream( stream );
        if( err != paNoError ) goto done;
        
        
        rms = get_RMS_volume(data.recordedSamples);
        if(rms > DB_SOUND_THRESHOLD)
        {
            recording = true;
            recording_buffer.insert(recording_buffer.end(), data.recordedSamples.begin(), data.recordedSamples.end());
        }
        else
        {
            if(recording)
            {
                recording_buffer.clear();
                goto done;
            }
            recording = false;
        }
    }

done:
    Pa_Terminate();
    if( err != paNoError )
    {
        std::cout << "An error occurred while using the portaudio stream" << std::endl;
        std::cout << "Error number: " << err << std::endl;
        std::cout << "Error message: " << Pa_GetErrorText( err ) << std::endl;
        err = 1;          /* Always return 0 or 1, but no other return codes. */
    }
    return recording_buffer;
}

double get_RMS_volume(std::vector<SAMPLE> buffer){
	double rms_amplitude = 0.0;
	for(SAMPLE sample : buffer){
		rms_amplitude += (double)sample * sample;
	}
	rms_amplitude = std::sqrt(rms_amplitude / buffer.size());
	
    double volume_dB = 20.0 * std::log10(rms_amplitude);
	return volume_dB;
}