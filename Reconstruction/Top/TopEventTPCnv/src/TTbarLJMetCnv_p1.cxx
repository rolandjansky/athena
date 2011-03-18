/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for classes instantiated from TTbarLJMet 

#include <vector>
#include <typeinfo>

// FIXME
#define private public
#define protected public

#include "TopEvent/TTbarLJMet.h"
#include "TopEventTPCnv/TTbarLJMet_p1.h"

#include "TopEventTPCnv/TTbarLJMetCnv_p1.h"
#include "TopEventTPCnv/TTbarLJCnv_p1.h"

#include "StoreGate/StoreGateSvc.h"

#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"

#include "egammaEvent/Electron.h"
#include "egammaEvent/ElectronContainer.h"

#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"

#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"

#include "MissingETEvent/MissingET.h"

#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"

template<class LeptonCollection>
void TTbarLJMetCnv_p1<LeptonCollection>::persToTrans( const TTbarLJMet_p1* pers,
						      TopRec::TTbarLJMet<LeptonCollection>* trans, 
						      MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TTbarLJMetCnv_p1::persToTrans() begin [LeptonCollection: "<<typeid(LeptonCollection).name()<<"]"<<endreq;

  TTbarLJCnv_p1<LeptonCollection> baseCnv;
  baseCnv.persToTrans(pers, trans,  msg);

  DataLink<MissingET> dl;
  DataLink_p1 dl_p1=pers->m_metLink_p1;
  m_metLinkCnv.persToTrans(&dl_p1,&dl,msg);
  dl.toTransient();
  trans->setMetLink(dl);

  msg<<MSG::DEBUG<<"TTbarLJMetCnv_p1::persToTrans() end"<<endreq;
}

template<class LeptonCollection>
void TTbarLJMetCnv_p1<LeptonCollection>::transToPers( const TopRec::TTbarLJMet<LeptonCollection>* trans, 
						      TTbarLJMet_p1* pers, 
						      MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TTbarLJMetCnv_p1::transToPers() begin [LeptonCollection: "<<typeid(LeptonCollection).name()<<"]"<<endreq;
 
  TTbarLJCnv_p1<LeptonCollection> baseCnv;
  baseCnv.transToPers(trans, pers, msg);

  DataLink<MissingET> dl = trans->metLink();
  DataLink_p1 dl_p1;
  m_metLinkCnv.transToPers(&dl,&dl_p1,msg);
  pers->m_metLink_p1=dl_p1;

  msg<<MSG::DEBUG<<"TTbarLJMetCnv_p1::transToPers() end"<<endreq;
}

//----------------------------------------------------------------
// Instantiate 

template class TTbarLJMetCnv_p1<ElectronContainer>;
template class TTbarLJMetCnv_p1<Analysis::MuonContainer>;
template class TTbarLJMetCnv_p1<Analysis::TauJetContainer>;
template class TTbarLJMetCnv_p1<TruthParticleContainer>;
template class TTbarLJMetCnv_p1<JetCollection>;

//----------------------------------------------------------------
