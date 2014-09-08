/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   PseudoMeasurementOnTrack_p1.h
// author: Andreas Wildauer
//
//-----------------------------------------------------------------------------

#ifndef PSEUDO_MEASUREMENT_ON_TRACK_P1_TRK_H
#define PSEUDO_MEASUREMENT_ON_TRACK_P1_TRK_H

#include "AthenaPoolUtilities/TPObjRef.h"

namespace Trk
{
   class PseudoMeasurementOnTrack_p1
   {
  public:
     PseudoMeasurementOnTrack_p1() {}

     TPObjRef m_localParams;  // mutable const LocalParameters*

     TPObjRef m_localErrMat; // mutable const ErrorMatrix*

     TPObjRef m_associatedSurface;// mutable const Surface*
   };
}

#endif // PSEUDO_MEASUREMENT_ON_TRACK_P1_TRK_H
