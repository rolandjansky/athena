/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonTrackParticleThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_MUONTRACKPARTICLETHINNING_H
#define DERIVATIONFRAMEWORK_MUONTRACKPARTICLETHINNING_H

#include <string>
#include <atomic>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInDet/TracksInCone.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "StoreGate/ThinningHandleKey.h"

#include "ExpressionEvaluation/ExpressionParserUser.h"

namespace DerivationFramework {

  class MuonTrackParticleThinning : public extends<ExpressionParserUser<AthAlgTool>, IThinningTool> {
    public: 
      MuonTrackParticleThinning(const std::string& t, const std::string& n, const IInterface* p);
      virtual ~MuonTrackParticleThinning();
      virtual StatusCode initialize() override;
      virtual StatusCode finalize() override;
      virtual StatusCode doThinning() const override;

    private:
      mutable std::atomic<unsigned int> m_ntot {};
      mutable std::atomic<unsigned int> m_npass {};
      StringProperty m_streamName
        { this, "StreamName", "", "Name of the stream being thinned" };
      SG::ThinningHandleKey<xAOD::TrackParticleContainer> m_inDetSGKey
        { this, "InDetTrackParticlesKey", "InDetTrackParticles", "" };
      SG::ReadHandleKey<xAOD::MuonContainer> m_muonKey
        { this, "MuonKey", "",""};
      Gaudi::Property<std::string> m_selectionString
        { this, "SelectionString","",""};
      Gaudi::Property<float> m_coneSize
        { this, "ConeSize", -1.0, ""};
  };
}

#endif // DERIVATIONFRAMEWORK_MUONTRACKPARTICLETHINNING_H
