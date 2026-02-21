cmake_minimum_required(VERSION 3.25)

project(qwapp)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        WebEngineWidgets
        DBus
        REQUIRED)

add_executable(qwapp main.cpp
        MainWindow.cpp
        MainWindow.h
        resources.qrc
        SettingsDialog.cpp
        SettingsDialog.h
        webpage.cpp
        webpage.h
)

target_link_libraries(qwapp
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::WebEngineWidgets
        Qt::DBus
)

install(FILES com._404coding.qwapp.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)

install(FILES icons/app.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/512x512/apps
        RENAME com._404coding.qwapp.png)

install(FILES icons/app.svg
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
        RENAME com._404coding.qwapp.svg)

install(TARGETS qwapp DESTINATION bin)

set(CPACK_PACKAGE_NAME "qwapp")
set(CPACK_PACKAGE_VENDOR "404Coding")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Modern and simple WhatsApp Wrapper")
set(CPACK_PACKAGE_CONTACT "contact@404coding.com")

set(CPACK_PACKAGE_EXECUTABLE "qwapp")

set(CPACK_GENERATOR "RPM;DEB")
set(CPACK_RPM_PACKAGE_GROUP "Applications/Internet")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "404Coding")

include(CPack)