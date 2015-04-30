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
class TH2I_LW;
class StatusCode;

namespace TrigConf {
  class ILVL1ConfigSvc;
}

// ============================================================================
namespace LVL1 {
// ============================================================================
// Forward declarations:
// ============================================================================
class ITrigT1CaloMonErrorTool;
class TrigT1CaloLWHistogramTool;
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
  
 private:

   /// Bin labels for summary plots
   void setLabels(LWHist* hist, bool xAxis = true);
     
   /// Trigger configuration service
   ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
   /// Corrupt events tool
   ToolHandle<ITrigT1CaloMonErrorTool>      m_errorTool;
   /// Histogram utilities tool
   ToolHandle<TrigT1CaloLWHistogramTool>   m_histTool;

   /// Root directory
   std::string m_PathInRootFile;   
   /// Debug printout flag
   bool m_debug;
   /// Histograms booked flag
   bool m_histBooked;

   /** Histos */   
   // Data transmission checks
   TH1F_LW* m_h_l1topo_1d_L1CaloL1TopoDAQCnvErrors;
   TH1F_LW* m_h_l1topo_1d_L1CaloL1TopoDAQPayloadCRCErrors;
   TH1F_LW* m_h_l1topo_1d_L1CaloL1TopoDAQTobs;
   TH1F_LW* m_h_l1topo_1d_L1CaloL1TopoDAQJetTobs;

};
// ============================================================================
}  // end namespace
// ============================================================================
#endif
