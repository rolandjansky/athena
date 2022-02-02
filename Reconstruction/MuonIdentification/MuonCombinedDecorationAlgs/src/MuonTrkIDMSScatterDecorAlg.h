/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDALGS_MUONTRKIDMSSCATTERDECPRALG_H
#define MUONCOMBINEDALGS_MUONTRKIDMSSCATTERDECPRALG_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"
/*
    Algorithm to decorate the scattering positions of the track in the calorimter to the TrackParticle
*/

class MuonTrkIDMSScatterDecorAlg: public AthReentrantAlgorithm {
   public:
        MuonTrkIDMSScatterDecorAlg(const std::string& name, ISvcLocator* pSvcLocator);
        ~MuonTrkIDMSScatterDecorAlg() = default;

        virtual StatusCode initialize() override;
        virtual StatusCode execute(const EventContext& ctx) const override;
    private:
      SG::ReadHandleKey<xAOD::TrackParticleContainer> m_TrkContainer{this, "TrackContainer", "", "track particle container"};
     
      SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> m_deltaPhi1Key{this, "deltaPhi1", "", "deltaphi_1 decoration key"};
      SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> m_deltaTheta1Key{this, "deltaTheta1", "", "deltatheta_1 decoration key"};

      SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> m_sigmaDeltaPhi1Key{this, "sigmaDeltaPhi1", "", "sigmadeltaphi_1 decoration key"};
      SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> m_sigmaDeltaTheta1Key{this, "sigmaDeltaTheta1", "", "sigmadeltatheta_1 decoration key"};
        
      SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> m_deltaPhi0Key{this, "deltaPhi0", "", "deltaphi_0 decoration key"};
      SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> m_deltaTheta0Key{this, "deltaTheta0", "", "deltatheta_0 decoration key"};

      SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> m_sigmaDeltaPhi0Key{this, "sigmaDeltaPhi0", "", "sigmadeltaphi_0 decoration key"};
      SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> m_sigmaDeltaTheta0Key{this, "sigmaDeltaTheta0", "", "sigmadeltatheta_0 decoration key"};
      
};
#endif