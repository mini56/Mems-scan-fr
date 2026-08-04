#include "analysistab.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QPainter>
#include <QTime>
#include <QMessageBox>
#include <QScrollBar>

//=============================================================================
// ChartWidget
//=============================================================================

ChartWidget::ChartWidget(QWidget *parent) : QWidget(parent), m_hasCursor(false), m_cursorX(0)
{
  setMinimumHeight(320);
  setMouseTracking(true);
  setAutoFillBackground(true);
  QPalette pal = palette();
  pal.setColor(QPalette::Window, QColor("#0e1420"));
  setPalette(pal);
}

void ChartWidget::setData(const QVector<double> &time,
                           const QVector<QVector<double> > &series,
                           const QStringList &names,
                           const QVector<QColor> &colors)
{
  m_time = time;
  m_series = series;
  m_names = names;
  m_colors = colors;
  m_visible = QVector<bool>(names.count(), false);
  update();
}

void ChartWidget::setVisible(int index, bool vis)
{
  if (index >= 0 && index < m_visible.count())
  {
    m_visible[index] = vis;
    update();
  }
}

void ChartWidget::clearData()
{
  m_time.clear();
  m_series.clear();
  m_names.clear();
  m_colors.clear();
  m_visible.clear();
  update();
}

void ChartWidget::mouseMoveEvent(QMouseEvent *event)
{
  m_hasCursor = true;
  m_cursorX = event->pos().x();
  update();
}

void ChartWidget::leaveEvent(QEvent *event)
{
  Q_UNUSED(event);
  m_hasCursor = false;
  update();
}

void ChartWidget::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  const int leftMargin = 8;
  const int rightMargin = 8;
  const int topMargin = 10;
  const int bottomMargin = 26;
  QRect plotRect(leftMargin, topMargin,
                  width() - leftMargin - rightMargin,
                  height() - topMargin - bottomMargin);

  // Grille de fond
  painter.setPen(QPen(QColor("#233047"), 1));
  for (int i = 0; i <= 4; i++)
  {
    int y = plotRect.top() + (plotRect.height() * i) / 4;
    painter.drawLine(plotRect.left(), y, plotRect.right(), y);
  }
  for (int i = 0; i <= 10; i++)
  {
    int x = plotRect.left() + (plotRect.width() * i) / 10;
    painter.drawLine(x, plotRect.top(), x, plotRect.bottom());
  }

  if (m_time.count() < 2)
  {
    painter.setPen(QColor("#8a93a6"));
    painter.drawText(rect(), Qt::AlignCenter,
                      "Chargez un fichier CSV et cochez une ou plusieurs voies ci-contre");
    return;
  }

  double tMin = m_time.first();
  double tMax = m_time.last();
  double tSpan = (tMax - tMin) > 0.0001 ? (tMax - tMin) : 1.0;

  // Tracé de chaque voie cochée, normalisée sur sa propre plage (comme un
  // oscilloscope multi-voies)
  int anyVisible = 0;
  for (int s = 0; s < m_series.count(); s++)
  {
    if (s >= m_visible.count() || !m_visible[s])
    {
      continue;
    }
    anyVisible++;
    const QVector<double> &values = m_series[s];
    if (values.isEmpty())
    {
      continue;
    }

    double vMin = values[0];
    double vMax = values[0];
    for (int i = 1; i < values.count(); i++)
    {
      if (values[i] < vMin) vMin = values[i];
      if (values[i] > vMax) vMax = values[i];
    }
    double vSpan = (vMax - vMin) > 0.0001 ? (vMax - vMin) : 1.0;

    QPen pen(m_colors[s % m_colors.count()], 2);
    painter.setPen(pen);

    QPolygonF poly;
    int n = qMin(m_time.count(), values.count());
    for (int i = 0; i < n; i++)
    {
      double xFrac = (m_time[i] - tMin) / tSpan;
      double yFrac = (values[i] - vMin) / vSpan;
      double x = plotRect.left() + xFrac * plotRect.width();
      double y = plotRect.bottom() - yFrac * plotRect.height();
      poly << QPointF(x, y);
    }
    painter.drawPolyline(poly);
  }

  if (anyVisible == 0)
  {
    painter.setPen(QColor("#8a93a6"));
    painter.drawText(rect(), Qt::AlignCenter,
                      "Cochez une ou plusieurs voies dans la liste à gauche pour les afficher");
  }

  // Axe temporel (secondes écoulées depuis le début de l'enregistrement)
  painter.setPen(QColor("#8a93a6"));
  for (int i = 0; i <= 5; i++)
  {
    double t = tMin + (tSpan * i) / 5.0;
    int x = plotRect.left() + (plotRect.width() * i) / 5;
    painter.drawText(QRect(x - 30, plotRect.bottom() + 4, 60, 18), Qt::AlignCenter,
                      QString("%1 s").arg(t - tMin, 0, 'f', 0));
  }

  // Curseur de survol avec valeurs instantanées (façon marqueur d'oscilloscope)
  if (m_hasCursor && m_cursorX >= plotRect.left() && m_cursorX <= plotRect.right() && anyVisible > 0)
  {
    painter.setPen(QPen(QColor("#e6e8ee"), 1, Qt::DashLine));
    painter.drawLine(m_cursorX, plotRect.top(), m_cursorX, plotRect.bottom());

    double xFrac = double(m_cursorX - plotRect.left()) / double(plotRect.width());
    double tAtCursor = tMin + xFrac * tSpan;

    // Trouver l'index temporel le plus proche
    int idx = 0;
    double best = 1e18;
    for (int i = 0; i < m_time.count(); i++)
    {
      double d = qAbs(m_time[i] - tAtCursor);
      if (d < best) { best = d; idx = i; }
    }

    int legendY = plotRect.top() + 6;
    QFont f = painter.font();
    f.setPointSize(8);
    painter.setFont(f);

    for (int s = 0; s < m_series.count(); s++)
    {
      if (s >= m_visible.count() || !m_visible[s]) continue;
      if (idx >= m_series[s].count()) continue;

      QString label = QString("%1 : %2").arg(m_names[s]).arg(m_series[s][idx], 0, 'f', 1);
      QFontMetrics fm(f);
      int textW = fm.horizontalAdvance(label) + 10;

      int boxX = m_cursorX + 8;
      if (boxX + textW > plotRect.right())
      {
        boxX = m_cursorX - 8 - textW;
      }

      painter.setPen(Qt::NoPen);
      painter.setBrush(QColor(14, 20, 32, 210));
      painter.drawRect(boxX, legendY, textW, 16);

      painter.setPen(m_colors[s % m_colors.count()]);
      painter.drawText(QRect(boxX + 5, legendY, textW, 16), Qt::AlignVCenter | Qt::AlignLeft, label);

      legendY += 18;
    }
  }
}

//=============================================================================
// AnalysisTab
//=============================================================================

AnalysisTab::AnalysisTab(QWidget *parent) : QWidget(parent)
{
  // Palette de couleurs distinctes pour les voies (cycle si plus de colonnes)
  m_colors << QColor("#4fc3f7") << QColor("#ff7043") << QColor("#66bb6a")
           << QColor("#ffca28") << QColor("#ba68c8") << QColor("#26c6da")
           << QColor("#ef5350") << QColor("#9ccc65") << QColor("#ec407a")
           << QColor("#7e57c2") << QColor("#8d6e63") << QColor("#5c6bc0");

  QHBoxLayout *mainLayout = new QHBoxLayout(this);

  // --- Colonne de gauche : chargement + liste des voies ---
  QWidget *leftPanel = new QWidget(this);
  leftPanel->setMaximumWidth(280);
  QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);

  m_loadButton = new QPushButton("Charger un fichier CSV...", leftPanel);
  connect(m_loadButton, SIGNAL(clicked()), this, SLOT(onLoadFileClicked()));
  leftLayout->addWidget(m_loadButton);

  m_fileLabel = new QLabel("Aucun fichier chargé", leftPanel);
  m_fileLabel->setWordWrap(true);
  m_fileLabel->setStyleSheet("color: #565d6b; font-style: italic;");
  leftLayout->addWidget(m_fileLabel);

  QHBoxLayout *selectRow = new QHBoxLayout();
  m_selectAllButton = new QPushButton("Tout cocher", leftPanel);
  m_selectNoneButton = new QPushButton("Tout décocher", leftPanel);
  connect(m_selectAllButton, SIGNAL(clicked()), this, SLOT(onSelectAllClicked()));
  connect(m_selectNoneButton, SIGNAL(clicked()), this, SLOT(onSelectNoneClicked()));
  selectRow->addWidget(m_selectAllButton);
  selectRow->addWidget(m_selectNoneButton);
  leftLayout->addLayout(selectRow);

  QLabel *voiesLabel = new QLabel("Voies disponibles :", leftPanel);
  voiesLabel->setStyleSheet("font-weight: 600; margin-top: 6px;");
  leftLayout->addWidget(voiesLabel);

  m_checkboxScrollArea = new QScrollArea(leftPanel);
  m_checkboxScrollArea->setWidgetResizable(true);
  m_checkboxContainer = new QWidget();
  m_checkboxLayout = new QVBoxLayout(m_checkboxContainer);
  m_checkboxLayout->addStretch();
  m_checkboxScrollArea->setWidget(m_checkboxContainer);
  leftLayout->addWidget(m_checkboxScrollArea);

  mainLayout->addWidget(leftPanel);

  // --- Zone de tracé (droite) ---
  m_chart = new ChartWidget(this);
  mainLayout->addWidget(m_chart, 1);
}

void AnalysisTab::onLoadFileClicked()
{
  QString path = QFileDialog::getOpenFileName(this, "Charger un fichier journal",
                                                "logs", "Fichiers CSV (*.csv);;Tous les fichiers (*.*)");
  if (path.isEmpty())
  {
    return;
  }
  parseCsv(path);
}

void AnalysisTab::parseCsv(const QString &path)
{
  QFile file(path);
  if (!file.open(QFile::ReadOnly | QFile::Text))
  {
    QMessageBox::warning(this, "Erreur", "Impossible d'ouvrir le fichier :\n" + path, QMessageBox::Ok);
    return;
  }

  QTextStream stream(&file);

  // Première ligne : ID de l'ECU (ignorée pour l'analyse)
  if (!stream.atEnd())
  {
    stream.readLine();
  }

  // Deuxième ligne : en-têtes des colonnes
  if (stream.atEnd())
  {
    QMessageBox::warning(this, "Erreur", "Fichier CSV vide ou incomplet.", QMessageBox::Ok);
    return;
  }
  QString headerLine = stream.readLine();
  QStringList headers = headerLine.split(",");
  if (!headers.isEmpty() && headers[0].startsWith("#"))
  {
    headers[0] = headers[0].mid(1);
  }

  int columnCount = headers.count() - 1; // la première colonne est le temps
  if (columnCount <= 0)
  {
    QMessageBox::warning(this, "Erreur", "Format de fichier CSV non reconnu.", QMessageBox::Ok);
    return;
  }

  m_columnNames.clear();
  for (int i = 1; i < headers.count(); i++)
  {
    m_columnNames << headers[i];
  }

  m_time.clear();
  m_columns.clear();
  m_columns.resize(columnCount);

  QTime firstTime;
  int lineCount = 0;

  while (!stream.atEnd())
  {
    QString line = stream.readLine();
    if (line.trimmed().isEmpty())
    {
      continue;
    }
    QStringList fields = line.split(",");
    if (fields.count() < 2)
    {
      continue;
    }

    QTime t = QTime::fromString(fields[0], "hh:mm:ss");
    double seconds;
    if (t.isValid())
    {
      if (!firstTime.isValid())
      {
        firstTime = t;
      }
      seconds = firstTime.secsTo(t);
      if (seconds < 0)
      {
        // franchissement de minuit pendant l'enregistrement
        seconds += 24 * 3600;
      }
    }
    else
    {
      seconds = lineCount;
    }
    m_time.append(seconds);

    for (int c = 0; c < columnCount; c++)
    {
      double value = 0.0;
      if (c + 1 < fields.count())
      {
        value = fields[c + 1].trimmed().toDouble();
      }
      m_columns[c].append(value);
    }
    lineCount++;
  }
  file.close();

  if (lineCount == 0)
  {
    QMessageBox::warning(this, "Erreur", "Aucune donnée exploitable dans ce fichier.", QMessageBox::Ok);
    return;
  }

  m_fileLabel->setText(QFileInfo(path).fileName() + " (" + QString::number(lineCount) + " points)");

  rebuildCheckboxes();

  m_chart->setData(m_time, m_columns, m_columnNames, m_colors);
}

void AnalysisTab::rebuildCheckboxes()
{
  // Nettoyer les anciennes cases à cocher
  for (int i = 0; i < m_checkboxes.count(); i++)
  {
    m_checkboxLayout->removeWidget(m_checkboxes[i]);
    delete m_checkboxes[i];
  }
  m_checkboxes.clear();

  // Retirer le stretch temporairement pour insérer avant lui
  QLayoutItem *stretchItem = m_checkboxLayout->takeAt(m_checkboxLayout->count() - 1);

  for (int i = 0; i < m_columnNames.count(); i++)
  {
    QCheckBox *cb = new QCheckBox(m_columnNames[i], m_checkboxContainer);
    QColor c = m_colors[i % m_colors.count()];
    cb->setStyleSheet(QString("QCheckBox::indicator { width: 12px; height: 12px; background-color: %1; border-radius: 2px; }")
                       .arg(c.name()));

    // Cocher automatiquement quelques voies usuelles au premier chargement
    if (m_columnNames[i].contains("engine-rpm") || m_columnNames[i].contains("coolant_temp") ||
        m_columnNames[i].contains("battery_voltage"))
    {
      cb->setChecked(true);
    }

    connect(cb, SIGNAL(toggled(bool)), this, SLOT(onCheckboxToggled(bool)));
    m_checkboxLayout->addWidget(cb);
    m_checkboxes.append(cb);
  }

  if (stretchItem)
  {
    m_checkboxLayout->addItem(stretchItem);
  }
  else
  {
    m_checkboxLayout->addStretch();
  }

  updateChartVisibility();
}

void AnalysisTab::onCheckboxToggled(bool checked)
{
  QCheckBox *cb = qobject_cast<QCheckBox*>(sender());
  if (!cb)
  {
    return;
  }
  int index = m_checkboxes.indexOf(cb);
  if (index >= 0)
  {
    m_chart->setVisible(index, checked);
  }
}

void AnalysisTab::onSelectAllClicked()
{
  for (int i = 0; i < m_checkboxes.count(); i++)
  {
    m_checkboxes[i]->setChecked(true);
  }
}

void AnalysisTab::onSelectNoneClicked()
{
  for (int i = 0; i < m_checkboxes.count(); i++)
  {
    m_checkboxes[i]->setChecked(false);
  }
}

void AnalysisTab::updateChartVisibility()
{
  for (int i = 0; i < m_checkboxes.count(); i++)
  {
    m_chart->setVisible(i, m_checkboxes[i]->isChecked());
  }
}
