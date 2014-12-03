/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COPYFLAVORLABELTRUTHPARTICLES_H
#define COPYFLAVORLABELTRUTHPARTICLES_H

#include "ParticleJetTools/CopyTruthParticles.h"


class CopyFlavorLabelTruthParticles : public CopyTruthParticles {
ASG_TOOL_CLASS2(CopyFlavorLabelTruthParticles, CopyTruthParticles, IJetExecuteTool)
public:

  /// Constructor
  CopyFlavorLabelTruthParticles(const std::string& name);

  /// Classifier function(s)
  bool classify(const xAOD::TruthParticle* tp) const;


private:

  /// Particle selection mode
  std::string m_ptype;

};


#endif
