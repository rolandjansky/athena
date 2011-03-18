/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for classes instantiated from TTbarLLJetsMet 

#include <vector>
#include <typeinfo>

// FIXME
#define private public
#define protected public

#include "TopEvent/TTbarLLJetsMet.h"
#include "TopEventTPCnv/TTbarLLJetsMet_p1.h"

#include "TopEventTPCnv/TTbarLLJetsMetCnv_p1.h"
#include "TopEventTPCnv/TTbarLLJetsCnv_p1.h"

#include "StoreGate/StoreGateSvc.h"

#include "egammaEvent/Electron.h"
#include "egammaEvent/ElectronContainer.h"

#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"

#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"

#include "MissingETEvent/MissingET.h"

#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"

template<class LeptonCollection1, class LeptonCollection2>
void TTbarLLJetsMetCnv_p1<LeptonCollection1, LeptonCollection2>::persToTrans( const TTbarLLJetsMet_p1* pers,
									      TopRec::TTbarLLJetsMet<LeptonCollection1, LeptonCollection2>* trans, 
									      MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TTbarLLJetsMetCnv_p1::persToTrans() begin [LeptonCollection1, LeptonCollection2: "<<typeid(LeptonCollection1).name()
     <<", "<<typeid(LeptonCollection1).name()<<"]"<<endreq;

  TTbarLLJetsCnv_p1<LeptonCollection1, LeptonCollection2> baseCnv;
  baseCnv.persToTrans(pers, trans,  msg);

  DataLink<MissingET> dl;
  DataLink_p1 dl_p1=pers->m_metLink_p1;
  m_metLinkCnv.persToTrans(&dl_p1,&dl,msg);
  dl.toTransient();
  trans->setMetLink(dl);

  msg<<MSG::DEBUG<<"TTbarLLJetsMetCnv_p1::persToTrans() end"<<endreq;
}

template<class LeptonCollection1, class LeptonCollection2>
void TTbarLLJetsMetCnv_p1<LeptonCollection1, LeptonCollection2>::transToPers( const TopRec::TTbarLLJetsMet<LeptonCollection1, LeptonCollection2>* trans, 
									      TTbarLLJetsMet_p1* pers, 
									      MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TTbarLLJetsMetCnv_p1::transToPers() begin [LeptonCollection1, LeptonCollection2: "<<typeid(LeptonCollection1).name()
     <<", "<<typeid(LeptonCollection1).name()<<"]"<<endreq;

  TTbarLLJetsCnv_p1<LeptonCollection1, LeptonCollection2> baseCnv;
  baseCnv.transToPers(trans, pers, msg);

  DataLink<MissingET> dl = trans->metLink();
  DataLink_p1 dl_p1;
  m_metLinkCnv.transToPers(&dl,&dl_p1,msg);
  pers->m_metLink_p1=dl_p1;

  msg<<MSG::DEBUG<<"TTbarLLJetsMetCnv_p1::transToPers() end"<<endreq;
}

//----------------------------------------------------------------
// Instantiate 

template class TTbarLLJetsMetCnv_p1<ElectronContainer, ElectronContainer>;
template class TTbarLLJetsMetCnv_p1<ElectronContainer, Analysis::MuonContainer>;
template class TTbarLLJetsMetCnv_p1<ElectronContainer, Analysis::TauJetContainer>;
template class TTbarLLJetsMetCnv_p1<ElectronContainer, Rec::TrackParticleContainer>;
template class TTbarLLJetsMetCnv_p1<Analysis::MuonContainer, Analysis::MuonContainer>;
template class TTbarLLJetsMetCnv_p1<Analysis::MuonContainer, Analysis::TauJetContainer>;
template class TTbarLLJetsMetCnv_p1<Analysis::MuonContainer, Rec::TrackParticleContainer>;
template class TTbarLLJetsMetCnv_p1<Analysis::TauJetContainer, Analysis::TauJetContainer>;
template class TTbarLLJetsMetCnv_p1<TruthParticleContainer, TruthParticleContainer>;

//----------------------------------------------------------------
