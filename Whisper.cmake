if(NOT TARGET whisper)
  # Module subdirectory
  add_subdirectory("libs/whisper.cpp")

  # Module folder
  set_target_properties(whisper PROPERTIES FOLDER "modules/PortAudio")
endif()