cmake_minimum_required(VERSION 3.30)

project(NDSFactory LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY "https://github.com/p-ranav/argparse.git"
)
FetchContent_MakeAvailable(argparse)

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
#        target_compile_options(${target_name} PRIVATE /WX /W4 /EHsc)
        target_compile_options(${target_name} PRIVATE /W4 /EHsc)  # TODO Disable before Git
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Werror)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Werror)
    endif()
endfunction()

target_compile(NDSFactory_CLI)
target_compile(NDSFactory_GUI)

target_link_libraries(NDSFactory_CLI PRIVATE
    argparse
)

# Link the necessary Qt6 libraries
target_link_libraries(NDSFactory_GUI PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

add_custom_target(Build_Step_1 ALL)  # Delete qt.conf (if exists)
add_custom_target(Build_Step_2 ALL)  # Delete "deploy_gui" folder
add_custom_target(Build_Step_3 ALL)  # Make new "deploy_gui" folder and deploy
add_custom_target(Build_Step_4 ALL)  # Copy new qt.conf

add_dependencies(Build_Step_1 NDSFactory_GUI)
add_dependencies(Build_Step_3 Build_Step_2)
add_dependencies(Build_Step_4 Build_Step_1)

set(QT_BUILD_TYPE_CONF "${CMAKE_SOURCE_DIR}/qt.${CMAKE_BUILD_TYPE}.conf")
set(QT_CONF "${NDSFactory_BINARY_DIR}/qt.conf")
if(EXISTS ${QT_CONF})
    add_custom_command(TARGET Build_Step_1 PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove ${QT_CONF}
    )
endif()
if(NOT EXISTS ${QT_CONF})
    add_custom_command(TARGET Build_Step_4 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Copying \"${QT_BUILD_TYPE_CONF}\" to \"${QT_CONF}\"..."
        COMMAND ${CMAKE_COMMAND} -E copy ${QT_BUILD_TYPE_CONF} ${QT_CONF}
        COMMAND ${CMAKE_COMMAND} -E echo "Done copying"
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(DEPLOY_GUI_DIR "${NDSFactory_BINARY_DIR}/deploy_gui")
    if(EXISTS ${DEPLOY_GUI_DIR})
        add_custom_command(TARGET Build_Step_2 PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Directory \"${DEPLOY_GUI_DIR}\" exists. Deleting..."
            COMMAND ${CMAKE_COMMAND} -E remove_directory "${DEPLOY_GUI_DIR}"
            COMMAND ${CMAKE_COMMAND} -E echo "Deleted directory"
        )
    endif()
    if(NOT EXISTS ${DEPLOY_GUI_DIR})
        if(WIN32)
            add_custom_command(TARGET Build_Step_3 POST_BUILD
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

# FIXME Fix concurrency issue with qt.conf being copied and qt being deployed
