#ifndef SUMMARYTAB_H
#define SUMMARYTAB_H

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include "rosco.h"

/**
 * Onglet "toutes les mesures" : liste complète et lisible de toutes les
 * valeurs actuellement lues sur l'ECU, sous forme de tableau à deux
 * colonnes (paramètre / valeur), mis à jour en direct.
 *
 * Cet onglet a été reconstruit intégralement par le code : dans le fichier
 * .ui d'origine, tout son contenu (environ 170 widgets) avait été laissé
 * commenté par l'auteur du logiciel, ce qui faisait apparaître un onglet
 * vide dans le programme.
 */
class SummaryTab : public QWidget
{
    Q_OBJECT

public:
    explicit SummaryTab(QWidget *parent = 0);
    void updateData(mems_data *data);

private:
    void addRow(const QString &label);
    void setValue(int row, const QString &text);

    QTableWidget *m_table0;
    QTableWidget *m_table1;
    QTableWidget *m_table2;
    int m_rowCount;

    // Index de chaque ligne du tableau, dans l'ordre d'ajout
    int m_rowEngineRpm;
    int m_rowCoolantTemp;
    int m_rowAmbientTemp;
    int m_rowIntakeAirTemp;
    int m_rowFuelTemp;
    int m_rowMapKpa;
    int m_rowBatteryVoltage;
    int m_rowThrottlePot;
    int m_rowIdleSwitch;
    int m_rowParkNeutralSwitch;
    int m_rowFaultCodes;
    int m_rowIdleSetPoint;
    int m_rowIdleHot;
    int m_rowIacPosition;
    int m_rowIdleError;
    int m_rowIgnitionAdvanceOffset;
    int m_rowIgnitionAdvance;
    int m_rowCoilTime;
    int m_rowIgnitionSwitch;
    int m_rowThrottleAngle;
    int m_rowAirFuelRatio;
    int m_rowDtc2;
    int m_rowLambdaVoltage;
    int m_rowLambdaFrequency;
    int m_rowLambdaDutycycle;
    int m_rowLambdaStatus;
    int m_rowClosedLoop;
    int m_rowLongTermFuelTrim;
    int m_rowShortTermFuelTrim;
    int m_rowCarbonCanisterDutycycle;
    int m_rowDtc3;
    int m_rowIdleBasePos;
    int m_rowDtc4;
    int m_rowIgnitionAdvance2;
    int m_rowIdleSpeedOffset;
    int m_rowIdleError2;
    int m_rowDtc5;
};

#endif // SUMMARYTAB_H
