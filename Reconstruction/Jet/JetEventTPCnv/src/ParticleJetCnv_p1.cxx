///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ParticleJetCnv_p1.cxx 
// Implementation file for class ParticleJetCnv_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// JetTagEvent includes
#include "JetEvent/Jet.h"
/* #include "JetTagEvent/JetConstituent.h"
   #include "JetTagEvent/TrackConstituents.h"
   #include "JetTagEvent/TrackAssociation.h" */
#include "egammaEvent/ElectronConstituent.h"
#include "egammaEvent/ElectronAssociation.h"
#include "MuonIDEvent/MuonConstituent.h"
#include "MuonIDEvent/MuonAssociation.h"

//#include "JetUtils/JetCaloHelper.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4ImplPxPyPzECnv_p1.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleBaseCnv_p1.h"


// ParticleEventTPCnv includes
#include "JetEventTPCnv/ParticleJetCnv_p1.h"




// converters
static P4ImplPxPyPzECnv_p1   momCnv;
//static P4PxPyPzECnv_p1    momCnv;
static ParticleBaseCnv_p1 partBaseCnv;

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

// Destructor
///////////////

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

#if 0
// This is copied from JetUtils/JetCaloHelper... avoid to depend on JetUtils
static std::string calosampling_name[24] = {
  "PreSamplerB", "EMB1", "EMB2", "EMB3",
  "PreSamplerE", "EME1", "EME2", "EME3",
  "HEC0", "HEC1", "HEC2", "HEC3",
  "TileBar0", "TileBar1", "TileBar2",
  "TileGap1", "TileGap2", "TileGap3",
  "TileExt0", "TileExt1", "TileExt2",     // Tile extended barrel
  "FCAL0", "FCAL1", "FCAL2"              // Forward EM endcap
};
#endif

void 
ParticleJetCnv_p1::persToTrans( const ParticleJet_p1* pers,
				Jet* trans, 
				MsgStream& msg ) 
{
  msg << MSG::DEBUG << "ParticleJet_p1 Loading Jet from persistent state...  pers="<< pers
      << "  trans="<< trans
      << endreq;

  msg << MSG::DEBUG << " ParticleJetCnv_p1  pers e="<< pers->m_momentum.m_ene  <<endreq;
  //msg << MSG::DEBUG << " pers e=" << pers->
  // base classes
  partBaseCnv.persToTrans( &pers->m_particleBase, &trans->particleBase(), msg );
  msg << MSG::DEBUG << "ParticleJet_p1  converted particlebase" << endreq;

  momCnv.persToTrans     ( &pers->m_momentum,     &trans->momentumBase(), msg );
  msg << MSG::DEBUG << "ParticleJet_p1  converted momentum" << endreq;


  trans->setSignalState(P4SignalState::CALIBRATED);
  msg << MSG::DEBUG << " ParticleJetCnv_p1  pers e="<< pers->m_momentum.m_ene << "   trans e="<< trans->e() <<endreq;
  
  trans->setCombinedLikelihood( pers->m_combinedLikelihood );

  trans->setJetAuthor( pers->m_author ); 

  size_t ntag = pers->m_tagJetInfo.size();
  for(size_t i=0;i<ntag;i++){
    Jet::taginfo_t * tagi = dynamic_cast<Jet::taginfo_t *>(pers->m_tagJetInfo[i]);
    trans->setTagInfo(tagi->infoType(), tagi);
  }

  // Constitutents are now Associations
  //trans->m_constituents = pers->m_constituents;
  size_t nconst = pers->m_constituents.size();
  for(size_t i=0;i<nconst;i++){
    Analysis::IConstituent* iconstit = pers->m_constituents[i]; 
    std::string constit_name = iconstit->name();
    msg << MSG::DEBUG << " constituent = " << constit_name << endreq; 
    
    // try jet constituent ----------
    /* Analysis::JetConstituent * jconstit = dynamic_cast<Analysis::JetConstituent*>(iconstit);
    if( jconstit ){
      // This is copied from JetUtils/JetCaloHelper... avoid to depend on JetUtils
      std::string base = "energy_";
      size_t nsample = jconstit->m_energyInSample.size();
      for(size_t c=0;c<nsample;c++){
	float e =  jconstit->m_energyInSample[c];
	if(e != 0) trans->setShape(base+ calosampling_name[c], e );
      }
	//JetCaloHelper::setEnergyInSampling(trans, (CaloSampling::CaloSample)c, jconstit->m_energyInSample[c]);

      delete jconstit;
      continue;
    }
    
    // try track constituent ----------
    Analysis::TrackConstituents * tkconst = dynamic_cast<Analysis::TrackConstituents*>(iconstit);
    if(tkconst){
      msg << MSG::DEBUG << " has track constituent "<< constit_name << endreq;
      Analysis::TrackAssociation * tassoc = new Analysis::TrackAssociation( constit_name);
      Analysis::TrackConstituents::object_iter it = tkconst->begin();
      Analysis::TrackConstituents::object_iter itE = tkconst->end();
      for(; it !=itE ; ++it){
	tassoc->set_track( tkconst->getContainer(it), *it);
      }
      trans->setAssociation(tassoc);
      delete tkconst;
      continue;
    }

    // try electron constituent ----------
    Analysis::ElectronConstituent * elconst = dynamic_cast<Analysis::ElectronConstituent*>(iconstit);
    if(elconst){
      msg << MSG::DEBUG << " has electron constituent "<< constit_name << endreq;
      Analysis::ElectronAssociation * tassoc = new Analysis::ElectronAssociation( constit_name);
      Analysis::ElectronConstituent::object_iter it = elconst->begin();
      Analysis::ElectronConstituent::object_iter itE = elconst->end();
      for(; it !=itE ; ++it){
	tassoc->set_electron( elconst->getContainer(it), *it);
      }
      trans->setAssociation(tassoc);
      delete elconst;
      continue;
    }

    // try muon constituent ----------
    Analysis::MuonConstituent * muconst = dynamic_cast<Analysis::MuonConstituent*>(iconstit);
    if(muconst){
      msg << MSG::DEBUG << " has electron constituent "<< constit_name << endreq;
      Analysis::MuonAssociation * tassoc = new Analysis::MuonAssociation( constit_name);
      Analysis::MuonConstituent::object_iter it = muconst->begin();
      Analysis::MuonConstituent::object_iter itE = muconst->end();
      for(; it !=itE ; ++it){
	tassoc->set_muon( muconst->getContainer(it), *it);
      }
      trans->setAssociation(tassoc);
      delete muconst;
      continue;
    }
    */
    
    delete iconstit;
  }
  //pers->m_constituents.clear();


  const_cast<ParticleJet_p1*>(pers)->m_tagJetInfo.clear();
  const_cast<ParticleJet_p1*>(pers)->m_constituents.clear();

  //   msg << MSG::DEBUG << "Loaded ParticleJet from persistent state [OK]"
  //       << endreq;
  return;
}

void 
ParticleJetCnv_p1::transToPers( const Jet* /*trans*/, 
				ParticleJet_p1* , 
				MsgStream& msg ) 
{
  msg << MSG::ERROR << "Creating persistent state of ParticleJet... This should not happen anymore"
      << endreq;

  // base classes

  //   msg << MSG::DEBUG << "Created persistent state of ParticleJet [OK]"
  //       << endreq;
  return;
}
