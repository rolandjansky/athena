/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for classes instantiated from TTbarLJ 

// FIXME
#define private public
#define protected public

#include "TopEvent/TopJetLeptonic.h"
#include "TopEventTPCnv/TopJetLeptonic_p1.h"

#include "TopEventTPCnv/TopJetLeptonicCnv_p1.h"
#include "TopEventTPCnv/TopJetCnv_p1.h"

#include "StoreGate/StoreGateSvc.h"

#include "egammaEvent/Electron.h"
#include "egammaEvent/ElectronContainer.h"

#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"

#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"

#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"

namespace {
  TopJetCnv_p1 baseCnv;
}

template<class LeptonCollection>
void TopJetLeptonicCnv_p1<LeptonCollection>::
persToTrans( const TopJetLeptonic_p1* pers,
	     TopRec::TopJetLeptonic<LeptonCollection>* trans, 
	     MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TopJetLeptonicCnv_p1::persToTrans() begin [LeptonCollection: "
     <<typeid(LeptonCollection).name()<<"]"<<endreq;
  baseCnv.persToTrans(pers, trans,  msg);
  m_leptonCnv.persToTrans(pers->m_lepton, trans->m_lepton,  msg);
  trans->m_x_lepton=pers->m_x_lepton;
  trans->m_y_lepton=pers->m_y_lepton;
  trans->m_yprime_lepton=pers->m_yprime_lepton;  
  msg<<MSG::DEBUG<<"TopJetLeptonicCnv_p1::persToTrans() end"<<endreq;
}

template<class LeptonCollection>
void TopJetLeptonicCnv_p1<LeptonCollection>::
transToPers( const TopRec::TopJetLeptonic<LeptonCollection>* trans, 
	     TopJetLeptonic_p1* pers, 
	     MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TopJetLeptonicCnv_p1::transToPers() begin [LeptonCollection: "
     <<typeid(LeptonCollection).name()<<"]"<<endreq;
  baseCnv.transToPers(trans, pers, msg);
  m_leptonCnv.transToPers(trans->m_lepton, pers->m_lepton, msg);
  pers->m_x_lepton=trans->m_x_lepton;
  pers->m_y_lepton=trans->m_y_lepton;
  pers->m_yprime_lepton=trans->m_yprime_lepton;  
  msg<<MSG::DEBUG<<"TopJetLeptonicCnv_p1::transToPers() end"<<endreq;
}

//----------------------------------------------------------------
// Instantiate 

template class TopJetLeptonicCnv_p1<ElectronContainer>;
template class TopJetLeptonicCnv_p1<Analysis::MuonContainer>;
template class TopJetLeptonicCnv_p1<Analysis::TauJetContainer>;
template class TopJetLeptonicCnv_p1<TruthParticleContainer>;

//----------------------------------------------------------------
