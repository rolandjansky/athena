/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRpcTimingDecorAlg.h"
#include "StoreGate/ReadHandle.h" 
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
namespace{
    using Decorator = SG::WriteDecorHandle<xAOD::MuonContainer, std::vector<float>>;
    using UintDecorator = SG::WriteDecorHandle<xAOD::MuonContainer, std::vector<unsigned int>>;   
}
MuonRpcTimingDecorAlg::MuonRpcTimingDecorAlg(const std::string& name, ISvcLocator* pSvcLocator):
    AthReentrantAlgorithm(name,pSvcLocator){}
 
 StatusCode MuonRpcTimingDecorAlg::initialize() {
    ATH_CHECK(m_MuonContainer.initialize());
    m_idKey = m_MuonContainer.key() +".rpcHitIdentifier";
    m_posXKey = m_MuonContainer.key() +".rpcHitPositionX";
    m_posYKey = m_MuonContainer.key() +".rpcHitPositionY";
    m_posZKey = m_MuonContainer.key() +".rpcHitPositionZ";
    m_timeKey = m_MuonContainer.key() +".rpcHitTime";   
    ATH_CHECK(m_idKey.initialize());
    ATH_CHECK(m_posXKey.initialize());
    ATH_CHECK(m_posYKey.initialize());
    ATH_CHECK(m_posZKey.initialize());
    ATH_CHECK(m_timeKey.initialize());  
    return StatusCode::SUCCESS;
 }

 StatusCode MuonRpcTimingDecorAlg::execute(const EventContext& ctx) const {
    SG::ReadHandle<xAOD::MuonContainer> muons{m_MuonContainer,ctx};
    if (!muons.isValid()) {
         ATH_MSG_FATAL("Failed to load track collection "<<m_MuonContainer.fullKey());
         return StatusCode::FAILURE;
    }
    Decorator dec_pos_x{m_posXKey, ctx};
    Decorator dec_pos_y{m_posYKey, ctx};
    Decorator dec_pos_z{m_posZKey, ctx};
    Decorator dec_time{m_timeKey, ctx};
    UintDecorator dec_id{m_idKey,ctx};
 
    for (const xAOD::Muon* mu : *muons) {
        const xAOD::TrackParticle* tp = mu->primaryTrackParticle();
        std::vector<RpcInfo> timing_info{};
        getRpcTiming(tp != mu->trackParticle(xAOD::Muon::InnerDetectorTrackParticle)? tp : nullptr, timing_info);
        insert<float>(timing_info, mu, dec_pos_x, [](const RpcInfo& info){ return info.pos[Amg::x]; });
        insert<float>(timing_info, mu, dec_pos_y, [](const RpcInfo& info){ return info.pos[Amg::y]; });
        insert<float>(timing_info, mu, dec_pos_z, [](const RpcInfo& info){ return info.pos[Amg::z]; });
        insert<float>(timing_info, mu, dec_time, [](const RpcInfo& info){ return info.time; });
        insert<unsigned int>(timing_info, mu, dec_id, [](const RpcInfo& info){ return info.id; });        
    }
    return StatusCode::SUCCESS;
}
void MuonRpcTimingDecorAlg::getRpcTiming(const xAOD::TrackParticle* tp, std::vector<RpcInfo>& info) const{
    if (!tp || !tp->track() || !tp->track()->trackStateOnSurfaces()) return;

    const Trk::TrackStates& states = *(tp->track()->trackStateOnSurfaces());
    info.reserve(states.size());
    for (const Trk::TrackStateOnSurface* tsos : states) {
        // require measurement and track parameters
        const Trk::MeasurementBase* meas = tsos->measurementOnTrack();
        const Trk::TrackParameters* pars = tsos->trackParameters();
            if (!meas || !pars) continue;

        /// only consider RPC hits
        Identifier mid = m_edmHelperSvc->getIdentifier(*meas);
        if (!m_idHelperSvc->isMuon(mid) || !m_idHelperSvc->isRpc(mid)) continue;
        auto addHit = [&info](const Trk::MeasurementBase& meas) {
               const Muon::RpcClusterOnTrack* rot = dynamic_cast<const Muon::RpcClusterOnTrack*>(&meas);
               if (!rot) return;
               info.emplace_back(rot->globalPosition(), rot->time(), rot->identify().get_identifier32().get_compact());
        };
        const Muon::CompetingMuonClustersOnTrack* crot = dynamic_cast<const Muon::CompetingMuonClustersOnTrack*>(meas);
        if (crot) {
            for (unsigned int i = 0; i < crot->numberOfContainedROTs(); ++i) { addHit(crot->rioOnTrack(i)); }
        } else {
            addHit(*meas);
        }
    }
}
template <typename T> void MuonRpcTimingDecorAlg::insert(const std::vector<RpcInfo>& info,
                                      const xAOD::Muon* muon,
                                      SG::WriteDecorHandle<xAOD::MuonContainer,std::vector<T>>& decor,
                                      std::function<T(const RpcInfo&)> func) const {
        std::vector<T> data;
        data.reserve(info.size());
        std::for_each(info.begin(),info.end(),[&](const RpcInfo& rpc){data.emplace_back(func(rpc));});
        decor(*muon) = std::move(data);
}
 