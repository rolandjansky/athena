/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EgammaTrackParticleThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_EGAMMATRACKPARTICLETHINNING_H
#define DERIVATIONFRAMEWORK_EGAMMATRACKPARTICLETHINNING_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInDet/TracksInCone.h"
#include "xAODEgamma/EgammaContainer.h"

namespace ExpressionParsing {
  class ExpressionParser;
}

class IThinningSvc;

namespace DerivationFramework {

  class EgammaTrackParticleThinning : public AthAlgTool, public IThinningTool {
    public: 
      EgammaTrackParticleThinning(const std::string& t, const std::string& n, const IInterface* p);
      ~EgammaTrackParticleThinning();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode doThinning() const;

    private:
      ServiceHandle<IThinningSvc> m_thinningSvc;
      mutable unsigned int m_ntot, m_ntotGSF, m_npass, m_nGSFPass;
      std::string m_sgKey;
      std::string m_inDetSGKey;
      std::string m_gsfSGKey;
      std::string m_selectionString;
      bool m_bestMatchOnly;
      float m_coneSize;
      bool m_and;
      ExpressionParsing::ExpressionParser *m_parser;
      void setPhotonMasks(std::vector<bool>&, std::vector<bool>&, const xAOD::EgammaContainer*&, const xAOD::TrackParticleContainer*&, const xAOD::TrackParticleContainer*&, const bool&) const;
      void setPhotonMasks(std::vector<bool>&, std::vector<bool>&, std::vector<const xAOD::Egamma*>&, const xAOD::TrackParticleContainer*&, const xAOD::TrackParticleContainer*&, const bool&) const;
      void setElectronMasks(std::vector<bool>&, std::vector<bool>&, const xAOD::EgammaContainer*&, const xAOD::TrackParticleContainer*&, const xAOD::TrackParticleContainer*&, const bool&) const;
      void setElectronMasks(std::vector<bool>&, std::vector<bool>&, std::vector<const xAOD::Egamma*>&, const xAOD::TrackParticleContainer*&, const xAOD::TrackParticleContainer*&, const bool&) const;

  };
}

#endif // DERIVATIONFRAMEWORK_EGAMMATRACKPARTICLETHINNING_H
