cmake_minimum_required(VERSION 3.13)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../hw/bsp/family_support.cmake)

# gets PROJECT name for the example (e.g. <BOARD>-<DIR_NAME>)
family_get_project_name(PROJECT ${CMAKE_CURRENT_LIST_DIR})

project(${PROJECT} C CXX ASM)

# Checks this example is valid for the family and initializes the project
family_initialize_project(${PROJECT} ${CMAKE_CURRENT_LIST_DIR})

# Espressif has its own cmake build system that calls add_executable, add_dependencies, etc
if(NOT FAMILY STREQUAL "espressif_idf")
  add_executable(${PROJECT})

  # Example source
  target_sources(${PROJECT} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usb_descriptors.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/matrix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dual_mode.c
  )

  # Example include
  target_include_directories(${PROJECT} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
  )

  # Configure compilation flags and libraries for the example without RTOS.
  # See the corresponding function in hw/bsp/FAMILY/family.cmake for details.
  family_configure_device_example(${PROJECT} noos)
endif()