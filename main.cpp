#include <QApplication>
#include <QPalette>
#include "mainwindow.h"
#include <QtPlugin>
int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  QPalette palette = a.palette();
  palette.setColor(QPalette::Window, Qt::white);
  palette.setColor(QPalette::Base, Qt::white);
  a.setPalette(palette);

  a.setStyleSheet(
    "QPushButton {"
    "  background-color: #2f6fed;"
    "  color: white;"
    "  border: none;"
    "  border-radius: 6px;"
    "  padding: 6px 14px;"
    "  font-weight: 600;"
    "}"
    "QPushButton:hover {"
    "  background-color: #245bd1;"
    "}"
    "QPushButton:pressed {"
    "  background-color: #1c48a8;"
    "}"
    "QPushButton:disabled {"
    "  background-color: #c7cbd4;"
    "  color: #8b909c;"
    "}"
  );

  MainWindow w;

  w.show();
  return a.exec();
}
