/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCOINDATA_P3_TRK_H
#define TGCCOINDATA_P3_TRK_H

//-----------------------------------------------------------------------------
//
// file:   TgcCoinData_p3.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"
#include "MuonTrigCoinData/TgcCoinData.h"

class TgcCoinDataCnv_p3;

namespace Muon
{
   class TgcCoinData_p3
   {
  public:
     TgcCoinData_p3() :
       m_channelIdIn(0), 
       m_channelIdOut(0), 
       m_indexAndHash(0), 
       m_type(Muon::TgcCoinData::TYPE_UNKNOWN), 
       m_isAside(false), 
       m_phi(0), 
       m_isInner(false), 
       m_isForward(false), 
       m_isStrip(false), 
       m_trackletId(0), 
       m_trackletIdStrip(0), 
       m_widthIn(0.), 
       m_widthOut(0.), 
       m_delta(0), 
       m_roi(0), 
       m_pt(0),
       m_veto(false),
       m_sub(0),
       m_inner(0),
       m_isPositiveDeltaR(false) 
       {}
     // base

     Identifier32::value_type m_channelIdIn;
     Identifier32::value_type  m_channelIdOut;
     IdentifierHash m_collectionIdHash;
     
     unsigned int  m_indexAndHash;
     
     int m_type;     
     bool m_isAside;
     int m_phi;
     bool m_isInner;
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
     bool m_veto;   

     int m_sub;
     int m_inner;
     bool m_isPositiveDeltaR;
   };
}

#endif 
