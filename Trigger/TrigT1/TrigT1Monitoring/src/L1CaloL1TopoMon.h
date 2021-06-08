/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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

#include "AthContainers/DataVector.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "TrigT1Interfaces/FrontPanelCTP.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"

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
		  NO_ROI, F_CRC, PAYL_CRC, CMX_MATCH, NUMBEROFBITS};

  enum TOB_TYPE {JETL_TOB=0, JETS_TOB, TAU_TOB, EM_TOB, MU_TOB};
  
  std::vector<std::string> ERROR_LABELS{"Calo conv","No CMX","DAQ conv",
      "No DAQ","ROI conv","No ROI","Fibre CRC","Payload CRC","CMX-Topo match"};
  
 private:
  
  void jem2Coord(const int crate, const int jem,
		 const int frame, const int location,
		 int &x, int &y, double &eta, double &phi);
  void cpm2Coord(const int crate, const int cpm,
		 const int chip, const int location,
		 int &x, int &y, double &eta, double &phi);
  
  static const int TOB_TYPES=5;
  static const int MAXTOBS=30;
  /// Trigger configuration service
   ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc;
   /// Corrupt events tool
   ToolHandle<ITrigT1CaloMonErrorTool>     m_errorTool;
   /// Histogram utilities tool
   ToolHandle<TrigT1CaloLWHistogramTool>   m_histTool;
   /// Output from L1Topo
   //const DataHandle< LVL1::FrontPanelCTP > m_topoCTP;
   StringProperty m_CMXJetTobLocation;
   StringProperty m_CMXCPTobLocation;
   SG::ReadHandleKey<LVL1::FrontPanelCTP> m_topoCTPLoc { this, "TopoCTPLocation", LVL1::DEFAULT_L1TopoCTPLocation, "StoreGate location of topo inputs" };

   /// Root directory
   std::string m_PathInRootFile;   
   /// Debug printout flag
   bool m_debug;
   /// Histograms booked flag
   bool m_histBooked;
   /// Current lumiblock
   unsigned int m_lumiNo;

   /** Histos */   
   // Data transmission checks

   TH1F_LW* m_h_l1topo_1d_CMXTobs;
   TH1F_LW* m_h_l1topo_1d_Simulation;
   TH1F_LW* m_h_l1topo_1d_JetTobs_EnergyLg;
   TH2F_LW* m_h_l1topo_2d_Tobs_Hitmap_mismatch[TOB_TYPES];
   TH2F_LW* m_h_l1topo_2d_Tobs_Hitmap_match[TOB_TYPES];
   TH2F_LW* m_h_l1topo_2d_Tobs_etaPhi_mismatch[TOB_TYPES];
   TH2F_LW* m_h_l1topo_2d_Tobs_etaPhi_match[TOB_TYPES];
   TH1F_LW* m_h_l1topo_1d_Errors;
   TH1F_LW* m_h_l1topo_1d_Overflows;
   TH1F_LW* m_h_l1topo_1d_DAQTobs;
   TH1F_LW* m_h_l1topo_1d_DAQJetTobs;
   TH1F_LW* m_h_l1topo_1d_DAQTauTobs;
   TH1F_LW* m_h_l1topo_1d_DAQEMTobs;
   TH1F_LW* m_h_l1topo_1d_DAQMuonTobs;
   TH1F_LW* m_h_l1topo_1d_DAQTriggerBits;
   TH1F_LW* m_h_l1topo_1d_DAQMismatchTriggerBits;
   TH1F_LW* m_h_l1topo_1d_DAQOverflowBits;
   TH1F_LW* m_h_l1topo_1d_ROITobs;
   TH1F_LW* m_h_l1topo_1d_ErrorsByLumiblock;
   TH2F* m_h_l1topo_2d_ItemsBC[4];
   TH2F* m_h_l1topo_2d_ItemsBC_ratio[4][2];
};
 
 // ============================================================================
}  // end namespace
// ============================================================================

#endif
