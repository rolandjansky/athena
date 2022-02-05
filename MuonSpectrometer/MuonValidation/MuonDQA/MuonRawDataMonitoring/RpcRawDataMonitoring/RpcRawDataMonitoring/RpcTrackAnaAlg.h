#ifndef RPCRAWDATAMONITORING_RPCTRACKANAALG_H
#define RPCRAWDATAMONITORING_RPCTRACKANAALG_H

#include <map>
#include <vector>
#include <utility>
#include <string>
#include <set>
#include <time.h>

// Athena/Gaudi
#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "EventInfo/EventInfo.h"

// ATLAS
#include "xAODMuon/MuonContainer.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "MuonTrigCoinData/RpcCoinDataContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "xAODTracking/TrackParticle.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrkParameters/TrackParameters.h"

// local
#include "RpcRawDataMonitoring/RPCDQUtils.h"

namespace MuonGM 
{
  class MuonDetectorManager;
}

class RpcTrackAnaAlg : public AthMonitorAlgorithm
{
  public:
    RpcTrackAnaAlg (const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~RpcTrackAnaAlg();

    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

    typedef std::map<Identifier, std::shared_ptr<RpcPanel>>        RpcPanelMap;
    typedef std::pair<ExResult, const std::shared_ptr<GasGapData>> GasGapResult;

  private:
    StatusCode initRpcPanel();
    StatusCode initTrigTag();
    StatusCode initArrayHistosMap();
    
    StatusCode fillMuonExtrapolateEff(const EventContext& ctx) const;
    StatusCode fillHistPRD(const EventContext& ctx) const;
    
    StatusCode triggerMatching(const xAOD::Muon* , const std::vector<TagDef>& ) const;

    StatusCode extrapolate2RPC(const xAOD::TrackParticle *track, const Trk::PropDirection direction, std::vector<GasGapResult>& results) const;
    StatusCode computeTrackIntersectionWithGasGap(ExResult &result, const xAOD::TrackParticle* track_particle, const std::shared_ptr<GasGapData> &gap ) const;
    StatusCode readHitsPerGasgap(const EventContext& ctx, std::vector<GasGapResult>& results) const;
    StatusCode fillClusterSize(std::vector<const Muon::RpcPrepData*> &view_hits, const int panel_index, int isPhi) const;
    bool       IsNearbyHit(const std::vector<const Muon::RpcPrepData*> &cluster_hits, const Muon::RpcPrepData* hit) const;

  private:
    BooleanProperty  m_plotMuonEff{this, "plotMuonEff", false, "switch to plot histograms for Muon Efficiency"};
    BooleanProperty  m_plotPRD{this, "plotPRD", false, "switch to plot histograms for Prepare Data objects"};
    BooleanProperty  m_useAODParticle{this, "useAODParticle", false, "use AOD Particle"};

    DoubleProperty   m_avrLumiThr{this, "avrLumiThr", 10., "Thrshold of average luminosity per Luminosity block"};
    DoubleProperty   m_lbDuraThr{this,  "lbDuraThr",  10.,   "Thrshold of luminosity block deruation"};
    StringProperty   m_packageName{this,"PackageName", "RpcTrackAnaAlg","group name for histograming"};

    StringProperty   m_trigTagList{this,"TagTrigList","HLT_mu26_ivarmedium_L1MU20","list of triggers to be used for trigger matching"};
    DoubleProperty   m_trigMatchWindow{this,"TrigMatchingWindow",0.005,"Window size in R for trigger matching"};
    BooleanProperty  m_TagAndProbe{this,"TagAndProbe",false,"switch to perform tag-and-probe method"};
    BooleanProperty  m_TagAndProbeZmumu{this,"TagAndProbeZmumu",false,"switch to perform tag-and-probe method Z->mumu"};

    DoubleProperty   m_minPt{this,"minPt",2.5e3,"minmum pT of muon"};
    DoubleProperty   m_muonMass{this,"MuonMass",105.6583755,"muon invariant mass in MeV"};
    DoubleProperty   m_zMass{this,"ZMass",91187.6,"muon invariant mass in MeV"};
    DoubleProperty   m_zMassWindow{this,"ZMassWindow",10000,"muon invariant mass half-window in MeV"};
    DoubleProperty   m_zMass_lowLimit{this,"zMass_lowLimit",50000.,"2 muon invariant mass low limit in Zmumu event"};
    DoubleProperty   m_zMass_upLimit{this,"zMass_upLimit",  150000.,"2 muon invariant mass up limit in Zmumu event"};

    DoubleProperty   m_isolationWindow{this,"IsolationWindow",0.1,"Window size in R for isolation with other muons"};
    DoubleProperty   m_l1trigMatchWindow{this,"L1TrigMatchingWindow",0.3,"Window size in R for L1 trigger matching"};
    StringProperty   m_MuonEFContainerName{this,"MuonEFContainerName","HLT_MuonsCBOutsideIn","HLT RoI-based muon track container"};

    DoubleProperty   m_minDRTrackToGasGap{this, "minDRTrackToGasGap", 0.5, "minimum of DR between track and gasgap"};
    DoubleProperty   m_minDRTrackToReadoutElement{this, "minDRTrackToReadoutElement", 1.0, "minimum of DR between track and ReadoutElement"};

    DoubleProperty   m_boundsToleranceReadoutElement{this, "boundsToleranceReadoutElement", 100.0, "boundsToleranceReadoutElement"};
    DoubleProperty   m_boundsToleranceReadoutElementTight{this, "boundsToleranceReadoutElementTight", 20.0, "boundsToleranceReadoutElementTight"};

    DoubleProperty   m_diffHitTrackPostion{this, "diffHitTrackPostion", 30.0, "the largest distance between hit and tracks local postion"};
    DoubleProperty   m_outtime{this, "outtime", 12.5, "the out-time time"};

    ///////////////////////////////////////////////////////////////////
    ServiceHandle<Muon::IMuonIdHelperSvc>         m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    SG::ReadHandleKey<xAOD::EventInfo>            m_eventInfo {this,"EventInfo","EventInfo","event info"};
    
    const RpcIdHelper                             *m_rpcIdHelper = nullptr;
    const MuonGM::MuonDetectorManager             *m_muonMgr = nullptr;

    ToolHandle<Trk::IExtrapolator>                m_extrapolator{this,"TrackExtrapolator","Trk::Extrapolator/AtlasExtrapolator","Track extrapolator"};
    SG::ReadHandleKey<xAOD::MuonRoIContainer>     m_MuonRoIContainerKey {this, "MuonRoIContainerName", "LVL1MuonRoIs", "Key for L1 ROIs" };
    SG::ReadHandleKey<xAOD::MuonContainer>        m_MuonContainerKey { this, "MuonContainerKey", "Muons", "Key for Offline muon track Containers" };
    SG::ReadHandleKey<Muon::RpcPrepDataContainer> m_rpcPrdKey {this,"RpcPrepDataContainer","RPC_Measurements","RPC PRDs"};

    RpcPanelMap              m_rpcPanelMap;

    std::vector<TagDef>                           m_trigTagDefs;
    std::vector<std::shared_ptr<GasGapData>>      m_gasGapData;
};

#endif
