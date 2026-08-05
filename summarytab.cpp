#include "summarytab.h"
#include <QHeaderView>

SummaryTab::SummaryTab(QWidget *parent) : QWidget(parent)
{
  QVBoxLayout *layout = new QVBoxLayout(this);

  m_table = new QTableWidget(0, 2, this);
  m_table->setHorizontalHeaderLabels(QStringList() << "Paramètre" << "Valeur");
  m_table->horizontalHeader()->setStretchLastSection(true);
  m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
  m_table->verticalHeader()->setVisible(false);
  m_table->setEditTriggers(QTableWidget::NoEditTriggers);
  m_table->setSelectionMode(QTableWidget::NoSelection);
  m_table->setAlternatingRowColors(true);
  layout->addWidget(m_table);

  m_rowEngineRpm = -1;
  int row = 0;

  addRow("Régime moteur (tr/min)");           m_rowEngineRpm = row++;
  addRow("Température liquide refroid. (°C)"); m_rowCoolantTemp = row++;
  addRow("Température ambiante (°C)");         m_rowAmbientTemp = row++;
  addRow("Température air admission (°C)");    m_rowIntakeAirTemp = row++;
  addRow("Température carburant (°C)");        m_rowFuelTemp = row++;
  addRow("Pression collecteur (kPa)");         m_rowMapKpa = row++;
  addRow("Tension batterie (V)");              m_rowBatteryVoltage = row++;
  addRow("Position papillon (%)");             m_rowThrottlePot = row++;
  addRow("Contact ralenti");                   m_rowIdleSwitch = row++;
  addRow("Contact point mort");                m_rowParkNeutralSwitch = row++;
  addRow("Codes défaut (brut)");               m_rowFaultCodes = row++;
  addRow("Consigne de ralenti");               m_rowIdleSetPoint = row++;
  addRow("Ralenti chaud");                     m_rowIdleHot = row++;
  addRow("Position moteur pas-à-pas (%)");     m_rowIacPosition = row++;
  addRow("Erreur de ralenti");                 m_rowIdleError = row++;
  addRow("Décalage d'avance");                 m_rowIgnitionAdvanceOffset = row++;
  addRow("Avance à l'allumage (°)");           m_rowIgnitionAdvance = row++;
  addRow("Temps bobine (ms)");                 m_rowCoilTime = row++;
  addRow("Contact allumage");                  m_rowIgnitionSwitch = row++;
  addRow("Angle papillon (°)");                m_rowThrottleAngle = row++;
  addRow("Ratio air/carburant");               m_rowAirFuelRatio = row++;
  addRow("DTC 2");                             m_rowDtc2 = row++;
  addRow("Tension sonde lambda (mV)");         m_rowLambdaVoltage = row++;
  addRow("Fréquence lambda");                  m_rowLambdaFrequency = row++;
  addRow("Cycle lambda (%)");                  m_rowLambdaDutycycle = row++;
  addRow("État lambda");                       m_rowLambdaStatus = row++;
  addRow("Boucle fermée");                     m_rowClosedLoop = row++;
  addRow("Correction carburant long terme (%)");  m_rowLongTermFuelTrim = row++;
  addRow("Correction carburant court terme (%)"); m_rowShortTermFuelTrim = row++;
  addRow("Cycle purge canister (%)");          m_rowCarbonCanisterDutycycle = row++;
  addRow("DTC 3");                             m_rowDtc3 = row++;
  addRow("Position de base ralenti");          m_rowIdleBasePos = row++;
  addRow("DTC 4");                             m_rowDtc4 = row++;
  addRow("Avance à l'allumage 2");             m_rowIgnitionAdvance2 = row++;
  addRow("Décalage régime de ralenti");        m_rowIdleSpeedOffset = row++;
  addRow("Erreur de ralenti 2");                m_rowIdleError2 = row++;
  addRow("DTC 5");                              m_rowDtc5 = row++;
}

void SummaryTab::addRow(const QString &label)
{
  int row = m_table->rowCount();
  m_table->insertRow(row);
  QTableWidgetItem *labelItem = new QTableWidgetItem(label);
  labelItem->setFlags(labelItem->flags() & ~Qt::ItemIsEditable);
  m_table->setItem(row, 0, labelItem);

  QTableWidgetItem *valueItem = new QTableWidgetItem("--");
  valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
  valueItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
  m_table->setItem(row, 1, valueItem);
}

void SummaryTab::setValue(int row, const QString &text)
{
  if (row >= 0 && row < m_table->rowCount())
  {
    m_table->item(row, 1)->setText(text);
  }
}

void SummaryTab::updateData(mems_data *data)
{
  if (!data)
  {
    return;
  }

  setValue(m_rowEngineRpm, QString::number(data->engine_rpm));
  setValue(m_rowCoolantTemp, QString::number(data->coolant_temp));
  setValue(m_rowAmbientTemp, QString::number(data->ambient_temp));
  setValue(m_rowIntakeAirTemp, QString::number(data->intake_air_temp));
  setValue(m_rowFuelTemp, QString::number(data->fuel_temp));
  setValue(m_rowMapKpa, QString::number(data->map_kpa));
  setValue(m_rowBatteryVoltage, QString::number(data->battery_voltage / 10.0, 'f', 1));
  setValue(m_rowThrottlePot, QString::number(data->throttle_pot));
  setValue(m_rowIdleSwitch, data->idle_switch ? "Activé" : "Désactivé");
  setValue(m_rowParkNeutralSwitch, data->park_neutral_switch ? "Activé" : "Désactivé");
  setValue(m_rowFaultCodes, QString::number(data->fault_codes));
  setValue(m_rowIdleSetPoint, QString::number(data->idle_set_point));
  setValue(m_rowIdleHot, QString::number(data->idle_hot));
  setValue(m_rowIacPosition, QString::number(data->iac_position));
  setValue(m_rowIdleError, QString::number(data->idle_error));
  setValue(m_rowIgnitionAdvanceOffset, QString::number(data->ignition_advance_offset));
  setValue(m_rowIgnitionAdvance, QString::number(data->ignition_advance));
  setValue(m_rowCoilTime, QString::number(data->coil_time));
  setValue(m_rowIgnitionSwitch, data->ignition_switch ? "Activé" : "Désactivé");
  setValue(m_rowThrottleAngle, QString::number(data->throttle_angle));
  setValue(m_rowAirFuelRatio, QString::number(data->air_fuel_ratio));
  setValue(m_rowDtc2, QString::number(data->dtc2));
  setValue(m_rowLambdaVoltage, QString::number(data->lambda_voltage));
  setValue(m_rowLambdaFrequency, QString::number(data->lambda_sensor_frequency));
  setValue(m_rowLambdaDutycycle, QString::number(data->lambda_sensor_dutycycle));
  setValue(m_rowLambdaStatus, data->lambda_sensor_status ? "OK" : "Défaut");
  setValue(m_rowClosedLoop, data->closed_loop ? "Fermée" : "Ouverte");
  setValue(m_rowLongTermFuelTrim, QString::number(data->long_term_fuel_trim));
  setValue(m_rowShortTermFuelTrim, QString::number(data->short_term_fuel_trim));
  setValue(m_rowCarbonCanisterDutycycle, QString::number(data->carbon_canister_dutycycle));
  setValue(m_rowDtc3, QString::number(data->dtc3));
  setValue(m_rowIdleBasePos, QString::number(data->idle_base_pos));
  setValue(m_rowDtc4, QString::number(data->dtc4));
  setValue(m_rowIgnitionAdvance2, QString::number(data->ignition_advance2));
  setValue(m_rowIdleSpeedOffset, QString::number(data->idle_speed_offset));
  setValue(m_rowIdleError2, QString::number(data->idle_error2));
  setValue(m_rowDtc5, QString::number(data->dtc5));
}
