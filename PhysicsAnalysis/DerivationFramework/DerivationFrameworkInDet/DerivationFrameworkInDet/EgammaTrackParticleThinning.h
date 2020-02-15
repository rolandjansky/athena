/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EgammaTrackParticleThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_EGAMMATRACKPARTICLETHINNING_H
#define DERIVATIONFRAMEWORK_EGAMMATRACKPARTICLETHINNING_H

#include <string>
#include <atomic>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInDet/TracksInCone.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "StoreGate/ThinningHandleKey.h"

namespace ExpressionParsing {
  class ExpressionParser;
}


namespace DerivationFramework {

  class EgammaTrackParticleThinning : public extends<AthAlgTool, IThinningTool> {
    public: 
      EgammaTrackParticleThinning(const std::string& t, const std::string& n, const IInterface* p);
      virtual ~EgammaTrackParticleThinning();
      virtual StatusCode initialize() override;
      virtual StatusCode finalize() override;
      virtual StatusCode doThinning() const override;

    private:
      mutable std::atomic<unsigned int> m_ntot, m_ntotGSF, m_npass, m_nGSFPass;
      StringProperty m_streamName
        { this, "StreamName", "", "Name of the stream being thinned" };
      SG::ThinningHandleKey<xAOD::TrackParticleContainer> m_inDetSGKey
        { this, "InDetTrackParticlesKey", "InDetTrackParticles", "" };
      SG::ThinningHandleKey<xAOD::TrackParticleContainer> m_gsfSGKey
        { this, "GSFTrackParticlesKey", "GSFTrackParticles", "" };
      std::string m_sgKey;
      std::string m_selectionString;
      bool m_bestMatchOnly;
      float m_coneSize;
      ExpressionParsing::ExpressionParser *m_parser;
      void setPhotonMasks(std::vector<bool>&, std::vector<bool>&, const xAOD::EgammaContainer*, const xAOD::TrackParticleContainer*, const xAOD::TrackParticleContainer*, const bool) const;
      void setPhotonMasks(std::vector<bool>&, std::vector<bool>&, std::vector<const xAOD::Egamma*>&, const xAOD::TrackParticleContainer*, const xAOD::TrackParticleContainer*, const bool) const;
      void setElectronMasks(std::vector<bool>&, std::vector<bool>&, const xAOD::EgammaContainer*, const xAOD::TrackParticleContainer*, const xAOD::TrackParticleContainer*, const bool) const;
      void setElectronMasks(std::vector<bool>&, std::vector<bool>&, std::vector<const xAOD::Egamma*>&, const xAOD::TrackParticleContainer*, const xAOD::TrackParticleContainer*, const bool) const;

  };
}

#endif // DERIVATIONFRAMEWORK_EGAMMATRACKPARTICLETHINNING_H
