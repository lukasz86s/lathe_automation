# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)
# added download option
include(FetchContent)

project(lathe_automation C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()


#download eternal lib
FetchContent_Declare(
    st7789
    GIT_REPOSITORY https://github.com/ArmDeveloperEcosystem/st7789-library-for-pico.git
    GIT_TAG main
)
set(ST7789_BUILD_EXAMPLES OFF CACHE BOOL "Disable building examples")
FetchContent_MakeAvailable(st7789)

# Add executable. Default name is the project name, version 0.1

add_executable(lathe_automation src/lathe_automation.c
     src/fonts/fonts.c src/buttons/buttons.c

    )
target_include_directories(lathe_automation PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/fonts/include ${CMAKE_CURRENT_LIST_DIR}/src/buttons/include)

pico_generate_pio_header(lathe_automation ${CMAKE_CURRENT_LIST_DIR}/src/stepper_control.pio)


pico_set_program_name(lathe_automation "lathe_automation")
pico_set_program_version(lathe_automation "0.3")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(lathe_automation 0)
pico_enable_stdio_usb(lathe_automation 1)

# Add the standard library to the build
target_link_libraries(lathe_automation
        pico_stdlib
        pico_st7789
        hardware_spi
        hardware_pio
        hardware_gpio      
)


# Add the standard include files to the build
target_include_directories(lathe_automation PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)


# Add any user requested libraries
target_link_libraries(lathe_automation 
        pico_cyw43_arch_none
        )

pico_add_extra_outputs(lathe_automation)

