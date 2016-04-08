/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MissingETPerformance/SplitElectronContainer.h"

#include <algorithm>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/Electron.h"

#include "McParticleEvent/TruthParticleContainer.h"
#include "McParticleEvent/TruthParticle.h"

#include "xAODEventInfo/EventInfo.h"

#include "FourMom/P4PtEtaPhiM.h"

SplitElectronContainer::SplitElectronContainer(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm( name, pSvcLocator ),
    m_evtInfoKey(""),
    m_event(0),
    m_matchedElectron(0),
    m_isMatched(false),
    m_stableEle(0),
    m_Dr(0)
{
  declareProperty("EvtInfoKey", m_evtInfoKey );

  // flags
  declareProperty("inverseSelection", m_invSel  =false );
  declareProperty("doTruth",          m_doTruth =true  );

  // input containers
  declareProperty("ElectronContainer",  m_eleContName        ="ElectronAODCollection");
  declareProperty("ZeeCollection",      m_ZeeCollName        ="ZeeElectrons"         );
  declareProperty("TruthPartContainer", m_truthContainerName ="SpclMC"               );

  // output containers
  declareProperty("modElectronContainer",      m_modEleContName     ="modElectronCollection");
  declareProperty("remElectronContainer",      m_remEleContName     ="remElectronCollection");
  declareProperty("selElectronContainer",      m_selEleContName     ="selElectronCollection");
  declareProperty("remElectronTruthContainer", m_remEleTruthContName="remEleTruthCollection");
  declareProperty("selElectronTruthContainer", m_selEleTruthContName="selEleTruthCollection");
}
//_____________________________________________________________________
SplitElectronContainer::~SplitElectronContainer()
{
}
//_____________________________________________________________________
StatusCode SplitElectronContainer::initialize()
{
 return StatusCode::SUCCESS;
}
//_____________________________________________________________________
StatusCode SplitElectronContainer::finalize()
{
  return StatusCode::SUCCESS;
}
//_____________________________________________________________________
StatusCode SplitElectronContainer::execute()
{
  msg() << MSG::INFO 
	  << "in execute()" 
	  << endreq;

  StatusCode sc;
  // Retrieve the container of electrons from Zee decay
  const ElectronContainer* Zelectrons;
  sc = evtStore()->retrieve(Zelectrons, m_ZeeCollName);
  
  if ( sc.isFailure() ) {
    
    msg() << MSG::ERROR 
	    << "Couldn't retrieve <"
	    << m_ZeeCollName
	    << "> container from StoreGateSvc"
	    << endreq;
    
    return sc;

  } else {

    msg() << MSG::DEBUG 
	    << "Retrieved <"
	    << m_ZeeCollName
	    << "> container from StoreGateSvc"
	    << endreq;
  }

  // Check the Zelectrons size
  if( Zelectrons->size() >2 ){
    
    msg() << MSG::WARNING
	    << "Found "
	    << Zelectrons->size()
	    << "electrons in Zee Container"
	    << endreq;
  }

  // Retrieve Electron Container from Storegate
  const ElectronContainer* elecColl;
  sc = evtStore()->retrieve(elecColl, m_eleContName);
  
  if ( sc.isFailure() ){
    
    msg() << MSG::ERROR
	    << "cannot allocate Electron Collection with key <"
	    << m_eleContName 
	    << ">"
	    << endreq;

    return sc;
    
  } else {
    
    msg() << MSG::DEBUG
	    << "Allocated Electron Collection with key <"
	    << m_eleContName 
	    << ">"
	    << "and size "
	    << elecColl->size()
	    << endreq;
  }

  // Retrieve Event INFO
  m_event = -1;
  const xAOD::EventInfo* pevt;
  
  sc = evtStore()->retrieve(pevt, m_evtInfoKey);
  
  if( sc.isFailure() ){

    return sc;

  } else {

    m_event = pevt->eventNumber();
    
    msg() << MSG::DEBUG 
	    << "Event #: "
	    << m_event
	    << endreq;
  }
  
  // Select the electron to remove/rescale 
  int rndNum = m_event;
  if(m_invSel) ++rndNum;

  const Analysis::Electron* remEle;
  const Analysis::Electron* selEle;

  if ( rndNum%2 == 0 ){
    
    remEle =  Zelectrons->at(0) ;
    selEle =  Zelectrons->at(1) ;
    
  } else {

    remEle =  Zelectrons->at(1) ;
    selEle =  Zelectrons->at(0) ;

  }

  // Output truth container
  ElectronContainer* m_remEleTruthCont = new ElectronContainer(SG::VIEW_ELEMENTS);
  sc = evtStore()->record(m_remEleTruthCont, m_remEleTruthContName, false);
  
  if( sc.isFailure() ) {
    
    msg() << MSG::ERROR
	    << "Unable to register particle Truth Container with name "
	    << m_remEleTruthContName
	    << endreq;
    
    return StatusCode::FAILURE;
  }
  
  // Output truth container
  // TruthParticleContainer* m_selEleTruthCont = new TruthParticleContainer(SG::VIEW_ELEMENTS);
  ElectronContainer* m_selEleTruthCont = new ElectronContainer(SG::VIEW_ELEMENTS);
  sc = evtStore()->record(m_selEleTruthCont, m_selEleTruthContName, false);
  
  if( sc.isFailure() ) {
    
    msg() << MSG::ERROR
	    << "Unable to register particle Truth Container with name "
	    << m_selEleTruthContName
	    << endreq;
    
    return StatusCode::FAILURE;
  }
  
  // New electron container
  egammaContainer* modEleCont = new egammaContainer(SG::VIEW_ELEMENTS);
  sc = evtStore()->record(modEleCont, m_modEleContName);

  // Removed electron container
  egammaContainer* remEleCont = new egammaContainer(SG::VIEW_ELEMENTS);
  sc = evtStore()->record(remEleCont, m_remEleContName);

  // Selected electron container
  egammaContainer* selEleCont = new egammaContainer(SG::VIEW_ELEMENTS);
  sc = evtStore()->record(selEleCont, m_selEleContName);

  // Find correspondent truth electrons
  if(m_doTruth){
    
    // Input truth container
    const TruthParticleContainer* truthCont;
    sc = evtStore()->retrieve( truthCont, m_truthContainerName);
   
    if( sc.isFailure() || ! truthCont ) {
    
      msg() << MSG::ERROR
	      << "Unable to retrieve particle Truth Container with name "
	      << m_truthContainerName
	      << endreq;
    
      return StatusCode::FAILURE;
    }
   
    // match truth with removed electron
    if( MatchTruthElectron(remEle, truthCont) ){

      m_remEleTruthCont->push_back( m_matchedElectron );

    } else {

      msg() << MSG::WARNING
	      << "NOT FOUND truth matching REMOVED electron!!!"
	      << endreq;
    }

    // match truth with selected electron
    if( MatchTruthElectron(selEle, truthCont) ){
      
      m_selEleTruthCont->push_back( m_matchedElectron );

    } else {

      msg() << MSG::WARNING
	      << "NOT FOUND truth matching SELECTED electron!!!"
	      << endreq;
    }
    
  }// end if(m_doTruth)

  /////////////////////////////////////////////////
  //     LOOP on Electrons
  ////////////////////////////////////////////////
  ElectronContainer::const_iterator fEle = elecColl->begin();
  ElectronContainer::const_iterator lEle = elecColl->end();

  msg() << MSG::DEBUG
          << "Electron Collection size: "
          << elecColl->size()
          << endreq;

  for (; fEle != lEle; fEle++ ){
    
    // Write in SG the removed electron
    if( (*fEle)->eta() == remEle->eta() &&
	(*fEle)->phi() == remEle->phi()    ){

      msg() << MSG::DEBUG
              << "Removed "
           // << (**fEle)
              << endreq;

      remEleCont->push_back(*fEle);

    } else {
      
      // Write in SG all electron except than removed
      msg() << MSG::DEBUG
              << "Inserted in standard container "
           // << (**fEle)
              << endreq;

      modEleCont->push_back(*fEle);

      // Write in SG the selected electron
      if( (*fEle)->eta() == selEle->eta() &&
	  (*fEle)->phi() == selEle->phi()    ){

	msg() << MSG::DEBUG
		<< "Selcted "
	     //	<< (**fEle)
		<< endreq;
      
	selEleCont->push_back(*fEle);

      }


    }
  }// end loop on electrons

  return StatusCode::SUCCESS;
}
//_____________________________________________________________________
bool SplitElectronContainer::MatchTruthElectron(const Analysis::Electron* recoEle, const TruthParticleContainer* truthCont)
{
  m_isMatched=false;
  
  // Loop on truth
  TruthParticleContainer::const_iterator pItr  = truthCont->begin();
  TruthParticleContainer::const_iterator pItrE = truthCont->end();

  for(;pItr != pItrE; ++pItr){
    
    // find stable electron
    m_stableEle = false;
    if( abs( (*pItr)->pdgId() )==11 && (*pItr)->status()==1 ) m_stableEle=true;
    
    // match in Dr(eta,phi)
    m_Dr = ( (*pItr)->eta() - recoEle->eta() ) * ( (*pItr)->eta() - recoEle->eta() )+
      ( (*pItr)->phi() - recoEle->phi() ) * ( (*pItr)->phi() - recoEle->phi() ) ;  	

    // select
    if( m_stableEle && m_Dr < 0.05 ){
      
      // look for mother
      if( (*pItr)->nParents() >=1 ){
	
	const HepMC::GenParticle* mother = (*pItr)->genMother(0);
	
	if( mother->pdg_id() == 23){
	  
	  // msg() << MSG::DEBUG
	  // 		  << "Removed electron"
	  // 		  << endreq;
	  
	  // 	  msg() << MSG::DEBUG
	  // 		  << "Matched reco ele: ("
	  // 		  << recoEle->eta() << ", "
	  // 		  << recoEle->phi() << ", "
	  // 		  << recoEle->pt()  << ")"
	  // 		  << endreq;
	  
	  // 	  msg() << MSG::DEBUG
	  // 		  << "WITH truth ele  : ("
	  // 		  << (*pItr)->eta() << ", "
	  // 		  << (*pItr)->phi() << ", "
	  // 		  << (*pItr)->pt()  << ")"
	  // 		  << endreq;
	  
	  if(!m_isMatched){
	   
	    P4PtEtaPhiM* truthElectron= new P4PtEtaPhiM( (*pItr)->pt(), (*pItr)->eta(), (*pItr)->phi(), (*pItr)->m() );

	    m_matchedElectron= new Analysis::Electron();
	    m_matchedElectron->set4Mom( truthElectron );
	      
	    m_isMatched=true;

	  }// end if(!m_isMatched)

	}// end if( mother->pdg_id() == 23)

      }// end if( (*pItr)->nParents() >=1 )

    }// end if( m_stableEle && m_Dr < 0.05 )

  }// end Loop on truth

  return m_isMatched;
}
