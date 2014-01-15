/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLECONTAINER_P2_REC_H
#define TRACKPARTICLECONTAINER_P2_REC_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>

namespace Rec
{
   class TrackParticleContainer_p2
      : public std::vector<TPObjRef>
   {
  public:
     TrackParticleContainer_p2() {}
   };
}

#endif 
