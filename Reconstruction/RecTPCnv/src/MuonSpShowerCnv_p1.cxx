///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// MuonSpShowerCnv_p1.cxx 
// Implementation file for class MuonSpShowerCnv_p1
// Author: Edward.Moyse@cern.ch
/////////////////////////////////////////////////////////////////// 

// muonEvent includes
#include "muonEvent/MuonSpShower.h"

// RecTPCnv includes
#include "RecTPCnv/MuonSpShowerCnv_p1.h"


void MuonSpShowerCnv_p1::persToTrans( const MuonSpShower_p1* pers,
                  Rec::MuonSpShower* trans, 
                  MsgStream& /*msg*/ ) const
{
    trans->set_eta                    (pers->m_eta);
    trans->set_phi                    (pers->m_phi);
    trans->set_numberOfTriggerHits    (pers->m_numberOfTriggerHits)   ;
    trans->set_numberOfInnerHits      (pers->m_numberOfInnerHits)     ;
    trans->set_numberOfInnerSegments  (pers->m_numberOfInnerSegments) ;
    trans->set_numberOfMiddleHits     (pers->m_numberOfMiddleHits)    ;
    trans->set_numberOfMiddleSegments (pers->m_numberOfMiddleSegments);
    trans->set_numberOfOuterHits      (pers->m_numberOfOuterHits)     ;
    trans->set_numberOfOuterSegments  (pers->m_numberOfOuterSegments) ;    
    return;
}

void MuonSpShowerCnv_p1::transToPers( const Rec::MuonSpShower* trans, 
                  MuonSpShower_p1* pers, 
                  MsgStream& /*msg*/ ) const
{
    pers->m_eta                    = trans->eta();
    pers->m_phi                    = trans->phi();
    pers->m_numberOfTriggerHits    = trans->numberOfTriggerHits()   ;
    pers->m_numberOfInnerHits      = trans->numberOfInnerHits()     ;
    pers->m_numberOfInnerSegments  = trans->numberOfInnerSegments() ;
    pers->m_numberOfMiddleHits     = trans->numberOfMiddleHits()    ;
    pers->m_numberOfMiddleSegments = trans->numberOfMiddleSegments();
    pers->m_numberOfOuterHits      = trans->numberOfOuterHits()     ;
    pers->m_numberOfOuterSegments  = trans->numberOfOuterSegments() ; 
    return;
}
