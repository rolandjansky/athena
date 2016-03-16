/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKEVENTTPCNV_COMPETINGRIOONTRACK_P1_H
#define TRKEVENTTPCNV_COMPETINGRIOONTRACK_P1_H

//-----------------------------------------------------------------------------
//
// file:   CompetingRIOsOnTrack_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolUtilities/TPObjRef.h"

namespace Trk
{
   class CompetingRIOsOnTrack_p1
   // : public MeasurementBase_p1
   {
  public:
     CompetingRIOsOnTrack_p1() {}

     //LocalParameters_p1 *
     TPObjRef               m_localParameters;
     //ErrorMatrix *
     TPObjRef               m_localErrorMatrix;

     //LocalParameters_p1 *
     #ifdef UseFloatsIn_CompetingRIOsOnTrack
     std::vector<float>    m_assignProb;
     #else
     std::vector<double>    m_assignProb;
     #endif
   };
}

#endif // TRKEVENTTPCNV_COMPETINGRIOONTRACK_P1_H
