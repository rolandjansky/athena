/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Book and fill offline histograms showing status of modules
///////////////////////////////////////////////////////////////////////////////

#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "InDetIdentifier/PixelID.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile2D_LW.h"
#include "LWHists/TProfile_LW.h"
#include "PixelMonitoring/PixelMainMon.h"
#include "PixelMonitoring/PixelMonModules.h"
#include "TH1F.h"
#include "TH1I.h"
#include "TH2F.h"
#include "TH2I.h"
#include "TMath.h"
#include "TProfile2D.h"
#include "TString.h"

///////////////////////////////////////////////////////////////////////////////
//////////////////////booking methods//////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

StatusCode PixelMainMon::bookPixelDCSMon(void) {
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "starting Book Status" << endmsg;

  msg(MSG::DEBUG) << "[BookPixelDCSMon]" << endmsg;

  std::string path = "Pixel/DCS";
  if (m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/DCSOnTrack");
  MonGroup dcsExpert(this, path.c_str(), run, ATTRIB_MANAGED);  // declare a group of histograms

  float min_temperature = -20.;
  float max_temperature = 0.;
  int nbins_temperature = 100;
  float min_LB = 0.;
  float max_LB = 1500.;
  int nbins_LB = 1500;
  float min_module = -10.;
  float max_module = 10.;
  int nbins_module = 20;
  float min_HV = 0.;
  float max_HV = 150.;
  int nbins_HV = 150;
  float min_HVcurrent = 0.;
  float max_HVcurrent = 0.1;
  int nbins_HVcurrent = 100;
  float min_LV = 0.;
  float max_LV = 5.;
  int nbins_LV = 50;
  float min_LVcurrent = 0.;
  float max_LVcurrent = 5.;
  int nbins_LVcurrent = 50;
  float min_moduleGroup = 0.;
  float max_moduleGroup = 8.;
  int nbins_moduleGroup = 8;
  float min_staveID = 1;
  float max_staveID = 15;
  int nbins_staveID = 14;
  float min_FSM = 0;
  float max_FSM = 11;
  int nbins_FSM = 11;
  TString label_moduleTemp = "Module Temperature [#circC]";
  TString label_inletTemp = "Cooling Pipe Inlet Temperature [#circC]";
  TString label_outletTemp = "Cooling Pipe Outlet Temperature [#circC]";
  TString label_HVoltage = "HV [-V]";
  TString label_LVoltage = "LV [V]";
  TString label_HVcurrent = "HV current [-mA]";
  TString label_LVcurrent = "LV current [A]";
  TString label_dT = "Module - Cooling Pipe Outlet Temperature [#circC]";
  TString label_HVPC = "HV Power Consumption [mW]";
  TString label_LVPC = "LV Power Consumption [W]";
  TString label_LVHVPC = "LV+HV Power Consumption [W]";
  TString label_effFLEXtemp = "Effective FLEX Temperature [#circC]";
  TString label_tfm = "Thermal Figure of Merit [#circC/W]";
  TString label_FSMstate = "FSM State";
  TString label_FSMstatus = "FSM Status";
  const int NUMMODULEGROUP = 8;
  const char* moduleGroup[NUMMODULEGROUP] = {"M4C", "M3C", "M2C", "M1C", "M1A", "M2A", "M3A", "M4A"};
  const int NUMMODULEETA = 20;
  const char* moduleEta[NUMMODULEETA] = {"C8_2", "C8_1", "C7_2", "C7_1",
                                         "C6", "C5", "C4", "C3", "C2", "C1",
                                         "A1", "A2", "A3", "A4", "A5", "A6",
                                         "A7_1", "A7_2", "A8_1", "A8_2"};
  const int NUMFSM = 11;
  const char* FSMSTATE[NUMFSM] = {"READY", "ON", "STANDBY", "LV_ON", "TRANSITION", "UNDEFINED", "LOCKED_OUT", "DISABLED", "OFF", "DEAD", "UNKNOWN"};
  const char* FSMSTATUS[NUMFSM] = {"OK", "WARNING", "ERROR", "UNINITIALIZED", "DEAD", "5", "6", "7", "8", "9", "UNKNOWN"};
  TAxis* txaxis;
  TAxis* tyaxis;
  LWHist::LWHistAxis* xaxis;
  LWHist::LWHistAxis* yaxis;
  StatusCode sc;
  for (int ii = 0; ii < IBLStave::COUNT; ii++) {
    // temperature
    sc = dcsExpert.regHist(m_hist_moduleTemperature2Dscatter[ii] = TH2F_LW::create(
                               Form("moduleTemperature_ModuleNumber_S%02d", ii + 1), "Module Temprerature vs Module Number; Module Number;" + label_moduleTemp + ";Number of LBs",
                               nbins_module, min_module, max_module, nbins_temperature, min_temperature, max_temperature));
    xaxis = m_hist_moduleTemperature2Dscatter[ii]->GetXaxis();
    for (int jj = 0; jj < NUMMODULEETA; jj++) {
      xaxis->SetBinLabel(jj + 1, moduleEta[jj]);
    }
    sc = dcsExpert.regHist(m_hist_moduleTemperatureLB[ii] = TProfile_LW::create(Form("moduleTemperature_S%02dvsLB", ii + 1), "Module Temperature vs LB; LB;" + label_moduleTemp, nbins_LB, min_LB, max_LB));
    m_hist_moduleTemperatureLB[ii]->SetMarkerSize(0.5);

    sc = dcsExpert.regHist(m_hist_LB_moduleGroup_moduleTemperature[ii] = TProfile2D_LW::create(
                               Form("LB_moduleGroup_moduleTemperature_S%02dvsLB", ii + 1), "LB_moduleGroup_moduleTemperature; LB;moduleGoup;" + label_moduleTemp,
                               nbins_LB, min_LB, max_LB, nbins_moduleGroup, min_moduleGroup, max_moduleGroup));
    yaxis = m_hist_LB_moduleGroup_moduleTemperature[ii]->GetYaxis();
    for (int jj = 0; jj < NUMMODULEGROUP; jj++) {
      yaxis->SetBinLabel(jj + 1, moduleGroup[jj]);
    }

    sc = dcsExpert.regHist(m_hist_HVoltage2Dscatter[ii] = TH2F_LW::create(
                               Form("HVvoltage_ModuleNumber_S%02d", ii + 1), "HV vs Module Number; Module Number;" + label_HVoltage + ";Number of LBs",
                               nbins_module, min_module, max_module, nbins_HV, min_HV, max_HV));
    xaxis = m_hist_HVoltage2Dscatter[ii]->GetXaxis();
    for (int jj = 0; jj < NUMMODULEETA; jj++) {
      xaxis->SetBinLabel(jj + 1, moduleEta[jj]);
    }

    sc = dcsExpert.regHist(m_hist_HVoltageLB[ii] = TProfile_LW::create(Form("HVvoltage_S%02dvsLB", ii + 1), "HV vs LB; LB;" + label_HVoltage, nbins_LB, min_LB, max_LB));
    m_hist_HVoltageLB[ii]->SetMarkerSize(0.5);

    sc = dcsExpert.regHist(m_hist_LB_moduleGroup_HVoltage[ii] = TProfile2D_LW::create(
                               Form("LB_moduleGroup_HV_S%02dvsLB", ii + 1), "LB_moduleGroup_HV; LB;moduleGoup;" + label_HVoltage,
                               nbins_LB, min_LB, max_LB, nbins_moduleGroup, min_moduleGroup, max_moduleGroup));
    yaxis = m_hist_LB_moduleGroup_HVoltage[ii]->GetYaxis();
    for (int jj = 0; jj < NUMMODULEGROUP; jj++) {
      yaxis->SetBinLabel(jj + 1, moduleGroup[jj]);
    }

    sc = dcsExpert.regHist(m_hist_Pipes_inletLB[ii] = TProfile_LW::create(
                               Form("Pipes_inlet_temperature_S%02dvsLB", ii + 1), "Pipes_inlet_temperature vs LB; LB;" + label_inletTemp + ";Number of LBs", nbins_LB, min_LB, max_LB));
    m_hist_Pipes_inletLB[ii]->SetMarkerSize(0.5);

    sc = dcsExpert.regHist(m_hist_Pipes_outletLB[ii] = TProfile_LW::create(
                               Form("Pipes_outlet_temperature_S%02dvsLB", ii + 1), "Pipes_outlet_temperature vs LB; LB;" + label_outletTemp + ";Number of LBs", nbins_LB, min_LB, max_LB));
    m_hist_Pipes_outletLB[ii]->SetMarkerSize(0.5);

    sc = dcsExpert.regHist(m_hist_LVoltage2Dscatter[ii] = TH2F_LW::create(
                               Form("LVvoltage_ModuleNumber_S%02d", ii + 1), "LV vs Module Number; Module Number;" + label_LVoltage + ";Number of LBs",
                               nbins_module, min_module, max_module, nbins_LV, min_LV, max_LV));
    xaxis = m_hist_LVoltage2Dscatter[ii]->GetXaxis();
    for (int jj = 0; jj < NUMMODULEETA; jj++) {
      xaxis->SetBinLabel(jj + 1, moduleEta[jj]);
    }

    sc = dcsExpert.regHist(m_hist_LVoltageLB[ii] = TProfile_LW::create(Form("LVvoltage_S%02dvsLB", ii + 1), "LV vs LB; LB;" + label_LVoltage, nbins_LB, min_LB, max_LB));
    m_hist_LVoltageLB[ii]->SetMarkerSize(0.5);

    sc = dcsExpert.regHist(m_hist_LB_moduleGroup_LVoltage[ii] = TProfile2D_LW::create(
                               Form("LB_moduleGroup_LVoltage_S%02dvsLB", ii + 1), "LB_moduleGroup_LVoltage; LB;moduleGoup;" + label_LVoltage,
                               nbins_LB, min_LB, max_LB, nbins_moduleGroup, min_moduleGroup, max_moduleGroup));
    yaxis = m_hist_LB_moduleGroup_LVoltage[ii]->GetYaxis();
    for (int jj = 0; jj < NUMMODULEGROUP; jj++) {
      yaxis->SetBinLabel(jj + 1, moduleGroup[jj]);
    }

    sc = dcsExpert.regHist(m_hist_LVcurrent2Dscatter[ii] = TH2F_LW::create(
                               Form("LVcurrent_ModuleNumber_S%02d", ii + 1), "LV current vs Module Number; Module Number;" + label_LVcurrent + ";Number of LBs",
                               nbins_module, min_module, max_module, nbins_LVcurrent, min_LVcurrent, max_LVcurrent));
    xaxis = m_hist_LVcurrent2Dscatter[ii]->GetXaxis();
    for (int jj = 0; jj < NUMMODULEETA; jj++) {
      xaxis->SetBinLabel(jj + 1, moduleEta[jj]);
    }

    sc = dcsExpert.regHist(m_hist_LVcurrentLB[ii] = TProfile_LW::create(Form("LVcurrent_S%02dvsLB", ii + 1), "LV current vs LB; LB;" + label_LVcurrent, nbins_LB, min_LB, max_LB));
    m_hist_LVcurrentLB[ii]->SetMarkerSize(0.5);

    sc = dcsExpert.regHist(m_hist_LB_moduleGroup_LVcurrent[ii] = TProfile2D_LW::create(
                               Form("LB_moduleGroup_LVcurrent_S%02dvsLB", ii + 1), "LB_moduleGroup_LVcurrent; LB;moduleGoup;" + label_LVcurrent,
                               nbins_LB, min_LB, max_LB, nbins_moduleGroup, min_moduleGroup, max_moduleGroup));
    yaxis = m_hist_LB_moduleGroup_LVcurrent[ii]->GetYaxis();
    for (int jj = 0; jj < NUMMODULEGROUP; jj++) {
      yaxis->SetBinLabel(jj + 1, moduleGroup[jj]);
    }

    sc = dcsExpert.regHist(m_hist_HVcurrent2Dscatter[ii] = TH2F_LW::create(
                               Form("HVcurrent_ModuleNumber_S%02d", ii + 1), "HV current vs Module Number; Module Number;" + label_HVcurrent + ";Number of LBs",
                               nbins_module, min_module, max_module, nbins_HVcurrent, min_HVcurrent, max_HVcurrent));
    xaxis = m_hist_HVcurrent2Dscatter[ii]->GetXaxis();
    for (int jj = 0; jj < NUMMODULEETA; jj++) {
      xaxis->SetBinLabel(jj + 1, moduleEta[jj]);
    }

    sc = dcsExpert.regHist(m_hist_HVcurrentLB[ii] = TProfile_LW::create(Form("HVcurrent_S%02dvsLB", ii + 1), "HV current vs LB; LB;" + label_HVcurrent, nbins_LB, min_LB, max_LB));
    m_hist_HVcurrentLB[ii]->SetMarkerSize(0.5);

    sc = dcsExpert.regHist(m_hist_LB_moduleGroup_HVcurrent[ii] = TProfile2D_LW::create(
                               Form("LB_moduleGroup_HVcurrent_S%02dvsLB", ii + 1), "LB_moduleGroup_HVcurrent; LB;moduleGoup;" + label_HVcurrent,
                               nbins_LB, min_LB, max_LB, nbins_moduleGroup, min_moduleGroup, max_moduleGroup));
    yaxis = m_hist_LB_moduleGroup_HVcurrent[ii]->GetYaxis();
    for (int jj = 0; jj < NUMMODULEGROUP; jj++) {
      yaxis->SetBinLabel(jj + 1, moduleGroup[jj]);
    }

    sc = dcsExpert.regHist(m_hist_LB_moduleGroup_dT[ii] = TProfile2D_LW::create(
                               Form("LB_moduleGroup_dT_S%02dvsLB", ii + 1), "LB_moduleGroup_dT; LB;moduleGoup;" + label_dT,
                               nbins_LB, min_LB, max_LB, nbins_moduleGroup, min_moduleGroup, max_moduleGroup));
    yaxis = m_hist_LB_moduleGroup_dT[ii]->GetYaxis();
    for (int jj = 0; jj < NUMMODULEGROUP; jj++) {
      yaxis->SetBinLabel(jj + 1, moduleGroup[jj]);
    }

    sc = dcsExpert.regHist(m_hist_LB_moduleGroup_LVPowerConsumption[ii] = TProfile2D_LW::create(
                               Form("LB_moduleGroup_LVPowerConsumption_S%02dvsLB", ii + 1), "LB_moduleGroup_LVPowerConsumption; LB;moduleGoup;" + label_LVPC,
                               nbins_LB, min_LB, max_LB, nbins_moduleGroup, min_moduleGroup, max_moduleGroup));
    yaxis = m_hist_LB_moduleGroup_LVPowerConsumption[ii]->GetYaxis();
    for (int jj = 0; jj < NUMMODULEGROUP; jj++) {
      yaxis->SetBinLabel(jj + 1, moduleGroup[jj]);
    }

    sc = dcsExpert.regHist(m_hist_LB_moduleGroup_HVPowerConsumption[ii] = TProfile2D_LW::create(
                               Form("LB_moduleGroup_HVPowerConsumption_S%02dvsLB", ii + 1), "LB_moduleGroup_HVPowerConsumption; LB;moduleGoup;" + label_HVPC,
                               nbins_LB, min_LB, max_LB, nbins_moduleGroup, min_moduleGroup, max_moduleGroup));
    yaxis = m_hist_LB_moduleGroup_HVPowerConsumption[ii]->GetYaxis();
    for (int jj = 0; jj < NUMMODULEGROUP; jj++) {
      yaxis->SetBinLabel(jj + 1, moduleGroup[jj]);
    }

    sc = dcsExpert.regHist(m_hist_LB_moduleGroup_LVHVPowerConsumption[ii] = TProfile2D_LW::create(
                               Form("LB_moduleGroup_LVHVPowerConsumption_S%02dvsLB", ii + 1), "LB_moduleGroup_LVHVPowerConsumption; LB;moduleGoup;" + label_LVHVPC,
                               nbins_LB, min_LB, max_LB, nbins_moduleGroup, min_moduleGroup, max_moduleGroup));
    yaxis = m_hist_LB_moduleGroup_LVHVPowerConsumption[ii]->GetYaxis();
    for (int jj = 0; jj < NUMMODULEGROUP; jj++) {
      yaxis->SetBinLabel(jj + 1, moduleGroup[jj]);
    }

    sc = dcsExpert.regHist(m_hist_LB_moduleGroup_effFLEXtemp[ii] = TProfile2D_LW::create(
                               Form("LB_moduleGroup_effFLEXtemp_S%02dvsLB", ii + 1), "LB_moduleGroup_effFLEXtemp; LB;moduleGoup;" + label_effFLEXtemp,
                               nbins_LB, min_LB, max_LB, nbins_moduleGroup, min_moduleGroup, max_moduleGroup));
    yaxis = m_hist_LB_moduleGroup_effFLEXtemp[ii]->GetYaxis();
    for (int jj = 0; jj < NUMMODULEGROUP; jj++) {
      yaxis->SetBinLabel(jj + 1, moduleGroup[jj]);
    }

    sc = dcsExpert.regHist(m_hist_LB_moduleGroup_thermalFigureMerit[ii] = TProfile2D_LW::create(
                               Form("LB_moduleGroup_thermalFigureMerit_S%02dvsLB", ii + 1), "LB_moduleGroup_thermalFigureMerit; LB;moduleGoup;" + label_tfm,
                               nbins_LB, min_LB, max_LB, nbins_moduleGroup, min_moduleGroup, max_moduleGroup));
    yaxis = m_hist_LB_moduleGroup_thermalFigureMerit[ii]->GetYaxis();
    for (int jj = 0; jj < NUMMODULEGROUP; jj++) {
      yaxis->SetBinLabel(jj + 1, moduleGroup[jj]);
    }

    sc = dcsExpert.regHist(m_hist_FSMstate2Dscatter[ii] = TH2F_LW::create(
                               Form("FSMstate_ModuleNumber_S%02d", ii + 1), "FSM State vs Module Number; Module Number;" + label_FSMstate + ";Number of LBs",
                               nbins_module, min_module, max_module, nbins_FSM, min_FSM, max_FSM));
    xaxis = m_hist_FSMstate2Dscatter[ii]->GetXaxis();
    for (int jj = 0; jj < NUMMODULEETA; jj++) {
      xaxis->SetBinLabel(jj + 1, moduleEta[jj]);
    }
    yaxis = m_hist_FSMstate2Dscatter[ii]->GetYaxis();
    for (int jj = 0; jj < NUMFSM; jj++) {
      yaxis->SetBinLabel(jj + 1, FSMSTATE[jj]);
    }

    sc = dcsExpert.regHist(m_hist_FSMstateLB[ii] = TH2F_LW::create(
                               Form("FSMstate_S%02dvsLB", ii + 1), "FSM State vs LB; LB;" + label_FSMstate + ";Number of Modules",
                               nbins_LB, min_LB, max_LB, nbins_FSM, min_FSM, max_FSM));
    yaxis = m_hist_FSMstateLB[ii]->GetYaxis();
    for (int jj = 0; jj < NUMFSM; jj++) {
      yaxis->SetBinLabel(jj + 1, FSMSTATE[jj]);
    }

    sc = dcsExpert.regHist(m_hist_LB_moduleGroup_FSMstate[ii] = new TH2F(
                               Form("LB_moduleGroup_FSMstate_S%02dvsLB", ii + 1), "LB_moduleGroup_FSMstate; LB;moduleGoup;" + label_FSMstate,
                               nbins_LB, min_LB, max_LB, nbins_moduleGroup, min_moduleGroup, max_moduleGroup));
    m_hist_LB_moduleGroup_FSMstate[ii]->SetMaximum(NUMFSM);
    m_hist_LB_moduleGroup_FSMstate[ii]->SetMinimum(0);
    m_hist_LB_moduleGroup_FSMstate[ii]->SetContour(NUMFSM);
    tyaxis = m_hist_LB_moduleGroup_FSMstate[ii]->GetYaxis();
    for (int jj = 0; jj < NUMMODULEGROUP; jj++) {
      tyaxis->SetBinLabel(jj + 1, moduleGroup[jj]);
    }

    sc = dcsExpert.regHist(m_hist_FSMstatus2Dscatter[ii] = TH2F_LW::create(
                               Form("FSMstatus_ModuleNumber_S%02d", ii + 1), "FSM Status vs Module Number; Module Number;" + label_FSMstatus + ";Number of LBs",
                               nbins_module, min_module, max_module, nbins_FSM, min_FSM, max_FSM));
    xaxis = m_hist_FSMstatus2Dscatter[ii]->GetXaxis();
    for (int jj = 0; jj < NUMMODULEETA; jj++) {
      xaxis->SetBinLabel(jj + 1, moduleEta[jj]);
    }
    yaxis = m_hist_FSMstatus2Dscatter[ii]->GetYaxis();
    for (int jj = 0; jj < NUMFSM; jj++) {
      yaxis->SetBinLabel(jj + 1, FSMSTATUS[jj]);
    }

    sc = dcsExpert.regHist(m_hist_FSMstatusLB[ii] = TH2F_LW::create(
                               Form("FSMstatus_S%02dvsLB", ii + 1), "FSM Status vs LB; LB;" + label_FSMstatus + ";Number of Modules",
                               nbins_LB, min_LB, max_LB, nbins_FSM, min_FSM, max_FSM));
    yaxis = m_hist_FSMstatusLB[ii]->GetYaxis();
    for (int jj = 0; jj < NUMFSM; jj++) {
      yaxis->SetBinLabel(jj + 1, FSMSTATUS[jj]);
    }

    sc = dcsExpert.regHist(m_hist_LB_moduleGroup_FSMstatus[ii] = new TH2F(
                               Form("LB_moduleGroup_FSMstatus_S%02dvsLB", ii + 1), "LB_moduleGroup_FSMstatus; LB;moduleGoup;" + label_FSMstatus,
                               nbins_LB, min_LB, max_LB, nbins_moduleGroup, min_moduleGroup, max_moduleGroup));
    m_hist_LB_moduleGroup_FSMstatus[ii]->SetMaximum(NUMFSM);
    m_hist_LB_moduleGroup_FSMstatus[ii]->SetMinimum(0);
    m_hist_LB_moduleGroup_FSMstatus[ii]->SetContour(NUMFSM);
    tyaxis = m_hist_LB_moduleGroup_FSMstatus[ii]->GetYaxis();
    for (int jj = 0; jj < NUMMODULEGROUP; jj++) {
      tyaxis->SetBinLabel(jj + 1, moduleGroup[jj]);
    }
  }

  sc = dcsExpert.regHist(m_hist_moduleTemperatureEtaPhi = TProfile2D_LW::create(
                             "moduleTemperature_EtaPhi", "Module Temperature EtaPhi; Module #eta Index;staveID;" + label_moduleTemp,
                             nbins_module, min_module, max_module, nbins_staveID, min_staveID, max_staveID));
  xaxis = m_hist_moduleTemperatureEtaPhi->GetXaxis();
  for (int jj = 0; jj < NUMMODULEETA; jj++) {
    xaxis->SetBinLabel(jj + 1, moduleEta[jj]);
  }

  sc = dcsExpert.regHist(m_hist_LB_staveID_moduleTemperature = TProfile2D_LW::create(
                             "LB_staveID_moduleTemperature", "LB_staveID_moduleTemperature; LB;staveID;" + label_moduleTemp,
                             nbins_LB, min_LB, max_LB, nbins_staveID, min_staveID, max_staveID));

  sc = dcsExpert.regHist(m_hist_HVoltageEtaPhi = TProfile2D_LW::create(
                             "HV_EtaPhi", "HV EtaPhi; Module #eta Index;staveID;" + label_HVoltage,
                             nbins_module, min_module, max_module, nbins_staveID, min_staveID, max_staveID));
  xaxis = m_hist_HVoltageEtaPhi->GetXaxis();
  for (int jj = 0; jj < NUMMODULEETA; jj++) {
    xaxis->SetBinLabel(jj + 1, moduleEta[jj]);
  }

  sc = dcsExpert.regHist(m_hist_LB_staveID_HVoltage = TProfile2D_LW::create(
                             "LB_staveID_HV", "LB_staveID_HV; LB;staveID;" + label_HVoltage,
                             nbins_LB, min_LB, max_LB, nbins_staveID, min_staveID, max_staveID));
  sc = dcsExpert.regHist(m_hist_Pipes_inlet2Dscatter = TH2F_LW::create(
                             "Pipes_inlet_temperature_StaveNumber", "Pipes_inlet_temperature vs Stave Number; Stave Number;" + label_inletTemp,
                             nbins_staveID, min_staveID, max_staveID, nbins_temperature, min_temperature, max_temperature));
  sc = dcsExpert.regHist(m_hist_LB_staveID_coolingPipeInlet = TProfile2D_LW::create(
                             "LB_staveID_coolingPipeInlet", "LB_staveID_coolingPipeInlet; LB;staveID;" + label_inletTemp,
                             nbins_LB, min_LB, max_LB, nbins_staveID, min_staveID, max_staveID));
  sc = dcsExpert.regHist(m_hist_Pipes_outlet2Dscatter = TH2F_LW::create(
                             "Pipes_outlet_temperature_StaveNumber", "Pipes_outlet_temperature vs Stave Number; Stave Number;" + label_outletTemp,
                             nbins_staveID, min_staveID, max_staveID, nbins_temperature, min_temperature, max_temperature));
  sc = dcsExpert.regHist(m_hist_LB_staveID_coolingPipeOutlet = TProfile2D_LW::create(
                             "LB_staveID_coolingPipeOutlet", "LB_staveID_coolingPipeOutlet; LB;staveID;" + label_outletTemp,
                             nbins_LB, min_LB, max_LB, nbins_staveID, min_staveID, max_staveID));

  sc = dcsExpert.regHist(m_hist_LVoltageEtaPhi = TProfile2D_LW::create(
                             "LV_EtaPhi", "LV EtaPhi; Module #eta Index;staveID;" + label_LVoltage,
                             nbins_module, min_module, max_module, nbins_staveID, min_staveID, max_staveID));
  xaxis = m_hist_LVoltageEtaPhi->GetXaxis();
  for (int jj = 0; jj < NUMMODULEETA; jj++) {
    xaxis->SetBinLabel(jj + 1, moduleEta[jj]);
  }
  sc = dcsExpert.regHist(m_hist_LB_staveID_LVoltage = TProfile2D_LW::create(
                             "LB_staveID_LV", "LB_staveID_LV; LB;staveID;" + label_LVoltage,
                             nbins_LB, min_LB, max_LB, nbins_staveID, min_staveID, max_staveID));

  sc = dcsExpert.regHist(m_hist_LVcurrentEtaPhi = TProfile2D_LW::create(
                             "LVcurrent_EtaPhi", "LV current EtaPhi; Module #eta Index;staveID;" + label_LVcurrent,
                             nbins_module, min_module, max_module, nbins_staveID, min_staveID, max_staveID));
  xaxis = m_hist_LVcurrentEtaPhi->GetXaxis();
  for (int jj = 0; jj < NUMMODULEETA; jj++) {
    xaxis->SetBinLabel(jj + 1, moduleEta[jj]);
  }
  sc = dcsExpert.regHist(m_hist_LB_staveID_LVcurrent = TProfile2D_LW::create(
                             "LB_staveID_LVcurrent", "LB_staveID_LVcurrent; LB;staveID;" + label_LVcurrent,
                             nbins_LB, min_LB, max_LB, nbins_staveID, min_staveID, max_staveID));

  sc = dcsExpert.regHist(m_hist_HVcurrentEtaPhi = TProfile2D_LW::create(
                             "HVcurrent_EtaPhi", "HV current EtaPhi; Module #eta Index;staveID;" + label_HVcurrent,
                             nbins_module, min_module, max_module, nbins_staveID, min_staveID, max_staveID));
  xaxis = m_hist_HVcurrentEtaPhi->GetXaxis();
  for (int jj = 0; jj < NUMMODULEETA; jj++) {
    xaxis->SetBinLabel(jj + 1, moduleEta[jj]);
  }

  sc = dcsExpert.regHist(m_hist_LB_staveID_HVcurrent = TProfile2D_LW::create(
                             "LB_staveID_HVcurrent", "LB_staveID_HVcurrent; LB;staveID;" + label_HVcurrent,
                             nbins_LB, min_LB, max_LB, nbins_staveID, min_staveID, max_staveID));
  sc = dcsExpert.regHist(m_hist_FSMstateEtaPhi = new TH2F(
                             "FSMstate_EtaPhi", "FSMstate EtaPhi; Module #eta Index;staveID;" + label_FSMstate,
                             nbins_module, min_module, max_module, nbins_staveID, min_staveID, max_staveID));
  m_hist_FSMstateEtaPhi->SetMaximum(NUMFSM);
  m_hist_FSMstateEtaPhi->SetMinimum(0);
  m_hist_FSMstateEtaPhi->SetContour(NUMFSM);
  txaxis = m_hist_FSMstateEtaPhi->GetXaxis();
  for (int jj = 0; jj < NUMMODULEETA; jj++) {
    //xaxis->SetBinLabel(jj+1, moduleEta[jj]);
    txaxis->SetBinLabel(jj + 1, moduleEta[jj]);
  }

  sc = dcsExpert.regHist(m_hist_LB_staveID_FSMstate = new TH2F(
                             "LB_staveID_FSMstate", "LB_staveID_FSMstate; LB;staveID;" + label_FSMstate,
                             nbins_LB, min_LB, max_LB, nbins_staveID, min_staveID, max_staveID));
  m_hist_LB_staveID_FSMstate->SetMaximum(NUMFSM);
  m_hist_LB_staveID_FSMstate->SetMinimum(0);
  m_hist_LB_staveID_FSMstate->SetContour(NUMFSM);

  sc = dcsExpert.regHist(m_hist_FSMstatusEtaPhi = new TH2F(
                             "FSMstatus_EtaPhi", "FSMstatus EtaPhi; Module #eta Index;staveID;" + label_FSMstatus,
                             nbins_module, min_module, max_module, nbins_staveID, min_staveID, max_staveID));
  m_hist_FSMstatusEtaPhi->SetMaximum(NUMFSM);
  m_hist_FSMstatusEtaPhi->SetMinimum(0);
  m_hist_FSMstatusEtaPhi->SetContour(NUMFSM);
  txaxis = m_hist_FSMstatusEtaPhi->GetXaxis();
  for (int jj = 0; jj < NUMMODULEETA; jj++) {
    txaxis->SetBinLabel(jj + 1, moduleEta[jj]);
  }

  sc = dcsExpert.regHist(m_hist_LB_staveID_FSMstatus = new TH2F(
                             "LB_staveID_FSMstatus", "LB_staveID_FSMstatus; LB;staveID;" + label_FSMstatus,
                             nbins_LB, min_LB, max_LB, nbins_staveID, min_staveID, max_staveID));
  m_hist_LB_staveID_FSMstatus->SetMaximum(NUMFSM);
  m_hist_LB_staveID_FSMstatus->SetMinimum(0);
  m_hist_LB_staveID_FSMstatus->SetContour(NUMFSM);

  sc = dcsExpert.regHist(m_hist_dTEtaPhi = TProfile2D_LW::create(
                             "dT_EtaPhi", "dT EtaPhi; Module #eta Index;staveID;" + label_dT,
                             nbins_module, min_module, max_module, nbins_staveID, min_staveID, max_staveID));
  xaxis = m_hist_dTEtaPhi->GetXaxis();
  for (int jj = 0; jj < NUMMODULEETA; jj++) {
    xaxis->SetBinLabel(jj + 1, moduleEta[jj]);
  }

  sc = dcsExpert.regHist(m_hist_LB_staveID_dT = TProfile2D_LW::create(
                             "LB_staveID_dT", "LB_staveID_dT; LB;staveID;" + label_dT,
                             nbins_LB, min_LB, max_LB, nbins_staveID, min_staveID, max_staveID));
  sc = dcsExpert.regHist(m_hist_LVPowerConsumptionEtaPhi = TProfile2D_LW::create(
                             "LVPowerConsumption_EtaPhi", "LV Power Consumption EtaPhi; Module #eta Index;staveID;" + label_LVPC,
                             nbins_module, min_module, max_module, nbins_staveID, min_staveID, max_staveID));
  xaxis = m_hist_LVPowerConsumptionEtaPhi->GetXaxis();
  for (int jj = 0; jj < NUMMODULEETA; jj++) {
    xaxis->SetBinLabel(jj + 1, moduleEta[jj]);
  }

  sc = dcsExpert.regHist(m_hist_LB_staveID_LVPowerConsumption = TProfile2D_LW::create(
                             "LB_staveID_LVPowerConsumption", "LB_staveID_LVPowerConsumption; LB;staveID;" + label_LVPC,
                             nbins_LB, min_LB, max_LB, nbins_staveID, min_staveID, max_staveID));
  sc = dcsExpert.regHist(m_hist_HVPowerConsumptionEtaPhi = TProfile2D_LW::create(
                             "HVPowerConsumption_EtaPhi", "HV Power Consumption EtaPhi; Module #eta Index;staveID;" + label_HVPC,
                             nbins_module, min_module, max_module, nbins_staveID, min_staveID, max_staveID));
  xaxis = m_hist_HVPowerConsumptionEtaPhi->GetXaxis();
  for (int jj = 0; jj < NUMMODULEETA; jj++) {
    xaxis->SetBinLabel(jj + 1, moduleEta[jj]);
  }

  sc = dcsExpert.regHist(m_hist_LB_staveID_HVPowerConsumption = TProfile2D_LW::create(
                             "LB_staveID_HVPowerConsumption", "LB_staveID_HVPowerConsumption; LB;staveID;" + label_HVPC,
                             nbins_LB, min_LB, max_LB, nbins_staveID, min_staveID, max_staveID));
  sc = dcsExpert.regHist(m_hist_LVHVPowerConsumptionEtaPhi = TProfile2D_LW::create(
                             "LVHVPowerConsumption_EtaPhi", "LV+HV Power Consumption EtaPhi; Module #eta Index;staveID;" + label_LVHVPC,
                             nbins_module, min_module, max_module, nbins_staveID, min_staveID, max_staveID));
  xaxis = m_hist_LVHVPowerConsumptionEtaPhi->GetXaxis();
  for (int jj = 0; jj < NUMMODULEETA; jj++) {
    xaxis->SetBinLabel(jj + 1, moduleEta[jj]);
  }

  sc = dcsExpert.regHist(m_hist_LB_staveID_LVHVPowerConsumption = TProfile2D_LW::create(
                             "LB_staveID_LVHVPowerConsumption", "LB_staveID_LVHVPowerConsumption; LB;staveID;" + label_LVHVPC,
                             nbins_LB, min_LB, max_LB, nbins_staveID, min_staveID, max_staveID));
  sc = dcsExpert.regHist(m_hist_effFLEXtempEtaPhi = TProfile2D_LW::create(
                             "effFLEXtemp_EtaPhi", "Effective FLEX Temperature EtaPhi; Module #eta Index;staveID;" + label_effFLEXtemp,
                             nbins_module, min_module, max_module, nbins_staveID, min_staveID, max_staveID));
  xaxis = m_hist_effFLEXtempEtaPhi->GetXaxis();
  for (int jj = 0; jj < NUMMODULEETA; jj++) {
    xaxis->SetBinLabel(jj + 1, moduleEta[jj]);
  }

  sc = dcsExpert.regHist(m_hist_LB_staveID_effFLEXtemp = TProfile2D_LW::create(
                             "LB_staveID_effFLEXtemp", "LB_staveID_effFLEXtemp; LB;staveID;" + label_effFLEXtemp,
                             nbins_LB, min_LB, max_LB, nbins_staveID, min_staveID, max_staveID));
  sc = dcsExpert.regHist(m_hist_thermalFigureMeritEtaPhi = TProfile2D_LW::create(
                             "thermalFigureMerit_EtaPhi", "Thermal Figure of Merit EtaPhi; Module #eta Index;staveID;" + label_tfm,
                             nbins_module, min_module, max_module, nbins_staveID, min_staveID, max_staveID));
  xaxis = m_hist_thermalFigureMeritEtaPhi->GetXaxis();
  for (int jj = 0; jj < NUMMODULEETA; jj++) {
    xaxis->SetBinLabel(jj + 1, moduleEta[jj]);
  }

  sc = dcsExpert.regHist(m_hist_LB_staveID_thermalFigureMerit = TProfile2D_LW::create(
                             "LB_staveID_thermalFigureMerit", "LB_staveID_thermalFigureMerit; LB;staveID;" + label_tfm,
                             nbins_LB, min_LB, max_LB, nbins_staveID, min_staveID, max_staveID));

  if (sc.isFailure() && msgLvl(MSG::WARNING)) {
    msg(MSG::WARNING) << "histograms not booked" << endmsg;
  }
  return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::fillPixelDCSMon(void) {
  // see only the first event of LB
  if (m_lumiBlockNum != m_currentLumiBlockNumber) {
    ATH_MSG_DEBUG("Current LB Number has been changed from " << m_currentLumiBlockNumber << " to " << m_lumiBlockNum);
    m_currentLumiBlockNumber = m_lumiBlockNum;
  } else {
    return StatusCode::SUCCESS;
  }

  msg(MSG::DEBUG) << "[FillPixelDCSMon]" << endmsg;

  // loop over DCS directories
  const CondAttrListCollection* atrlistcol = nullptr;
  const CondAttrListCollection* atrlistcol_temperature;
  const CondAttrListCollection* atrlistcol_hv;
  const CondAttrListCollection* atrlistcol_hvcurrent;
  const CondAttrListCollection* atrlistcol_pipes;
  const CondAttrListCollection* atrlistcol_lv;
  const CondAttrListCollection* atrlistcol_fsmstate;
  const CondAttrListCollection* atrlistcol_fsmstatus;

  // m_atrcollist is initialised in PixelMainMon.cxx, containing DCS folder names (e.g. /PIXEL/DCS/TEMPERATURE)
  for (std::vector<std::string>::const_iterator itr = m_atrcollist.begin(); itr != m_atrcollist.end(); ++itr) {
    ATH_MSG_DEBUG("execute(): Reading the data from " << *itr);
    StatusCode sc = StatusCode::FAILURE;
    if (*itr == "/PIXEL/DCS/TEMPERATURE") {
      sc = detStore()->retrieve(atrlistcol_temperature, *itr);
      atrlistcol = atrlistcol_temperature;
    } else if (*itr == "/PIXEL/DCS/HV") {
      sc = detStore()->retrieve(atrlistcol_hv, *itr);
      atrlistcol = atrlistcol_hv;
    } else if (*itr == "/PIXEL/DCS/HVCURRENT") {
      sc = detStore()->retrieve(atrlistcol_hvcurrent, *itr);
      atrlistcol = atrlistcol_hvcurrent;
    } else if (*itr == "/PIXEL/DCS/PIPES") {
      sc = detStore()->retrieve(atrlistcol_pipes, *itr);
      atrlistcol = atrlistcol_pipes;
    } else if (*itr == "/PIXEL/DCS/LV") {
      sc = detStore()->retrieve(atrlistcol_lv, *itr);
      atrlistcol = atrlistcol_lv;
    } else if (*itr == "/PIXEL/DCS/FSMSTATE") {
      sc = detStore()->retrieve(atrlistcol_fsmstate, *itr);
      atrlistcol = atrlistcol_fsmstate;
    } else if (*itr == "/PIXEL/DCS/FSMSTATUS") {
      sc = detStore()->retrieve(atrlistcol_fsmstatus, *itr);
      atrlistcol = atrlistcol_fsmstatus;
    }
    if (sc == StatusCode::SUCCESS && atrlistcol != nullptr) {
      for (CondAttrListCollection::const_iterator citr = atrlistcol->begin(); citr != atrlistcol->end(); ++citr) {
        // the following code dumps the attribute list into a string for printing
        // to access individual elements by name, use e.g.
        // float var1=(((*citr).second)["T04"]).data<float>();
        // to get the value of a float column called T04 into var1
        float tmp_value = 0.;
        int chanNum = 0;
        try {
          auto& value = (*citr).second;
          chanNum = (*citr).first;
          if (*itr == "/PIXEL/DCS/TEMPERATURE") {
            if (m_moduleTemperature->m_values->find(chanNum) == m_moduleTemperature->m_values->end()) {
              m_moduleTemperature->m_values->insert(std::make_pair(chanNum, new std::map<int, float>));
            }
            auto itr_moduleTemp = m_moduleTemperature->m_values->find(chanNum);
            tmp_value = value["temperature"].data<float>();
            itr_moduleTemp->second->insert(std::make_pair(m_currentLumiBlockNumber, tmp_value));
          } else if (*itr == "/PIXEL/DCS/PIPES") {
            if (m_coolingPipeTemperatureInlet->m_values->find(chanNum) == m_coolingPipeTemperatureInlet->m_values->end()) {
              m_coolingPipeTemperatureInlet->m_values->insert(std::make_pair(chanNum, new std::map<int, float>));
            }
            tmp_value = value["temp_inlet"].data<float>();
            auto itr_coolingPipeTempInlet = m_coolingPipeTemperatureInlet->m_values->find(chanNum);
            itr_coolingPipeTempInlet->second->insert(std::make_pair(m_currentLumiBlockNumber, tmp_value));
            if (m_coolingPipeTemperatureOutlet->m_values->find(chanNum) == m_coolingPipeTemperatureOutlet->m_values->end()) {
              m_coolingPipeTemperatureOutlet->m_values->insert(std::make_pair(chanNum, new std::map<int, float>));
            }
            tmp_value = value["temp_outlet"].data<float>();
            auto itr_coolingPipeTempOutlet = m_coolingPipeTemperatureOutlet->m_values->find(chanNum);
            itr_coolingPipeTempOutlet->second->insert(std::make_pair(m_currentLumiBlockNumber, tmp_value));
          } else if (*itr == "/PIXEL/DCS/HV") {
            if (m_HV->m_values->find(chanNum) == m_HV->m_values->end()) {
              m_HV->m_values->insert(std::make_pair(chanNum, new std::map<int, float>));
            }
            tmp_value = value["HV"].data<float>();
            auto itr_HV = m_HV->m_values->find(chanNum);
            itr_HV->second->insert(std::make_pair(m_currentLumiBlockNumber, tmp_value));
          } else if (*itr == "/PIXEL/DCS/HVCURRENT") {
            if (m_HV_current->m_values->find(chanNum) == m_HV_current->m_values->end()) {
              m_HV_current->m_values->insert(std::make_pair(chanNum, new std::map<int, float>));
            }
            tmp_value = value["hv_current"].data<float>();
            auto itr_HV_current = m_HV_current->m_values->find(chanNum);
            itr_HV_current->second->insert(std::make_pair(m_currentLumiBlockNumber, tmp_value));
          } else if (*itr == "/PIXEL/DCS/LV") {
            if (m_LV_voltage->m_values->find(chanNum) == m_LV_voltage->m_values->end()) {
              m_LV_voltage->m_values->insert(std::make_pair(chanNum, new std::map<int, float>));
            }
            tmp_value = value["lv_voltage"].data<float>();
            auto itr_LV_voltage = m_LV_voltage->m_values->find(chanNum);
            itr_LV_voltage->second->insert(std::make_pair(m_currentLumiBlockNumber, tmp_value));
            if (m_LV_current->m_values->find(chanNum) == m_LV_current->m_values->end()) {
              m_LV_current->m_values->insert(std::make_pair(chanNum, new std::map<int, float>));
            }
            tmp_value = value["lv_current"].data<float>();
            auto itr_LV_current = m_LV_current->m_values->find(chanNum);
            itr_LV_current->second->insert(std::make_pair(m_currentLumiBlockNumber, tmp_value));
          } else if (*itr == "/PIXEL/DCS/FSMSTATE") {
            if (m_FSM_state->m_values->find(chanNum) == m_FSM_state->m_values->end()) {
              m_FSM_state->m_values->insert(std::make_pair(chanNum, new std::map<int, float>));
            }
            try {
              tmp_value = m_fsmState2enum.at(value["FSM_state"].data<std::string>());
            } catch (...) {
              tmp_value = 10;  // Unknown key
              ATH_MSG_WARNING("FSM State: " << value["FSM_state"].data<std::string>());
            }
            auto itr_FSM_state = m_FSM_state->m_values->find(chanNum);
            itr_FSM_state->second->insert(std::make_pair(m_currentLumiBlockNumber, tmp_value));
          } else if (*itr == "/PIXEL/DCS/FSMSTATUS") {
            if (m_FSM_status->m_values->find(chanNum) == m_FSM_status->m_values->end()) {
              m_FSM_status->m_values->insert(std::make_pair(chanNum, new std::map<int, float>));
            }
            try {
              tmp_value = m_fsmStatus2enum.at(value["FSM_status"].data<std::string>());
            } catch (...) {
              tmp_value = 10;  // Unknown key
              ATH_MSG_WARNING("FSM Status: " << value["FSM_status"].data<std::string>());
            }
            auto itr_FSM_status = m_FSM_status->m_values->find(chanNum);
            itr_FSM_status->second->insert(std::make_pair(m_currentLumiBlockNumber, tmp_value));
          }
        } catch (...) {
          ATH_MSG_WARNING("Channel " << chanNum << " does not have any values!");
          continue;
        }
      }  // end for loop over
    } else {
      ATH_MSG_WARNING("Could not retrieve CondAttrListCollection " << *itr);
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::procPixelDCSMon(void) {
  msg(MSG::DEBUG) << "[ProcPixelDCSMon]" << endmsg;

  //---------------------------------------
  // fill dcs data in moduleDcsDataHolder
  //--------------------------------------
  int LB = 0;
  float value = 0;
  for (const auto& nameMap : *(m_moduleDCSDataHolder->m_moduleMap)) {
    if (nameMap.first == "") {
      ATH_MSG_WARNING("ProcPixelDCSMon(): Channel " << nameMap.second << " does not have module name");
      continue;
    }
    std::string moduleName = nameMap.first;
    int moduleNum = nameMap.second;
    if (moduleName.find("LI_") == std::string::npos ||
        moduleName.find("LI_S15") != std::string::npos) continue;  // if not IBL
    int staveNum = PixelMainMon::getIBLstaveIdFromModuleName(moduleName);
    int moduleGroup = PixelMainMon::getIBLmoduleGroupFromModuleName(moduleName);
    if (m_moduleDCSDataHolder->m_tempModule->find(moduleNum) == m_moduleDCSDataHolder->m_tempModule->end()) {
      m_moduleDCSDataHolder->m_tempModule->insert(std::make_pair(moduleNum, new std::map<int, float>));
    }
    if (m_moduleDCSDataHolder->m_hv_voltage->find(moduleNum) == m_moduleDCSDataHolder->m_hv_voltage->end()) {
      m_moduleDCSDataHolder->m_hv_voltage->insert(std::make_pair(moduleNum, new std::map<int, float>));
    }
    if (m_moduleDCSDataHolder->m_fsm_state->find(moduleNum) == m_moduleDCSDataHolder->m_fsm_state->end()) {
      m_moduleDCSDataHolder->m_fsm_state->insert(std::make_pair(moduleNum, new std::map<int, float>));
    }
    if (m_moduleDCSDataHolder->m_fsm_status->find(moduleNum) == m_moduleDCSDataHolder->m_fsm_status->end()) {
      m_moduleDCSDataHolder->m_fsm_status->insert(std::make_pair(moduleNum, new std::map<int, float>));
    }

    if (m_moduleTemperature->m_values->find(moduleNum) != m_moduleTemperature->m_values->end()) {
      for (const auto& valueMap : *(m_moduleTemperature->m_values->at(moduleNum))) {
        try {
          LB = valueMap.first;
          value = valueMap.second;
          auto itr_tempModule = m_moduleDCSDataHolder->m_tempModule->find(moduleNum);
          itr_tempModule->second->insert(std::make_pair(LB, value));
        } catch (const std::out_of_range& oor) {
          ATH_MSG_DEBUG("Out of Range error: " << oor.what() << " , m_moduleTemperature: chanNum = " << moduleNum);
          continue;
        }
      }
    }

    if (m_HV->m_values->find(moduleNum) != m_HV->m_values->end()) {
      for (const auto& valueMap : *(m_HV->m_values->at(moduleNum))) {
        try {
          LB = valueMap.first;
          value = valueMap.second;
          auto itr_hv_voltage = m_moduleDCSDataHolder->m_hv_voltage->find(moduleNum);
          itr_hv_voltage->second->insert(std::make_pair(LB, value));
        } catch (const std::out_of_range& oor) {
          ATH_MSG_DEBUG("Out of Range error: " << oor.what() << " , m_HV: chanNum = " << moduleNum);
          continue;
        }
      }
    }

    if (m_FSM_state->m_values->find(moduleNum) != m_FSM_state->m_values->end()) {
      for (const auto& valueMap : *(m_FSM_state->m_values->at(moduleNum))) {
        try {
          LB = valueMap.first;
          value = valueMap.second;
          auto itr_fsm_state = m_moduleDCSDataHolder->m_fsm_state->find(moduleNum);
          itr_fsm_state->second->insert(std::make_pair(LB, value));
        } catch (const std::out_of_range& oor) {
          ATH_MSG_DEBUG("Out of Range error: " << oor.what() << " , m_FSM_state: chanNum = " << moduleNum);
          continue;
        }
      }
    }

    if (m_FSM_status->m_values->find(moduleNum) != m_FSM_status->m_values->end()) {
      for (const auto& valueMap : *(m_FSM_status->m_values->at(moduleNum))) {
        try {
          LB = valueMap.first;
          value = valueMap.second;
          auto itr_fsm_status = m_moduleDCSDataHolder->m_fsm_status->find(moduleNum);
          itr_fsm_status->second->insert(std::make_pair(LB, value));
        } catch (const std::out_of_range& oor) {
          ATH_MSG_DEBUG("Out of Range error: " << oor.what() << " , m_FSM_status: chanNum = " << moduleNum);
          continue;
        }
      }
    }

    if (m_moduleDCSDataHolder->m_tempInlet->find(moduleNum) == m_moduleDCSDataHolder->m_tempInlet->end()) {
      m_moduleDCSDataHolder->m_tempInlet->insert(std::make_pair(moduleNum, new std::map<int, float>));
    }
    if (m_moduleDCSDataHolder->m_tempOutlet->find(moduleNum) == m_moduleDCSDataHolder->m_tempOutlet->end()) {
      m_moduleDCSDataHolder->m_tempOutlet->insert(std::make_pair(moduleNum, new std::map<int, float>));
    }
    for (const auto& pipeNameMap : *(m_coolingPipeTemperatureInlet->m_maps)) {
      std::string pipeName = pipeNameMap.first;
      int chanNum = pipeNameMap.second;
      if (staveNum != PixelMainMon::getIBLstaveIdFromModuleName(pipeName)) continue;
      if (m_coolingPipeTemperatureInlet->m_values->find(chanNum) != m_coolingPipeTemperatureInlet->m_values->end()) {
        for (const auto& valueMap : *(m_coolingPipeTemperatureInlet->m_values->at(chanNum))) {
          try {
            LB = valueMap.first;
            value = valueMap.second;
            auto itr_tempInlet = m_moduleDCSDataHolder->m_tempInlet->find(moduleNum);
            itr_tempInlet->second->insert(std::make_pair(LB, value));
          } catch (const std::out_of_range& oor) {
            ATH_MSG_DEBUG("Out of Range error: " << oor.what() << " , m_coolingPipeTemperatureInlet: chanNum = " << chanNum);
            continue;
          }
        }
      }
      if (m_coolingPipeTemperatureOutlet->m_values->find(chanNum) != m_coolingPipeTemperatureOutlet->m_values->end()) {
        for (const auto& valueMap : *(m_coolingPipeTemperatureOutlet->m_values->at(chanNum))) {
          try {
            LB = valueMap.first;
            value = valueMap.second;
            auto itr_tempOutlet = m_moduleDCSDataHolder->m_tempOutlet->find(moduleNum);
            itr_tempOutlet->second->insert(std::make_pair(LB, value));
          } catch (const std::out_of_range& oor) {
            ATH_MSG_DEBUG("Out of Range error: " << oor.what() << " , m_coolingPipeTemperatureOutlet: chanNum = " << chanNum);
            continue;
          }
        }
      }
    }

    if (m_moduleDCSDataHolder->m_lv_voltage->find(moduleNum) == m_moduleDCSDataHolder->m_lv_voltage->end()) {
      m_moduleDCSDataHolder->m_lv_voltage->insert(std::make_pair(moduleNum, new std::map<int, float>));
    }
    if (m_moduleDCSDataHolder->m_lv_current->find(moduleNum) == m_moduleDCSDataHolder->m_lv_current->end()) {
      m_moduleDCSDataHolder->m_lv_current->insert(std::make_pair(moduleNum, new std::map<int, float>));
    }
    if (m_moduleDCSDataHolder->m_hv_current->find(moduleNum) == m_moduleDCSDataHolder->m_hv_current->end()) {
      m_moduleDCSDataHolder->m_hv_current->insert(std::make_pair(moduleNum, new std::map<int, float>));
    }
    for (const auto& lvNameMap : *(m_LV_voltage->m_maps)) {
      std::string lvName = lvNameMap.first;
      int chanNum = lvNameMap.second;
      if (staveNum != PixelMainMon::getIBLstaveIdFromModuleName(lvName) || moduleGroup != PixelMainMon::getIBLmoduleGroupFromModuleName(lvName)) continue;
      if (m_LV_voltage->m_values->find(chanNum) != m_LV_voltage->m_values->end()) {
        for (const auto& valueMap : *(m_LV_voltage->m_values->at(chanNum))) {
          try {
            LB = valueMap.first;
            value = valueMap.second;
            auto itr_lv_voltage = m_moduleDCSDataHolder->m_lv_voltage->find(moduleNum);
            itr_lv_voltage->second->insert(std::make_pair(LB, value));
          } catch (const std::out_of_range& oor) {
            ATH_MSG_DEBUG("Out of Range error: " << oor.what() << " , m_LV_voltage: chanNum = " << chanNum);
            continue;
          }
        }
      }

      if (m_LV_current->m_values->find(chanNum) != m_LV_current->m_values->end()) {
        for (const auto& valueMap : *(m_LV_current->m_values->at(chanNum))) {
          try {
            LB = valueMap.first;
            value = valueMap.second;
            auto itr_lv_current = m_moduleDCSDataHolder->m_lv_current->find(moduleNum);
            itr_lv_current->second->insert(std::make_pair(LB, value));
          } catch (const std::out_of_range& oor) {
            ATH_MSG_DEBUG("Out of Range error: " << oor.what() << " , m_LV_current: chanNum = " << chanNum);
            continue;
          }
        }
      }

      if (m_HV_current->m_values->find(chanNum) != m_HV_current->m_values->end()) {
        for (const auto& valueMap : *(m_HV_current->m_values->at(chanNum))) {
          try {
            LB = valueMap.first;
            value = valueMap.second;
            auto itr_hv_current = m_moduleDCSDataHolder->m_hv_current->find(moduleNum);
            itr_hv_current->second->insert(std::make_pair(LB, value));
          } catch (const std::out_of_range& oor) {
            ATH_MSG_DEBUG("Out of Range error: " << oor.what() << " , m_HV_current: chanNum = " << chanNum);
            continue;
          }
        }
      }
    }  // end lv/hv
  }    // end fill moduleDcsDataHolder

  //----------------------------
  // loop over modules
  //---------------------------
  for (const auto& nameMap : *(m_moduleDCSDataHolder->m_moduleMap)) {
    // nameMap.first is module name. nameMap.second is channel number.
    if (nameMap.first == "") {
      ATH_MSG_WARNING("ProcPixelDCSMon(): Channel " << nameMap.second << " does not have module name");
      continue;
    }
    std::string moduleName = nameMap.first;
    int moduleNum = nameMap.second;
    if (moduleName.find("LI_") == std::string::npos || moduleName.find("LI_S15") != std::string::npos) {
      continue;  // if not IBL
    }
    int staveNum = PixelMainMon::getIBLstaveIdFromModuleName(moduleName);
    int module_eta = PixelMainMon::getIBLetaIndexFromModuleName(moduleName);
    int moduleGroup = PixelMainMon::getIBLmoduleGroupFromModuleName(moduleName);
    try {
      for (const auto& valueMap : *(m_moduleDCSDataHolder->m_tempModule->at(moduleNum))) {
        int LB = valueMap.first;
        float tempModule = valueMap.second;
        float tempInlet = m_moduleDCSDataHolder->m_tempInlet->at(moduleNum)->at(LB);
        float tempOutlet = m_moduleDCSDataHolder->m_tempOutlet->at(moduleNum)->at(LB);
        float hv_voltage = TMath::Abs(m_moduleDCSDataHolder->m_hv_voltage->at(moduleNum)->at(LB));
        float lv_voltage = m_moduleDCSDataHolder->m_lv_voltage->at(moduleNum)->at(LB);
        float hv_current = TMath::Abs(m_moduleDCSDataHolder->m_hv_current->at(moduleNum)->at(LB));
        float lv_current = m_moduleDCSDataHolder->m_lv_current->at(moduleNum)->at(LB);
        float fsm_state = m_moduleDCSDataHolder->m_fsm_state->at(moduleNum)->at(LB);
        if (fsm_state > m_fsmState2enum.size()) {
          fsm_state = m_fsmState2enum["READY"];
        }
        float fsm_status = m_moduleDCSDataHolder->m_fsm_status->at(moduleNum)->at(LB);
        if (fsm_status > m_fsmState2enum.size()) {
          fsm_status = m_fsmStatus2enum["OK"];
        }
        m_hist_moduleTemperatureEtaPhi->Fill(module_eta, staveNum, tempModule);
        m_hist_moduleTemperature2Dscatter[staveNum - 1]->Fill(module_eta, tempModule);
        m_hist_moduleTemperatureLB[staveNum - 1]->Fill(LB, tempModule);
        m_hist_LB_staveID_moduleTemperature->Fill(LB, staveNum, tempModule);
        m_hist_LB_moduleGroup_moduleTemperature[staveNum - 1]->Fill(LB, moduleGroup, tempModule);
        float dT = tempModule - tempOutlet;
        m_hist_dTEtaPhi->Fill(module_eta, staveNum, dT);
        m_hist_LB_staveID_dT->Fill(LB, staveNum, dT);
        m_hist_LB_moduleGroup_dT[staveNum - 1]->Fill(LB, moduleGroup, dT);

        m_hist_Pipes_inlet2Dscatter->Fill(staveNum, tempInlet);
        m_hist_Pipes_inletLB[staveNum - 1]->Fill(LB, tempInlet);
        m_hist_LB_staveID_coolingPipeInlet->Fill(LB, staveNum, tempInlet);

        m_hist_Pipes_outlet2Dscatter->Fill(staveNum, tempOutlet);
        m_hist_Pipes_outletLB[staveNum - 1]->Fill(LB, tempOutlet);
        m_hist_LB_staveID_coolingPipeOutlet->Fill(LB, staveNum, tempOutlet);

        m_hist_HVoltageEtaPhi->Fill(module_eta, staveNum, hv_voltage);
        m_hist_HVoltage2Dscatter[staveNum - 1]->Fill(module_eta, hv_voltage);
        m_hist_HVoltageLB[staveNum - 1]->Fill(LB, hv_voltage);
        m_hist_LB_staveID_HVoltage->Fill(LB, staveNum, hv_voltage);
        m_hist_LB_moduleGroup_HVoltage[staveNum - 1]->Fill(LB, moduleGroup, hv_voltage);

        m_hist_HVcurrentEtaPhi->Fill(module_eta, staveNum, hv_current);
        m_hist_HVcurrent2Dscatter[staveNum - 1]->Fill(module_eta, hv_current);
        m_hist_HVcurrentLB[staveNum - 1]->Fill(LB, hv_current);
        m_hist_LB_moduleGroup_HVcurrent[staveNum - 1]->Fill(LB, moduleGroup, hv_current);
        m_hist_LB_staveID_HVcurrent->Fill(LB, staveNum, hv_current);

        m_hist_LVoltageEtaPhi->Fill(module_eta, staveNum, lv_voltage);
        m_hist_LVoltage2Dscatter[staveNum - 1]->Fill(module_eta, lv_voltage);
        m_hist_LVoltageLB[staveNum - 1]->Fill(LB, lv_voltage);
        m_hist_LB_staveID_LVoltage->Fill(LB, staveNum, lv_voltage);
        m_hist_LB_moduleGroup_LVoltage[staveNum - 1]->Fill(LB, moduleGroup, lv_voltage);

        m_hist_LVcurrentEtaPhi->Fill(module_eta, staveNum, lv_current);
        m_hist_LVcurrent2Dscatter[staveNum - 1]->Fill(module_eta, lv_current);
        m_hist_LVcurrentLB[staveNum - 1]->Fill(LB, lv_current);
        m_hist_LB_moduleGroup_LVcurrent[staveNum - 1]->Fill(LB, moduleGroup, lv_current);
        m_hist_LB_staveID_LVcurrent->Fill(LB, staveNum, lv_current);

        if (fsm_state > m_hist_FSMstateEtaPhi->GetBinContent(module_eta + 11, staveNum)) {  // -10 <= module_eta <= 9,
          m_hist_FSMstateEtaPhi->SetBinContent(module_eta + 11, staveNum, fsm_state);       // 1 <= staveNum <= 14
        }
        m_hist_FSMstate2Dscatter[staveNum - 1]->Fill(module_eta, fsm_state);
        m_hist_FSMstateLB[staveNum - 1]->Fill(LB, fsm_state);
        if (fsm_state > m_hist_LB_staveID_FSMstate->GetBinContent(LB + 1, staveNum)) {
          m_hist_LB_staveID_FSMstate->SetBinContent(LB + 1, staveNum, fsm_state);
        }
        if (fsm_state > m_hist_LB_moduleGroup_FSMstate[staveNum - 1]->GetBinContent(LB + 1, moduleGroup + 1)) {
          m_hist_LB_moduleGroup_FSMstate[staveNum - 1]->SetBinContent(LB + 1, moduleGroup + 1, fsm_state);
        }

        if (fsm_status > m_hist_FSMstatusEtaPhi->GetBinContent(module_eta + 11, staveNum)) {  // -10 <= module_eta <= 9,
          m_hist_FSMstatusEtaPhi->SetBinContent(module_eta + 11, staveNum, fsm_status);       // 1 <= staveNum <= 14
        }
        m_hist_FSMstatus2Dscatter[staveNum - 1]->Fill(module_eta, fsm_status);
        m_hist_FSMstatusLB[staveNum - 1]->Fill(LB, fsm_status);
        if (fsm_status > m_hist_LB_staveID_FSMstatus->GetBinContent(LB + 1, staveNum)) {
          m_hist_LB_staveID_FSMstatus->SetBinContent(LB + 1, staveNum, fsm_status);
        }
        if (fsm_status > m_hist_LB_moduleGroup_FSMstatus[staveNum - 1]->GetBinContent(LB + 1, moduleGroup + 1)) {
          m_hist_LB_moduleGroup_FSMstatus[staveNum - 1]->SetBinContent(LB + 1, moduleGroup + 1, fsm_status);
        }
        m_hist_LB_moduleGroup_FSMstatus[staveNum - 1]->Fill(LB, moduleGroup, fsm_status);

        float hvpc = TMath::Abs(hv_voltage * hv_current);
        m_hist_HVPowerConsumptionEtaPhi->Fill(module_eta, staveNum, hvpc);
        m_hist_LB_staveID_HVPowerConsumption->Fill(LB, staveNum, hvpc);
        m_hist_LB_moduleGroup_HVPowerConsumption[staveNum - 1]->Fill(LB, moduleGroup, hvpc);

        float lvpc = TMath::Abs(lv_voltage * lv_current);
        m_hist_LVPowerConsumptionEtaPhi->Fill(module_eta, staveNum, lvpc);
        m_hist_LB_staveID_LVPowerConsumption->Fill(LB, staveNum, lvpc);
        m_hist_LB_moduleGroup_LVPowerConsumption[staveNum - 1]->Fill(LB, moduleGroup, lvpc);

        m_hist_LVHVPowerConsumptionEtaPhi->Fill(module_eta, staveNum, lvpc + hvpc * 0.001);
        m_hist_LB_staveID_LVHVPowerConsumption->Fill(LB, staveNum, lvpc + hvpc * 0.001);
        m_hist_LB_moduleGroup_LVHVPowerConsumption[staveNum - 1]->Fill(LB, moduleGroup, lvpc + hvpc * 0.001);

        float tempFlex = dT * 0.6 + tempOutlet;  // 0.6 is measured value
        m_hist_effFLEXtempEtaPhi->Fill(module_eta, staveNum, tempFlex);
        m_hist_LB_staveID_effFLEXtemp->Fill(LB, staveNum, tempFlex);
        m_hist_LB_moduleGroup_effFLEXtemp[staveNum - 1]->Fill(LB, moduleGroup, tempFlex);

        if (lvpc + hvpc * 0.001 != 0) {
          float tfm = dT / (lvpc + hvpc * 0.001);
          m_hist_thermalFigureMeritEtaPhi->Fill(module_eta, staveNum, tfm);
          m_hist_LB_staveID_thermalFigureMerit->Fill(LB, staveNum, tfm);
          m_hist_LB_moduleGroup_thermalFigureMerit[staveNum - 1]->Fill(LB, moduleGroup, tfm);
        }
      }
    } catch (const std::out_of_range& oor) {
      ATH_MSG_DEBUG("moduleNum " << moduleNum << ", Out of Range error: " << oor.what());
      continue;
    }
  }  // end loop over modules

#if 0
  for (const auto& nameMap : *(m_moduleTemperature->m_maps)) {
    // nameMap.first is module name. nameMap.second is channel number.
    if (nameMap.first == "") {
      ATH_MSG_WARNING("ProcPixelDCSMon(): Channel " << nameMap.second << " does not have module name");
      continue;
    }
    std::string moduleName = nameMap.first;
    int chanNum = nameMap.second;
    if (moduleName.find("LI_") == std::string::npos || moduleName.find("LI_S15") != std::string::npos) {
      continue;  // if not IBL
    }
    int staveNum = PixelMainMon::getIBLstaveIdFromModuleName(moduleName);
    int module_eta = PixelMainMon::getIBLetaIndexFromModuleName(moduleName);
    int moduleGroup = PixelMainMon::getIBLmoduleGroupFromModuleName(moduleName);
    try {
      for (const auto& valueMap : *(m_moduleTemperature->m_values->at(chanNum))) {
        // valueMap.first is LB. valueMap.second is value.
        LB = valueMap.first;
        value = valueMap.second;
        m_hist_moduleTemperatureEtaPhi->Fill(module_eta, staveNum, value);
        m_hist_moduleTemperature2Dscatter[staveNum - 1]->Fill(module_eta, value);
        m_hist_moduleTemperatureLB[staveNum - 1]->Fill(LB, value);
        m_hist_LB_staveID_moduleTemperature->Fill(LB, staveNum, value);
        m_hist_LB_moduleGroup_moduleTemperature[staveNum - 1]->Fill(LB, moduleGroup, value);
        try {
          float dT = value - m_coolingPipeTemperatureOutlet->m_values->at(staveNum)->at(LB);
          m_hist_dTEtaPhi->Fill(module_eta, staveNum, dT);
          m_hist_LB_staveID_dT->Fill(LB, staveNum, dT);
          m_hist_LB_moduleGroup_dT[staveNum - 1]->Fill(LB, moduleGroup, dT);
        } catch (const std::out_of_range& oor) {
          std::cerr << "Out of Range error: " << oor.what() << " , m_coolingPipeTemperatureOutlet: chanNum = " << chanNum << '\n';
          continue;
        }
      }
    } catch (const std::out_of_range& oor) {
      std::cerr << "Out of Range error: " << oor.what() << " , m_moduleTemperature: chanNum = " << chanNum << '\n';
      continue;
    }
    try {
      for (const auto& valueMap : *(m_HV->m_values->at(chanNum))) {
        LB = valueMap.first;
        value = valueMap.second;
        m_hist_HVoltageEtaPhi->Fill(module_eta, staveNum, value);
        m_hist_HVoltage2Dscatter[staveNum - 1]->Fill(module_eta, value);
        m_hist_HVoltageLB[staveNum - 1]->Fill(LB, value);
        m_hist_LB_staveID_HVoltage->Fill(LB, staveNum, value);
        m_hist_LB_moduleGroup_HVoltage[staveNum - 1]->Fill(LB, moduleGroup, value);
      }
    } catch (const std::out_of_range& oor) {
      std::cerr << "Out of Range error: " << oor.what() << " , m_HV: chanNum = " << chanNum << '\n';
      continue;
    }
  }

  //--------------------------------
  // loop over cooling pipes
  //--------------------------------
  for (const auto& nameMap : *(m_coolingPipeTemperatureInlet->m_maps)) {
    ATH_MSG_DEBUG("ProcPixelDCSMon(): Pipe Name: " << nameMap.first << ", Channel Num: " << nameMap.second);
    if (nameMap.first == "") {
      ATH_MSG_WARNING("ProcPixelDCSMon(): Channel " << nameMap.second << " does not have pipe name");
      continue;
    }
    std::string moduleName = nameMap.first;
    int chanNum = nameMap.second;
    int staveNum = PixelMainMon::getIBLstaveIdFromModuleName(moduleName);
    try {
      std::map<int, float>* test = m_coolingPipeTemperatureInlet->m_values->at(chanNum);
      test = nullptr;  // just to avoid warning message "unused variable..."
    } catch (const std::out_of_range& oor) {
      std::cerr << "Out of Range error: " << oor.what() << " , m_coolingPipeTemperatureInlet: chanNum = " << chanNum << '\n';
      continue;
    }

    for (const auto& valueMap : *(m_coolingPipeTemperatureInlet->m_values->at(chanNum))) {
      LB = valueMap.first;
      value = valueMap.second;
      m_hist_Pipes_inlet2Dscatter->Fill(staveNum, value);
      m_hist_Pipes_inletLB[staveNum - 1]->Fill(LB, value);
      m_hist_LB_staveID_coolingPipeInlet->Fill(LB, staveNum, value);
    }
    try {
      std::map<int, float>* test = m_coolingPipeTemperatureOutlet->m_values->at(chanNum);
      test = nullptr;  // just to avoid warning message "unused variable..."
    } catch (const std::out_of_range& oor) {
      std::cerr << "Out of Range error: " << oor.what() << " , m_coolingPipeTemperatureOutlet: chanNum = " << chanNum << '\n';
      continue;
    }

    for (const auto& valueMap : *(m_coolingPipeTemperatureOutlet->m_values->at(chanNum))) {
      LB = valueMap.first;
      value = valueMap.second;
      m_hist_Pipes_outlet2Dscatter->Fill(staveNum, value);
      m_hist_Pipes_outletLB[staveNum - 1]->Fill(LB, value);
      m_hist_LB_staveID_coolingPipeOutlet->Fill(LB, staveNum, value);
    }
  }

  //--------------------
  // loop over LV
  //--------------------
  for (const auto& nameMap : *(m_LV_voltage->m_maps)) {
    ATH_MSG_DEBUG("ProcPixelDCSMon(): LV Name: " << nameMap.first << ", Channel Num: " << nameMap.second);
    if (nameMap.first == "") {
      ATH_MSG_WARNING("ProcPixelDCSMon(): Channel " << nameMap.second << " does not have LV channel name");
      continue;
    }
    std::string moduleName = nameMap.first;
    int chanNum = nameMap.second;
    int staveNum = PixelMainMon::getIBLstaveIdFromModuleName(moduleName);
    int moduleGroup = PixelMainMon::getIBLmoduleGroupFromModuleName(moduleName);
    try {
      std::map<int, float>* test = m_LV_voltage->m_values->at(chanNum);
      test = nullptr;  // just to avoid warning message "unused variable..."
    } catch (const std::out_of_range& oor) {
      std::cerr << "Out of Range error: " << oor.what() << " , m_LV_voltage: chanNum = " << chanNum << '\n';
      continue;
    }

    for (const auto& valueMap : *(m_LV_voltage->m_values->at(chanNum))) {
      LB = valueMap.first;
      value = valueMap.second;
      m_hist_LB_moduleGroup_LVoltage[staveNum - 1]->Fill(LB, moduleGroup, value);
      m_hist_LB_staveID_LVoltage->Fill(LB, staveNum, value);
      try {
        float lvpc = value * m_LV_current->m_values->at(chanNum)->at(LB);
        m_hist_LB_staveID_LVPowerConsumption->Fill(LB, staveNum, lvpc);
        m_hist_LB_moduleGroup_LVPowerConsumption[staveNum - 1]->Fill(LB, moduleGroup, lvpc);
      } catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error: " << oor.what() << " , m_coolingPipeTemperatureOutlet: chanNum = " << chanNum << '\n';
        continue;
      }
    }
    try {
      std::map<int, float>* test = m_LV_current->m_values->at(chanNum);
      test = nullptr;  // just to avoid warning message "unused variable..."
    } catch (const std::out_of_range& oor) {
      std::cerr << "Out of Range error: " << oor.what() << " , m_LV_current: chanNum = " << chanNum << '\n';
      continue;
    }

    for (const auto& valueMap : *(m_LV_current->m_values->at(chanNum))) {
      LB = valueMap.first;
      value = valueMap.second;
      m_hist_LB_moduleGroup_LVcurrent[staveNum - 1]->Fill(LB, moduleGroup, value);
      m_hist_LB_staveID_LVcurrent->Fill(LB, staveNum, value);
    }

    for (const auto& valueMap : *(m_HV_current->m_values->at(chanNum))) {
      LB = valueMap.first;
      value = valueMap.second;
      m_hist_LB_moduleGroup_HVcurrent[staveNum - 1]->Fill(LB, moduleGroup, value);
      m_hist_LB_staveID_HVcurrent->Fill(LB, staveNum, value);
    }

  }  // end loop over LV
#endif

  return StatusCode::SUCCESS;
}
