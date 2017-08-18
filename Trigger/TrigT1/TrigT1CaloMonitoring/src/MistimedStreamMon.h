/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:        MistimedStreamMon.h
// PACKAGE:     TrigT1CaloMonitoring
//
// Author:      Julia I. Djuvsland (julia.djuvsland@kip.uni-heidelberg.de)
//              Sebastian M. Weber (sebastian.weber@kip.uni-heidelberg.de)
//              Universitaet Heidelberg
//
// ********************************************************************

#ifndef TRIGT1CALOMONITORING_MISTIMEDSTREAMMON_H
#define TRIGT1CALOMONITORING_MISTIMEDSTREAMMON_H

#include <string>
#include <vector>

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"

#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

class TH1F_LW;
class TH2F_LW;
class TH2I_LW;

class L1CaloCondSvc;
class L1CaloRunParametersContainer;
class L1CaloReadoutConfigContainer;

class StatusCode;
class EventInfo;

namespace Trig {
  class TrigDecisionTool;
}

// ============================================================================
namespace LVL1 {
// ============================================================================
// Forward declarations:
// ============================================================================
class IL1TriggerTowerTool;
class ITrigT1CaloMonErrorTool;
class TrigT1CaloLWHistogramTool;

/** This class monitors events that fired the trigger HLT_mistimemonj400
 *  to spot potential late or mistimed trigger towers, but will also pick up noise
 *
 *  <b>ROOT Histogram Directories (Tier0):</b>
 *
 *  <table>
 *  <tr><th> Directory                                    </th><th> Contents               </th></tr>
 *  <tr><td> @c L1Calo/MistimedStream/EventsPerLumiBlock  </td><td> Selected events per lumiblock <br>
 *  </table>
 *
 *
 *  @authors Julia I. Djuvsland, Sebastian M. Weber
 *
 * */

class MistimedStreamMon: public ManagedMonitorToolBase
{

 public:
  
  MistimedStreamMon(const std::string & type, const std::string & name,const IInterface* parent);
  virtual ~MistimedStreamMon();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  
private:
  /// Tool to retrieve bytestream errors
  ToolHandle<ITrigT1CaloMonErrorTool>     m_errorTool;
  /// Histogram helper tool
  ToolHandle<TrigT1CaloLWHistogramTool>   m_histTool;
  /// TT simulation tool for Identifiers
  ToolHandle<LVL1::IL1TriggerTowerTool>   m_ttTool;
  /// Tool to retrieve the trigger decision
  ToolHandle<Trig::TrigDecisionTool> m_trigDec;

    
  StatusCode retrieveConditions();
  void bookEventHistograms(std::string number);
  bool pulseQuality(std::vector<uint16_t> ttPulse, int peakSlice);
  
  int m_selectedEventCounter;

  /// Histograms booked flag
  bool m_histBooked;
  // Histograms
  
  // Overview histos
  // Overall selected events and cut flow of analysis
  TH1F_LW* m_h_1d_cutFlow_mistimedStreamAna;  
  // Selected events per lumi block
  TH1F_LW* m_h_1d_selectedEvents_mistimedStreamAna;
  
  // Detailed histos per selected event
  // eta-phi Map of EM TT classification
  std::vector<TH2F_LW*> m_v_em_2d_etaPhi_tt_classification_mistimedStreamAna;
  // eta-phi Map of HAD TT classification
  std::vector<TH2F_LW*> m_v_had_2d_etaPhi_tt_classification_mistimedStreamAna;
  // eta-phi Map of EM PSE bits
  std::vector<TH2F_LW*> m_v_em_2d_etaPhi_tt_pseBits_mistimedStreamAna;
  // eta-phi Map of HAD PSE bits
  std::vector<TH2F_LW*> m_v_had_2d_etaPhi_tt_pseBits_mistimedStreamAna;
  // eta-phi Map of EM lut-cp for timeslice 0 = BCID-1 
  std::vector<TH2F_LW*> m_v_em_2d_etaPhi_tt_lut0_mistimedStreamAna;
  // eta-phi Map of EM lut-cp for timeslice 1 = BCID
  std::vector<TH2F_LW*> m_v_em_2d_etaPhi_tt_lut1_mistimedStreamAna;
  // eta-phi Map of EM lut-cp for timeslice 2 = BCID+1
  std::vector<TH2F_LW*> m_v_em_2d_etaPhi_tt_lut2_mistimedStreamAna;
  // eta-phi Map of HAD lut-cp for timeslice 0 = BCID-1
  std::vector<TH2F_LW*> m_v_had_2d_etaPhi_tt_lut0_mistimedStreamAna;
  // eta-phi Map of HAD lut-cp for timeslice 1 = BCID
  std::vector<TH2F_LW*> m_v_had_2d_etaPhi_tt_lut1_mistimedStreamAna;
  // eta-phi Map of HAD lut-cp for timeslice 2 = BCID+1
  std::vector<TH2F_LW*> m_v_had_2d_etaPhi_tt_lut2_mistimedStreamAna;
  // eta-phi Map of EM lut-jep for timeslice 0 = BCID-1 
  std::vector<TH2F_LW*> m_v_em_2d_etaPhi_tt_lut_jep0_mistimedStreamAna;
  // eta-phi Map of EM lut-jep for timeslice 1 = BCID
  std::vector<TH2F_LW*> m_v_em_2d_etaPhi_tt_lut_jep1_mistimedStreamAna;
  // eta-phi Map of EM lut-jep for timeslice 2 = BCID+1
  std::vector<TH2F_LW*> m_v_em_2d_etaPhi_tt_lut_jep2_mistimedStreamAna;
  // eta-phi Map of HAD lut-jep for timeslice 0 = BCID-1
  std::vector<TH2F_LW*> m_v_had_2d_etaPhi_tt_lut_jep0_mistimedStreamAna;
  // eta-phi Map of HAD lut-jep for timeslice 1 = BCID
  std::vector<TH2F_LW*> m_v_had_2d_etaPhi_tt_lut_jep1_mistimedStreamAna;
  // eta-phi Map of HAD lut-jep for timeslice 2 = BCID+1
  std::vector<TH2F_LW*> m_v_had_2d_etaPhi_tt_lut_jep2_mistimedStreamAna;
  
  //Variables for the properties
  /// Root directory
  std::string m_PathInRootFile;
  /// xAODTriggerTower Container key
  std::string m_xAODTriggerTowerContainerName;
  /// L1Calo conditions                                                                               
  ServiceHandle<L1CaloCondSvc> m_l1CondSvc;
  /// Database container for the run parameters
  L1CaloRunParametersContainer* m_runParametersContainer;
  /// Database container for the readout configuration
  L1CaloReadoutConfigContainer* m_readoutConfigContainer;
    
};

// ============================================================================
}  // end namespace
// ============================================================================

#endif
