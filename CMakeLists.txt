cmake_minimum_required( VERSION 3.22.0 )

project( vpr-extract )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED 20 )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin )

set( CXX_HEADER_DIR ${CMAKE_SOURCE_DIR}/include )
set( CXX_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src )

include_directories( ${PROJECT_NAME} ${CXX_HEADER_DIR} )

set( CXX_HEADERS
    ${CXX_HEADER_DIR}/definitions.hpp
)
set( CXX_SOURCES
    ${CXX_SOURCE_DIR}/main.cpp
)

set( SOURCES ${CXX_HEADERS} ${CXX_SOURCES})

add_executable( ${PROJECT_NAME} ${SOURCES} )

if(MSVC)
else()
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
            target_compile_options(
                ${PROJECT_NAME} PRIVATE -march=native -m64
                                        -O3
                                        -Wall -Wextra -Werror -Wshadow -Wpedantic -Wconversion
                                        -ffunction-sections -fPIE
                                        -fno-ident -fvisibility=hidden
            )
            target_link_options(
                ${PROJECT_NAME} PRIVATE -s
            )
        else()
            target_compile_options(
                ${PROJECT_NAME} PRIVATE -O2
                                        -Wall -Wextra -Werror -Wshadow -Wpedantic -Wconversion
            )
    endif()
endif()

install(
    TARGETS     ${PROJECT_NAME} 
    DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
