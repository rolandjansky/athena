/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackParticle_p1.h
// author: Andreas Wildauer
//
//-----------------------------------------------------------------------------

#ifndef TRACKPARTICLE_P1_REC_H
#define TRACKPARTICLE_P1_REC_H

#include "AthenaPoolUtilities/TPObjRef.h"

namespace Rec
{
   class TrackParticle_p1
   {
  public:
     TrackParticle_p1() {}
     
     TPObjRef m_trackParticleBase;
   };
}

#endif // TRACKPARTICLE_P1_REC_H
