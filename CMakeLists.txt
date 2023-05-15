cmake_minimum_required(VERSION 3.12)

# Change your executable name to something creative!
set(NAME pico-unicorn-test) # <-- Name your project/executable here!

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include(${PIMORONI_PICO_PATH}/pimoroni_pico_import.cmake)

# Gooey boilerplate
project(${NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi")

# Initialize the SDK
pico_sdk_init()

# Add your source files
add_executable(${NAME}
    main.cpp # <-- Add source files here!
)

# Include required libraries
include(${PIMORONI_PICO_PATH}/common/pimoroni_i2c.cmake)
include(${PIMORONI_PICO_PATH}/common/pimoroni_bus.cmake)
include(${PIMORONI_PICO_PATH}/libraries/pico_graphics/pico_graphics.cmake)
include(${PIMORONI_PICO_PATH}/libraries/pico_unicorn/pico_unicorn.cmake)

# Don't forget to link the libraries you need!
target_link_libraries(${NAME}
    pico_stdlib # <-- List libraries here!
    hardware_pio
    hardware_dma
    pico_graphics
    pico_unicorn
)

# create map/bin/hex file etc.
pico_add_extra_outputs(${NAME})

# Set up files for the release packages
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.uf2
    ${CMAKE_CURRENT_LIST_DIR}/README.md
    DESTINATION .
)

set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_GENERATOR "ZIP" "TGZ")
include(CPack)
