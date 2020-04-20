/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Framework includes
#include "FourMomUtils/xAODP4Helpers.h"

// Local includes
#include "AssociationUtils/DeltaRMatcher.h"

namespace ORUtils
{

  //---------------------------------------------------------------------------
  // DeltaRMatcher constructor
  //---------------------------------------------------------------------------
  DeltaRMatcher::DeltaRMatcher(double dR, bool useRapidity)
    : m_dR(dR),
      m_useRapidity(useRapidity)
  {}

  //---------------------------------------------------------------------------
  // Check if particles match in dR
  //---------------------------------------------------------------------------
  bool DeltaRMatcher::objectsMatch(const xAOD::IParticle& p1,
                                   const xAOD::IParticle& p2) const
  {
    return xAOD::P4Helpers::isInDeltaR(p1, p2, m_dR, m_useRapidity);
  }

  //---------------------------------------------------------------------------
  // SlidingDeltaRMatcher constructor
  //---------------------------------------------------------------------------
  SlidingDeltaRMatcher::SlidingDeltaRMatcher(double c1, double c2,
                                             double maxCone, bool useRapidity)
    : m_c1(c1), m_c2(c2), m_maxCone(maxCone), m_useRapidity(useRapidity)
  {}

  //---------------------------------------------------------------------------
  // Check if particles match in sliding dR
  //---------------------------------------------------------------------------
  bool SlidingDeltaRMatcher::objectsMatch(const xAOD::IParticle& p1,
                                          const xAOD::IParticle& p2) const
  {
    // Calculate the dR cone to match with
    double dR = m_c1 + (m_c2 / p1.pt());
    // Apply upper limit to the dR match cone
    dR = std::min(dR, m_maxCone);
    return xAOD::P4Helpers::isInDeltaR(p1, p2, dR, m_useRapidity);
  }

} // namespace ORUtils
