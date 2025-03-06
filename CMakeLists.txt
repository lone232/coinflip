cmake_minimum_required(VERSION 3.16)

project(coinflip VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 修改点 1: 统一查找 Qt 模块，确保包含 Multimedia 组件
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)  # 添加 Multimedia 组件

set(PROJECT_SOURCES
    main.cpp
    mainscene.cpp
    mainscene.h
    mainscene.ui
    res.qrc
    crteffect.h crteffect.cpp
    crtbutton.h crtbutton.cpp
    chooselevelscene.h chooselevelscene.cpp
    playscene.h playscene.cpp
    crtcoin.h crtcoin.cpp
    dataconfig.cpp dataconfig.h
    winscene.h winscene.cpp
    LevelData.json
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(coinflip
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        videowidget.cpp videowidget.h
        app_icon.rc
        coinIcon.ico
        # 添加条件：仅在 Windows 包含 .rc 文件
        $<$<PLATFORM_ID:Windows>:app_icon.rc>
    )
else()
    if(ANDROID)
        add_library(coinflip SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(coinflip
            ${PROJECT_SOURCES}
            # 添加条件：仅在 Windows 包含 .rc 文件
            $<$<PLATFORM_ID:Windows>:app_icon.rc>
        )
    endif()
endif()

# 修改点 2: 统一使用 Qt${QT_VERSION_MAJOR} 变量链接库
target_link_libraries(coinflip PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia  # 正确链接 Multimedia 模块
    Qt${QT_VERSION_MAJOR}::MultimediaWidgets
)

# 移除冗余的 Qt6::Core 链接（Qt Widgets 已依赖 Core）

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.coinflip)
endif()

set_target_properties(coinflip PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    # 添加 Windows 图标属性
    RESOURCE "app_icon.rc"
)

include(GNUInstallDirs)
install(TARGETS coinflip
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(coinflip)
endif()
