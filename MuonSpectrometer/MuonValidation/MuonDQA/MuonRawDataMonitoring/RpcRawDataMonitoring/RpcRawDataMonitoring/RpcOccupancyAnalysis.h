#ifndef RPCRAWDATAMONITORING_RPCOCCUPANCYANALYSIS_H
#define RPCRAWDATAMONITORING_RPCOCCUPANCYANALYSIS_H

#include <utility>
#include <set>

// Athena/Gaudi
#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
// #include "StoreGate/ReadCondHandleKey.h"
// #include "StoreGate/StoreGateSvc.h"
#include "xAODEventInfo/EventInfo.h"
#include "EventInfo/EventInfo.h"

// ATLAS
// #include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRDO/RpcPadContainer.h"
#include "MuonRDO/RpcSectorLogicContainer.h"
#include "MuonTrigCoinData/RpcCoinDataContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "LumiBlockData/LuminosityCondData.h"
#include "LumiBlockData/LBDurationCondData.h"

// local
#include "RpcRawDataMonitoring/RPCDQUtils.h"


namespace MuonGM 
{
  class MuonDetectorManager;
}

class RpcOccupancyAnalysis : public AthMonitorAlgorithm
{
  public:
    RpcOccupancyAnalysis (const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~RpcOccupancyAnalysis();

    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

    typedef std::map<Identifier, std::shared_ptr<RpcPanel>> RpcPanelMap;

  private:
    StatusCode initRpcPanel();

    StatusCode fillHistPRD(const EventContext& ctx) const;

  private:
    BooleanProperty  m_plotPRD{this, "plotPRD", false, "switch to plot histograms for Prepare Data objects"};
    BooleanProperty  m_hit_percma{this, "hit_percma", false, "switch to plot histograms for Raw Data objects for per CMA"};
    BooleanProperty  m_analyseTrack{this, "analyseTrack", false, "switch to analysis track, extrapolate track to RPC"};
    BooleanProperty  m_useAODParticle{this, "useAODParticle", false, "use AOD Particle"};

    DoubleProperty   m_avrLumiThr{this, "avrLumiThr", 10., "Thrshold of average luminosity per Luminosity block"};
    DoubleProperty   m_lbDuraThr{this,  "lbDuraThr",  10.,   "Thrshold of luminosity block deruation"};
    StringProperty   m_packageName{this,"PackageName", "RpcOccupancyAnalysis","group name for histograming"};
    StringProperty   m_trigTagList{this,"TagTrigList","HLT_mu26_ivarmedium_L1MU20","list of triggers to be used for trigger matching"};

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    const MuonGM::MuonDetectorManager     *m_muonMgr;

    SG::ReadHandleKey<xAOD::EventInfo>            m_eventInfo {this,"EventInfo","EventInfo","event info"};
    SG::ReadHandleKey<Muon::RpcPrepDataContainer> m_rpcPrdKey {this,"RpcPrepDataContainer","RPC_Measurements","RPC PRDs"};

    RpcPanelMap                   m_rpcPanelMap;
    
    std::vector<TagDef>           m_trigTagDefs;
    std::vector<GasGapData*>      m_gasGapData;

    std::map<std::string,int>     m_timeTagGroups;
};



#endif
