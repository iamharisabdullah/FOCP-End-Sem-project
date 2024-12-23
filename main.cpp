#include "mainwindow.h"
#include <QFile>
#include <QString>
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;

    // Correct file path
    QFile file("C:/Users/Hp/Desktop/New folder/Fibrary.qss");  // Provide the correct relative or absolute path
    if (file.exists()) {
        qDebug() << "File exists!";
    } else {
        qDebug() << "File does not exist!";
    }

    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QString::fromLatin1(file.readAll());
        file.close();
        app.setStyleSheet(styleSheet);
    } else {
        qDebug() << "Failed to open file. Error:" << file.errorString();
        qDebug() << "Error code:" << file.error();
    }

    // Show the window
    w.show();

    return app.exec();
}
