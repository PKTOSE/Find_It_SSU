#include "tag_manager.h"
#include <iostream>

// 생성자
TagManager::TagManager(std::shared_ptr<DatabaseManager> dbManager): dbManager(std::move(dbManager)) {
    if (!this->dbManager) {
        std::cerr << "TagManager: DatabaseManager shared ptr is null" << std::endl;
    }
}

// 태그 추가
bool TagManager::addTag(const std::string& tagName) {
    if (!dbManager || !dbManager->isConnected()) { // db 연결 실패
        std::cerr << "TagManager: Database is not connected" << std::endl;
        return false;
    }
    int tagId = dbManager->addTag(tagName);
    return tagId != -1;
}

// 태그 삭제
bool TagManager::deleteTag(const std::string& tagName) {
    if (!dbManager || !dbManager->isConnected()) {
        std::cerr << "TagManager: Database is not connected" << std::endl;
        return false;
    }

    int tagId = dbManager->getTagIdByName(tagName);
    if (tagId == -1) {
        std::cerr << "TagManager::deleteTag: Tag does not exist" << std::endl;
        return false;
    }

    bool deleteSuccess = dbManager->deleteTagByID(tagId);
    if (deleteSuccess) {
        std::cout << "Tag deleted successfully with tag: " << tagName << std::endl;
    }else {
        std::cerr << "TagManager::deleteTag: Tag does not exist" << tagName << std::endl;
    }
    return deleteSuccess;
}

// 모든 태그 이름 가져오기
std::vector<std::string> TagManager::getAllTagNames() {
    if (!dbManager || !dbManager->isConnected()) {
        std::cerr << "TagManager: Database is not connected" << std::endl;
        return {};
    }
    return dbManager->getAllTagNamesFromDb(); // dbManager의 함수 호출
}

// 파일에 태그 연결
bool TagManager::linkFileWithTag(const std::string& filePath, const std::string& tagName) {
    if (!dbManager || !dbManager->isConnected()) { // db 연결 실패
        std::cerr << "TagManager: Database is not connected" << std::endl;
        return false;
    }

    int fileID = dbManager->getFileIdByPath(filePath); // 파일 있으면 ID 반환, 없으면 -1 반환
      if (fileID == -1) {
          std::cerr << "TagManager::linkFileWithTag: File does not exist with " << filePath << std::endl;
          return false;
    }
    int tagID = dbManager->getTagIdByName(tagName); // 태그 있으면 ID 반환, 없으면 -1 반환
    if (tagID == -1) {
        std::cerr << "TagManager::linkFileWithTag: Tag does not exist with " << tagName << std::endl;
        return false;
    }

    bool linkResult = dbManager->linkFileTag(fileID, tagID);
    if (!linkResult) {
        std::cerr << "TagManager::linkFileWithTag: File link failed (fileID, tagID : " << fileID + " " + tagID << std::endl;
    }
    return linkResult;
}

// unlink
bool TagManager::unlinkFileFromTag(const std::string& filePath, const std::string& tagName) {
    if (!dbManager || !dbManager->isConnected()) {
        std::cerr << "TagManager: Database is not connected" << std::endl;
        return false;
    }

    int fileID = dbManager->getFileIdByPath(filePath);
    if (fileID == -1) {
        std::cerr << "TagManager::unlinkFileFromTag: File does not exist : " << filePath << std::endl;
        return false;
    }
    int tagID = dbManager->getTagIdByName(tagName);
    if (tagID == -1) {
        std::cerr << "TagManager::unlinkFileFromTag: Tag does not exist : " << tagName << std::endl;
        return false;
    }

    bool unlinkSuccess = dbManager->unLinkFileTagByIds(fileID, tagID);
    if (!unlinkSuccess) {
        std::cerr << "TagManager::unlinkFileFromTag: Failed to unlink " << filePath << " from " << tagName << std::endl;
    }
    return unlinkSuccess; // 실패 시 false 값 리턴
}

// 특정 파일에 연결된 태그 목록 가져오기
std::vector<std::string> TagManager::getTagsForFile(const std::string& filePath) {
    std::vector<std::string> tagsForFile;
    if (!dbManager || !dbManager->isConnected()) {
        std::cerr << "TagManager: Database is not connected" << std::endl;
        return tagsForFile;
    }

    int fileID = dbManager->getFileIdByPath(filePath);
    if (fileID == -1) {
        std::cerr << "TagManager::getTagsForFile: File does not exist : " << filePath << std::endl;
        return tagsForFile;
    }
    return dbManager->getTagsForFileId(fileID);
}