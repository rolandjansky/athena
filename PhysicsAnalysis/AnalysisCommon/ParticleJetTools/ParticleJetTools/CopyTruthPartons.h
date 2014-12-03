/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COPYTRUTHPARTONS_H
#define COPYTRUTHPARTONS_H

#include "ParticleJetTools/CopyTruthParticles.h"


class CopyTruthPartons : public CopyTruthParticles {
ASG_TOOL_CLASS2(CopyTruthPartons, CopyTruthParticles, IJetExecuteTool)
public:

  /// Constructor
  CopyTruthPartons(const std::string& name) : CopyTruthParticles(name) { }

  /// Classifier function(s)
  bool classify(const xAOD::TruthParticle* tp) const;

};


#endif
