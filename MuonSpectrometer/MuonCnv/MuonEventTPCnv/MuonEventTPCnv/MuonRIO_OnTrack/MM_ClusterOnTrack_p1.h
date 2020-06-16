/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM__CLUSTER_ON_TRACK_P1_TRK_H
#define MM__CLUSTER_ON_TRACK_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   MM_ClusterOnTrack_p1.h
//
//-----------------------------------------------------------------------------
#include <vector>

#include "AthenaPoolUtilities/TPObjRef.h"
#include "DataModelAthenaPool/ElementLink_p3.h"
namespace Muon
{
    /** Version 1 of the persistent class representing the transient class Muon::MM_ClusterOnTrack */
   class MM_ClusterOnTrack_p1
   {
  public:
     MM_ClusterOnTrack_p1() : m_id(0), m_positionAlongStrip(0.0) {}

      /** base RIO_OnTrack_p1 info*/
      Identifier32::value_type        m_id;
      TPObjRef                      m_localParams;
      TPObjRef                      m_localErrMat;
           
      /** Link to PRD */
      ElementLinkInt_p3  m_prdLink;      
      
      /** position along strip (double in transient world)*/
      float m_positionAlongStrip;
      std::vector<float> m_stripDriftDists;
      std::vector<float> m_stripDriftDistErrors_0_0;
      std::vector<float> m_stripDriftDistErrors_1_1;
   };
}

#endif // MM__CLUSTER_ON_TRACK_P1_TRK_H
