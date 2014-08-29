/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGC_CLUSTER_ON_TRACK_P1_TRK_H
#define TGC_CLUSTER_ON_TRACK_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   TgcClusterOnTrack_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"
#include "DataModelAthenaPool/ElementLink_p1.h"


namespace Muon
{
    /** Version 1 of the persistent class representing the transient class Muon::TgcClusterOnTrack */
   class TgcClusterOnTrack_p1
   {
  public:
     TgcClusterOnTrack_p1() : m_positionAlongStrip(0.0) {}

     /** link to base MuonClusterOnTrack_p1, inheriting from RIO_OnTrack_p1 */
     TPObjRef                       m_RIO;
     
      /** Link to PRD */
      ElementLinkInt_p1  m_prdLink;
      
      /** position along strip (double in transient world)*/
      float m_positionAlongStrip;
   };
}

#endif // TGC_CLUSTER_ON_TRACK_P1_TRK_H
