#include <iostream>
#include <AudioRecorder.h>
#include <whisper.h>
#include <chrono>

std::vector<float> convert16to32(const std::vector<SAMPLE>& input) {
    std::vector<float> output(input.size());

    for (size_t i = 0; i < input.size(); i++) {
        int16_t sample = input[i];
        output[i] = static_cast<float>(sample) / 32768.0f; // Convert to floating point
    }

    return output;
}

int main()
{
	whisper_context * ctx;
	whisper_full_params wparams;

    ctx = whisper_init_from_file("./libs/whisper.cpp/models/ggml-small.bin");
	wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
	wparams.language = "es";
    wparams.n_threads = 6;

    std::string userInput;
    std::vector<SAMPLE> record_query;
    while(true)
    {
        std::cout << "Escribe 'grabar' para empezar query. Escribe 'salir' para detener el programa" << std::endl;

        std::getline(std::cin, userInput);
        if(userInput == "grabar")
        {
		    auto start = std::chrono::high_resolution_clock::now();
            record_query = sample_function();
            std::vector<float> recording = convert16to32(record_query);

            
            if (whisper_full(ctx, wparams, recording.data(), recording.size()) != 0)
                fprintf(stdout, "failed to process audio\n");
            else
            {
                std::string message_detected = "Not available";
                message_detected.clear();
                const int n_segments = whisper_full_n_segments(ctx);
                for (int i = 0; i < n_segments; ++i) 
                {
                    const char * text = whisper_full_get_segment_text(ctx, i);
                    message_detected += text;
                }
                std::cout << "Mensaje detectado:" << std::endl;
                std::cout << message_detected << std::endl;
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            std::cout << "Function execution time: " << duration.count() << " milliseconds" << std::endl;


            // TODO: Send to whisper to convert to text.
            // TODO: Send said text to LLM.
            // /* Playback recorded data.  -------------------------------------------- */
            // std::cout << "Whisper" << std::endl;
            // std::cout << "Grabación acabada. Texto entendido: " << std::endl;
            std::cout << "LLM" << std::endl;
            std::cout << "Voice answer" << std::endl;
        }
        else if (userInput == "salir")
        {
            break;
        }
    }
    return 0;
}