cmake_minimum_required(VERSION 3.5)

project(OGSLauncher LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include(GNUInstallDirs)

# Build QHotkey from submodule
add_subdirectory(lib/QHotkey)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(OGS-Launcher
    src/main.cpp
    src/app/myapp.cpp
    src/app/myapp.h
)

target_link_libraries(OGS-Launcher
    PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets QHotkey::QHotkey
)

install(TARGETS OGS-Launcher
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES src/ogs.launcher.desktop
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
