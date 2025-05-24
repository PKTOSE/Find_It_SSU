#include "database_manager.h"
#include "file_manager.h"
#include "tag_manager.h"
#include <vector>
#include <iostream>
#include <memory> // shared ptr 사용
#include <set> // uniqueTagName에 사용되는 set (searchFileByTags에서 사용)

int main() {
    // 데이터 베이스 파일 경로 지정 (ex. 실행 파일과 같은 폴더에 생성)
    const std::string dbFilePath = "tag_database.db";

    // DatabaseManager 객체 생성 (생성자에서 db연결 시도) - shared_ptr로 생성
    std::shared_ptr<DatabaseManager> dbManager = std::make_shared<DatabaseManager>(dbFilePath);

    if (!dbManager->isConnected()) { // 연결 오류
        std::cerr << "Database connection failed!" << std::endl;
        return 1;
    } // else
    std::cout << "Database connection successful!" << std::endl;

    if (!dbManager->createTables()) { // db 테이블 생성 실패
        std::cerr << "Failed to create database tables!" << std::endl;
        return 1;
    } // else(성공)
    std::cout << "Database setup complete(tables checked/created)!" << std::endl;

    // fileManager, tagManager 객체 생성 (관리 하는 객체)
    FileManager fileManager(dbManager);
    TagManager tagManager(dbManager);

    // --- 파일 추가 테스트 (FileManager 사용) ---
    std::cout << "\n--- Adding Files (using FileManager) ---" << std::endl;
    std::vector<std::string> testFiles = {
        "C:/FILES/2025_school/25-1/고급프로그래밍/C++_0_OT_2025.pdf",
        "C:/FILES/2025_school/25-1/고급프로그래밍/씹어먹는c++.pdf",
        "C:/FILES/2025_school/25-1/고급프로그래밍/C++_0_OT_2025.pdf", // 중복
        "D:/Project/source.cpp"
    };

    for (const auto& filePath : testFiles) {
        if (fileManager.addFile(filePath)) {
            std::cout << "File found or added successfully! ( path: " << filePath << " )" << std::endl;
        }else {
            std::cerr << "Failed to add file ( path:" << filePath << " )" <<std::endl;
        }
    }

    // --- 태그 추가 테스트 (TagManager 사용) ---
    std::cout << "\n--- Adding Tags (using TagManager) ---" << std::endl;
    std::vector<std::string> testTags = {
        "고급프로그래밍",
        "발표자료",
        "강의자료",
        "고급프로그래밍", // 중복
        "문서",
    };
    for (const auto &tagName : testTags) {
        if (tagManager.addTag(tagName)) {
            std::cout << "Tag found or added successfully! ( tag:  " << tagName << " )" <<  std::endl;
        } else {
            std::cerr << "Failed to add tag ( tag: " << tagName << " )"  << std::endl;
        }
    }

    // --- 파일과 태그 연결 테스트 (TagManager 사용) ---
    std::cout << "\n--- Linking Files and Tags (using TagManager) ---" << std::endl;

    if (tagManager.linkFileWithTag("C:/FILES/2025_school/25-1/고급프로그래밍/C++_0_OT_2025.pdf", "고급프로그래밍")) {
        std::cout << "Linked C++_0_OT_2025.pdf with 고급프로그래밍" << std::endl;
    } else { std::cerr << "Failed to link C++_0_OT_2025.pdf with 고급프로그래밍" << std::endl; }

    if (tagManager.linkFileWithTag("C:/FILES/2025_school/25-1/고급프로그래밍/C++_0_OT_2025.pdf", "강의자료")) {
        std::cout << "Linked C++_0_OT_2025.pdf with 강의자료" << std::endl;
    } else { std::cerr << "Failed to link C++_0_OT_2025.pdf with 강의자료" << std::endl; }

    // 예시 연결: 씹어먹는c++.pdf 에 "고급프로그래밍", "문서" 태그 연결
    if (tagManager.linkFileWithTag("C:/FILES/2025_school/25-1/고급프로그래밍/씹어먹는c++.pdf", "고급프로그래밍")) {
        std::cout << "Linked 씹어먹는c++.pdf with 고급프로그래밍" << std::endl;
    } else { std::cerr << "Failed to link 씹어먹는c++.pdf with 고급프로그래밍" << std::endl; }

    if (tagManager.linkFileWithTag("C:/FILES/2025_school/25-1/고급프로그래밍/씹어먹는c++.pdf", "문서")) {
        std::cout << "Linked 씹어먹는c++.pdf with 문서" << std::endl;
    } else { std::cerr << "Failed to link 씹어먹는c++.pdf with 문서" << std::endl; }

    // 존재하지 않는 파일/태그로 연결 시도 (Foreign Key 제약 조건 테스트)
    std::cout << "\n--- Testing invalid linking ---" << std::endl;
    if (tagManager.linkFileWithTag("NonExistentPath/file.pdf", "고급프로그래밍")) {
        std::cout << "Linked NonExistentPath/file.pdf with 고급프로그래밍 (Unexpected!)" << std::endl;
    } else {
        std::cerr << "Failed to link NonExistentPath/file.pdf with 고급프로그래밍 (Expected failure)" << std::endl;
    }
    if (tagManager.linkFileWithTag("C:/FILES/2025_school/25-1/고급프로그래밍/C++_0_OT_2025.pdf", "NonExistentTag")) {
        std::cout << "Linked C++_0_OT_2025.pdf with NonExistentTag (Unexpected!)" << std::endl;
    } else {
        std::cerr << "Failed to link C++_0_OT_2025.pdf with NonExistentTag (Expected failure)" << std::endl;
    }

    // --- 파일 검색 테스트 (FileManager 사용) ---
    std::cout << "\n--- Searching Files by Tags (using FileManager) ---" << std::endl;

    std::vector<std::string> searchTags1 = {"고급프로그래밍"};
    std::vector<std::string> results1 = fileManager.searchFiles(searchTags1);
    std::cout << "Files with tag '고급프로그래밍':" << std::endl;
    if (results1.empty()) {
        std::cout << "  No files found." << std::endl;
    } else {
        for (const auto& path : results1) {
            std::cout << "  - " << path << std::endl;
        }
    }

    // --- delete test ---
    std::cout << "\n--- Deleting Files (using FileManager) ---" << std::endl;
    // 삭제할 파일 경로: "D:/Project/source.cpp"
    std::string fileToDelete = "D:/Project/source.cpp";
    if (fileManager.deleteFile(fileToDelete)) {
        std::cout << "Delete request successful for: " << fileToDelete << std::endl;
    } else {
        std::cerr << "Failed to delete: " << fileToDelete << std::endl;
    }

    // 존재하지 않는 파일 삭제 시도
    std::string nonExistentFileToDelete = "C:/NonExistent/File.txt";
    if (fileManager.deleteFile(nonExistentFileToDelete)) {
        std::cout << "Delete request successful for non-existent file (Unexpected!): " << nonExistentFileToDelete << std::endl;
    } else {
        std::cerr << "Failed to delete non-existent file (Expected failure): " << nonExistentFileToDelete << std::endl;
    }

    // 삭제 후 다시 검색하여 확인 (선택 사항)
    std::cout << "\n--- Verifying after deletion ---" << std::endl;
    std::vector<std::string> searchTagsAfterDelete = {"고급프로그래밍"}; // 예시 태그
    std::cout << "Files with tag '고급프로그래밍' after deletion attempt:" << std::endl;
    std::vector<std::string> resultsAfterDelete = fileManager.searchFiles(searchTagsAfterDelete);
    if (resultsAfterDelete.empty()) {
        std::cout << "  No files found." << std::endl;
    } else {
        for (const auto& path : resultsAfterDelete) {
            std::cout << "  - " << path << std::endl;
        }
    }

    // --- 태그 삭제 테스트 ---
    std::cout << "\n--- Deleting Tags (using TagManager) ---" << std::endl;
    // 삭제할 태그: "발표자료"
    std::string tagToDelete = "발표자료";
    if (tagManager.deleteTag(tagToDelete)) {
        std::cout << "Delete request successful for tag: " << tagToDelete << std::endl;
    } else {
        std::cerr << "Failed to delete tag: " << tagToDelete << std::endl;
    }

    // 존재하지 않는 태그 삭제 시도
    std::string nonExistentTagToDelete = "존재하지_않는_태그";
    if (tagManager.deleteTag(nonExistentTagToDelete)) {
        std::cout << "Delete request successful for non-existent tag (Unexpected!): " << nonExistentTagToDelete << std::endl;
    } else {
        std::cerr << "Failed to delete non-existent tag (Expected failure): " << nonExistentTagToDelete << std::endl;
    }

    // 삭제 후 다시 태그 목록 조회하여 확인 (선택 사항)
    std::cout << "\n--- Verifying tags after deletion ---" << std::endl;
    std::vector<std::string> allTagsAfterDelete = tagManager.getAllTagNames(); // 삭제 후 남은 태그 전부 호출
    std::cout << "All existing tags after deletion attempt:" << std::endl;
    if (allTagsAfterDelete.empty()) {
        std::cout << "  No tags found." << std::endl;
    } else {
        for (const auto& tagName : allTagsAfterDelete) {
            std::cout << "  - " << tagName << std::endl;
        }
    }

    // --- **추가:** 파일-태그 연결 해제 테스트 (TagManager 사용) ---
    std::cout << "\n--- Unlinking Files and Tags (using TagManager) ---" << std::endl;
    // 연결 해제할 대상: C++_0_OT_2025.pdf 에서 "강의자료" 태그
    if (tagManager.unlinkFileFromTag("C:/FILES/2025_school/25-1/고급프로그래밍/C++_0_OT_2025.pdf", "강의자료")) {
        std::cout << "Successfully unlinked C++_0_OT_2025.pdf from 강의자료" << std::endl;
    } else {
        std::cerr << "Failed to unlink C++_0_OT_2025.pdf from 강의자료" << std::endl;
    }

    // 존재하지 않는 연결 해제 시도
    if (tagManager.unlinkFileFromTag("C:/FILES/2025_school/25-1/고급프로그래밍/씹어먹는c++.pdf", "존재하지않는태그")) {
        std::cout << "Successfully unlinked (Unexpected!): 씹어먹는c++.pdf from 존재하지않는태그" << std::endl;
    } else {
        std::cerr << "Failed to unlink (Expected failure): 씹어먹는c++.pdf from 존재하지않는태그" << std::endl;
    }

    // --- **추가:** 특정 파일의 태그 목록 조회 테스트 (TagManager 사용) ---
    std::cout << "\n--- Getting Tags for Specific Files (using TagManager) ---" << std::endl;
    std::string fileToQueryTags = "C:/FILES/2025_school/25-1/고급프로그래밍/C++_0_OT_2025.pdf";
    std::vector<std::string> tagsForSpecificFile = tagManager.getTagsForFile(fileToQueryTags);
    std::cout << "Tags for '" << fileToQueryTags << "':" << std::endl;
    if (tagsForSpecificFile.empty()) {
        std::cout << "  No tags found for this file." << std::endl;
    } else {
        for (const auto& tag : tagsForSpecificFile) {
            std::cout << "  - " << tag << std::endl;
        }
    }

    // 연결 해제된 파일의 태그 목록 다시 조회하여 확인
    std::cout << "\n--- Verifying tags after unlinking ---" << std::endl;
    std::vector<std::string> tagsForSpecificFileAfterUnlink = tagManager.getTagsForFile(fileToQueryTags);
    std::cout << "Tags for '" << fileToQueryTags << "' after unlinking '강의자료':" << std::endl;
    if (tagsForSpecificFileAfterUnlink.empty()) {
        std::cout << "  No tags found for this file." << std::endl;
    } else {
        for (const auto& tag : tagsForSpecificFileAfterUnlink) {
            std::cout << "  - " << tag << std::endl;
        }
    }

    return 0; // main함수 종료 시 dbManager 소멸자 호출 -> DB연결이 자동으로 닫힘
}

// TODO : 입력받아 파일 추가.. GUI 구현..