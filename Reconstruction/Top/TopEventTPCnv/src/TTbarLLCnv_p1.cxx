/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for classes instantiated from TTbarLL 
// 
// Andrei Gaponenko <agaponenko@lbl.gov>, 2008

#include <vector>
#include <typeinfo>

// FIXME
#define private public
#define protected public

#include "TopEvent/TTbarLL.h"
#include "TopEventTPCnv/TTbarLL_p1.h"

#include "TopEventTPCnv/TTbarLLCnv_p1.h"

#include "StoreGate/StoreGateSvc.h"

#include "egammaEvent/Electron.h"
#include "egammaEvent/ElectronContainer.h"

#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"

#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"

#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"

template<class LeptonCollection1, class LeptonCollection2>
void TTbarLLCnv_p1<LeptonCollection1, LeptonCollection2>::persToTrans( const TTbarLL_p1* pers,
								       TopRec::TTbarLL<LeptonCollection1,LeptonCollection2>* trans, 
								       MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TTbarLLCnv_p1::persToTrans() begin"
     <<" [LeptonCollection1: "<<typeid(LeptonCollection1).name()
     <<", LeptonCollection2: "<<typeid(LeptonCollection2).name()
     <<"]"<<endreq;

  m_lepton1Cnv.persToTrans(pers->m_lepton1, trans->m_lepton1,  msg);
  m_lepton2Cnv.persToTrans(pers->m_lepton2, trans->m_lepton2,  msg);

  msg<<MSG::DEBUG<<"TTbarLLCnv_p1::persToTrans() end"<<endreq;
}

template<class LeptonCollection1,class LeptonCollection2>
void TTbarLLCnv_p1<LeptonCollection1,LeptonCollection2>::transToPers( const TopRec::TTbarLL<LeptonCollection1,LeptonCollection2>* trans, 
								      TTbarLL_p1* pers, 
								      MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TTbarLLCnv_p1::transToPers() begin"
     <<" [LeptonCollection1: "<<typeid(LeptonCollection1).name()
     <<", LeptonCollection2: "<<typeid(LeptonCollection2).name()
     <<"]"<<endreq;

  m_lepton1Cnv.transToPers(trans->m_lepton1, pers->m_lepton1, msg);
  m_lepton2Cnv.transToPers(trans->m_lepton2, pers->m_lepton2, msg);

  msg<<MSG::DEBUG<<"TTbarLLCnv_p1::transToPers() end"<<endreq;
}

//----------------------------------------------------------------
// Instantiate 

template class TTbarLLCnv_p1<ElectronContainer, ElectronContainer>;
template class TTbarLLCnv_p1<ElectronContainer, Analysis::MuonContainer>;
template class TTbarLLCnv_p1<ElectronContainer, Analysis::TauJetContainer>;
template class TTbarLLCnv_p1<ElectronContainer, Rec::TrackParticleContainer>;
template class TTbarLLCnv_p1<Analysis::MuonContainer, Analysis::MuonContainer>;
template class TTbarLLCnv_p1<Analysis::MuonContainer, Analysis::TauJetContainer>;
template class TTbarLLCnv_p1<Analysis::MuonContainer, Rec::TrackParticleContainer>;
template class TTbarLLCnv_p1<Analysis::TauJetContainer, Analysis::TauJetContainer>;
template class TTbarLLCnv_p1<TruthParticleContainer, TruthParticleContainer>;

//----------------------------------------------------------------
