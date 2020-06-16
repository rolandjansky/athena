/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CscPrdValMonAlg_H
#define CscPrdValMonAlg_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "AthenaMonitoringKernel/Monitored.h"
//#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include "CscClusterization/ICscStripFitter.h"

class TH1;
class TH1F;
class TH2F;

class CscPrdValMonAlg: public AthMonitorAlgorithm  {
    
public:
    CscPrdValMonAlg( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~CscPrdValMonAlg();
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
    
 private:

    // Id helper
  //  ToolHandle<Muon::MuonIdHelperTool> m_muonIdHelperTool{this, "idHelper",
    //    "Muon::MuonIdHelperTool/MuonIdHelperTool", "Handle to the MuonIdHelperTool"};
    
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    
  size_t m_cscNoiseCut;
  SG::ReadHandleKey<Muon::CscStripPrepDataContainer> m_cscPrdKey{this,"CSCPrepRawDataKey","CSC_Measurements","CSC PRDs"};
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this,"EventInfo","EventInfo","event info"};
  std::string m_cscPRDPath, m_cscGenPath;
  bool m_mapxyrz;
    
    
    
  // Strip fitter
   ToolHandle<ICscStripFitter> m_stripFitter;
    

 // StatusCode fillLumiBlock();
 // int m_lumiblock;

  

};

#endif
