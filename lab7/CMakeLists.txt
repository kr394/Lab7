add_executable(lab7.elf main.c tetrisControl.c tetrisDisplay.c)
target_link_libraries(lab7.elf ${330_LIBS} intervalTimer buttons_switches)
set_target_properties(lab7.elf PROPERTIES LINKER_LANGUAGE CXX)
