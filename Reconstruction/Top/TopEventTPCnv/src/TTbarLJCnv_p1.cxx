/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for classes instantiated from TTbarLJ 
// 
// Andrei Gaponenko <agaponenko@lbl.gov>, 2008

#include <vector>
#include <typeinfo>

// FIXME
#define private public
#define protected public

#include "TopEvent/TTbarLJ.h"
#include "TopEventTPCnv/TTbarLJ_p1.h"

#include "TopEventTPCnv/TTbarLJCnv_p1.h"
#include "TopEventTPCnv/TTbarJetsBaseCnv_p1.h"

#include "StoreGate/StoreGateSvc.h"

#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"

#include "egammaEvent/Electron.h"
#include "egammaEvent/ElectronContainer.h"

#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"

#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"

#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"

namespace {
  TTbarJetsBaseCnv_p1 baseCnv;
}

template<class LeptonCollection>
void TTbarLJCnv_p1<LeptonCollection>::persToTrans( const TTbarLJ_p1* pers,
						   TopRec::TTbarLJ<LeptonCollection>* trans, 
						   MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TTbarLJCnv_p1::persToTrans() begin [LeptonCollection: "<<typeid(LeptonCollection).name()<<"]"<<endreq;
  baseCnv.persToTrans(pers, trans,  msg);
  m_leptonCnv.persToTrans(pers->m_lepton, trans->m_lepton,  msg);
  msg<<MSG::DEBUG<<"TTbarLJCnv_p1::persToTrans() end"<<endreq;
}

template<class LeptonCollection>
void TTbarLJCnv_p1<LeptonCollection>::transToPers( const TopRec::TTbarLJ<LeptonCollection>* trans, 
						   TTbarLJ_p1* pers, 
						   MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TTbarLJCnv_p1::transToPers() begin [LeptonCollection: "<<typeid(LeptonCollection).name()<<"]"<<endreq;
  baseCnv.transToPers(trans, pers, msg);
  m_leptonCnv.transToPers(trans->m_lepton, pers->m_lepton, msg);
  msg<<MSG::DEBUG<<"TTbarLJCnv_p1::transToPers() end"<<endreq;
}

//----------------------------------------------------------------
// Instantiate 

template class TTbarLJCnv_p1<ElectronContainer>;
template class TTbarLJCnv_p1<Analysis::MuonContainer>;
template class TTbarLJCnv_p1<Analysis::TauJetContainer>;
template class TTbarLJCnv_p1<TruthParticleContainer>;
template class TTbarLJCnv_p1<JetCollection>;

//----------------------------------------------------------------
