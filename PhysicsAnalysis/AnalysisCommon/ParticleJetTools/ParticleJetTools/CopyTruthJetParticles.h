/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COPYTRUTHJETPARTICLES_H
#define COPYTRUTHJETPARTICLES_H

#include "ParticleJetTools/CopyTruthParticles.h"


class CopyTruthJetParticles : public CopyTruthParticles {
ASG_TOOL_CLASS2(CopyTruthJetParticles, CopyTruthParticles, IJetExecuteTool)
public:

  /// Constructor
  CopyTruthJetParticles(const std::string& name);

  /// Classifier function(s)
  bool classify(const xAOD::TruthParticle* tp) const;

private:
  // Options for storate
  bool m_includeNu; //!< Include neutrinos in particles
  bool m_includeMu; //!< Include muons in particles
  bool m_includeWZ; //!< Include particles from W/Z decays
  bool m_includeTau; //!< Include particles from tau decays

  bool fromWZ( const xAOD::TruthParticle* tp ) const;
  bool fromTau( const xAOD::TruthParticle* tp ) const;
};


#endif
