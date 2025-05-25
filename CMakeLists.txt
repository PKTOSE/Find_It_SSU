cmake_minimum_required(VERSION 3.30)

project(file_tag) # project() 명령은 find_package 이전에 두는 것이 일반적입니다.

# Qt 6 패키지 찾기
find_package(Qt6 REQUIRED COMPONENTS Widgets) # 이 줄은 이대로 둡니다.

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON) # 시그널/슬롯 사용 시
set(CMAKE_AUTORCC ON) # 리소스 파일 사용 시

set(CMAKE_CXX_STANDARD 20)

# SQLite3 헤더 경로
include_directories(libs/sqlite3)

# Qt 프로젝트를 위한 실행 파일 정의 (모든 소스 파일을 여기에 포함)
qt_add_executable(file_tag
        main.cpp
        database_manager.cpp
        file_manager.cpp
        tag_manager.cpp
        mainwindow.cpp
        MainWindow.ui
        app.rc
        # 나중에 UI 파일 (.ui) 이나 리소스 파일 (.qrc) 이 생기면 여기에 추가
)

# Qt 모듈과 SQLite3 라이브러리 링크
target_link_libraries(file_tag PRIVATE
        Qt6::Widgets # Qt GUI 기능에 필요
        ${CMAKE_SOURCE_DIR}/libs/sqlite3/sqlite3.lib # SQLite3 라이브러리
)