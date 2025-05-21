#include <algorithm>

#include "database_manager.h"
#include "vector"

void linkingTest(DatabaseManager &dbManager, int fileId, int tagId) {
    bool linking = dbManager.linkFileTag(fileId, tagId);
    if (linking) {
        std::cout << "Linking FileID "<< fileId << "with "<< tagId <<"!" << std::endl;
    }else {
        std::cerr << "Failed to link "<< fileId << "with "<< tagId << std::endl;
    }
}

void searchTagTest(DatabaseManager &dbManager, std::vector<std::string> tags) {
    std::vector<std::string> foundTags = dbManager.searchFileByTags(tags);
    std::cout << "Files with tag:" << std::endl;
    if (foundTags.empty()) {
        std::cout << " No files found.." << std::endl;
    }else {
        for (const auto& path : foundTags) {
            std::cout << "  - "<< path << std::endl;
        }
    }
}

int main() {
    // 데이터 베이스 파일 경로 지정 (ex. 실행 파일과 같은 폴더에 생성)
    const std::string dbFilePath = "tag_database.db";

    // DatabaseManager 객체 생성 (생성자에서 db연결 시도)
    DatabaseManager dbManager(dbFilePath);

    // 연결 확인
    if (dbManager.isConnected()) {
        std::cout << "Database is Successfully connected!" << std::endl;

        if (dbManager.createTables()) {
            std::cout << "Database setup complete (tables checked/created)!" << std::endl;

            // 이하는 테스트 코드
            std::vector<std::string> testFiles = {
                "C:/FILES/2025_school/25-1/고급프로그래밍/C++_0_OT_2025.pdf",
                "C:/FILES/2025_school/25-1/고급프로그래밍/씹어먹는c++.pdf",
                "C:/FILES/2025_school/25-1/고급프로그래밍/C++_0_OT_2025.pdf", // 중복 테스트
            };

            std::vector<int> fileIds;
            for (const auto &filePath : testFiles) {
                int fileID = dbManager.addFile(filePath);
                if (fileID != -1) { // addFile 실패시 -1 반환 -> -1이 아니면 파일 추가 성공!
                    std::cout << "Processed file " << filePath <<" with fileID " << fileID << std::endl;
                    fileIds.push_back(fileID); // fileID를 fileIDs 벡터(가변길이 배열)에 추가
                }else {
                    std::cerr << "Failed to add file: " << filePath << std::endl;
                }
            }
            std::vector<std::string> testTags = {
                "고급프로그래밍",
                "발표자료",
                "강의자료",
                "고급프로그래밍",
                "문서",
            };
            std::vector<int> tagIds;
            for (const auto &tagName : testTags) {
                int tagId = dbManager.addTag(tagName);
                if (tagId != -1) { // addFile 실패시 -1 반환 -> -1이 아니면 파일 추가 성공!
                    std::cout << "Processed tag " << tagName <<" with tagId " << tagId << std::endl;
                    fileIds.push_back(tagId); // fileID를 fileIDs 벡터(가변길이 배열)에 추가
                }else {
                    std::cerr << "Failed to process tag: " << tagName << std::endl;
                }
            }

            std::cout << "\n--- Linking Files and Tags ---" << std::endl;
            // 예시 연결 (실제 ID는 콘솔 출력 또는 뷰어 확인 필요)
            // 예: 파일 ID 1 (C++_0_OT_2025.pdf)에 태그 ID 1 (고급프로그래밍) 연결
            linkingTest(dbManager, 1, 1);
            // 예: 파일 ID 1에 태그 ID 3 (강의자료) 연결
            linkingTest(dbManager, 1, 3);
            linkingTest(dbManager, 2, 1);
            linkingTest(dbManager, 2, 5);
            // 존재하지 않는 ID로 오류 테스트..
            linkingTest(dbManager, 999, 5);

            // 검색 테스트
            std::cout << "\n--- Searching Files by Tags ---" << std::endl;
            std::vector<std::string> tags1 = {"고급프로그래밍"};
            searchTagTest(dbManager, tags1);
            std::vector<std::string> tags2 = {"고급프로그래밍", "강의자료"};
            searchTagTest(dbManager, tags2);
            std::vector<std::string> tags3 = {"발표자료"};
            searchTagTest(dbManager, tags3);
            std::vector<std::string> tags4 = {"error"};
            searchTagTest(dbManager, tags4);

        }else {
            std::cerr << "Database setup failed!" << std::endl;
            return 1;
        }
    }else {
        std::cerr << "Database is not Connected!" << std::endl;
        return 1; // 프로그램 종류(혹은 오류 처리) TODO: 오류처리해서 종료 안되도록 해보기
    }

    return 0; // main함수 종료 시 dbManager 소멸자 호출 -> DB연결이 자동으로 닫힘
}