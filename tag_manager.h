#ifndef TAG_MANAGER_H
#define TAG_MANAGER_H

#include <string>
#include <vector>
#include <memory> // shared_ptr을 위함
#include "database_manager.h"

class TagManager {
public:
   // 생성자
   explicit TagManager(std::shared_ptr<DatabaseManager> database_manager);

   // 태그 추가
   bool addTag(const std::string& tagName);

   // 태그 삭제
   bool deleteTag(const std::string& tagName);

   // 모든 태그 가져오기 (for GUI)
   std::vector<std::string> getAllTagNames();

   // 파일에 태그 연결
   bool linkFileWithTag(const std::string& filePath, const std::string& tagName);

   // 파일에서 태그 연결 해제
   bool unlinkFileFromTag(const std::string& filePath, const std::string& tagName);

   // 특정 파일에 연결된 태그 목록 가져오기
   std::vector<std::string> getTagsForFile(const std::string& filePath);
private:
   std::shared_ptr<DatabaseManager> dbManager;
};



#endif //TAG_MANAGER_H
