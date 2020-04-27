/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Infrastructure
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"

// EDM includes
#include "xAODBase/IParticleContainer.h"
#include "xAODMuon/Muon.h"

// Local includes
#include "AssociationUtils/MuJetGhostDRMatcher.h"
#include "AssociationUtils/DeltaRMatcher.h"

namespace ORUtils
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  MuJetGhostDRMatcher::MuJetGhostDRMatcher(double dR, bool useRapidity)
    : asg::AsgMessaging("MuJetGhostDRMatcher")
  {
    m_drMatcher = std::make_unique<DeltaRMatcher>(dR, useRapidity);
  }

  //---------------------------------------------------------------------------
  // Check for a match via ghost association or delta-R
  //---------------------------------------------------------------------------
  bool MuJetGhostDRMatcher::objectsMatch(const xAOD::IParticle& mu,
                                         const xAOD::IParticle& jet) const
  {
    // Ghost track list accessor
    using GhostList_t = std::vector< ElementLink<xAOD::IParticleContainer> >;
    static SG::AuxElement::ConstAccessor<GhostList_t> ghostAcc("GhostTrack");

    // Check the particle types. First particle should be the muon,
    // and the second particle should be the jet.
    if(mu.type() != xAOD::Type::Muon) {
      ATH_MSG_WARNING("First particle arg to objectsMatch is not a muon!");
      return false;
    }
    if(jet.type() != xAOD::Type::Jet) {
      ATH_MSG_WARNING("Second particle arg to objectsMatch is not a jet!");
      return false;
    }

    // Try the delta-R match first.
    if(m_drMatcher->objectsMatch(mu, jet)) {
      ATH_MSG_DEBUG("  Found a dR association");
      return true;
    }

    // Retrieve the muon's ID track, or bail if none available.
    const auto idTrkFlag = xAOD::Muon::InnerDetectorTrackParticle;
    auto muTrk = static_cast<const xAOD::Muon&>(mu).trackParticle(idTrkFlag);
    if(!muTrk) return false;

    // Search for the muon ID track in the list of ghosts.
    for(const auto& ghostLink : ghostAcc(jet)) {
      if(ghostLink.isValid() && (muTrk == *ghostLink)) {
        ATH_MSG_DEBUG("  Found a ghost association!");
        return true;
      }
    }

    return false;
  }

} // namespace ORUtils
