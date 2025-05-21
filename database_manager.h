#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include "sqlite3.h"
#include "string"
#include "iostream"
#include "filesystem"

class DatabaseManager {
private:
    sqlite3 *dbConnection; // SQLite 데이터베이스 연결 객체 포인터
    std::string databasePath; // 데이터베이스 파일 경로 지정

    // 복사 및 대입 방지 (DatabaseManager 객체는 하나만 존재하도록) * 중요 *
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
public:
    DatabaseManager(const std::string &dbPath); // 생성자, 데이터베이스 파일을 열거나 생성
    ~DatabaseManager(); // 소멸자, 데이터베이스 연결 닫기
    bool isConnected() const{
        return dbConnection != nullptr;
    };
    bool createTables();// db 테이블 생성
    int addFile(const std::string& filePath); // 파일 주소 추가, file ID 반환, 실패시 -1 반환
    int addTag(const std::string& tagName); // 파일에 태그 추가, 태그 ID 반환, 실패시 -1 반환
    bool linkFileTag(int fileId, int tagId); // 파일 - 태그 연결 (성공 / 실패)
    // TODO: 데이터 조회 등의 멤버 함수를 추가
};

#endif //DATABASE_MANAGER_H
