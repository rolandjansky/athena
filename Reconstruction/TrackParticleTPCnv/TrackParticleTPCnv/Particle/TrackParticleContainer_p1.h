/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLECONTAINER_P1_REC_H
#define TRACKPARTICLECONTAINER_P1_REC_H

//-----------------------------------------------------------------------------
//
// file:   TrackParticleContainer_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>

namespace Rec
{
   class TrackParticleContainer_p1
      : public std::vector<TPObjRef>
   {
  public:
     TrackParticleContainer_p1() {}
   };
}

#endif // TRACKPARTICLECONTAINER_P1_REC_H
