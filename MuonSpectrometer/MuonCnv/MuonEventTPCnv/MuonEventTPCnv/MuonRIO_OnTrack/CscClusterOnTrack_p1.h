/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSC_CLUSTER_ON_TRACK_P1_TRK_H
#define CSC_CLUSTER_ON_TRACK_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   CscClusterOnTrack_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"
#include "DataModelAthenaPool/ElementLink_p1.h"


namespace Muon
{
    /** Version 1 of the persistent class representing the transient class Muon::CscClusterOnTrack */
   class CscClusterOnTrack_p1
   {
  public:
     CscClusterOnTrack_p1() : m_status(0), m_positionAlongStrip(0.0) {}

     /** link to base MuonClusterOnTrack_p1, inheriting from RIO_OnTrack_p1 */
     TPObjRef                       m_RIO;
     
      /** Link to PRD */
      ElementLinkInt_p1  m_prdLink;
      
      /** Corresponds to CscClusterStatus in transient class*/
      unsigned int m_status;

      /** position along strip (double in transient world)*/
      float m_positionAlongStrip;
   };
}

#endif // CSC_CLUSTER_ON_TRACK_P1_TRK_H
