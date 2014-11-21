/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COPYTRUTHPARTONS_H
#define COPYTRUTHPARTONS_H

#include "ParticleJetTools/CopyTruthParticles.h"


class CopyTruthPartons : public CopyTruthParticles {
public:

  /// Constructor
  CopyTruthPartons(const std::string& name, ISvcLocator* pSvcLocator);

  /// Classifier function(s)
  bool classify(const xAOD::TruthParticle* tp);

};


#endif
