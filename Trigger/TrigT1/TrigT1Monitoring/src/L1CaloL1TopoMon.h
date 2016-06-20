/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:        L1CaloL1TopoMon.h
// PACKAGE:     TrigT1CaloMonitoring  
//
// AUTHOR:      Joergen Sjoelin (sjolin@fysik.su.se)
//           
// DESCRIPTION: Monitoring of L1Calo --> L1Topo transmission
//
// ********************************************************************

#ifndef L1CaloL1TopoMon_H
#define L1CaloL1TopoMon_H

#include <string>
#include <vector>

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "DataModel/DataVector.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

class LWHist;
class TH1F_LW;
class TH2F_LW;
class StatusCode;

namespace TrigConf {
  class ITrigConfigSvc;
}

namespace LVL1 {
  
// ============================================================================
class ITrigT1CaloMonErrorTool;
class TrigT1CaloLWHistogramTool;
//class FrontPanelCTP;
// ============================================================================
  
 class L1CaloL1TopoMon : public ManagedMonitorToolBase
{
 public:
  
  L1CaloL1TopoMon( const std::string & type,
		   const std::string & name,
		   const IInterface* parent ); 
  
  virtual ~L1CaloL1TopoMon();
  
  virtual StatusCode initialize();
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

  enum ERROR_BIT {CALO_CONV=0, NO_CMX, DAQ_CONV, NO_DAQ, ROI_CONV,
		  NO_ROI, F_OVERFLOW,
		  F_CRC, PAYL_CRC, CMX_EMATCH, NUMBEROFBITS};
  
  std::vector<std::string> ERROR_LABELS{"Calo conv","No CMX","DAQ conv",
      "No DAQ","ROI conv","No ROI","Fibre Overf","Fibre CRC",
      "Payload CRC","CMX ematch"};
  
 private:
  
  const int MAXTOBS=30;
  /// Trigger configuration service
   ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc;
   /// Corrupt events tool
   ToolHandle<ITrigT1CaloMonErrorTool>     m_errorTool;
   /// Histogram utilities tool
   ToolHandle<TrigT1CaloLWHistogramTool>   m_histTool;
   /// Output from L1Topo
   //const DataHandle< LVL1::FrontPanelCTP > m_topoCTP;
   StringProperty m_CMXJetTobLocation;
   StringProperty m_topoCTPLoc;

   /// Root directory
   std::string m_PathInRootFile;   
   /// Debug printout flag
   bool m_debug;
   /// Histograms booked flag
   bool m_histBooked;

   /** Histos */   
   // Data transmission checks
   TH1F_LW* m_h_l1topo_1d_CMXTobs;
   TH1F_LW* m_h_l1topo_1d_Simulation;
   TH1F_LW* m_h_l1topo_1d_JetTobs_EnergyLg;
   TH2F_LW* m_h_l1topo_2d_JetTobs_Hitmap_mismatch;
   TH2F_LW* m_h_l1topo_2d_JetTobs_Hitmap_match;
   TH1F_LW* m_h_l1topo_1d_Errors;
   TH1F_LW* m_h_l1topo_1d_DAQTobs;
   TH1F_LW* m_h_l1topo_1d_DAQJetTobs_no0;
   TH1F_LW* m_h_l1topo_1d_DAQJetTobs;
   TH1F_LW* m_h_l1topo_1d_DAQTauTobs_no0;
   TH1F_LW* m_h_l1topo_1d_DAQTauTobs;
   TH1F_LW* m_h_l1topo_1d_DAQEMTobs_no0;
   TH1F_LW* m_h_l1topo_1d_DAQEMTobs;
   TH1F_LW* m_h_l1topo_1d_DAQMuonTobs_no0;
   TH1F_LW* m_h_l1topo_1d_DAQMuonTobs;
   TH1F_LW* m_h_l1topo_1d_DAQEnergyTobs;
   TH1F_LW* m_h_l1topo_1d_DAQTriggerBits;
   TH1F_LW* m_h_l1topo_1d_DAQMismatchTriggerBits;
   TH1F_LW* m_h_l1topo_1d_DAQOverflowBits;
   TH1F_LW* m_h_l1topo_1d_ROITobs;
   TH2F*    m_h_l1topo_2d_ItemsBC[4];
};

// ============================================================================
}  // end namespace
// ============================================================================

#endif
