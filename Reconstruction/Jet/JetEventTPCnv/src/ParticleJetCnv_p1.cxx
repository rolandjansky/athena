///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ParticleJetCnv_p1.cxx 
// Implementation file for class ParticleJetCnv_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// JetTagEvent includes
#include "JetEvent/Jet.h"

#include "egammaEvent/ElectronConstituent.h"
#include "egammaEvent/ElectronAssociation.h"
#include "MuonIDEvent/MuonConstituent.h"
#include "MuonIDEvent/MuonAssociation.h"


// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4ImplPxPyPzECnv_p1.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleBaseCnv_p1.h"


// ParticleEventTPCnv includes
#include "JetEventTPCnv/ParticleJetCnv_p1.h"




// converters
static const P4ImplPxPyPzECnv_p1   momCnv;
static const ParticleBaseCnv_p1 partBaseCnv;


void 
ParticleJetCnv_p1::persToTrans( const ParticleJet_p1* pers,
				Jet* trans, 
				MsgStream& msg ) const
{
  msg << MSG::DEBUG << "ParticleJet_p1 Loading Jet from persistent state...  pers="<< pers
      << "  trans="<< trans
      << endmsg;

  msg << MSG::DEBUG << " ParticleJetCnv_p1  pers e="<< pers->m_momentum.m_ene  <<endmsg;
  // base classes
  partBaseCnv.persToTrans( &pers->m_particleBase, &trans->particleBase(), msg );
  msg << MSG::DEBUG << "ParticleJet_p1  converted particlebase" << endmsg;

  momCnv.persToTrans     ( &pers->m_momentum,     &trans->momentumBase(), msg );
  msg << MSG::DEBUG << "ParticleJet_p1  converted momentum" << endmsg;


  trans->setSignalState(P4SignalState::CALIBRATED);
  msg << MSG::DEBUG << " ParticleJetCnv_p1  pers e="<< pers->m_momentum.m_ene << "   trans e="<< trans->e() <<endmsg;
  
  trans->setCombinedLikelihood( pers->m_combinedLikelihood );

  trans->setJetAuthor( pers->m_author ); 

  size_t ntag = pers->m_tagJetInfo.size();
  for(size_t i=0;i<ntag;i++){
    Jet::taginfo_t * tagi = dynamic_cast<Jet::taginfo_t *>(pers->m_tagJetInfo[i]);
    if (tagi) trans->setTagInfo(tagi->infoType(), tagi);
  }

  // Constitutents are now Associations
  //trans->m_constituents = pers->m_constituents;
  size_t nconst = pers->m_constituents.size();
  for(size_t i=0;i<nconst;i++){
    Analysis::IConstituent* iconstit = pers->m_constituents[i]; 
    std::string constit_name = iconstit->name();
    msg << MSG::DEBUG << " constituent = " << constit_name << endmsg; 
    
    
    
    delete iconstit;
  }


  const_cast<ParticleJet_p1*>(pers)->m_tagJetInfo.clear();
  const_cast<ParticleJet_p1*>(pers)->m_constituents.clear();

  
  return;
}

void 
ParticleJetCnv_p1::transToPers( const Jet* /*trans*/, 
				ParticleJet_p1* , 
				MsgStream& msg ) const
{
  msg << MSG::ERROR << "Creating persistent state of ParticleJet... This should not happen anymore"
      << endmsg;

 
  return;
}
