/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for classes instantiated from TTbarLJDiTops 
// 
// Jean-Raphael Lessard <jlessard@uvic.ca>, 2008

#include <vector>
#include <algorithm>
#include <iterator>
#include <typeinfo>

// FIXME
#define private public
#define protected public

#include "TopEvent/TTbarLJDiTops.h"
#include "TopEventTPCnv/TTbarLJDiTops_p1.h"

#include "TopEventTPCnv/TTbarLJDiTopsCnv_p1.h"

#include "StoreGate/StoreGateSvc.h"

#include "FourMom/P4PxPyPzE.h"

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
void TTbarLJDiTopsCnv_p1<LeptonCollection>::persToTrans(const TTbarLJDiTops_p1* pers,
							TopRec::TTbarLJDiTops<LeptonCollection>* trans, 
							MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TTbarLJDiTopsCnv_p1::persToTrans() begin [LeptonCollection: "<<typeid(LeptonCollection).name()<<"]"<<endreq;

  TopRec::TTbarLJ<LeptonCollection> base;
  m_baseCnv.persToTrans(pers, &base,  msg);

  TopRec::IndexSet htop_indexes;
  std::copy(pers->m_htop_indexes.begin(),  pers->m_htop_indexes.end(), std::inserter(htop_indexes, htop_indexes.begin()) );

  P4PxPyPzE neutrino(pers->m_neutrino.at(0),pers->m_neutrino.at(1),pers->m_neutrino.at(2),pers->m_neutrino.at(3));

  unsigned int indexHB = pers->m_indexHB; 
  unsigned int indexLB = pers->m_indexLB; 

  *trans = TopRec::TTbarLJDiTops<LeptonCollection>(base, htop_indexes, indexHB, neutrino, indexLB);

  msg<<MSG::DEBUG<<"TTbarLJDiTopsCnv_p1::persToTrans() end"<<endreq;
}

template<class LeptonCollection>
void TTbarLJDiTopsCnv_p1<LeptonCollection>::transToPers( const TopRec::TTbarLJDiTops<LeptonCollection>* trans, 
							 TTbarLJDiTops_p1* pers, 
							 MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TTbarLJDiTopsCnv_p1::transToPers() begin [LeptonCollection: "<<typeid(LeptonCollection).name()<<"]"<<endreq;
  m_baseCnv.transToPers(trans, pers, msg);

  pers->m_htop_indexes = trans->htop().jetIndexes();

  pers->m_neutrino.clear();
  pers->m_neutrino.push_back(trans->ltop().neutrino().px());
  pers->m_neutrino.push_back(trans->ltop().neutrino().py());
  pers->m_neutrino.push_back(trans->ltop().neutrino().pz());
  pers->m_neutrino.push_back(trans->ltop().neutrino().e());

  pers->m_indexHB = trans->htop().jetindexB();
  pers->m_indexLB = trans->ltop().jetindexB();

  msg<<MSG::DEBUG<<"TTbarLJDiTopsCnv_p1::transToPers() end"<<endreq;
}

//----------------------------------------------------------------
// Instantiate 

template class TTbarLJDiTopsCnv_p1<ElectronContainer>;
template class TTbarLJDiTopsCnv_p1<Analysis::MuonContainer>;
template class TTbarLJDiTopsCnv_p1<Analysis::TauJetContainer>;
template class TTbarLJDiTopsCnv_p1<TruthParticleContainer>;
template class TTbarLJDiTopsCnv_p1<JetCollection>;

//----------------------------------------------------------------
