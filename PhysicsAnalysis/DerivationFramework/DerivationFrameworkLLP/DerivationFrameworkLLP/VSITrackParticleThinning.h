/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VSITrackParticleThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_VSITRACKPARTICLETHINNING_H
#define DERIVATIONFRAMEWORK_VSITRACKPARTICLETHINNING_H

#include <string>
#include <atomic>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "StoreGate/ThinningHandleKey.h"
#include "StoreGate/ReadHandleKey.h"

#include "ExpressionEvaluation/ExpressionParserUser.h"

namespace DerivationFramework {

  enum EDvTrPThinningParser { kDvSelection, kdvTrackThinning, kNumDvTPThinningParser};
  class VSITrackParticleThinning : public extends<ExpressionParserUser<AthAlgTool, kNumDvTPThinningParser>, IThinningTool> {
    public: 
      VSITrackParticleThinning(const std::string& t, const std::string& n, const IInterface* p);
      virtual ~VSITrackParticleThinning();
      virtual StatusCode initialize() override;
      virtual StatusCode finalize() override;
      virtual StatusCode doThinning() const override;

    private:
      mutable std::atomic<unsigned int> m_ntot  {};
      mutable std::atomic<unsigned int> m_npass {};

      StringProperty m_streamName
        { this, "StreamName", "", "Name of the stream being thinned" };
      SG::ThinningHandleKey<xAOD::TrackParticleContainer> m_inDetSGKey
        { this, "InDetTrackParticlesKey", "InDetTrackParticles", "" };
      StringProperty m_selectionString
      { this, "SelectionString", "", "" };
      StringProperty m_trackSelectionString
        { this, "TrackSelectionString", "", "" };
  }; 
}

#endif // DERIVATIONFRAMEWORK_VSITRACKPARTICLETHINNING_H
