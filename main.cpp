#include "database_manager.h"
#include "vector"

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

            // TODO: 태그 연결, 검색 등

            std::cout << "\n--- Linking Files and Tags ---" << std::endl;
            // 예시 연결 (실제 ID는 콘솔 출력 또는 뷰어 확인 필요)
            // 예: 파일 ID 1 (C++_0_OT_2025.pdf)에 태그 ID 1 (고급프로그래밍) 연결
            bool linkSuccess1 = dbManager.linkFileTag(1, 1);
            if (linkSuccess1) {
                std::cout << "Linking FileID 1 with TagID!" << std::endl;
            }else {
                std::cerr << "Failed to link FileID 1 with TagID 1" << std::endl;
            }

            // 예: 파일 ID 1에 태그 ID 3 (강의자료) 연결
            bool linkSuccess2 = dbManager.linkFileTag(1, 3);
            if (linkSuccess2) {
                std::cout << "Linking FileID 1 with TagID 3!" << std::endl;
            }else {
                std::cerr << "Failed to link FileID 1 with TagID 3" << std::endl;
            }

            bool linkSuccess3 = dbManager.linkFileTag(2, 1);
            if (linkSuccess3) {
                std::cout << "Linking FileID 2 with TagID 1!" << std::endl;
            }else {
                std::cerr << "Failed to link FileID 2 with TagID 1" << std::endl;
            }
            bool linkSuccess4 = dbManager.linkFileTag(2, 5);
            if (linkSuccess4) {
                std::cout << "Linked FileID 2 with TagID 5" << std::endl;
            }else {
                std::cerr << "Failed to link FileID 2 with TagID 5" << std::endl;
            }

            // 존재하지 않는 ID로 오류 테스트..
            bool linkFailure1 = dbManager.linkFileTag(999, 1);
            if (linkFailure1) {
                std::cout << "Success!" << std::endl;
            }else {
                std::cerr << "Fail.." << std::endl;
            }

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