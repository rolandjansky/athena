/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for classes instantiated from TopJet

// FIXME
#define private public
#define protected public

#include "TopEvent/TopJet.h"
#include "TopEventTPCnv/TopJetCnv_p1.h"

#include "StoreGate/StoreGateSvc.h"

void TopJetCnv_p1::persToTrans( const TopJet_p1* pers,
				TopRec::TopJet* trans, 
				MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TopJetCnv_p1::persToTrans() begin"<<endreq;
  m_jetCnv.persToTrans(pers->m_jet, trans->m_jet,  msg);
  msg<<MSG::DEBUG<<"TopJetCnv_p1::persToTrans() end"<<endreq;
}

void TopJetCnv_p1::transToPers( const TopRec::TopJet* trans, 
				TopJet_p1* pers, 
				MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TopJetCnv_p1::transToPers() begin"<<endreq;
  m_jetCnv.transToPers(trans->m_jet, pers->m_jet, msg);
  msg<<MSG::DEBUG<<"TopJetCnv_p1::transToPers() end"<<endreq;
}
