/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXEL_CLUSTER_ON_TRACK_P1_TRK_H
#define PIXEL_CLUSTER_ON_TRACK_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   PixelClusterOnTrack_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"

#include "DataModelAthenaPool/ElementLink_p1.h"
//#include "DataModelAthenaPool/ElementLinkContNames_p2.h"

namespace InDet
{
   class PixelClusterOnTrack_p1
   {
  public:
     PixelClusterOnTrack_p1() {}

     // base SiClusterOnTrack_p1, inheriting from RIO_OnTrack_p1
     TPObjRef			m_RIO;
//      IdentifierHash::value_type  m_idDE; // is in SiCluster base class
               
     bool			m_hasClusterAmbiguity;
     bool			m_isFake;
     float                      m_energyLoss;     
      /** Link to PRD */
      ElementLinkInt_p1  m_prdLink;
   };
}

#endif // PIXEL_CLUSTER_ON_TRACK_P1_TRK_H
