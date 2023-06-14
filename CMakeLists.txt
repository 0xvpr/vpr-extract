cmake_minimum_required( VERSION 3.22.0 )

project( vpr-extract )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED 20 )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin )

set( CXX_HEADER_DIR ${CMAKE_SOURCE_DIR}/include )
set( CXX_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src )

find_package(TBB REQUIRED)

include_directories( ${PROJECT_NAME} ${CXX_HEADER_DIR} )

set( CXX_HEADERS
    ${CXX_HEADER_DIR}/definitions.hpp
)
set( CXX_SOURCES
    ${CXX_SOURCE_DIR}/main.cpp
)

set( SOURCES ${CXX_HEADERS} ${CXX_SOURCES})

add_executable( ${PROJECT_NAME} ${SOURCES} )
target_link_libraries(${PROJECT_NAME} PRIVATE TBB::tbb)
install(
    TARGETS     ${PROJECT_NAME} 
    DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

if(MSVC)
    target_compile_options(
        ${PROJECT_NAME} PRIVATE /W4 /WX
                                $<$<CONFIG:Debug>:/MTd>
                                $<$<CONFIG:Debug>:/EHa>
                                $<$<CONFIG:Release>:/ARCH:AVX512>
                                $<$<CONFIG:Release>:/EHs>
                                $<$<CONFIG:Release>:/MT>
                                $<$<CONFIG:Release>:/O2>
                                $<$<CONFIG:Release>:/DNDEBUG>
    )
    target_link_options( ${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:/MANIFEST:NO> )
else()
    target_compile_options(
        ${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -Wshadow -Wpedantic -Wconversion
                                $<$<CONFIG:Debug>:-O2>
                                $<$<CONFIG:Release>:-Ofast>
                                $<$<CONFIG:Release>:-m64>
                                $<$<CONFIG:Release>:-march=native>
                                $<$<CONFIG:Release>:-ffast-math>
                                $<$<CONFIG:Release>:-ffunction-sections>
                                $<$<CONFIG:Release>:-fPIE>
                                $<$<CONFIG:Release>:-fno-ident>
                                $<$<CONFIG:Release>:-fvisibility=hidden>
    )
    target_link_options( ${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:-s> )
endif()

