/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CscClusterValMonAlg_H
#define CscClusterValMonAlg_H

//Core Include
#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "StoreGate/ReadHandleKey.h"
#include "CscClusterization/ICscStripFitter.h"
#include "CscCalibTools/ICscCalibTool.h"


class ICscStripFitter;
class ICscCalibTool;
class CscPrepRawData;

class CscClusterValMonAlg : public AthMonitorAlgorithm {

public:
    CscClusterValMonAlg( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~CscClusterValMonAlg();
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

private:

    //Trigger aware monitoring
    bool evtSelTriggersPassed();

    // Id helper
  //   ToolHandle<Muon::MuonIdHelperTool> m_muonIdHelperTool{this, "idHelper", 
  //   "Muon::MuonIdHelperTool/MuonIdHelperTool", "Handle to the MuonIdHelperTool"};
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    // Strip fitter.
    ToolHandle<ICscStripFitter> m_stripFitter;//{this, "CSCStripFitter", "CSCStripFitter", "Strip fitter Tool"};

    // calibration tool
    ToolHandle<ICscCalibTool> m_cscCalibTool;//{this, "CscCalibTool", "CscCalib", "Calib Tool"};

    //!< TDT handle
    ToolHandle<Trig::TrigDecisionTool> m_trigDec;//{this, "TrigDecisionTool", "TrigDecision","Trigger Decision Tool"}; 

    // Properties.
    SG::ReadHandleKey<Muon::CscPrepDataContainer> m_cscClusterKey{this,"CSCClusterKey","CSC_Clusters","CSC clusters"};
    SG::ReadHandleKey<Muon::CscStripPrepDataContainer> m_cscPRDKey{this,"CSCPrepRawDataKey", "CSC_Measurements","CSC PRDs"};
   // SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this,"EventInfo","EventInfo","event info"};
    std::string m_cscClusterPath, m_cscGenPath;
    unsigned int m_qmaxADCCut;

    bool m_doEvtSel;
    std::vector<std::string> m_sampSelTriggers;

    


};
#endif
