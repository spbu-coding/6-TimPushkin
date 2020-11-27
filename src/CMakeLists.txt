file(GLOB_RECURSE C_SOURCES ./*.c)

add_executable(strings_comparer ${C_SOURCES})

if(MSVC)
  target_compile_options(strings_comparer PRIVATE /W4 /WX)
else()
  target_compile_options(strings_comparer PRIVATE -Wall -Wextra -pedantic -Werror)
endif()
