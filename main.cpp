#include <QApplication>
#include <QIcon>
#include <QPalette>
#include <QSplashScreen>
#include <QPixmap>
#include <QPainter>
#include <QTimer>
#include <QtMath>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include "mainwindow.h"
#include "splashprogress.h"
#include <QtPlugin>

#ifdef WIN32
#include <windows.h>
#include <shlobj.h>
#include <objbase.h>

/**
 * Crée un raccourci vers l'exécutable sur le Bureau de l'utilisateur.
 * N'est appelée qu'une seule fois, au tout premier lancement du logiciel.
 */
static void createDesktopShortcut()
{
  QString exePath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
  QString workDir = QDir::toNativeSeparators(QFileInfo(exePath).absolutePath());
  QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
  if (desktopPath.isEmpty())
  {
    return;
  }
  QString linkPath = QDir::toNativeSeparators(desktopPath + "/MEMS-Scan.lnk");

  HRESULT hres = CoInitialize(NULL);
  IShellLinkW *psl = NULL;
  hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                           IID_IShellLinkW, (LPVOID*)&psl);
  if (SUCCEEDED(hres) && psl)
  {
    psl->SetPath(reinterpret_cast<LPCWSTR>(exePath.utf16()));
    psl->SetWorkingDirectory(reinterpret_cast<LPCWSTR>(workDir.utf16()));
    psl->SetDescription(L"MEMS-Scan - Diagnostic Rover MEMS");
    psl->SetIconLocation(reinterpret_cast<LPCWSTR>(exePath.utf16()), 0);

    IPersistFile *ppf = NULL;
    hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);
    if (SUCCEEDED(hres) && ppf)
    {
      ppf->Save(reinterpret_cast<LPCOLESTR>(linkPath.utf16()), TRUE);
      ppf->Release();
    }
    psl->Release();
  }
  CoUninitialize();
}
#endif

static int g_currentProgress = 0;

static void drawSplash(QPixmap &pix, int percent)
{
  pix.fill(QColor("#1f66e6"));
  QPainter painter(&pix);
  painter.setRenderHint(QPainter::Antialiasing);

  painter.setPen(Qt::white);
  QFont titleFont("Segoe UI", 16, QFont::Bold);
  painter.setFont(titleFont);
  painter.drawText(pix.rect().adjusted(0, -25, 0, 0), Qt::AlignCenter,
                    QString("Démarrage de MEMS-Scan"));
  QFont versionFont("Segoe UI", 10);
  painter.setFont(versionFont);
  QString versionText = QString("v%1.%2.%3").arg(VER_MAJOR).arg(VER_MINOR).arg(VER_PATCH);
  painter.drawText(pix.rect().adjusted(0, 15, 0, 0), Qt::AlignCenter, versionText);

  // Barre de progression façon "Crystal" (dégradé brillant, coins arrondis)
  const int barWidth = 340;
  const int barHeight = 18;
  const int barX = (pix.width() - barWidth) / 2;
  const int barY = pix.height() - 45;

  QRect track(barX, barY, barWidth, barHeight);
  painter.setPen(QColor("#c7cbd4"));
  painter.setBrush(QColor("#ffffff"));
  painter.drawRoundedRect(track, 9, 9);

  int fillWidth = barWidth * qBound(0, percent, 100) / 100;
  if (fillWidth > 4)
  {
    QRect fillRect(barX, barY, fillWidth, barHeight);
    QLinearGradient grad(fillRect.topLeft(), fillRect.bottomLeft());
    grad.setColorAt(0.0, QColor("#d8f5d8"));
    grad.setColorAt(0.45, QColor("#ffffff"));
    grad.setColorAt(0.46, QColor("#bdeeb8"));
    grad.setColorAt(1.0, QColor("#4cd964"));
    painter.setPen(Qt::NoPen);
    painter.setBrush(grad);
    painter.drawRoundedRect(fillRect, 9, 9);
  }

  painter.setPen(Qt::white);
  QFont pctFont("Segoe UI", 8);
  painter.setFont(pctFont);
  painter.drawText(QRect(barX, barY + barHeight + 4, barWidth, 16),
                    Qt::AlignCenter, QString("%1 %").arg(percent));
  painter.end();
}
int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  a.setWindowIcon(QIcon(":/icons/key.png"));
  
  #ifdef WIN32
  QSettings firstRunSettings(QSettings::IniFormat, QSettings::UserScope, PROJECTNAME);
  firstRunSettings.beginGroup("Settings");
  if (!firstRunSettings.value("DesktopShortcutCreated", false).toBool())
  {
    createDesktopShortcut();
    firstRunSettings.setValue("DesktopShortcutCreated", true);
  }
  firstRunSettings.endGroup();
#endif

  // Écran de démarrage avec barre de progression réelle
  QPixmap splashPix(420, 150);
  drawSplash(splashPix, 0);

  QSplashScreen splash(splashPix);
  splash.show();
  a.processEvents();

  g_splashProgressCallback = [](int percent) {
    g_currentProgress = percent;
  };

  QTimer refreshTimer;
  QObject::connect(&refreshTimer, &QTimer::timeout, [&]() {
    QPixmap frame(420, 150);
    drawSplash(frame, g_currentProgress);
    splash.setPixmap(frame);
    a.processEvents();
  });
  refreshTimer.start(60);
  a.processEvents();

  QPalette palette = a.palette();
  palette.setColor(QPalette::Window, Qt::white);
  palette.setColor(QPalette::Base, Qt::white);
  a.setPalette(palette);

  a.setStyleSheet(
    "* {"
    "  font-family: 'Segoe UI', 'Calibri', Arial, sans-serif;"
    "  font-size: 9pt;"
    "}"
    "QPushButton {"
    "  background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
    "    stop:0 #7bb8ff, stop:0.49 #2f7dff, stop:0.5 #1f66e6, stop:1 #1653c2);"
    "  color: white;"
    "  border: 1px solid #14479e;"
    "  border-radius: 8px;"
    "  padding: 6px 16px;"
    "  font-weight: 600;"
    "}"
    "QPushButton:hover {"
    "  background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
    "    stop:0 #96c8ff, stop:0.49 #4a90ff, stop:0.5 #2f7ded, stop:1 #1f66c8);"
    "}"
    "QPushButton:pressed {"
    "  background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
    "    stop:0 #1653c2, stop:0.5 #1f66e6, stop:0.51 #2f7dff, stop:1 #7bb8ff);"
    "  padding-top: 7px;"
    "}"
    "QPushButton:disabled {"
    "  background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
    "    stop:0 #f2f3f5, stop:0.5 #dfe2e8, stop:1 #cdd1d8);"
    "  color: #9aa0ab;"
    "  border: 1px solid #c7cbd4;"
    "}"
    "QGroupBox {"
    "  border: 1px solid #dcdfe4;"
    "  border-radius: 8px;"
    "  margin-top: 14px;"
    "  padding-top: 8px;"
    "  font-weight: 600;"
    "}"
    "QGroupBox::title {"
    "  subcontrol-origin: margin;"
    "  left: 10px;"
    "  padding: 0 6px;"
    "  color: #2f6fed;"
    "}"
    "QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox {"
    "  border: 1px solid #d3d7de;"
    "  border-radius: 5px;"
    "  padding: 4px 6px;"
    "  background-color: white;"
    "}"
    "QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QDoubleSpinBox:focus {"
    "  border: 1px solid #2f6fed;"
    "}"
    "QTabWidget::pane {"
    "  border: 1px solid #dcdfe4;"
    "  border-radius: 8px;"
    "  top: -1px;"
    "}"
    "QTabBar::tab {"
    "  background-color: #eef0f3;"
    "  color: #565d6b;"
    "  border: 1px solid #dcdfe4;"
    "  border-bottom: none;"
    "  border-top-left-radius: 8px;"
    "  border-top-right-radius: 8px;"
    "  padding: 8px 18px;"
    "  margin-right: 2px;"
    "  font-weight: 600;"
    "}"
    "QTabBar::tab:selected {"
    "  background-color: white;"
    "  color: #2f6fed;"
    "}"
    "QTabBar::tab:hover {"
    "  color: #2f6fed;"
    "}"
    "QMenuBar {"
    "  background-color: white;"
    "  border-bottom: 1px solid #dcdfe4;"
    "}"
    "QMenuBar::item:selected {"
    "  background-color: #e7edfd;"
    "  border-radius: 4px;"
    "}"
    "QMenu {"
    "  background-color: white;"
    "  border: 1px solid #dcdfe4;"
    "}"
    "QMenu::item:selected {"
    "  background-color: #2f6fed;"
    "  color: white;"
    "}"
    "QTableWidget, QTableView {"
    "  gridline-color: #eceef2;"
    "  border: 1px solid #dcdfe4;"
    "  border-radius: 6px;"
    "  background-color: white;"
    "}"
    "QHeaderView::section {"
    "  background-color: #eceef2;"
    "  color: #565d6b;"
    "  padding: 4px;"
    "  border: none;"
    "  font-weight: 600;"
    "}"
    "QScrollBar:vertical {"
    "  background: transparent;"
    "  width: 10px;"
    "}"
    "QScrollBar::handle:vertical {"
    "  background: #c7cbd4;"
    "  border-radius: 5px;"
    "  min-height: 24px;"
    "}"
    "QScrollBar:horizontal {"
    "  background: transparent;"
    "  height: 10px;"
    "}"
    "QScrollBar::handle:horizontal {"
    "  background: #c7cbd4;"
    "  border-radius: 5px;"
    "  min-width: 24px;"
    "}"
    "QToolTip {"
    "  background-color: #1f2430;"
    "  color: white;"
    "  border: none;"
    "  padding: 4px 8px;"
    "  border-radius: 4px;"
    "}"
  );

  MainWindow w;

  g_splashProgressCallback = 0;
  refreshTimer.stop();
  w.showMaximized();
  splash.finish(&w);
  return a.exec();
}
