#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <memory>
#include <vector>
#include "file_manager.h"
#include "tag_manager.h"
#include <QDragEnterEvent>
#include <QDropEvent>

// Qt Designer에서 생성된 UI 클래스 Forward Declaration
namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT // QT의 시그널/슬롯 메커니즘 사용하기 위함
public:
    explicit MainWindow(std::shared_ptr<FileManager> fileManager,
                        std::shared_ptr<TagManager> tagManager,
                        QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    // 검색
    void on_searchButton_clicked();
    void on_searchLineEdit_returnPressed(); // 검색 필드에서 Enter키 입력 시
    void on_tagListWidget_itemSelectionChanged(); // 태그 목록에서 선택 변경 시

    // 파일 관리
    void on_addFileButton_clicked();
    void on_deleteFileButton_clicked();
    void on_fileTableWidget_itemSelectionChanged(); // 파일 목록에서 선택 변경 시
    void on_fileTableWidget_cellDoubleClicked(int row, int column);

    // 태그 관리 관련
    void on_addTagButton_clicked();
    void on_tagLineEdit_returnPressed();
    void on_deleteTagButton_clicked();
    void on_applyTagsButton_clicked();

    void on_applyTagFilterButton_clicked();
    void on_showAllFilesButton_clicked();

    //  드래그 앤 드롭 이벤트를 위한 오버라이드 함수
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    Ui::MainWindow *ui; // Qt designer에서 생성한 객체
    std::shared_ptr<FileManager> fileManager;
    std::shared_ptr<TagManager> tagManager;

    // UI 상태 업데이트를 위한 헬퍼 함수
    void refreshFileList(const std::vector<std::string>& filePath); // 파일 목록 갱신
    void refreshTagList(); // 태그 목록 갱신
    void updateStatusBar(const QString& message); // 상태 표시줄 업데이트
    void clearInputFields(); // 입력 필드 초기화

    // 선택된 파일의 경로를 가져오는 헬퍼 함수
    std::vector<std::string> getSelectedFilePaths() const;
    // 선택된 태그의 이름을 가져오는 헬퍼 함수
    std::vector<std::string> getSelectedTagNames() const;

};

#endif //MAINWINDOW_H