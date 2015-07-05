/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_CLUSTER_ON_TRACK_P1_TRK_H
#define MUON_CLUSTER_ON_TRACK_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   MuonClusterOnTrack_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"
#include "Identifier/IdentifierHash.h"


namespace Muon
{

    /** Version 1 of the persistent class representing the transient class Muon::MuonClusterOnTrack 
    
    NO LONGER NEEDED, except for backwards compatibility.
    
    */
   class MuonClusterOnTrack_p1
   {
  public:
     MuonClusterOnTrack_p1() : m_positionAlongStrip(0) {}

     /** Link to base RIO_OnTrack_p1*/
     TPObjRef                       m_RIO;
     /** Hashed ID - needed for fast lookup of e.g. pointer to PRD on read */
     //IdentifierHash::value_type     m_idDE;

     /** position along strip (double in transient world)*/
     float m_positionAlongStrip;
   };
}

#endif // PIXEL_CLUSTER_ON_TRACK_P1_TRK_H
