/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COPYBOSONTOPLABELTRUTHPARTICLES_H
#define COPYBOSONTOPLABELTRUTHPARTICLES_H

#include "ParticleJetTools/CopyTruthParticles.h"


class CopyBosonTopLabelTruthParticles : public CopyTruthParticles {
ASG_TOOL_CLASS2(CopyBosonTopLabelTruthParticles, CopyTruthParticles, IJetExecuteTool)
public:

  /// Constructor
  CopyBosonTopLabelTruthParticles(const std::string& name);

  /// Classifier function(s)
  bool classify(const xAOD::TruthParticle* tp) const;


private:

  /// Particle selection mode
  std::string m_ptype;

};


#endif
