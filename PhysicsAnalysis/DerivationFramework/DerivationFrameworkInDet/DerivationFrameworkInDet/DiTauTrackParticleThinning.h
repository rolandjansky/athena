/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DiTauTrackParticleThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_DITAUTRACKPARTICLETHINNING_H
#define DERIVATIONFRAMEWORK_DITAUTRACKPARTICLETHINNING_H

#include <string>
#include <atomic>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInDet/TracksInCone.h"
#include "xAODTau/DiTauJetContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODEgamma/EgammaContainer.h"
#include "StoreGate/ThinningHandleKey.h"
#include "StoreGate/ReadHandleKey.h"

#include "ExpressionEvaluation/ExpressionParserUser.h"

namespace DerivationFramework {

  class DiTauTrackParticleThinning : public extends<ExpressionParserUser<AthAlgTool>, IThinningTool> {
    public: 
      DiTauTrackParticleThinning(const std::string& t, const std::string& n, const IInterface* p);
      virtual ~DiTauTrackParticleThinning();
      virtual StatusCode initialize() override;
      virtual StatusCode finalize() override;
      virtual StatusCode doThinning() const override;

    private:
      mutable std::atomic<unsigned int> m_ntot  {};
      mutable std::atomic<unsigned int> m_npass {};
      StringProperty m_streamName
        { this, "StreamName", "", "Name of the stream being thinned" };
      SG::ReadHandleKey<xAOD::DiTauJetContainer>          m_ditauKey
        { this, "DiTauKey","",""};
      SG::ThinningHandleKey<xAOD::TrackParticleContainer> m_inDetSGKey
        { this, "InDetTrackParticlesKey", "InDetTrackParticles", "" };
      Gaudi::Property<std::string>                        m_selectionString
         { this, "SelectionString", "", ""};
  }; 
}

#endif // DERIVATIONFRAMEWORK_DITAUTRACKPARTICLETHINNING_H
