cmake_minimum_required(VERSION 3.23)

project(IPSViewer VERSION 2.1 DESCRIPTION "Multiplatform IPS patch viewer" LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(PROJECT_AUTHOR "kylon")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.10 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

set(QHEXVIEW_BUILD_EXAMPLE OFF)
add_subdirectory(3rdparty/QHexView)

set(PROJECT_SOURCES
  src/Record.h
  src/RLERecord.h
  src/IPS.h
  src/IPS.cpp
  src/main.cpp
  src/mainwindow.h
  src/mainwindow.cpp

  ui/mainwindow.ui
)

if (WIN32)
  configure_file(Resources/Windows/win.rc.in ${CMAKE_CURRENT_SOURCE_DIR}/win.rc)

  list(APPEND PROJECT_SOURCES "win.rc")
endif ()

qt_add_executable(${PROJECT_NAME}
  ${PROJECT_SOURCES}
)

include(CheckIPOSupported)
check_ipo_supported(RESULT has_ipo OUTPUT ipo_error)
if (has_ipo)
  message(STATUS "${PROJECT_NAME}: IPO/LTO enabled")
  set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Core Qt::Widgets QHexView)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER kylon.ipsv.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
