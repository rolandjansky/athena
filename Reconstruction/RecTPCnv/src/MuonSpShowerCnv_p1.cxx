///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonSpShowerCnv_p1.cxx 
// Implementation file for class MuonSpShowerCnv_p1
// Author: Edward.Moyse@cern.ch
/////////////////////////////////////////////////////////////////// 

// muonEvent includes
#define private public
#define protected public
#include "muonEvent/MuonSpShower.h"
#undef private
#undef protected

// RecTPCnv includes
#include "RecTPCnv/MuonSpShowerCnv_p1.h"

/////////////////////////////////////////////////////////////////// 
// Non-Const methods: 
///////////////////////////////////////////////////////////////////

void MuonSpShowerCnv_p1::persToTrans( const MuonSpShower_p1* pers,
                  Rec::MuonSpShower* trans, 
                  MsgStream& /*msg*/ ) 
{
    trans->m_eta                    = pers->m_eta;
    trans->m_phi                    = pers->m_phi;
    trans->m_numberOfTriggerHits    = pers->m_numberOfTriggerHits   ;
    trans->m_numberOfInnerHits      = pers->m_numberOfInnerHits     ;
    trans->m_numberOfInnerSegments  = pers->m_numberOfInnerSegments ;
    trans->m_numberOfMiddleHits     = pers->m_numberOfMiddleHits    ;
    trans->m_numberOfMiddleSegments = pers->m_numberOfMiddleSegments;
    trans->m_numberOfOuterHits      = pers->m_numberOfOuterHits     ;
    trans->m_numberOfOuterSegments  = pers->m_numberOfOuterSegments ;    
    return;
}

void MuonSpShowerCnv_p1::transToPers( const Rec::MuonSpShower* trans, 
                  MuonSpShower_p1* pers, 
                  MsgStream& /*msg*/ ) 
{
    pers->m_eta                    = trans->m_eta;
    pers->m_phi                    = trans->m_phi;
    pers->m_numberOfTriggerHits    = trans->m_numberOfTriggerHits   ;
    pers->m_numberOfInnerHits      = trans->m_numberOfInnerHits     ;
    pers->m_numberOfInnerSegments  = trans->m_numberOfInnerSegments ;
    pers->m_numberOfMiddleHits     = trans->m_numberOfMiddleHits    ;
    pers->m_numberOfMiddleSegments = trans->m_numberOfMiddleSegments;
    pers->m_numberOfOuterHits      = trans->m_numberOfOuterHits     ;
    pers->m_numberOfOuterSegments  = trans->m_numberOfOuterSegments ; 
    return;
}
