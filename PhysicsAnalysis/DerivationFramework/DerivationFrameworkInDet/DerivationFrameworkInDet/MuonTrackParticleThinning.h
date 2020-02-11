/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "StoreGate/ThinningHandleKey.h"

namespace ExpressionParsing {
  class ExpressionParser;
}


namespace DerivationFramework {

  class MuonTrackParticleThinning : public extends<AthAlgTool, IThinningTool> {
    public: 
      MuonTrackParticleThinning(const std::string& t, const std::string& n, const IInterface* p);
      virtual ~MuonTrackParticleThinning();
      virtual StatusCode initialize() override;
      virtual StatusCode finalize() override;
      virtual StatusCode doThinning() const override;

    private:
      mutable std::atomic<unsigned int> m_ntot, m_npass;
      StringProperty m_streamName
        { this, "StreamName", "", "Name of the stream being thinned" };
      SG::ThinningHandleKey<xAOD::TrackParticleContainer> m_inDetSGKey
        { this, "InDetTrackParticlesKey", "InDetTrackParticles", "" };
      std::string m_muonSGKey;
      std::string m_selectionString; 
      float m_coneSize;	
      ExpressionParsing::ExpressionParser *m_parser;
  }; 
}

#endif // DERIVATIONFRAMEWORK_MUONTRACKPARTICLETHINNING_H
