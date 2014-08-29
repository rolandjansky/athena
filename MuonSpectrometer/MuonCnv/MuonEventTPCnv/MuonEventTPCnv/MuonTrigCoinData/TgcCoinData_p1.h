/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Takashi Kubota - June 30, 2008 */
#ifndef TGCCOINDATA_P1_TRK_H
#define TGCCOINDATA_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   TgcCoinData_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"
#include "MuonTrigCoinData/TgcCoinData.h"

class TgcCoinDataCnv_p1;

namespace Muon
{
   class TgcCoinData_p1
   {
  public:
     TgcCoinData_p1() :
       m_channelIdIn(0), 
       m_channelIdOut(0), 
       m_indexAndHash(0),
       m_type(Muon::TgcCoinData::TYPE_UNKNOWN), 
       m_isAside(false),
       m_phi(0),
       m_isForward(false),
       m_isStrip(false),
       m_trackletId(0),
       m_trackletIdStrip(0),
       m_widthIn(0.),
       m_widthOut(0),
       m_delta(0),
       m_roi(0),
       m_pt(0)
       {}
     // base

     Identifier32::value_type m_channelIdIn;
     Identifier32::value_type  m_channelIdOut;
     IdentifierHash m_collectionIdHash;
     
     unsigned int  m_indexAndHash;
     
     int m_type;     
     bool m_isAside;
     int m_phi;
     bool m_isForward;
     bool m_isStrip;
     int m_trackletId;
     int m_trackletIdStrip;

     TPObjRef m_posIn;
     TPObjRef m_posOut;
     TPObjRef m_errMat;
     double m_widthIn;
     double m_widthOut;
     
     int m_delta;
     int m_roi;
     int m_pt;   
   };
}

#endif 
