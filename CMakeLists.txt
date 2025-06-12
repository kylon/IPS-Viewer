cmake_minimum_required(VERSION 3.23)

project(IPSviewer VERSION 1.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(QHEXVIEW_BUILD_EXAMPLE OFF)
add_subdirectory(3rdparty/QHexView)

set(PROJECT_SOURCES
    src/IPSRecord.h
    src/RLERecord.h
    src/Record.h
    src/IPS.cpp
    src/IPS.h
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h

    ui/mainwindow.ui
)

qt_add_executable(IPSviewer
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

include_directories(3rdparty/QHexView/include)
target_link_libraries(IPSviewer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets QHexView)

set_target_properties(IPSviewer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER kylon.ipsv.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

qt_finalize_executable(IPSviewer)
