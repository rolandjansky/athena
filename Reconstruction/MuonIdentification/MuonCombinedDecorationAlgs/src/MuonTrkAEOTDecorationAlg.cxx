/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrkAEOTDecorationAlg.h"
#include "StoreGate/WriteDecorHandle.h"
#include "StoreGate/ReadHandle.h" 

 MuonTrkAEOTDecorationAlg::MuonTrkAEOTDecorationAlg(const std::string& name, ISvcLocator* pSvcLocator):
    AthReentrantAlgorithm(name,pSvcLocator){}
 
 StatusCode MuonTrkAEOTDecorationAlg::initialize() {

    ATH_CHECK(m_TrkContainer.initialize());
    ATH_CHECK(m_idHelperSvc.retrieve());
    m_chIdKey = m_TrkContainer.key()+".alignEffectChId";
    m_deltaTransKey = m_TrkContainer.key()+".alignEffectDeltaTrans";
    m_sigmaDeltaTransKey = m_TrkContainer.key()+".alignEffectSigmaDeltaTrans";

    m_deltaAngleKey = m_TrkContainer.key()+".alignEffectDeltaAngle";
    m_sigmaDeltaAngleKey = m_TrkContainer.key() +".alignEffectSigmaDeltaAngle";
    ATH_CHECK(m_chIdKey.initialize());
    ATH_CHECK(m_deltaTransKey.initialize());
    ATH_CHECK(m_sigmaDeltaTransKey.initialize());
     
    ATH_CHECK(m_deltaAngleKey.initialize());
    ATH_CHECK(m_sigmaDeltaAngleKey.initialize());
     
     return StatusCode::SUCCESS;
 }

 StatusCode MuonTrkAEOTDecorationAlg::execute(const EventContext& ctx) const {
     SG::ReadHandle<xAOD::TrackParticleContainer> trkCont{m_TrkContainer,ctx};
     if (!trkCont.isValid()) {
         ATH_MSG_FATAL("Failed to load track collection "<<m_TrkContainer.fullKey());
         return StatusCode::FAILURE;
     }
     
    SG::WriteDecorHandle<xAOD::TrackParticleContainer, std::vector<std::vector<unsigned int>>> acc_ChId{m_chIdKey,ctx};
    SG::WriteDecorHandle<xAOD::TrackParticleContainer, std::vector<float>> acc_DeltaTrans{m_deltaTransKey,ctx};
    SG::WriteDecorHandle<xAOD::TrackParticleContainer, std::vector<float>> acc_SigmaDeltaTrans{m_sigmaDeltaTransKey, ctx};
    SG::WriteDecorHandle<xAOD::TrackParticleContainer, std::vector<float>> acc_deltaAngle{m_deltaAngleKey, ctx};
    SG::WriteDecorHandle<xAOD::TrackParticleContainer, std::vector<float>> acc_sigmaDeltaAngle{m_sigmaDeltaAngleKey, ctx};
     
     for (const xAOD::TrackParticle* tp : *trkCont) {
     
        std::vector<std::vector<unsigned int>>& chId = acc_ChId(*tp);
        std::vector<float>& deltaTrans = acc_DeltaTrans(*tp);
        std::vector<float>& sigmaDeltaTrans = acc_SigmaDeltaTrans(*tp);
        std::vector<float>& deltaAngle = acc_deltaAngle(*tp);
        std::vector<float>& sigmaDeltaAngle = acc_sigmaDeltaAngle(*tp);

        const Trk::Track* trk = tp->track();
        if (!trk || !trk->trackStateOnSurfaces()){
            ATH_MSG_DEBUG("Track has no TSOS?");
            continue;
        }         
        int nAEOT = 0;
        for (const Trk::TrackStateOnSurface* tsos : *trk->trackStateOnSurfaces()) {
            if (!tsos->type(Trk::TrackStateOnSurface::Alignment)) continue;
            const Trk::AlignmentEffectsOnTrack* aeot = tsos->alignmentEffectsOnTrack();
            if (!aeot) continue;
            ++nAEOT;
            std::set<unsigned int> chIdSet;
            for (const Identifier& id : aeot->vectorOfAffectedTSOS()) {
                if (!id.is_valid() || !m_idHelperSvc->isMuon(id)) continue;
                chIdSet.insert(m_idHelperSvc->chamberIndex(id));
            }
            std::vector<unsigned int> chIdVec;
            std::copy(chIdSet.begin(), chIdSet.end(), std::back_inserter(chIdVec));
            chId.push_back(std::move(chIdVec));
            deltaTrans.push_back(aeot->deltaTranslation());
            sigmaDeltaTrans.push_back(aeot->sigmaDeltaTranslation());
            deltaAngle.push_back(aeot->deltaAngle());
            sigmaDeltaAngle.push_back(aeot->sigmaDeltaAngle());            
        }            
     }
     return StatusCode::SUCCESS;
 }
 