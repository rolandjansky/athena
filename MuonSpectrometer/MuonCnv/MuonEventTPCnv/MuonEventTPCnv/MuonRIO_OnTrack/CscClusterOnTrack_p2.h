/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSC_CLUSTER_ON_TRACK_P2_TRK_H
#define CSC_CLUSTER_ON_TRACK_P2_TRK_H

//-----------------------------------------------------------------------------
//
// file:   CscClusterOnTrack_p2.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"
#include "DataModelAthenaPool/ElementLink_p3.h"
#include "Identifier/Identifier.h"

namespace Muon
{
    /** Version 1 of the persistent class representing the transient class Muon::CscClusterOnTrack */
   class CscClusterOnTrack_p2
   {
  public:
     CscClusterOnTrack_p2() : m_id(0), m_status(0), m_positionAlongStrip(0.0), m_time(0.0) {}

      /** base RIO_OnTrack_p1 info*/
      Identifier32::value_type        m_id;
      TPObjRef                      m_localParams;
      TPObjRef                      m_localErrMat;
     
      /** Link to PRD */
      ElementLinkInt_p3  m_prdLink;
      
      /** Corresponds to CscClusterStatus in transient class*/
      unsigned int m_status;

      /** position along strip (double in transient world)*/
      float m_positionAlongStrip;
      
      float m_time;
   };
}

#endif 
