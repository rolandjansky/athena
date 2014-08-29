/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGC_CLUSTER_ON_TRACK_P2_TRK_H
#define TGC_CLUSTER_ON_TRACK_P2_TRK_H

//-----------------------------------------------------------------------------
//
// file:   TgcClusterOnTrack_p2.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"
#include "DataModelAthenaPool/ElementLink_p3.h"


namespace Muon
{
    /** Version 2 of the persistent class representing the transient class Muon::TgcClusterOnTrack */
   class TgcClusterOnTrack_p2
   {
  public:
     TgcClusterOnTrack_p2() : m_id(0), m_positionAlongStrip(0.0){}

      /** base RIO_OnTrack_p1 info*/
      Identifier32::value_type        m_id;
      TPObjRef                      m_localParams;
      TPObjRef                      m_localErrMat;
           
      /** Link to PRD */
      ElementLinkInt_p3  m_prdLink;
      
      /** position along strip (double in transient world)*/
      float m_positionAlongStrip;
   };
}

#endif // TGC_CLUSTER_ON_TRACK_P1_TRK_H
