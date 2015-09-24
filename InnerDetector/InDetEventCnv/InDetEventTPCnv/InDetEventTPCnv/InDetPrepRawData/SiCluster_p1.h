/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SI_CLUSTER_P1_TRK_H
#define SI_CLUSTER_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   SiCluster_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"
#include "TrkEventTPCnv/TrkEventPrimitives/HepPoint3D_p1.h"


namespace InDet
{
   class SiCluster_p1
    {
   public:
      SiCluster_p1() {}

      // base PrepRawData_p1
      TPObjRef		m_rawData;

      //SiWidth_p1*
      TPObjRef		m_width;

    };
}

#endif // SI_CLUSTER_P1_TRK_H
