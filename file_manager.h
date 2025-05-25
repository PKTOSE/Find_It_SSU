#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include <vector>
#include <memory> // std::shared_ptr를 위해 (database_manager 포인터의 소유권을 명확해 해준다고 함.. 객체를 가리키는 모든 스마트포인터들이 소멸되어야만 객체의 소멸자를 호출한다.)
#include "database_manager.h" // databaseManager 클래스 정의를 위해

class FileManager {
public:
    // 생성자 : DatatbaseManger shared_ptr을 받아옴. explicit을 써서 암시적 변환을 방지..
    explicit FileManager(std::shared_ptr<DatabaseManager> dbManager);

    // 파일 추가 / 삭제 : filePath를 &로 주고 받고, 성공 실패 불리언으로 리턴.
    bool addFile(const std::string& filePath);
    bool deleteFile(const std::string& filePath); // DB에서도 해당 파일 정보 + 연결된 태그 정보 삭제

    // 태그 기반 파일 검색 : 태그 목록 받고, 파일 경로 목록 리턴
    std::vector<std::string> searchFilesByTags(const std::vector<std::string>& tagNames);

    // 파일 이름 기반 검색 함수
    std::vector<std::string> searchFilesByName(const std::string& keyword);

    /* 파일 경로 업데이트 (파일 이동/이름 변경 시)
    * FileID를 먼저 찾고, 해당 FileID의 filePath를 업데이트 해야 함.
    * 기존 filePath를 Unique로 사용함으로, FileID로 업데이트 하는게 중요함!
    */
    bool updateFile(const std::string& oldFilePath, const std::string& newFilePath);

    // 모든 파일 경로 가져오기 (GUI에서 파일 목록 표시용)
    std::vector<std::string> getAllFilePaths();
private:
    std::shared_ptr<DatabaseManager> dbManager; // DatabaseManager를 공유 포인터로 가짐.
};



#endif //FILE_MANAGER_H
