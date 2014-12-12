/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RIO_ON_TRACK_P1_TRK_H
#define RIO_ON_TRACK_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   RIO_OnTrack_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolUtilities/TPObjRef.h"

#include "Identifier/Identifier.h"

namespace Trk
{
   class RIO_OnTrack_p1
   // : public MeasurementBase_p1
   {
  public:
     RIO_OnTrack_p1() : m_id(0) {}

     unsigned int  m_id;

     //LocalParameters_p1 *
     TPObjRef               m_localParams;
     //ErrorMatrix *
     TPObjRef               m_localErrMat;
   };
}

#endif // RIO_ON_TRACK_P1_TRK_H
