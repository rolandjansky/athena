/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHYSICSLINKS_PHYSICSLINKSDICT_H
#define PHYSICSLINKS_PHYSICSLINKSDICT_H

#include "PhysicsElementLinks/MuonLinks.h"
#include "PhysicsElementLinks/JetLinks.h"
#include "PhysicsElementLinks/TauJetLinks.h"
#include "PhysicsElementLinks/ElectronLinks.h"
#include "PhysicsElementLinks/PhotonLinks.h"
#include "PhysicsElementLinks/TruthParticleLinks.h"
#include "PhysicsElementLinks/TrackLinks.h"

namespace PhysicsElementLinksDict {
  struct dict {
      MuonLinks m_1;
      JetLinks m_2;
      TauJetLinks m_3;
      ElectronLinks m_4;
      PhotonLinks m_5;
      TruthParticleLinks m_6;
      TrackLinks m_7;
  };
}

#endif
