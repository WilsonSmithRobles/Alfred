if(NOT TARGET PortAudio)
  # Module subdirectory
  add_subdirectory("libs/portaudio")

  # Module folder
  set_target_properties(PortAudio PROPERTIES FOLDER "modules/PortAudio")
endif()