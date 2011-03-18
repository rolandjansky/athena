/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for classes instantiated from TTbarLJDiTops 
// 
// Jean-Raphael Lessard <jlessard@uvic.ca>, 2009

#include <vector>
#include <algorithm>
#include <iterator>
#include <typeinfo>

// FIXME
#define private public
#define protected public

#include "TopEvent/TTbarLJDiTopsFitted.h"
#include "TopEventTPCnv/TTbarLJDiTopsFitted_p1.h"

#include "TopEventTPCnv/TTbarLJDiTopsFittedCnv_p1.h"

#include "TopEventTPCnv/TTbarLJDiTopsCnv_p1.h"

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

template<class LeptonCollection>
void TTbarLJDiTopsFittedCnv_p1<LeptonCollection>::persToTrans(const TTbarLJDiTopsFitted_p1* pers,
							TopRec::TTbarLJDiTopsFitted<LeptonCollection>* trans, 
							MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TTbarLJDiTopsFittedCnv_p1::persToTrans() begin [LeptonCollection: "<<typeid(LeptonCollection).name()<<"]"<<endreq;

  TopRec::TTbarLJDiTops<LeptonCollection> theInitialTTbar;
  TTbarLJDiTopsCnv_p1<LeptonCollection> ljDiTopsCnv_p1;
  ljDiTopsCnv_p1.persToTrans(&(pers->m_initialTTbar), &theInitialTTbar, msg);

  *trans = TopRec::TTbarLJDiTopsFitted<LeptonCollection>(theInitialTTbar,
							 pers->m_rescalingHadB_E,
							 pers->m_rescalingHadB_P,
							 pers->m_hadB_deltaEta,
							 pers->m_hadB_deltaPhi,
							 pers->m_rescalingLepB_E,
							 pers->m_rescalingLepB_P,
							 pers->m_lepB_deltaEta,
							 pers->m_lepB_deltaPhi,
							 pers->m_rescalingLightJets_E,
							 pers->m_rescalingLightJets_P,
							 pers->m_lightJets_deltaEta,
							 pers->m_lightJets_deltaPhi,
							 pers->m_rescalingLepton_E,
							 pers->m_rescalingLepton_P,
							 pers->m_lepton_deltaEta,
							 pers->m_lepton_deltaPhi,
							 pers->m_rescalingEtMissX,
							 pers->m_rescalingEtMissY,
							 pers->m_neutrinoPz,
							 pers->m_permutationProbability,
							 pers->m_permutationDiscriminant);
  
  msg<<MSG::DEBUG<<"TTbarLJDiTopsFittedCnv_p1::persToTrans() end"<<endreq;
}

template<class LeptonCollection>
void TTbarLJDiTopsFittedCnv_p1<LeptonCollection>::transToPers( const TopRec::TTbarLJDiTopsFitted<LeptonCollection>* trans, 
							 TTbarLJDiTopsFitted_p1* pers, 
							 MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TTbarLJDiTopsFittedCnv_p1::transToPers() begin [LeptonCollection: "<<typeid(LeptonCollection).name()<<"]"<<endreq;

  TopRec::TTbarLJDiTops<LeptonCollection> theInitialTTbar(trans->initialTTbar());

  TTbarLJDiTopsCnv_p1<LeptonCollection> ljDiTopsCnv_p1;
  ljDiTopsCnv_p1.transToPers(&theInitialTTbar, &pers->m_initialTTbar, msg);

  pers->m_rescalingHadB_E = trans->rescalingHadB_E();
  pers->m_rescalingHadB_P = trans->rescalingHadB_P();
  pers->m_hadB_deltaEta = trans->hadB_deltaEta();
  pers->m_hadB_deltaPhi = trans->hadB_deltaPhi();
  
  pers->m_rescalingLepB_E = trans->rescalingLepB_E();
  pers->m_rescalingLepB_P = trans->rescalingLepB_P();
  pers->m_lepB_deltaEta = trans->lepB_deltaEta();
  pers->m_lepB_deltaPhi =trans->lepB_deltaPhi();
  
  pers->m_rescalingLightJets_E = trans->rescalingLightJets_E();
  pers->m_rescalingLightJets_P = trans->rescalingLightJets_P();
  pers->m_lightJets_deltaEta = trans->lightJets_deltaEta();
  pers->m_lightJets_deltaPhi = trans->lightJets_deltaPhi();
  
  pers->m_rescalingLepton_E = trans->rescalingLepton_E();
  pers->m_rescalingLepton_P = trans->rescalingLepton_P();
  pers->m_lepton_deltaEta = trans->lepton_deltaEta();
  pers->m_lepton_deltaPhi = trans->lepton_deltaPhi();

  pers->m_rescalingEtMiss = trans->rescalingEtMiss();
  pers->m_rescalingEtMissX = trans->rescalingEtMissX();
  pers->m_rescalingEtMissY = trans->rescalingEtMissY();
  pers->m_neutrinoPz = trans->neutrinoPz();
  
  pers->m_permutationProbability = trans->permutationProbability();
  pers->m_permutationDiscriminant = trans->permutationDiscriminant();

  msg<<MSG::DEBUG<<"TTbarLJDiTopsFittedCnv_p1::transToPers() end"<<endreq;
}

//----------------------------------------------------------------
// Instantiate 

template class TTbarLJDiTopsFittedCnv_p1<ElectronContainer>;
template class TTbarLJDiTopsFittedCnv_p1<Analysis::MuonContainer>;
template class TTbarLJDiTopsFittedCnv_p1<Analysis::TauJetContainer>;
template class TTbarLJDiTopsFittedCnv_p1<TruthParticleContainer>;
template class TTbarLJDiTopsFittedCnv_p1<JetCollection>;

//----------------------------------------------------------------
