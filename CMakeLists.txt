cmake_minimum_required(VERSION 3.20)
project(myc VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_compile_definitions(_POSIX_C_SOURCE=200809L)
add_compile_options(-Wfatal-errors -Wall -Wextra -pipe -pedantic -Winvalid-pch
    # "$<$<C_COMPILER_ID:GNU>:-fanalyzer>"
)

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
    # add_compile_options(-fsanitize=leak -fsanitize=address -fsanitize=undefined)  # -fsanitize=thread
    # add_link_options(-fsanitize=leak -fsanitize=address -fsanitize=undefined)
else()
    # add_compile_definitions(_FORTIFY_SOURCE=1)
endif()

include_directories(include)



# packages
# find_package(LLVM REQUIRED CONFIG)
# # llvm_map_components_to_libnames(LLVM_LIBS core native)
# # LLVM_DISABLE_ABI_BREAKING_CHECKS_ENFORCING

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)


# targets
add_library(myc STATIC
    src/myc/util.c
    src/myc/SourceFile.c
    src/myc/Diagnostic.c
    src/myc/lexer.c
    src/myc/string.c
)
# target_include_directories(myc PUBLIC src/myc ${LLVM_INCLUDE_DIRS})
# target_compile_definitions(myc PUBLIC ${LLVM_DEFINITIONS})
# target_link_libraries(myc mcl frozen::frozen fmt::fmt ${LLVM_LIBS})



add_executable(mycc
    src/main.c
)
target_link_libraries(mycc myc Threads::Threads)



# install
install(TARGETS mycc
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
