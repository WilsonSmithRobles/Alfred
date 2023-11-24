#include <iostream>
#include <AudioRecorder.h>

int main()
{
    std::string userInput;
    std::vector<SAMPLE> record_query;
    while(true)
    {
        std::cout << "Escribe 'grabar' para empezar query. Escribe 'salir' para detener el programa" << std::endl;

        std::getline(std::cin, userInput);
        if(userInput == "grabar")
        {
            record_query = sample_function();
            // TODO: Send to whisper to convert to text.
            // TODO: Send said text to LLM.
            // /* Playback recorded data.  -------------------------------------------- */
            std::cout << "Whisper" << std::endl;
            std::cout << "Grabación acabada. Texto entendido: " << std::endl;
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