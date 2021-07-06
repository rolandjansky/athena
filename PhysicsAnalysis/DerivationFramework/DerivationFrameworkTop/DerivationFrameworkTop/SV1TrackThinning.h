/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SV1TrackThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_SV1TRACKTHINNING_H
#define DERIVATIONFRAMEWORK_SV1TRACKTHINNING_H

#include <string>
#include <atomic>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "StoreGate/ThinningHandleKey.h"
#include "ExpressionEvaluation/ExpressionParserUser.h"


namespace DerivationFramework {

  class SV1TrackThinning : public extends<ExpressionParserUser<AthAlgTool>, IThinningTool> {
    public: 
      SV1TrackThinning(const std::string& t, const std::string& n, const IInterface* p);
      ~SV1TrackThinning();
      virtual StatusCode initialize() override;
      virtual StatusCode finalize() override;
      virtual StatusCode doThinning() const override;

    private:
      mutable std::atomic<unsigned int> m_ntot, m_npass;
      StringProperty m_streamName
        { this, "StreamName", "", "Name of the stream being thinned" };
      SG::ThinningHandleKey<xAOD::TrackParticleContainer> m_inDetSGKey
        { this, "InDetTrackParticlesKey", "InDetTrackParticles", "" };
      std::string m_jetSGKey, m_selectionString;
  }; 
}

#endif // DERIVATIONFRAMEWORK_SV1TRACKTHINNING_H
