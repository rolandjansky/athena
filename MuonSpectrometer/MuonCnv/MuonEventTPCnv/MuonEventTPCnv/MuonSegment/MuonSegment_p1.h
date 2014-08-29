/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTTPCNV_MUONSEGMENT_P1_H
#define MUONEVENTTPCNV_MUONSEGMENT_P1_H

//-----------------------------------------------------------------------------
//
// file:   MuonSegment_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"
#include "TrkEventTPCnv/TrkSurfaces/Surface_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalDirection_p1.h"

namespace Muon
{
   class MuonSegment_p1
   {
  public:
     MuonSegment_p1() : m_t0 (9999.0), m_t0Error(9999.0) {}
     // base
     TPObjRef                   m_segment;

     Trk::LocalDirection_p1     m_localDirection;
     TPObjRef                   m_associatedSurface; //PlaneSurface 
     float m_t0;
     float m_t0Error;
   };
}

#endif 
