cmake_minimum_required(VERSION 3.16)

project(client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Network Widgets)

qt_standard_project_setup()

qt_add_executable(client_gui
    main.cpp
    mainwindow.h mainwindow.cpp
    senderthread.h senderthread.cpp
    receiverthread.h receiverthread.cpp
)


set_target_properties(client_gui PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

set(SERVER_SOURCES ../server.cpp)
qt_add_executable(server ${SERVER_SOURCES})

set(CLI_CLIENT_SOURCES ../client.cpp)
qt_add_executable(client_cli ${CLI_CLIENT_SOURCES})

target_link_libraries(client_gui PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Network
    Qt6::Widgets
)

install(TARGETS client_gui
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
