#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <sqlite3.h>
#include <string>
#include "iostream"
#include <filesystem>
#include <vector>
#include <sstream>

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
    std::vector<std::string> searchFileByTags(const std::vector<std::string>& tagNames); // 태그 기반 파일 검색

    bool deleteFileByID(int fileId); // 파일 Id으로 파일 정보 삭제

    int getFileIdByPath(const std::string& filePath);// 파일 경로로 FileId 얻는 함수
    int getTagIdByName(const std::string& tagName); // 태그 이름으로 tagId 얻는 함수

    bool deleteTagByID(int tagId); // 태그ID기반으로 태그 삭제

    std::vector<std::string> getAllTagNamesFromDb(); // db에서 모든 태그 이름을 가져오는 함수

    bool unLinkFileTagByIds(int fileId, int tagId); // 파일 Id, 태그 Id 로 서로간의 연결 끊는 함수

    std::vector<std::string> getTagsForFileId(int fileId);

    sqlite3* getDBConnection(){return dbConnection;};
};

#endif //DATABASE_MANAGER_H