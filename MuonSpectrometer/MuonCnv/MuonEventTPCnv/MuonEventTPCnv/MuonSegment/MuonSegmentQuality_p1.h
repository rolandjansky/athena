/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSEGMENTQUALITY_P1_H
#define MUONSEGMENTQUALITY_P1_H

//-----------------------------------------------------------------------------
//
// file:   MuonSegmentQuality_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"

namespace Muon
{
   class MuonSegmentQuality_p1
   {
  public:
     MuonSegmentQuality_p1() {}
     TPObjRef                    m_base;
     // copied from FitQuality
     //float                      m_chiSquared;
     //float                      m_numberDoF;
     std::vector<unsigned int>  m_channelsWithoutHit;

   };
}

#endif 
