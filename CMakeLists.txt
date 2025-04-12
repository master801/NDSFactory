cmake_minimum_required(VERSION 3.30)

project(NDSFactory LANGUAGES CXX)

find_package(Boost REQUIRED COMPONENTS
    program_options
)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
)

# Include sanitizers
include("${CMAKE_SOURCE_DIR}/cmake/sanitizers.cmake")

# Determine the Git commit hash if in a git repository
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
else()
  set(GIT_COMMIT_HASH "")
endif()

# Configure the revision header file
configure_file(
  "${CMAKE_SOURCE_DIR}/ui/dialogs/about/revision.h.in"
  "${CMAKE_SOURCE_DIR}/ui/dialogs/about/revision.h"
)

# Collect source and header files
## Dialogs
file(GLOB_RECURSE DIALOGS_HEADERS "ui/dialogs/*.h")
file(GLOB_RECURSE DIALOGS_SOURCES "ui/dialogs/*.cpp")
file(GLOB_RECURSE DIALOGS_UIS "ui/dialogs/*.ui")

## Tabs
file(GLOB_RECURSE TABS_HEADERS "ui/tabs/*.h")
file(GLOB_RECURSE TABS_SOURCES "ui/tabs/*.cpp")

## Models
file(GLOB_RECURSE MODELS_HEADERS "ui/models/*.h")
file(GLOB_RECURSE MODELS_SOURCES "ui/models/*.cpp")

## Utils
file(GLOB_RECURSE UTILS_HEADERS "ui/utils/*.h")

## NDSFactory
file(GLOB_RECURSE NDSFACTORY_SOURCES "ndsfactory/*.cpp")
file(GLOB_RECURSE NDSFACTORY_HEADERS "ndsfactory/*.h")

set(SOURCES_CLI
    "main_cli.cpp"
    ${NDSFACTORY_SOURCES}
)

set(SOURCES_GUI
    "main_gui.cpp"
    "ui/mainwindow.cpp"
    ${NDSFACTORY_SOURCES}
    ${DIALOGS_SOURCES}
    ${MODELS_SOURCES}
    ${TABS_SOURCES}
)

set(HEADERS_GUI
    "ui/mainwindow.h"
    ${NDSFACTORY_HEADERS}
    ${TABS_HEADERS}
    ${MODELS_HEADERS}
    ${DIALOGS_HEADERS}
    ${UTILS_HEADERS}
)

set(FORMS
    "ui/mainwindow.ui"
    ${DIALOGS_UIS}
)

# CLI
add_executable(NDSFactory_CLI ${SOURCES_CLI} ${NDSFACTORY_HEADERS})

#GUI
# Platform-specific executable creation
if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE ndsfactory.icns)
    set(MACOS_ICNS "res/ndsfactory.icns")
    set_source_files_properties(${MACOS_ICNS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    add_executable(NDSFactory_GUI MACOSX_BUNDLE ${SOURCES_GUI} ${HEADERS_GUI} ${FORMS} ${MACOS_ICNS})
elseif(WIN32)
    set(WIN32_RESOURCES "res/resources.rc")
    add_executable(NDSFactory_GUI WIN32 ${SOURCES_GUI} ${HEADERS_GUI} ${FORMS} ${WIN32_RESOURCES})
else()
    add_executable(NDSFactory_GUI ${SOURCES_GUI} ${HEADERS_GUI} ${FORMS})
endif()

# Boost features
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

# Enable Qt's automatic features for the target
set_target_properties(NDSFactory_GUI PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
)

function(target_compile target_name)
    # Set C++ standard for the target
    target_compile_features(${target_name} PUBLIC cxx_std_20)

    # Add compile options for different compilers
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /WX /W4 /EHsc)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Werror)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Werror)
    endif()
endfunction()

target_compile(NDSFactory_CLI)
target_compile(NDSFactory_GUI)

target_link_libraries(NDSFactory_CLI PRIVATE
    Boost::program_options
)

# Link the necessary Qt6 libraries
target_link_libraries(NDSFactory_GUI PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

if(EXISTS "${NDSFactory_BINARY_DIR}/qt.conf")
    add_custom_command(TARGET NDSFactory_GUI PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove "${NDSFactory_BINARY_DIR}/qt.conf"
    )
endif()
if(NOT EXISTS "${NDSFactory_BINARY_DIR}/qt.conf")
    add_custom_command(TARGET NDSFactory_GUI PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/qt.${CMAKE_BUILD_TYPE}.conf" "${NDSFactory_BINARY_DIR}/qt.conf"
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(DEPLOY_GUI_DIR "${NDSFactory_BINARY_DIR}/deploy_gui")
    if(EXISTS ${DEPLOY_GUI_DIR})
        add_custom_command(TARGET NDSFactory_GUI PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove_directory "${DEPLOY_GUI_DIR}"
        )
    endif()
    if(NOT EXISTS ${DEPLOY_GUI_DIR})
        if(WIN32)
            add_custom_command(TARGET NDSFactory_GUI POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E echo "Deploying Windows Qt Release build..."
                COMMAND ${CMAKE_COMMAND} -E make_directory ${DEPLOY_GUI_DIR}
                COMMAND ${CMAKE_COMMAND} -E copy "${NDSFactory_BINARY_DIR}/${PROJECT_NAME}_GUI.exe" "${DEPLOY_GUI_DIR}/${PROJECT_NAME}.exe"
                COMMAND ${WINDEPLOYQT_EXECUTABLE}
                    ARGS --verbose 2  # For debugging
                    ARGS --release
                    ARGS --no-compiler-runtime --no-system-dxc-compiler
                    ARGS --skip-plugin-types generic
                    ARGS --exclude-plugins qgif,qjpeg  # We don't need these DLLs
                    ARGS "${DEPLOY_GUI_DIR}/${PROJECT_NAME}.exe"
                COMMAND ${CMAKE_COMMAND} -E echo "Done deploying"
            )
            # Stupid dependencies...
            add_custom_command(TARGET NDSFactory_GUI POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${NDSFactory_BINARY_DIR}/double-conversion.dll"
                    "${NDSFactory_BINARY_DIR}/pcre2-16.dll"
                    ${DEPLOY_GUI_DIR}
            )
        endif()
    endif()
endif()
