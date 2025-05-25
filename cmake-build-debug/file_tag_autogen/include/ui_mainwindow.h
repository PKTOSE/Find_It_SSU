/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *searchHorizontalLayout;
    QLineEdit *searchLineEdit;
    QPushButton *searchButton;
    QSplitter *mainSplitter;
    QWidget *widget_2;
    QVBoxLayout *verticalLayout_3;
    QTableWidget *fileTableWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *deleteFileButton;
    QPushButton *addFileButton;
    QSpacerItem *horizontalSpacer;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *applyTagFilterButton;
    QPushButton *showAllFilesButton;
    QListWidget *tagListWidget;
    QLineEdit *tagLineEdit;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *deleteTagButton;
    QPushButton *addTagButton;
    QPushButton *applyTagsButton;
    QVBoxLayout *verticalLayout_2;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1000, 700);
        MainWindow->setMinimumSize(QSize(1000, 700));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy);
        verticalLayout_4 = new QVBoxLayout(centralwidget);
        verticalLayout_4->setObjectName("verticalLayout_4");
        searchHorizontalLayout = new QHBoxLayout();
        searchHorizontalLayout->setObjectName("searchHorizontalLayout");
        searchLineEdit = new QLineEdit(centralwidget);
        searchLineEdit->setObjectName("searchLineEdit");
        QFont font;
        font.setFamilies({QString::fromUtf8("\353\202\230\353\210\224\354\212\244\355\200\230\354\226\264\353\235\274\354\232\264\353\223\234OTF")});
        font.setPointSize(11);
        searchLineEdit->setFont(font);
        searchLineEdit->setAutoFillBackground(false);

        searchHorizontalLayout->addWidget(searchLineEdit);

        searchButton = new QPushButton(centralwidget);
        searchButton->setObjectName("searchButton");
        QFont font1;
        font1.setFamilies({QString::fromUtf8("\352\260\225\355\225\234\352\263\265\352\265\260\354\262\264")});
        font1.setPointSize(10);
        searchButton->setFont(font1);

        searchHorizontalLayout->addWidget(searchButton);


        verticalLayout_4->addLayout(searchHorizontalLayout);

        mainSplitter = new QSplitter(centralwidget);
        mainSplitter->setObjectName("mainSplitter");
        sizePolicy.setHeightForWidth(mainSplitter->sizePolicy().hasHeightForWidth());
        mainSplitter->setSizePolicy(sizePolicy);
        mainSplitter->setMinimumSize(QSize(300, 0));
        mainSplitter->setOrientation(Qt::Orientation::Horizontal);
        widget_2 = new QWidget(mainSplitter);
        widget_2->setObjectName("widget_2");
        verticalLayout_3 = new QVBoxLayout(widget_2);
        verticalLayout_3->setObjectName("verticalLayout_3");
        fileTableWidget = new QTableWidget(widget_2);
        if (fileTableWidget->columnCount() < 3)
            fileTableWidget->setColumnCount(3);
        fileTableWidget->setObjectName("fileTableWidget");
        fileTableWidget->setMinimumSize(QSize(300, 0));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("\353\202\230\353\210\224\354\212\244\355\200\230\354\226\264\353\235\274\354\232\264\353\223\234OTF")});
        font2.setPointSize(12);
        fileTableWidget->setFont(font2);
        fileTableWidget->setColumnCount(3);

        verticalLayout_3->addWidget(fileTableWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        deleteFileButton = new QPushButton(widget_2);
        deleteFileButton->setObjectName("deleteFileButton");
        deleteFileButton->setFont(font1);

        horizontalLayout->addWidget(deleteFileButton);

        addFileButton = new QPushButton(widget_2);
        addFileButton->setObjectName("addFileButton");
        addFileButton->setFont(font1);

        horizontalLayout->addWidget(addFileButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout_3->addLayout(horizontalLayout);

        mainSplitter->addWidget(widget_2);
        widget = new QWidget(mainSplitter);
        widget->setObjectName("widget");
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        applyTagFilterButton = new QPushButton(widget);
        applyTagFilterButton->setObjectName("applyTagFilterButton");
        applyTagFilterButton->setFont(font1);

        horizontalLayout_2->addWidget(applyTagFilterButton);

        showAllFilesButton = new QPushButton(widget);
        showAllFilesButton->setObjectName("showAllFilesButton");
        showAllFilesButton->setFont(font1);

        horizontalLayout_2->addWidget(showAllFilesButton);


        verticalLayout->addLayout(horizontalLayout_2);

        tagListWidget = new QListWidget(widget);
        tagListWidget->setObjectName("tagListWidget");
        tagListWidget->setMinimumSize(QSize(100, 0));
        tagListWidget->setMaximumSize(QSize(16777215, 16777215));
        QFont font3;
        font3.setFamilies({QString::fromUtf8("\353\202\230\353\210\224\354\212\244\355\200\230\354\226\264\353\235\274\354\232\264\353\223\234OTF")});
        font3.setPointSize(14);
        tagListWidget->setFont(font3);
        tagListWidget->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);

        verticalLayout->addWidget(tagListWidget);

        tagLineEdit = new QLineEdit(widget);
        tagLineEdit->setObjectName("tagLineEdit");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(tagLineEdit->sizePolicy().hasHeightForWidth());
        tagLineEdit->setSizePolicy(sizePolicy1);
        tagLineEdit->setFont(font);
        tagLineEdit->setCursor(QCursor(Qt::CursorShape::IBeamCursor));

        verticalLayout->addWidget(tagLineEdit);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        deleteTagButton = new QPushButton(widget);
        deleteTagButton->setObjectName("deleteTagButton");
        deleteTagButton->setFont(font1);

        horizontalLayout_4->addWidget(deleteTagButton);

        addTagButton = new QPushButton(widget);
        addTagButton->setObjectName("addTagButton");
        addTagButton->setFont(font1);

        horizontalLayout_4->addWidget(addTagButton);

        applyTagsButton = new QPushButton(widget);
        applyTagsButton->setObjectName("applyTagsButton");
        applyTagsButton->setFont(font1);

        horizontalLayout_4->addWidget(applyTagsButton);


        verticalLayout->addLayout(horizontalLayout_4);

        mainSplitter->addWidget(widget);

        verticalLayout_4->addWidget(mainSplitter);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");

        verticalLayout_4->addLayout(verticalLayout_2);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1000, 33));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
#if QT_CONFIG(whatsthis)
        searchLineEdit->setWhatsThis(QString());
#endif // QT_CONFIG(whatsthis)
#if QT_CONFIG(accessibility)
        searchLineEdit->setAccessibleName(QCoreApplication::translate("MainWindow", "searching files...", nullptr));
#endif // QT_CONFIG(accessibility)
        searchButton->setText(QCoreApplication::translate("MainWindow", "\352\262\200\354\203\211", nullptr));
        deleteFileButton->setText(QCoreApplication::translate("MainWindow", "\355\214\214\354\235\274 \354\202\255\354\240\234", nullptr));
        addFileButton->setText(QCoreApplication::translate("MainWindow", "\355\214\214\354\235\274 \354\266\224\352\260\200", nullptr));
        applyTagFilterButton->setText(QCoreApplication::translate("MainWindow", "\355\203\234\352\267\270 \355\225\204\355\204\260 \354\240\201\354\232\251", nullptr));
        showAllFilesButton->setText(QCoreApplication::translate("MainWindow", "\354\240\204\354\262\264 \355\214\214\354\235\274 \353\263\264\352\270\260", nullptr));
        deleteTagButton->setText(QCoreApplication::translate("MainWindow", "\355\203\234\352\267\270 \354\202\255\354\240\234", nullptr));
        addTagButton->setText(QCoreApplication::translate("MainWindow", "\355\203\234\352\267\270 \354\266\224\352\260\200", nullptr));
        applyTagsButton->setText(QCoreApplication::translate("MainWindow", "\355\203\234\352\267\270 \354\240\201\354\232\251", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
