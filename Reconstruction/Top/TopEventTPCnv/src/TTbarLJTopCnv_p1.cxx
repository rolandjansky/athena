/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for classes instantiated from TTbarLJTop 
// 
// Andrei Gaponenko <agaponenko@lbl.gov>, 2008

#include <vector>
#include <algorithm>
#include <iterator>
#include <typeinfo>

// FIXME
#define private public
#define protected public

#include "TopEvent/TTbarLJTop.h"
#include "TopEventTPCnv/TTbarLJTop_p1.h"

#include "TopEventTPCnv/TTbarLJTopCnv_p1.h"

#include "StoreGate/StoreGateSvc.h"

#include "egammaEvent/Electron.h"
#include "egammaEvent/ElectronContainer.h"

#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"

#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"

#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"

template<class LeptonCollection>
void TTbarLJTopCnv_p1<LeptonCollection>::persToTrans(const TTbarLJTop_p1* pers,
						     TopRec::TTbarLJTop<LeptonCollection>* trans, 
						     MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TTbarLJTopCnv_p1::persToTrans() begin [LeptonCollection: "<<typeid(LeptonCollection).name()<<"]"<<endreq;

  TopRec::TTbarLJ<LeptonCollection> base;
  m_baseCnv.persToTrans(pers, &base,  msg);

  TopRec::IndexSet indexes;
  std::copy(pers->m_htop_indexes.begin(),  pers->m_htop_indexes.end(), std::inserter(indexes, indexes.begin()) );

  *trans = TopRec::TTbarLJTop<LeptonCollection>(base, indexes);

  msg<<MSG::DEBUG<<"TTbarLJTopCnv_p1::persToTrans() end"<<endreq;
}

template<class LeptonCollection>
void TTbarLJTopCnv_p1<LeptonCollection>::transToPers( const TopRec::TTbarLJTop<LeptonCollection>* trans, 
						   TTbarLJTop_p1* pers, 
						   MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TTbarLJTopCnv_p1::transToPers() begin [LeptonCollection: "<<typeid(LeptonCollection).name()<<"]"<<endreq;
  m_baseCnv.transToPers(trans, pers, msg);
  pers->m_htop_indexes = trans->hadronicTop().jetIndexes();
  msg<<MSG::DEBUG<<"TTbarLJTopCnv_p1::transToPers() end"<<endreq;
}

//----------------------------------------------------------------
// Instantiate 

template class TTbarLJTopCnv_p1<ElectronContainer>;
template class TTbarLJTopCnv_p1<Analysis::MuonContainer>;
template class TTbarLJTopCnv_p1<Analysis::TauJetContainer>;
template class TTbarLJTopCnv_p1<TruthParticleContainer>;

//----------------------------------------------------------------
