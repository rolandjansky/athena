/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CLUSTER_ON_TRACK_P1_TRK_H
#define SCT_CLUSTER_ON_TRACK_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   SCT_ClusterOnTrack_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"

#include "DataModelAthenaPool/ElementLink_p1.h"
//#include "DataModelAthenaPool/ElementLinkContNames_p2.h"

namespace InDet
{
   class SCT_ClusterOnTrack_p1
   {
  public:
     SCT_ClusterOnTrack_p1(): m_positionAlongStrip{0.} {}

     // base SiClusterOnTrack_p1, inheriting from RIO_OnTrack_p1
     TPObjRef			m_RIO;
//      IdentifierHash::value_type  m_idDE; // is in SiCluster base class
               
      /** Link to PRD */
      ElementLinkInt_p1  m_prdLink;
     
//element allowing for a calculation of global position     
      float m_positionAlongStrip;
   };
}

#endif // SCT_CLUSTER_ON_TRACK_P1_TRK_H
