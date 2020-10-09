/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASSOCIATIONUTILS_MUJETGHOSTMATCHER_H
#define ASSOCIATIONUTILS_MUJETGHOSTMATCHER_H

// System includes
#include <memory>

// Infrastructure includes
#include "AsgMessaging/AsgMessaging.h"

// Local includes
#include "AssociationUtils/IObjectAssociator.h"

namespace ORUtils
{

  /// @class MuJetGhostDRMatcher
  /// @brief Matches a muon to a jet via ghost association or a delta-R.
  ///
  /// Ghost association is done by looking for the muon's ID track in the
  /// jet's list of ghost tracks. The Delta-R matching is implemented via the
  /// DeltaRMatcher. The final result is a match if either the ghost
  /// association or the delta-R succeeds.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class MuJetGhostDRMatcher : public IParticleAssociator,
                              public asg::AsgMessaging
  {

    public:

      /// Constructor takes same arguments as the DeltaRMatcher.
      MuJetGhostDRMatcher(double dR, bool useRapidity=true);

      /// Check for a match via ghost association or delta-R
      virtual bool objectsMatch
      (const xAOD::IParticle& mu, const xAOD::IParticle& jet) const override;

    private:

      /// The delta-R matcher
      std::unique_ptr<IParticleAssociator> m_drMatcher;

  }; // class MuJetGhostDRMatcher

} // namespace ORUtils

#endif
