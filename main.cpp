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

  MainWindow w;

  w.show();
  return a.exec();
}
