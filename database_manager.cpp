#include "database_manager.h"
#include <filesystem>
#include <set>

// 생성자
DatabaseManager::DatabaseManager(const std::string& dbPath): dbConnection(nullptr), databasePath(dbPath){
    // sqlite3_open 함수로 db파일을 연다
    // 파일이 없다면 자동으로 생성
    int rc = sqlite3_open(databasePath.c_str(), &dbConnection);

    if (rc != SQLITE_OK){ // 연결 실패 시 오류 출력
        std::cerr << "Can't open database: " << sqlite3_errmsg(dbConnection) << std::endl;
        // 연결이 제대로 안되었으면 dbConnection은 nullptr가 아닐 수 있으므로 닫아주는게 안전
        if (this->dbConnection != nullptr){
            sqlite3_close(this->dbConnection);
            this->dbConnection = nullptr; // nullptr로 확실히 지정
        }
        // TODO: 오류처리 필요 - 예외를 throw하거나 오류 상태를 기록
    }else {
        std::cout << "Database opened Successfully " << this->databasePath << std::endl;
        // **추가:** FOREIGN KEY 제약 조건 활성화
        // 이 PRAGMA는 데이터베이스 연결마다 한 번씩 실행
        rc = sqlite3_exec(this->dbConnection, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to enable FOREIGN KEY constraints: " << sqlite3_errmsg(this->dbConnection) << std::endl;
            // FOREIGN KEY 활성화 실패는 심각한 오류 -> 연결을 닫고 오류 처리
            sqlite3_close(this->dbConnection);
            this->dbConnection = nullptr;
        }
    }
}
// 소멸자
DatabaseManager::~DatabaseManager() {
    // dbConnection이 유효한 경우에만 닫는다
    if (this->dbConnection) {
        sqlite3_close(this->dbConnection);
        std::cout << "Database closed: " << this->databasePath << std::endl;
        this->dbConnection = nullptr;
    }
}
// db 테이블 생성
bool DatabaseManager::createTables() {
    if (!isConnected()) { // db 연결 실패
        std::cerr << "Database is not Connected!" << std::endl;
        return false;
    }

    // table 생성 SQL 쿼리 문자열
    // CREATE TABLE IF NOT EXISTS 구문을 사용하여 이미 테이블이 존재해도 오류 없이 진행
    const char* createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS Files (
            FileID INTEGER PRIMARY KEY AUTOINCREMENT,
            FilePath TEXT UNIQUE NOT NULL,
            FileName TEXT NOT NULL
            -- 나중에 필요하다면 파일 크기, 수정 시간 등 컬럼 추가
        );

        CREATE TABLE IF NOT EXISTS Tags (
            TagID INTEGER PRIMARY KEY AUTOINCREMENT,
            TagName TEXT UNIQUE NOT NULL
        );

        CREATE TABLE IF NOT EXISTS FileTags (
            FileID INTEGER NOT NULL,
            TagID INTEGER NOT NULL,
            PRIMARY KEY (FileID, TagID),
            FOREIGN KEY (FileID) REFERENCES Files(FileID) ON DELETE CASCADE,
            FOREIGN KEY (TagID) REFERENCES Tags(TagID) ON DELETE CASCADE
        );

        -- 검색 성능 향상을 위한 인덱스 추가 (선택 사항이지만 권장)
        CREATE INDEX IF NOT EXISTS idx_filetags_tagid ON FileTags (TagID);
        CREATE INDEX IF NOT EXISTS idx_files_filepath ON Files (FilePath);
    )"; // R"()" 문법은 여러 줄의 문자열을 쉽게 작성하게 해줌

    char* errMsg = nullptr; // 오류 메시지 저장할 포인터
    // sqlite3_exec 함수로 SQL 실행
    // 네 번째 인자 (nullptr)는 콜백 함수 포인터, 다섯 번째 인자 (nullptr)는 콜백 함수 인자 (여기서는 사용 안 함)
    int rc = sqlite3_exec(this->dbConnection, createTableSQL, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error when creating tables: " << errMsg << std::endl;
        sqlite3_free(errMsg); // sqlite3_exec에서 할당한 오류 메시지 메모리 해제
        return false;
    } else {
        std::cout << "Database tables checked/created successfully." << std::endl;
        return true;
    }
}

int DatabaseManager::addFile(const std::string& filePath) {
    if (!isConnected()) {
        std::cerr << "Database is not Connected!" << std::endl;
        return -1;
    }
    // 1. 파일 경로에서 파일 이름 추출
    std::filesystem::path p(filePath); // path 객체 생성
    std::string fileName = p.filename().string(); // path 객체에 있는 filename 함수..

    // SQLite Prepared Statements 객체
    sqlite3_stmt* insertStmt = nullptr;
    sqlite3_stmt* selectStmt = nullptr;
    int fileId = -1; // 결과로 얻을 FileID

    // 트랜잭션 시작 (선택 사항이지만 여러 쿼리를 묶을 때 유용)
    // int rc = sqlite3_exec(dbConnection, "BEGIN;", nullptr, nullptr, nullptr);
    // if (rc != SQLITE_OK) { /* 오류 처리 */ }

    // 2. INSERT OR IGNORE 쿼리 준비 및 실행
    // 파일 경로가 이미 존재하면 삽입을 무시합니다.
    const char* insertSQL = "INSERT OR IGNORE INTO Files (FilePath, FileName) VALUES (?, ?);";
    // Prepared Statement를 사용할 때, 실제 값은 SQL 문자열 안에 직접 넣는 대신 이렇게 ?로 표시해두고,
    // 나중에 실행하기 직전에 sqlite3_bind_* 계열 함수들을 사용하여 이 ? 위치에 실제 데이터를 연결하는 방식
    int rc = sqlite3_prepare_v2(this->dbConnection, insertSQL, -1, &insertStmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare insert statement: " << sqlite3_errmsg(this->dbConnection) << std::endl;
        // rc가 err일 경우, insertStmt가 제대로된 값을 가리키지 않기에 finalize로 메모리 해제는 위험할 수 있음
        // sqlite3_finalize(insertStmt); // 실패 시 stmt는 nullptr일 수 있음, sqlite3_prepare_v2에서 만든 객체를 제거하고, 메모리를 해제
        // if (/* 트랜잭션 시작했다면 */) sqlite3_exec(dbConnection, "ROLLBACK;", nullptr, nullptr, nullptr);
        // sqlite 트랜잭션을 BEGIN;으로 시작했다면 위 if문을 사용하여 오류 전까지 처리된 부분들을 "롤백"해주어야 함
        return -1;
    }

    // SQL 매개변수에 값 바인딩 (1부터 시작) <- 중요
    // SQLITE_API int sqlite3_bind_text(sqlite3_stmt*,int,const char*,int,void(*)(void*));
    // 3번째 인자 -1: c_str() c스타일 문자열은 -1을 넘겨주면 SQlite가 알아서 Null을 찾아 문자열의 길이를 계산한다
    sqlite3_bind_text(insertStmt, 1, filePath.c_str(), -1, SQLITE_TRANSIENT); // FilePath 바인딩, 첫번째 ? 에 할당
    sqlite3_bind_text(insertStmt, 2, fileName.c_str(), -1, SQLITE_TRANSIENT); // FileName 바인딩, 두번째 ? 에 할당

    // 쿼리 실행
    rc = sqlite3_step(insertStmt);
    if (rc != SQLITE_DONE) {
        // SQLITE_DONE은 INSERT, UPDATE, DELETE 성공 시 반환
        // 다른 값은 오류를 의미
        std::cerr << "Failed to execute insert statement: " << sqlite3_errmsg(this->dbConnection) << " (Code: " << rc << ")" << std::endl;
        sqlite3_finalize(insertStmt); // prepare_v2에서 만든 객체 제거
        // if (/* 트랜잭션 시작했다면 */) sqlite3_exec(dbConnection, "ROLLBACK;", nullptr, nullptr, nullptr);
        return -1;
    }

    // Prepared Statement 정리
    sqlite3_finalize(insertStmt); // 중요: 사용 후 반드시 정리

    // 3. 파일의 FileID 조회
    // INSERT OR IGNORE 되었든 원래 있었든 FilePath를 기준으로 FileID를 가져옵니다.
    const char* selectSQL = "SELECT FileID FROM Files WHERE FilePath = ?;";
    rc = sqlite3_prepare_v2(this->dbConnection, selectSQL, -1, &selectStmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare select statement: " << sqlite3_errmsg(this->dbConnection) << std::endl;
         // sqlite3_finalize(selectStmt); // 실패 시 stmt는 nullptr일 수 있음, rc가 실패한거면 쓰레기 값일 수 있어서 finalize는 위험
         // if (/* 트랜잭션 시작했다면 */) sqlite3_exec(dbConnection, "ROLLBACK;", nullptr, nullptr, nullptr);
        return -1;
    }

    // SQL 매개변수에 값 바인딩
    sqlite3_bind_text(selectStmt, 1, filePath.c_str(), -1, SQLITE_TRANSIENT); // FilePath 바인딩

    // 쿼리 실행 및 결과 가져오기
    rc = sqlite3_step(selectStmt);
    if (rc == SQLITE_ROW) {
        // 결과 행이 있으면 FileID 가져오기 (컬럼 인덱스 0부터 시작)
        fileId = sqlite3_column_int(selectStmt, 0);
        std::cout << "File found or added with FileID: " << fileId << std::endl;
    } else if (rc == SQLITE_DONE) {
        // 결과 행이 없음 (이 경우는 발생하면 안 됨 - INSERT OR IGNORE 했으므로)
        std::cerr << "Error: File not found after insert attempt." << std::endl;
         // if (/* 트랜잭션 시작했다면 */) sqlite3_exec(dbConnection, "ROLLBACK;", nullptr, nullptr, nullptr);
         fileId = -1; // 오류 상태로 설정
    } else {
        // 다른 오류 발생
        std::cerr << "Failed to execute select statement: " << sqlite3_errmsg(this->dbConnection) << " (Code: " << rc << ")" << std::endl;
         // if (/* 트랜잭션 시작했다면 */) sqlite3_exec(dbConnection, "ROLLBACK;", nullptr, nullptr, nullptr);
         fileId = -1; // 오류 상태로 설정
    }

    // Prepared Statement 정리
    sqlite3_finalize(selectStmt); // 중요: 사용 후 반드시 정리

    // 트랜잭션 커밋 (선택 사항) // BEGIN;으로 시작시...
    // if (/* 성공했다면 */ fileId != -1) {
    //     sqlite3_exec(dbConnection, "COMMIT;", nullptr, nullptr, nullptr);
    // } else {
    //     sqlite3_exec(dbConnection, "ROLLBACK;", nullptr, nullptr, nullptr);
    // }


    return fileId; // 얻은 FileID 반환 (오류 시 -1)
}

int DatabaseManager::addTag(const std::string& tagName) {
    if (!isConnected()) {
        std::cerr << "Database is not Connected!" << std::endl;
        return -1;
    }
    if (tagName.empty()) { // 태그가 비어있을 경우, string.empty() -> *this == "" 반환
        std::cerr << "Tag name cannot be empty!" << std::endl;
        return -1;
    }

    sqlite3_stmt* insertStmt = nullptr; // 넣을 경우
    sqlite3_stmt* selectStmt = nullptr; // 찾는 경우
    int tagId = -1; // 결과로 얻을 tagId

    // 1. INSERT OR IGNORE 쿼리, tagName이 존재하면 무시한다
    const char* insertSQL = "INSERT OR IGNORE INTO Tags (TagName) VALUES (?);";
    int rc = sqlite3_prepare_v2(this->dbConnection, insertSQL, -1, &insertStmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare insert tag statement : " << sqlite3_errmsg(this->dbConnection) << std::endl;
        return -1; // 객체 생성 실패.. finalize 하지 않음..
    }

    // sql 매개변수에 바인딩 (1부터 시작)
    sqlite3_bind_text(insertStmt, 1, tagName.c_str(), -1, SQLITE_TRANSIENT);
    // 쿼리 실행
    rc = sqlite3_step(insertStmt);
    if (rc != SQLITE_DONE) { // 실패 처리
        std::cerr << "Failed to execute insert tag statement: " << sqlite3_errmsg(this->dbConnection) << std::endl;
        sqlite3_finalize(insertStmt);
        return -1;
    }

    // Prepare Statement 정리, 메모리 해제
    sqlite3_finalize(insertStmt);

    // 2. tagId 조회
    const char* selectSQL = "SELECT TagID FROM Tags WHERE TagName = ?;";
    rc = sqlite3_prepare_v2(this->dbConnection, selectSQL, -1, &selectStmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare select tag statement: " << sqlite3_errmsg(this->dbConnection) << std::endl;
        return -1;
    }

    // sql 매개변수에 값 바인딩
    sqlite3_bind_text(selectStmt, 1, tagName.c_str(), -1, SQLITE_TRANSIENT);
    rc = sqlite3_step(selectStmt); // 쿼리 실행 + 결과 가져오기
    if (rc == SQLITE_ROW) {
        // 결과 행이 있으면 tagID 가져오기, index 0부터 시작
        tagId = sqlite3_column_int(selectStmt, 0);
        std::cout << "Tag found or added with TagID: " << tagId << std::endl;
    }else {
        std::cerr << "Tag not found after insert attempt or execution failed.. (code: " << rc << ")" <<  std::endl;
        tagId = -1;
    }
    // selectStmt에 대한 prepare 객체 메모리 해제..
    sqlite3_finalize(selectStmt);

    return tagId;
}

bool DatabaseManager::linkFileTag(int fileId, int tagId) {
    if (!isConnected()) {
        std::cerr << "Database is not Connected!" << std::endl;
        return false;
    }
    // fileId, tagId 범위 유효성 검사
    if (fileId < 0 || tagId < 0) { // 존재하는 지 판단은 아직 구현 X
        std::cerr << "FileId or tagId must be over 0(zero)." << std::endl;
        return false;
    }

    sqlite3_stmt* insertStmt = nullptr;
    const char* insertSQL = "INSERT OR IGNORE INTO FileTags (FileID, TagID) VALUES (?, ?);";
    int rc = sqlite3_prepare_v2(this->dbConnection, insertSQL, -2, &insertStmt, nullptr);
    if (rc != SQLITE_OK) { // INSERT / IGNORE 성공시 SQLITE_OK 반환
        std::cerr << "Failed to prepare link statement: " << sqlite3_errmsg(dbConnection) << std::endl;
        sqlite3_finalize(insertStmt);
        return false;
    }

    // SQL 매개변수에 값 바인딩 (정수)
    sqlite3_bind_int(insertStmt, 1, fileId);
    sqlite3_bind_int(insertStmt, 2, tagId);
    // 쿼리 실행
    rc = sqlite3_step(insertStmt);
    if (rc != SQLITE_DONE) {
        // SQLITE_DONE는 INSERT 성공 혹은 IGNORE 시 반환
        std::cerr << "Failed to execute link statement: " << sqlite3_errmsg(dbConnection) << " (Code: " << rc << ")" << std::endl;
        sqlite3_finalize(insertStmt);
        return false;
    }
    // insertStmt 정리
    sqlite3_finalize(insertStmt);

    // 성공적으로 삽입 또는 무시됨
    // 만약 실제로 삽입되었는지 확인하고 싶다면 sqlite3_changes() 함수를 사용할 수 있다.
    // int changes = sqlite3_changes(dbConnection);
    // if (changes > 0) { std::cout << "Link added: FileID " << fileId << " <-> TagID " << tagId << std::endl; }
    // else { std::cout << "Link already exists (ignored): FileID " << fileId << " <-> TagID " << tagId << std::endl; }

    return true; // 성공 (insert or ignore 됨)
}

std::vector<std::string> DatabaseManager::searchFileByTags(const std::vector<std::string>& tagNames) {
    std::vector<std::string> foundFilePaths;
    if (!isConnected()) {
        std::cerr << "Database is not Connected!" << std::endl;
        return foundFilePaths;
    }
    if (tagNames.empty()) { // 숏컷 - 태그 없으면 바로 종료
        std::cout << "No tags found!" << std::endl;
        return foundFilePaths;
    }

    // 태그에서 중복된걸 제거하고, 검색에 쓸 새로운 unique 태그 목록을 만든다
    std::set<std::string> uniqueTagNames(tagNames.begin(), tagNames.end());
    int enumUniqueTags = uniqueTagNames.size();

    // SQL 쿼리 문자열 동적 생성
    std::ostringstream sqlStream;
    sqlStream << "SELECT F.FilePath "
              << "From Files AS F "
              << "JOIN FileTags AS FT ON F.FileID = FT.FileID "
              << "JOIN Tags AS T ON FT.TagID = T.TagID "
              << "WHERE T.TagName IN (";

    // '?' 플레이스홀더를 태그 개수만큼 추가
    for (int i = 0; i < enumUniqueTags; ++i) { // 왜 ++i?
        sqlStream << "?";
        if (i < enumUniqueTags - 1) {
            sqlStream << ", ";
        }
    }
    sqlStream << ") "
            << "GROUP BY F.FileID, F.FilePath "
            << "HAVING COUNT(DISTINCT T.TagID) = ?;"; // 마지막 ? 는 고유 태그의 개수

    std::string sqlQuery = sqlStream.str();
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(this->dbConnection, sqlQuery.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare search statement: " << sqlite3_errmsg(dbConnection) << std::endl;
        return foundFilePaths;
    }

    // 플레이스홀더에 값 바인딩
    int bindIdx = 1;
    for (const auto& tagName : uniqueTagNames) {
        sqlite3_bind_text(stmt, bindIdx++, tagName.c_str(), -1, SQLITE_TRANSIENT); // bindIdx 는 한 루프마다 ++ 해준다.
    }
    sqlite3_bind_int(stmt, bindIdx, enumUniqueTags); // 마지막 플레이스홀더(?)에 고유 태그 개수 바인딩

    // 쿼리 실행
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const unsigned char* filePath = sqlite3_column_text(stmt, 0);
        if (filePath) {
            foundFilePaths.push_back(reinterpret_cast<const char*>(filePath));
        }
    }

    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to execute search statement: " << sqlite3_errmsg(dbConnection) << std::endl;
        foundFilePaths.clear(); // 오류 시 결과 초기화
    }

    sqlite3_finalize(stmt); // stmt 종료
    return foundFilePaths;

}