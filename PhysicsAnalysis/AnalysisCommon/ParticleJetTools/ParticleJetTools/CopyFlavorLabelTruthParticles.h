/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COPYFLAVORLABELTRUTHPARTICLES_H
#define COPYFLAVORLABELTRUTHPARTICLES_H

#include "ParticleJetTools/CopyTruthParticles.h"


class CopyFlavorLabelTruthParticles : public CopyTruthParticles {
public:

  /// Constructor
  CopyFlavorLabelTruthParticles(const std::string& name, ISvcLocator* pSvcLocator);

  /// Classifier function(s)
  bool classify(const xAOD::TruthParticle* tp);


private:

  /// Particle selection mode
  std::string m_ptype;

};


#endif
