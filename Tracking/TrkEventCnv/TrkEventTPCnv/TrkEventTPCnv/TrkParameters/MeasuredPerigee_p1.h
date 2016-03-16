/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MEASURED_PERIGEE_P1_TRK_H
#define MEASURED_PERIGEE_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   MeasuredPerigee_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolUtilities/TPObjRef.h"

namespace Trk
{
   class MeasuredPerigee_p1    {
  public:
     MeasuredPerigee_p1() {}

     TPObjRef	m_perigee;
     // ErrorMatrix
     TPObjRef	m_errorMatrix;
   };
}

#endif // MEASURED_PERIGEE_P1_TRK_H
