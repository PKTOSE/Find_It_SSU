#include "mainwindow.h"
#include "ui_mainwindow.h" // Qt UI 파일을 컴파일한 헤더 (자동 생성)
#include <QMessageBox> // 사용자에게 메시지 표시용
#include <QFileDialog> // 파일 열기/저장 대화상자용
#include <QDebug> // 디버깅 출력용 (qDebug() 사용)
#include <QTableWidgetItem> // QTableWidget 아이템용
#include <QListWidgetItem> // QListWidget 아이템용
#include <QDir>
#include <QFileInfo>
#include <set>
#include <algorithm>
#include <QDesktopServices> // 파일을 열기 위함 (더블클릭 기능 구현)
#include <Qurl> // 파일 경로를 URL로 변환하기 위함
#include <QMimeData>
#include <QFontDatabase> // 폰트 로드
#include <QApplication> // QApplication::setFont()

MainWindow::MainWindow(std::shared_ptr<FileManager> fileManager,
                       std::shared_ptr<TagManager> tagManager,
                       QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow), // UI initializing
      fileManager(std::move(fileManager)),
      tagManager(std::move(tagManager))
{

    // --- 두 개의 폰트 로드 ---
    QString notoFontFamilyName;  // "notoSansKR"의 실제 패밀리 이름
    QString ganghanFontFamilyName; // "강한공군체 Medium"의 실제 패밀리 이름

    // 첫 번째 폰트 로드 (나눔스퀘어라운드OTFR.otf)
    int fontId_noto = QFontDatabase::addApplicationFont(":/fonts/NotoSansKR-Regular.ttf");
    if (fontId_noto != -1) {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId_noto);
        if (!fontFamilies.isEmpty()) {
            notoFontFamilyName = fontFamilies.at(0); // 로드된 폰트의 실제 패밀리 이름
            qDebug() << "Custom font 'NotoSansKR-Regular.ttf' loaded. Detected family: " << notoFontFamilyName;
        } else {
            qWarning() << "Custom font 'NotoSansKR-Regular.ttf' loaded, but no font families found.";
        }
    } else {
        qWarning() << "Failed to load custom font from ':/fonts/NotoSansKR-Regular.ttf'. Error ID: " << fontId_noto;
    }

    // 두 번째 폰트 로드 (ROKAFSlabSerifMedium.otf)
    int fontId_ganghan = QFontDatabase::addApplicationFont(":/fonts/ROKAFSlabSerifMedium.otf");
    if (fontId_ganghan != -1) {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId_ganghan);
        if (!fontFamilies.isEmpty()) {
            ganghanFontFamilyName = fontFamilies.at(0); // 로드된 폰트의 실제 패밀리 이름
            qDebug() << "Custom font 'ROKAFSlabSerifMedium.otf' loaded. Detected family: " << ganghanFontFamilyName;
        } else {
            qWarning() << "Custom font 'ROKAFSlabSerifMedium.otf' loaded, but no font families found.";
        }
    } else {
        qWarning() << "Failed to load custom font from ':/fonts/ROKAFSlabSerifMedium.otf'. Error ID: " << fontId_ganghan;
    }

    if (!notoFontFamilyName.isEmpty()) {
        QFont appFont(notoFontFamilyName); // 실제 패밀리 이름 사용
        appFont.setPointSize(11); // 원하는 기본 크기 설정
        QApplication::setFont(appFont); // 전체 애플리케이션에 적용
        qDebug() << "Applied primary font to application: " << notoFontFamilyName;
    } else {
        qWarning() << "Primary font not loaded, using default system font for application.";
    }

    std::cout << notoFontFamilyName.toStdString().c_str() << std::endl;
    std::cout << ganghanFontFamilyName.toStdString().c_str() << std::endl;

    ui->setupUi(this);
    setWindowTitle("Find it SSU");

    if (!ganghanFontFamilyName.isEmpty()) {
        QFont specificWidgetFont(ganghanFontFamilyName); // 실제 패밀리 이름 사용
        specificWidgetFont.setPointSize(11); // 원하는 크기 설정
        // 예시: ui->searchLineEdit->setFont(specificWidgetFont);
        // 예시: ui->someLabel->setFont(specificWidgetFont);
        ui->searchButton->setFont(specificWidgetFont);
        ui->addFileButton->setFont(specificWidgetFont);
        ui->deleteFileButton->setFont(specificWidgetFont);
        ui->addTagButton->setFont(specificWidgetFont);
        ui->deleteTagButton->setFont(specificWidgetFont);
        ui->applyTagsButton->setFont(specificWidgetFont);
        ui->applyTagFilterButton->setFont(specificWidgetFont);
        ui->showAllFilesButton->setFont(specificWidgetFont);
        qDebug() << "Applied secondary font to specific widgets (if applicable): " << ganghanFontFamilyName;
    }


    // --- UI 초기 설정 ---
    // 파일 테이블 위젯 초기 설정 (예: 컬럼 헤더)
    ui->fileTableWidget->setColumnCount(3);
    ui->fileTableWidget->setHorizontalHeaderLabels({"이름", "경로", "태그"});
    ui->fileTableWidget->horizontalHeader()->setStretchLastSection(true); // 마지막 컬럼 자동 늘리기
    ui->fileTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); // 행 단위 선택
    ui->fileTableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection); // 여러 행 선택 가능
    ui->fileTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); // 셀 직접 편집 비활성화

    QList<int> splitterSizes;
    splitterSizes << 600 << 300;
    ui->mainSplitter->setSizes(splitterSizes);

    ui->mainSplitter->setChildrenCollapsible(false);

    // drag&drop 설정
    setAcceptDrops(true); // MainWindow가 드롭 이벤트를 받아들이도록 설정

    // // --- 시그널-슬롯 연결 ---
    // // QLineEdit (searchLineEdit)에서 Enter 키 눌렀을 때
    // connect(ui->searchLineEdit, &QLineEdit::returnPressed, this, &MainWindow::on_searchLineEdit_returnPressed);
    // // 검색 버튼 클릭 시
    // connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::on_searchButton_clicked);
    // // 태그 추가 버튼 클릭 시
    // connect(ui->addTagButton, &QPushButton::clicked, this, &MainWindow::on_addTagButton_clicked);
    // // 파일 추가 버튼 클릭 시
    // connect(ui->addFileButton, &QPushButton::clicked, this, &MainWindow::on_addFileButton_clicked);
    // // 파일 삭제 버튼 클릭 시
    // connect(ui->deleteFileButton, &QPushButton::clicked, this, &MainWindow::on_deleteFileButton_clicked);
    // // 태그 삭제 버튼 클릭 시
    // connect(ui->deleteTagButton, &QPushButton::clicked, this, &MainWindow::on_deleteTagButton_clicked);
    // // 선택된 태그 적용 버튼 클릭 시
    // connect(ui->applyTagsButton, &QPushButton::clicked, this, &MainWindow::on_applyTagsButton_clicked);
    // // 태그 목록 선택 변경 시 (자동 검색 필터링)
    // connect(ui->tagListWidget, &QListWidget::itemSelectionChanged, this, &MainWindow::on_tagListWidget_itemSelectionChanged);

    // --- 초기 데이터 로드 ---
    refreshTagList(); // 시작 시 태그 목록 로드
    // refreshFileList(fileManager->getAllFilePaths()); // 시작 시 모든 파일 로드
    on_searchLineEdit_returnPressed();
    updateStatusBar("Application started, loading all files. . .");
}

MainWindow::~MainWindow() {
    delete ui; // UI 객체 메모리 해제
}
// --- 슬롯 함수 구현 ---

// 검색 버튼 클릭 또는 Enter 키 입력 시
void MainWindow::on_searchButton_clicked() {
    on_searchLineEdit_returnPressed(); // 검색 버튼과 Enter 키 동작을 동일하게 처리
}

void MainWindow::on_searchLineEdit_returnPressed() {
    QString searchText = ui->searchLineEdit->text();
    std::vector<std::string> searchTags;
    // 태그 리스트에서 선택된 태그들을 검색 조건에 추가
    for (QListWidgetItem* item : ui->tagListWidget->selectedItems()) {
        searchTags.push_back(item->text().toStdString());
    }

    std::vector<std::string> filePaths;
    if (searchText.isEmpty() && searchTags.empty()) {
        filePaths = fileManager->getAllFilePaths();
        updateStatusBar("모든 파일 표시 중.");
    } else if (!searchText.isEmpty() && !searchTags.empty()) {
        // 파일 이름 및 태그 동시 검색 (더 복잡한 로직 필요)
        // 현재는 태그 검색만 사용하거나, 파일 이름 검색을 따로 호출.
        // 예시: 태그로 먼저 필터링하고, 그 결과에서 파일 이름으로 다시 필터링 (간단화)
        std::vector<std::string> filesByTags = fileManager->searchFilesByTags(searchTags);
        std::vector<std::string> filesByName = fileManager->searchFilesByName(searchText.toStdString());

        // 두 결과를 교집합하여 최종 결과 생성 (AND 조건)
        // 더 효율적인 방법은 DatabaseManager에서 하나의 SQL 쿼리로 처리하는 것
        std::set<std::string> intersectionResult;
        for (const auto& path : filesByTags) {
            if (std::find(filesByName.begin(), filesByName.end(), path) != filesByName.end()) {
                intersectionResult.insert(path);
            }
        }
        filePaths.assign(intersectionResult.begin(), intersectionResult.end());
        updateStatusBar("태그 및 이름 동시 검색 완료: " + QString::number(filePaths.size()) + "개 파일");
    } else if (!searchText.isEmpty()) {
        // 파일 이름으로만 검색
        filePaths = fileManager->searchFilesByName(searchText.toStdString());
        updateStatusBar("이름 검색 완료: " + QString::number(filePaths.size()) + "개");
    } else { // only tags selected
        // 태그로만 검색
        filePaths = fileManager->searchFilesByTags(searchTags);
        updateStatusBar("태그 검색 완료: " + QString::number(filePaths.size()) + "개");
    }

    refreshFileList(filePaths);
}

// 태그 목록 선택 변경 시 (자동 검색 필터링)
void MainWindow::on_tagListWidget_itemSelectionChanged() {
    // on_searchLineEdit_returnPressed(); // 태그 선택 변경 시 자동 검색
}

// 파일 추가 버튼 클릭 시
void MainWindow::on_addFileButton_clicked() {
    // QFileDialog를 사용하여 파일 선택 대화상자 열기
    QStringList files = QFileDialog::getOpenFileNames(this, "파일 추가", QDir::homePath(), "All Files (*.*)");
    if (!files.isEmpty()) {
        for (const QString& filePath : files) {
            if (fileManager->addFile(filePath.toStdString())) {
                qDebug() << "File added:" << filePath;
                updateStatusBar("파일 추가됨: " + filePath);
            } else {
                QMessageBox::warning(this, "파일 추가 오류", "파일을 추가할 수 없습니다: " + filePath);
                qDebug() << "Failed to add file:" << filePath;
            }
        }
        // 파일 목록 갱신 (현재 검색 조건으로)
        on_searchLineEdit_returnPressed();
    }
}

// 파일 삭제 버튼 클릭 시
void MainWindow::on_deleteFileButton_clicked() {
    std::vector<std::string> selectedFilePaths = getSelectedFilePaths();
    if (selectedFilePaths.empty()) {
        QMessageBox::information(this, "파일 삭제", "삭제할 파일을 선택해주세요.");
        return;
    }

    if (QMessageBox::question(this, "파일 삭제 확인", "선택한 " + QString::number(selectedFilePaths.size()) + "개 파일을 삭제하시겠습니까?\n(데이터베이스에서만 삭제되며 실제 파일은 삭제되지 않습니다.)", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        int deletedCount = 0;
        for (const std::string& filePath : selectedFilePaths) {
            if (fileManager->deleteFile(filePath)) {
                deletedCount++;
                qDebug() << "File deleted:" << QString::fromStdString(filePath);
            } else {
                QMessageBox::warning(this, "파일 삭제 오류", "파일을 삭제할 수 없습니다: " + QString::fromStdString(filePath));
                qDebug() << "Failed to delete file:" << QString::fromStdString(filePath);
            }
        }
        updateStatusBar(QString::number(deletedCount) + "개 파일이 삭제되었습니다.");
        // 파일 목록 갱신
        on_searchLineEdit_returnPressed(); // 현재 검색 조건으로 갱신
    }
}

void MainWindow::on_fileTableWidget_cellDoubleClicked(int row, int column) {
    Q_UNUSED(column); // column 인자는 사용하지 않으므로 경고를 피하기 위해 추가

    // 더블클릭된 행의 파일 경로를 가져옴 (경로 컬럼은 1번 컬럼)
    QTableWidgetItem* pathItem = ui->fileTableWidget->item(row, 1); // 1번 컬럼이 파일 경로라고 가정
    if (pathItem) {
        QString filePath = pathItem->text();
        // QDesktopServices::openUrl을 사용하여 파일을 엽니다.
        // 이는 시스템의 기본 애플리케이션(예: PDF 뷰어, 워드 프로세서)으로 파일을 엽니다.
        if (QDesktopServices::openUrl(QUrl::fromLocalFile(filePath))) {
            updateStatusBar("파일 열기 성공: " + filePath);
            qDebug() << "Successfully opened file: " << filePath;
        } else {
            updateStatusBar("파일 열기 실패: " + filePath);
            QMessageBox::warning(this, "파일 열기 오류", "파일을 열 수 없습니다: " + filePath);
            qDebug() << "Failed to open file: " << filePath;
        }
    }
}

// 태그 추가 버튼 클릭 시
void MainWindow::on_addTagButton_clicked() {
    // Qt Designer에서 tagLineEdit라는 이름의 QLineEdit를 추가해야 합니다.
    QString tagName = ui->tagLineEdit->text();
    if (tagName.isEmpty()) {
        QMessageBox::warning(this, "태그 추가 오류", "태그 이름을 입력해주세요.");
        return;
    }

    if (tagManager->addTag(tagName.toStdString())) {
        qDebug() << "Tag added:" << tagName;
        updateStatusBar("태그 추가됨: " + tagName);
        refreshTagList(); // 태그 목록 갱신
        clearInputFields(); // 입력 필드 초기화 (tagLineEdit 포함)
    } else {
        QMessageBox::warning(this, "태그 추가 오류", "태그를 추가할 수 없습니다: " + tagName);
        qDebug() << "Failed to add tag:" << tagName;
    }
}

void MainWindow::on_tagLineEdit_returnPressed() {
    on_addTagButton_clicked(); // 태그 추가 버튼 눌린 것과 같은 효과
}

// 태그 삭제 버튼 클릭 시
void MainWindow::on_deleteTagButton_clicked() {
    QList<QListWidgetItem*> selectedItems = ui->tagListWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::information(this, "태그 삭제", "삭제할 태그를 선택해주세요.");
        return;
    }

    QStringList tagNamesToDelete;
    for (QListWidgetItem* item : selectedItems) {
        tagNamesToDelete.append(item->text());
    }

    if (QMessageBox::question(this, "태그 삭제 확인", "선택한 " + QString::number(tagNamesToDelete.size()) + "개 태그를 삭제하시겠습니까?\n(파일에 연결된 태그 정보도 함께 삭제됩니다.)", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        int deletedCount = 0;
        for (const QString& tagName : tagNamesToDelete) {
            if (tagManager->deleteTag(tagName.toStdString())) { // tagManager::deleteTag 구현 필요
                deletedCount++;
                qDebug() << "Tag deleted:" << tagName;
            } else {
                QMessageBox::warning(this, "태그 삭제 오류", "태그를 삭제할 수 없습니다: " + tagName);
                qDebug() << "Failed to delete tag:" << tagName;
            }
        }
        updateStatusBar(QString::number(deletedCount) + "개 태그가 삭제되었습니다.");
        refreshTagList(); // 태그 목록 갱신
        on_searchLineEdit_returnPressed(); // 태그가 삭제되었으므로 파일 검색 결과도 갱신
    }
}


// 선택된 파일에 선택된 태그 적용
void MainWindow::on_applyTagsButton_clicked() {
    std::vector<std::string> selectedFilePaths = getSelectedFilePaths();
    std::vector<std::string> selectedTagNames = getSelectedTagNames();

    if (selectedFilePaths.empty()) {
        QMessageBox::information(this, "태그 적용", "태그를 적용할 파일을 먼저 선택해주세요.");
        return;
    }
    if (selectedTagNames.empty()) {
        QMessageBox::information(this, "태그 적용", "적용할 태그를 먼저 선택해주세요.");
        return;
    }

    int linkedCount = 0;
    for (const std::string& filePath : selectedFilePaths) {
        for (const std::string& tagName : selectedTagNames) {
            if (tagManager->linkFileWithTag(filePath, tagName)) {
                linkedCount++;
                qDebug() << "Linked: " << QString::fromStdString(filePath) << " with " << QString::fromStdString(tagName);
            } else {
                qDebug() << "Failed to link: " << QString::fromStdString(filePath) << " with " << QString::fromStdString(tagName);
            }
        }
    }
    updateStatusBar(QString::number(linkedCount) + "개 태그 연결 작업 완료.");
    // 파일 목록 다시 로드하여 태그 정보 갱신
    on_searchLineEdit_returnPressed(); // 태그 적용 시, 파일 목록 리로드
    refreshTagList(); // 태그 목록 갱신
}

// 파일 테이블 위젯 선택 변경 시
void MainWindow::on_fileTableWidget_itemSelectionChanged() {
    // 파일 목록에서 선택이 변경될 때 수행할 작업
    // 예: 선택된 파일의 태그를 태그 목록(tagListWidget)에 표시하거나, 상태바 업데이트 등
    std::vector<std::string> selectedPaths = getSelectedFilePaths();
    if (!selectedPaths.empty()) {
        // 첫 번째 선택된 파일의 태그를 가져와서 표시 (단일 선택 모드라면)
        // 다중 선택 모드이므로, 모든 선택된 파일의 태그들을 합치거나, UI 디자인에 따라 다르게 처리
        std::vector<std::string> tagsOfFirstSelectedFile = tagManager->getTagsForFile(selectedPaths[0]);
        QString tagsString;
        for (const std::string& tag : tagsOfFirstSelectedFile) {
            tagsString += QString::fromStdString(tag) + ", ";
        }
        if (!tagsString.isEmpty()) {
            tagsString.chop(2); // 마지막 ", " 제거
        }
        updateStatusBar("선택된 파일: " + QString::fromStdString(selectedPaths[0]) + " | 태그: " + tagsString);

        // 태그 목록에서 선택된 파일의 태그를 미리 선택해주는 기능 (선택 사항)
        ui->tagListWidget->clearSelection(); // 기존 선택 해제
        for (int i = 0; i < ui->tagListWidget->count(); ++i) {
            QListWidgetItem* item = ui->tagListWidget->item(i);
            if (std::find(tagsOfFirstSelectedFile.begin(), tagsOfFirstSelectedFile.end(), item->text().toStdString()) != tagsOfFirstSelectedFile.end()) {
                item->setSelected(true);
            }
        }

    } else {
        updateStatusBar("선택된 파일 없음.");
        ui->tagListWidget->clearSelection(); // 파일 선택 해제 시 태그 목록 선택도 해제
    }
}



// UI 상태 업데이트 헬퍼 함수 구현

// 파일 목록 갱신
void MainWindow::refreshFileList(const std::vector<std::string>& filePaths) {
    ui->fileTableWidget->setRowCount(0); // 기존 목록 초기화
    for (const std::string& filePath : filePaths) {
        int row = ui->fileTableWidget->rowCount();
        ui->fileTableWidget->insertRow(row);

        // 파일 이름 추출
        QFileInfo fileInfo(QString::fromStdString(filePath));
        QString fileName = fileInfo.fileName();

        // 컬럼 0: 파일 이름
        ui->fileTableWidget->setItem(row, 0, new QTableWidgetItem(fileName));
        // 컬럼 1: 파일 경로
        ui->fileTableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(filePath)));

        // 컬럼 2: 연결된 태그들 표시 (tagManager::getTagsForFile 필요)
        std::vector<std::string> tags = tagManager->getTagsForFile(filePath); // tagManager::getTagsForFile 구현 필요
        QString tagsString;
        for (const std::string& tag : tags) {
            tagsString += QString::fromStdString(tag) + ", ";
        }
        if (!tagsString.isEmpty()) {
            tagsString.chop(2); // 마지막 ", " 제거
        }
        ui->fileTableWidget->setItem(row, 2, new QTableWidgetItem(tagsString));
    }
}

// 태그 목록 갱신
void MainWindow::refreshTagList() {
    ui->tagListWidget->clear(); // 기존 목록 초기화
    std::vector<std::string> allTags = tagManager->getAllTagNames();
    for (const std::string& tagName : allTags) {
        ui->tagListWidget->addItem(QString::fromStdString(tagName));
    }
}

// 상태 표시줄 업데이트
void MainWindow::updateStatusBar(const QString& message) {
    // ui->statusbar가 Qt Designer에 추가되어 있어야 함
    if (ui->statusbar) {
        ui->statusbar->showMessage(message, 3000); // 3초간 메시지 표시
    } else {
        qWarning() << "StatusBar not found in UI. Please add it in Qt Designer.";
    }
}

// 입력 필드 초기화
void MainWindow::clearInputFields() {
    ui->searchLineEdit->clear();
    // 태그 입력 필드가 있다면 초기화 (Qt Designer에 tagLineEdit 추가 가정)
    if (ui->tagLineEdit) {
        ui->tagLineEdit->clear();
    } else {
        qWarning() << "tagLineEdit not found in UI. Cannot clear.";
    }
}

// 선택된 파일의 경로를 가져오는 헬퍼 함수
std::vector<std::string> MainWindow::getSelectedFilePaths() const {
    std::vector<std::string> selectedPaths;
    QList<QTableWidgetItem*> selectedItems = ui->fileTableWidget->selectedItems();
    std::set<std::string> uniquePaths; // 중복 경로 방지
    for (QTableWidgetItem* item : selectedItems) {
        if (item->column() == 1) { // 경로 컬럼 (1번 컬럼)
            uniquePaths.insert(item->text().toStdString());
        }
    }
    selectedPaths.assign(uniquePaths.begin(), uniquePaths.end());
    return selectedPaths;
}

// 선택된 태그의 이름을 가져오는 헬퍼 함수
std::vector<std::string> MainWindow::getSelectedTagNames() const {
    std::vector<std::string> selectedTagNames;
    for (QListWidgetItem* item : ui->tagListWidget->selectedItems()) {
        selectedTagNames.push_back(item->text().toStdString());
    }
    return selectedTagNames;
}

// 태그 필터 적용 버튼 클릭시
void MainWindow::on_applyTagFilterButton_clicked() {
    // 태그 목록에서 선택된 태그들을 검색 조건으로 사용
    std::vector<std::string> searchTags = getSelectedTagNames();

    // 검색 입력란에 텍스트가 있다면, 태그 필터와 파일 이름 검색을 함께 처리
    QString searchText = ui->searchLineEdit->text();
    std::vector<std::string> filePaths;

    if (!searchTags.empty() && !searchText.isEmpty()) {
        // 태그와 파일 이름으로 동시 검색 (기존 on_searchLineEdit_returnPressed 로직 재활용)
        std::vector<std::string> filesByTags = fileManager->searchFilesByTags(searchTags);
        std::vector<std::string> filesByName = fileManager->searchFilesByName(searchText.toStdString());

        std::set<std::string> intersectionResult;
        for (const auto& path : filesByTags) {
            if (std::find(filesByName.begin(), filesByName.end(), path) != filesByName.end()) {
                intersectionResult.insert(path);
            }
        }
        filePaths.assign(intersectionResult.begin(), intersectionResult.end());
        updateStatusBar("태그 및 이름 동시 검색 완료: " + QString::number(filePaths.size()) + "개 파일");

    } else if (!searchTags.empty()) {
        // 태그로만 검색
        filePaths = fileManager->searchFilesByTags(searchTags);
        updateStatusBar("선택된 태그로 검색 완료: " + QString::number(filePaths.size()) + "개 파일");
    } else {
        // 아무 태그도 선택되지 않았다면 모든 파일 표시
        filePaths = fileManager->getAllFilePaths();
        updateStatusBar("태그 필터 해제. 모든 파일 표시 중.");
    }

    refreshFileList(filePaths);
}

void MainWindow::on_showAllFilesButton_clicked() {
    refreshTagList();
    on_searchLineEdit_returnPressed();
}

// 드래그 진입 이벤트 처리
void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    // 드롭될 데이터가 파일 경로를 포함하고 있는지 확인
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction(); // 드롭 이벤트를 받아들임 (마우스 커서 변경)
    } else {
        event->ignore(); // 받아들이지 않음
    }
}

// 드롭 이벤트 처리
void MainWindow::dropEvent(QDropEvent *event) {
    QStringList addedFiles;
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> urls = event->mimeData()->urls(); // 드롭된 URL 목록 가져오기
        for (const QUrl &url : urls) {
            // 로컬 파일 경로인지 확인하고, 있다면 std::string으로 변환하여 addFile 호출
            if (url.isLocalFile()) {
                QString filePath = url.toLocalFile();
                if (fileManager->addFile(filePath.toStdString())) {
                    addedFiles.append(filePath);
                    qDebug() << "File added via drag & drop:" << filePath;
                } else {
                    QMessageBox::warning(this, "파일 추가 오류", "파일을 추가할 수 없습니다: " + filePath);
                    qDebug() << "Failed to add file via drag & drop:" << filePath;
                }
            }
        }
        if (!addedFiles.isEmpty()) {
            updateStatusBar(QString::number(addedFiles.size()) + "개 파일이 드래그 앤 드롭으로 추가됨.");
            on_searchLineEdit_returnPressed(); // 파일 목록 갱신
        }
        event->acceptProposedAction(); // 이벤트 처리 완료
    } else {
        event->ignore(); // 받아들이지 않음
    }
}