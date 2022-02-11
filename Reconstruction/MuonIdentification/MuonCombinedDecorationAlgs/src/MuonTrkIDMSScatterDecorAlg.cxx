/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrkIDMSScatterDecorAlg.h"
#include "StoreGate/WriteDecorHandle.h"
#include "StoreGate/ReadHandle.h" 
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
namespace{
    using Decorator = SG::WriteDecorHandle<xAOD::TrackParticleContainer, float>;
    constexpr float dummy_val = -999.;
}
MuonTrkIDMSScatterDecorAlg::MuonTrkIDMSScatterDecorAlg(const std::string& name, ISvcLocator* pSvcLocator):
    AthReentrantAlgorithm(name,pSvcLocator){}
 
 StatusCode MuonTrkIDMSScatterDecorAlg::initialize() {

    ATH_CHECK(m_TrkContainer.initialize());
    
    m_deltaPhi1Key = m_TrkContainer.key() +".deltaphi_1";
    m_deltaTheta1Key = m_TrkContainer.key() +".deltatheta_1";
    
    m_sigmaDeltaPhi1Key = m_TrkContainer.key() +".sigmadeltaphi_1";
    m_sigmaDeltaTheta1Key = m_TrkContainer.key() +".sigmadeltatheta_1";
  
    m_deltaPhi0Key = m_TrkContainer.key() +".deltaphi_0";
    m_deltaTheta0Key = m_TrkContainer.key() +".deltatheta_0";
    
    m_sigmaDeltaPhi0Key = m_TrkContainer.key() +".sigmadeltaphi_0";
    m_sigmaDeltaTheta0Key = m_TrkContainer.key() +".sigmadeltatheta_0";
   
    ATH_CHECK(m_deltaPhi1Key.initialize());
    ATH_CHECK(m_deltaTheta1Key.initialize());
    ATH_CHECK(m_sigmaDeltaPhi1Key.initialize());
    ATH_CHECK(m_sigmaDeltaTheta1Key.initialize());
    
    ATH_CHECK(m_deltaPhi0Key.initialize());
    ATH_CHECK(m_deltaTheta0Key.initialize());
    ATH_CHECK(m_sigmaDeltaPhi0Key.initialize());
    ATH_CHECK(m_sigmaDeltaTheta0Key.initialize());
    return StatusCode::SUCCESS;
 }

 StatusCode MuonTrkIDMSScatterDecorAlg::execute(const EventContext& ctx) const {
    SG::ReadHandle<xAOD::TrackParticleContainer> trkCont{m_TrkContainer,ctx};
    if (!trkCont.isValid()) {
         ATH_MSG_FATAL("Failed to load track collection "<<m_TrkContainer.fullKey());
         return StatusCode::FAILURE;
    }
    Decorator dec_deltaphi_1{m_deltaPhi1Key,ctx};
    Decorator dec_deltatheta_1{m_deltaTheta1Key, ctx};
    Decorator dec_sigmadeltaphi_1{m_sigmaDeltaPhi1Key, ctx};
    Decorator dec_sigmadeltatheta_1{m_sigmaDeltaTheta1Key, ctx};

    Decorator dec_deltaphi_0{m_deltaPhi0Key,ctx};
    Decorator dec_deltatheta_0{m_deltaTheta0Key, ctx};
    Decorator dec_sigmadeltaphi_0{m_sigmaDeltaPhi0Key, ctx};
    Decorator dec_sigmadeltatheta_0{m_sigmaDeltaTheta0Key, ctx};

    for (const xAOD::TrackParticle* tp : *trkCont) {
        dec_deltaphi_1(*tp) = dummy_val;
        dec_deltatheta_1(*tp) = dummy_val;
        dec_sigmadeltaphi_1(*tp) = dummy_val;
        dec_sigmadeltatheta_1(*tp) = dummy_val;

        dec_deltaphi_0(*tp) = dummy_val;
        dec_deltatheta_0(*tp) = dummy_val;
        dec_sigmadeltaphi_0(*tp) = dummy_val;
        dec_sigmadeltatheta_0(*tp) = dummy_val;
        int nscatter = 0;
        if (!tp->track() || !tp->track()->trackStateOnSurfaces()) continue;
        for (const Trk::TrackStateOnSurface* tsos : *tp->track()->trackStateOnSurfaces()) {
            if (tsos->materialEffectsOnTrack()) {
                const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(tsos->materialEffectsOnTrack());
                if (!meot->energyLoss() || !meot->scatteringAngles()) continue;
                if (meot->energyLoss()->deltaE() == 0) {  // artificial scatterer found
                    if (nscatter == 0) {
                        dec_deltaphi_0(*tp) = meot->scatteringAngles()->deltaPhi();
                        dec_deltatheta_0(*tp) = meot->scatteringAngles()->deltaTheta();
                        dec_sigmadeltaphi_0(*tp) = meot->scatteringAngles()->sigmaDeltaPhi();
                        dec_sigmadeltatheta_0(*tp) = meot->scatteringAngles()->sigmaDeltaTheta();
                    } else if (nscatter == 1) {
                        dec_deltaphi_1(*tp) = meot->scatteringAngles()->deltaPhi();
                        dec_deltatheta_1(*tp) = meot->scatteringAngles()->deltaTheta();
                        dec_sigmadeltaphi_1(*tp) = meot->scatteringAngles()->sigmaDeltaPhi();
                        dec_sigmadeltatheta_1(*tp) = meot->scatteringAngles()->sigmaDeltaTheta();
                    }
                    ++nscatter;
                }
            }
            if (nscatter > 1) break;
        }       
    }
    return StatusCode::SUCCESS;
}
 