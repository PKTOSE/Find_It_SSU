cmake_minimum_required(VERSION 3.30)
project(file_tag)

set(CMAKE_CXX_STANDARD 20)

# --- SQLite3 라이브러리 설정 시작 ---

# sqlite3.h 헤더 파일이 있는 디렉토리를 컴파일러에게 알려줍니다.
# 'libs/sqlite3' 폴더에 sqlite3.h가 있다고 가정합니다.
include_directories(libs/sqlite3)

# 실행 파일과 소스 파일들을 정의합니다.
# sqlite3.c 파일은 미리 컴파일된 .lib를 사용하므로 여기에 추가하지 않습니다.
add_executable(file_tag main.cpp
        database_manager.cpp)

# 실행 파일에 sqlite3 라이브러리를 링크합니다.
# 'libs/sqlite3' 폴더에 sqlite3.lib 파일이 있다고 가정하고 전체 경로를 지정합니다.
target_link_libraries(file_tag
        ${CMAKE_SOURCE_DIR}/libs/sqlite3/sqlite3.lib
)

# --- SQLite3 라이브러리 설정 끝 ---

