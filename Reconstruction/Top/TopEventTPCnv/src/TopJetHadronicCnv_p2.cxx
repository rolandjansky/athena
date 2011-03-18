/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for classes instantiated from TTbarLJ 

// FIXME
#define private public
#define protected public

#include "TopEvent/TopJetHadronic.h"
#include "TopEventTPCnv/TopJetHadronic_p2.h"

#include "TopEventTPCnv/TopJetHadronicCnv_p2.h"
#include "TopEventTPCnv/TopJetCnv_p1.h"

#include "StoreGate/StoreGateSvc.h"

namespace {
  TopJetCnv_p1 baseCnv;
}

void TopJetHadronicCnv_p2::persToTrans( const TopJetHadronic_p2* pers,
					TopRec::TopJetHadronic* trans, 
					MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TopJetHadronicCnv_p2::persToTrans() begin"<<endreq;
  baseCnv.persToTrans(pers, trans,  msg);
  trans->m_split12=pers->m_split12;
  trans->m_split23=pers->m_split23;
  trans->m_split34=pers->m_split34;
  trans->m_scalefactor=pers->m_scalefactor;  
  trans->m_nsubjet=pers->m_nsubjet;
  trans->m_zcut12=pers->m_zcut12;
  trans->m_zcut23=pers->m_zcut23;
  trans->m_zcut34=pers->m_zcut34;
  trans->m_qw=pers->m_qw;
  trans->m_use_zcuts=pers->m_use_zcuts;

  msg<<MSG::DEBUG<<"TopJetHadronicCnv_p2::persToTrans() end"<<endreq;
}

void TopJetHadronicCnv_p2::transToPers( const TopRec::TopJetHadronic* trans, 
					TopJetHadronic_p2* pers, 
					MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TopJetHadronicCnv_p2::transToPers() begin"<<endreq;
  baseCnv.transToPers(trans, pers, msg);
  pers->m_split12=trans->m_split12;
  pers->m_split23=trans->m_split23;
  pers->m_split34=trans->m_split34;
  pers->m_scalefactor=trans->m_scalefactor;
  pers->m_nsubjet=trans->m_nsubjet;
  pers->m_zcut12=trans->m_zcut12;
  pers->m_zcut23=trans->m_zcut23;
  pers->m_zcut34=trans->m_zcut34;
  pers->m_qw=trans->m_qw;
  pers->m_use_zcuts=trans->m_use_zcuts;

  msg<<MSG::DEBUG<<"TopJetHadronicCnv_p2::transToPers() end"<<endreq;
}
