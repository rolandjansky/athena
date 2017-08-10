#ifndef ASSOCIATIONUTILS_DELTARMATCHER_H
#define ASSOCIATIONUTILS_DELTARMATCHER_H

// System includes
#include <limits>

// EDM includes
#include "xAODBase/IParticle.h"

// Local includes
#include "AssociationUtils/IObjectAssociator.h"

namespace ORUtils
{

  /// @class DeltaRMatcher
  /// @brief Encapsulates the delta-R matching strategy.
  ///
  /// I am considering abstracting matching with an interface.
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class DeltaRMatcher : public IParticleAssociator
  {

    public:

      /// Constructor
      DeltaRMatcher(double dR, bool useRapidity=true);

      /// Check if particles match in dR
      virtual bool objectsMatch
      (const xAOD::IParticle& p1, const xAOD::IParticle& p2) const override;

    private:

      double m_dR;
      bool m_useRapidity;

  }; // class DeltaRMatcher


  /// @class SlidingDeltaRMatcher
  /// @brief DR matching strategy where cone decreases with pt.
  ///
  /// Calculates the dR cone w.r.t. the pt of the first argument as:
  /// dR = c1 + (c2/pt)
  /// Assumes that you're only using rapidity (for now).
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class SlidingDeltaRMatcher : public IParticleAssociator
  {

    public:

      /// @brief Constructor.
      /// @param[in] c1 The offset constant (dimensionless).
      /// @param[in] c2 The pt factor constant (MeV).
      /// @param[in] maxCone Cone size upper limit.
      SlidingDeltaRMatcher(double c1, double c2,
                           double maxCone = std::numeric_limits<double>::max(),
                           bool useRapidity=true);

      /// @brief Check if particles match in dR.
      /// The dR cone is calculated as dR = c1 + (c2/p1.pt())
      virtual bool objectsMatch
      (const xAOD::IParticle& p1, const xAOD::IParticle& p2) const override;

    private:

      /// Sliding dR constant c1
      double m_c1;
      /// Sliding dR constant c2
      double m_c2;
      /// Maximum sliding cone size
      double m_maxCone;

      /// Toggle rapidity usage
      bool m_useRapidity;

  }; // class SlidingDeltaRMatcher

} // namespace ORUtils

#endif
