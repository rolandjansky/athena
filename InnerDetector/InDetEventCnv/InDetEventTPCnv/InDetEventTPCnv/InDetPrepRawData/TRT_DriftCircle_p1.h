/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DRIFT_CIRCLE_P1_TRK_H
#define TRT_DRIFT_CIRCLE_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   TRT_DriftCircle_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolUtilities/TPObjRef.h"

namespace InDet
{
   class TRT_DriftCircle_p1
   {
  public:
     TRT_DriftCircle_p1() {}

     // base PrepRawData_p1
     TPObjRef		m_rawData;
      
     float 		m_timeOverThreshold;
     float 		m_driftTime;
     bool		m_driftTimeValid;
     bool   		m_highLevel;
   };
}

#endif // TRT_DRIFT_CIRCLE_P1_TRK_H
