#ifndef ANALYSISTAB_H
#define ANALYSISTAB_H

#include <QWidget>
#include <QVector>
#include <QStringList>
#include <QColor>
#include <QCheckBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPaintEvent>
#include <QMouseEvent>

/**
 * Widget de tracé façon "oscilloscope" : dessine plusieurs séries de
 * données superposées, chacune normalisée sur sa propre échelle (0-100%)
 * pour rester lisible malgré des unités très différentes (tr/min, °C, V...).
 */
class ChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChartWidget(QWidget *parent = 0);

    void setData(const QVector<double> &time,
                 const QVector<QVector<double> > &series,
                 const QStringList &names,
                 const QVector<QColor> &colors);
    void setVisible(int index, bool visible);
    void clearData();

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void leaveEvent(QEvent *event);

private:
    QVector<double> m_time;
    QVector<QVector<double> > m_series;
    QStringList m_names;
    QVector<QColor> m_colors;
    QVector<bool> m_visible;
    bool m_hasCursor;
    int m_cursorX;
};

/**
 * Onglet "Analyse" : charge un fichier CSV enregistré par MEMS-Scan et
 * permet d'afficher/masquer chaque colonne de données comme les voies
 * d'un oscilloscope.
 */
class AnalysisTab : public QWidget
{
    Q_OBJECT

public:
    explicit AnalysisTab(QWidget *parent = 0);

private slots:
    void onLoadFileClicked();
    void onCheckboxToggled(bool checked);
    void onSelectAllClicked();
    void onSelectNoneClicked();

private:
    void parseCsv(const QString &path);
    void rebuildCheckboxes();
    void updateChartVisibility();

    QLabel *m_fileLabel;
    QPushButton *m_loadButton;
    QPushButton *m_selectAllButton;
    QPushButton *m_selectNoneButton;
    QScrollArea *m_checkboxScrollArea;
    QWidget *m_checkboxContainer;
    QVBoxLayout *m_checkboxLayout;
    ChartWidget *m_chart;

    QVector<double> m_time;
    QVector<QVector<double> > m_columns;
    QStringList m_columnNames;
    QVector<QCheckBox*> m_checkboxes;
    QVector<QColor> m_colors;
};

#endif // ANALYSISTAB_H
