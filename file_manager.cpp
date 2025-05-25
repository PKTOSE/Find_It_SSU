#include "file_manager.h"
#include <iostream>
#include <filesystem> // 파일 이름 추출 등을 위함

// 생성자
FileManager::FileManager(std::shared_ptr<DatabaseManager> dbManager) : dbManager(std::move(dbManager)) { // shared_ptr 이동 시맨틱 사용
    if (!this->dbManager) {
        std::cerr << "FileManager: DatabaseManager share_ptr is null" << std::endl;
        // 예외처리..
    }
}
// 파일 추가
bool FileManager::addFile(const std::string& filePath) {
    if (!dbManager || !dbManager->isConnected()) { // db연결 실패시
        std::cerr << "FileManager: Database is not connected" << std::endl;
        return false;
    }
    int fileID = dbManager->addFile(filePath);
    return fileID != -1;
}

// 태그 기반 검색
std::vector<std::string> FileManager::searchFilesByTags(const std::vector<std::string>& tagNames) {
    if (!dbManager || !dbManager->isConnected()) { // db 연결 실패시
        std::cerr << "FileManager: Database is not connected" << std::endl;
        return {}; // 빈 벡터
    }
    return dbManager->searchFileByTags(tagNames);
}

/* TODO: 파일 경로 업데이트
 * 현재 filePath가 Unique 라서 직접 업데이트에 어려움이 있음..
 * 기존 FileID를 찾고, 해당 filePath를 업데이트 하는 방식이 필요. 그러나 SQLite의 Unique 제약조건 때문에 업데이트가 복잡..
 * 기존 파일을 삭제하고 add로 추가하면 태그 연결 정보등이 유지가 안됨. --> dbManager를 수정이나 필요한 부분 새로 작성 필요함 */
bool FileManager::updateFile(const std::string& oldFilePath, const std::string& newFilePath) {
    std::cerr << "updateFile: Not implemented feature..." << std::endl;
    return false;
}

// 파일 삭제 (FileID를 얻어와  DB에서 삭제)
bool FileManager::deleteFile(const std::string& filePath) {
    if (!dbManager || !dbManager->isConnected()) { // 오류 처리
        std::cerr << "FileManager: Database is not connected" << std::endl;
        return false;
    }

    int fileId = dbManager->getFileIdByPath(filePath);

    if (fileId == -1) {
        std::cerr << "FileManager::deleteFile: File does not exist ( path: " << filePath << " )" << std::endl;
    }

    bool deleteSuccess = dbManager->deleteFileByID(fileId);
    if (deleteSuccess) {
        std::cout << "Successfully deleted file : " << filePath << std::endl;
    }else {
        std::cerr << "FileManager::deleteFile: Failed to delete file : " << filePath << std::endl;
    }
    return deleteSuccess;
}

// 모든 파일 경로 가져오기
std::vector<std::string> FileManager::getAllFilePaths() {
    if (!dbManager || !dbManager->isConnected()) {
        std::cerr << "FileManager::getAllFilePaths: Database is not connected" << std::endl;
        return {};
    }
    // real db jackup do it
    std::cout << "[DEBUG] FileManager::getAllFilePaths: Calling DatabaseManager::getAllFilePathsFromDb()." << std::endl; // 디버그 6
    std::vector<std::string> paths = dbManager->getAllFilePathsFromDb();
    std::cout << "[DEBUG] FileManager::getAllFilePaths: Received " << paths.size() << " paths. Returning." << std::endl; // 디버그 7
    return paths;
}

std::vector<std::string> FileManager::searchFilesByName(const std::string& keyword) {
    std::vector<std::string> foundFilePaths;
    if (!dbManager || !dbManager->isConnected()) {
        std::cerr << "FileManager: Database not connected. Cannot search files by name." << std::endl;
        return foundFilePaths;
    }

    sqlite3_stmt* stmt = nullptr;
    const char* selectSQL = "SELECT FilePath FROM Files WHERE FileName LIKE ?;";

    int rc = sqlite3_prepare_v2(dbManager->getDBConnection(), selectSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare search by name statement: " << sqlite3_errmsg(dbManager->getDBConnection()) << std::endl;
        return foundFilePaths;
    }

    std::string searchPattern = "%" + keyword + "%"; // LIKE 검색을 위한 와일드카드 추가
    sqlite3_bind_text(stmt, 1, searchPattern.c_str(), -1, SQLITE_TRANSIENT);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const unsigned char* filePath = sqlite3_column_text(stmt, 0);
        if (filePath) {
            foundFilePaths.push_back(reinterpret_cast<const char*>(filePath));
        }
    }

    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to execute search by name statement: " << sqlite3_errmsg(dbManager->getDBConnection()) << " (Code: " << rc << ")" << std::endl;
        foundFilePaths.clear();
    }

    sqlite3_finalize(stmt);

    return foundFilePaths;
}
