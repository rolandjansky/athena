/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CLUSTER_P1_TRK_H
#define SCT_CLUSTER_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   SCT_Cluster_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"

namespace InDet
{
   class SCT_Cluster_p1
   {
  public:
     SCT_Cluster_p1() {}
     // base
     TPObjRef	m_siCluster;
   };
}

#endif // SCT_CLUSTER_P1_TRK_H
