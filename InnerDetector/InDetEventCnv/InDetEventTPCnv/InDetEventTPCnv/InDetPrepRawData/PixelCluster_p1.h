/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXEL_CLUSTER_P1_TRK_H
#define PIXEL_CLUSTER_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   PixelCluster_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"

namespace InDet
{
   class PixelCluster_p1
   {
  public:
     PixelCluster_p1() {}
     // base
     TPObjRef	m_siCluster;
     float 	m_omegax;
     float 	m_omegay;
   };
}

#endif // PIXEL_CLUSTER_P1_TRK_H
