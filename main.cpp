#include <QApplication>
#include "mainwindow.h"

#include "database_manager.h"
#include "file_manager.h"
#include "tag_manager.h"
#include <iostream> // 디버깅용

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 백엔드 매니저 객체들을 생성하고 공유 포인터로 묶기
    std::shared_ptr<DatabaseManager> dbManager = std::make_shared<DatabaseManager>("tag_database.db");

    if (!dbManager->isConnected()) {
        std::cerr << "Fatal error: could not connect to database server" << std::endl;
        return EXIT_FAILURE; // = return 1;
    }
    if (!dbManager->createTables()) {
        std::cerr << "Fatal error: could not create database tables" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Database connection and setup successful. Starting GUI..." << std::endl;

    std::shared_ptr<FileManager> fileManager = std::make_shared<FileManager>(dbManager);
    std::shared_ptr<TagManager> tagManager = std::make_shared<TagManager>(dbManager);

    // MainWindow 객체 생성 및 백엔드 매니저 전달
    MainWindow window(fileManager, tagManager);
    window.show(); // window 표시

    return app.exec(); // QT 이벤트 루프 시작
}