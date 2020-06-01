/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TauTrackParticleThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TAUTRACKPARTICLETHINNING_H
#define DERIVATIONFRAMEWORK_TAUTRACKPARTICLETHINNING_H

#include <string>
#include <atomic>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInDet/TracksInCone.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTau/TauJetContainer.h"

#include "StoreGate/ThinningHandleKey.h"
#include "StoreGate/ReadHandleKey.h"

namespace ExpressionParsing {
  class ExpressionParser;
}


namespace DerivationFramework {

    class TauTrackParticleThinning : public extends<AthAlgTool, IThinningTool> {
    public: 
      TauTrackParticleThinning(const std::string& t, const std::string& n, const IInterface* p);
      virtual ~TauTrackParticleThinning();
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
      SG::ReadHandleKey<xAOD::TauJetContainer> m_tauKey
         { this, "TauKey", "", ""};
      Gaudi::Property<bool> m_doTauTracksThinning
         { this, "DoTauTracksThinning", "", "Apply thinning to tau tracks in addition to ID tracks"};
      SG::ThinningHandleKey<xAOD::TauTrackContainer> m_tauTracksSGKey
        { this, "TauTracksKey", "TauTracks", "StoreGate key of the tau track container" }; 
      Gaudi::Property<std::string> m_selectionString
         { this, "SelectionString", "",""};
      Gaudi::Property<float> m_coneSize
         { this, "ConeSize", -1.0,  ""};

      std::unique_ptr<ExpressionParsing::ExpressionParser> m_parser;
  };
}

#endif // DERIVATIONFRAMEWORK_TAUTRACKPARTICLETHINNING_H
