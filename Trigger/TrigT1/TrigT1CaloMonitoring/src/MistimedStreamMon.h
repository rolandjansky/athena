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
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

class TH1F_LW;

class L1CaloCondSvc;
class L1CaloRunParametersContainer;
class L1CaloReadoutConfigContainer;

class StatusCode;
class EventInfo;
class ITHistSvc;

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
    
  StatusCode retrieveConditions();
  void fillEtaPhiMap(TH2F* hist, double eta, double phi, double weight, bool shrinkEtaBins = true);
  TH2F* createEtaPhiMap(std::string name, std::string title, bool isHADLayer = false, bool shrinkEtaBins = true);
  bool pulseQuality(std::vector<uint16_t> ttPulse, int peakSlice);
   
  /// Tool to retrieve bytestream errors
  ToolHandle<ITrigT1CaloMonErrorTool>   m_errorTool;
  /// Histogram helper tool
  ToolHandle<TrigT1CaloLWHistogramTool> m_histTool;
  /// TT simulation tool for Identifiers
  ToolHandle<LVL1::IL1TriggerTowerTool> m_ttTool;
  /// Tool to retrieve the trigger decision
  ToolHandle<Trig::TrigDecisionTool> m_trigDec;

  /// Histograms booked flag
  bool m_histBooked;
  // Histograms
  
  // Overview histos
  // Overall selected events and cut flow of analysis
  TH1F_LW* m_h_1d_cutFlow_mistimedStreamAna;  
  // Selected events per lumi block
  TH1F_LW* m_h_1d_selectedEvents_mistimedStreamAna;
    
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

  //Control maximum number of histograms per job
  int m_maxHistos;
  int m_curHistos;
  
  //  Athena hist service
  ServiceHandle<ITHistSvc> m_thistSvc;
};

// ============================================================================
}  // end namespace
// ============================================================================

#endif
