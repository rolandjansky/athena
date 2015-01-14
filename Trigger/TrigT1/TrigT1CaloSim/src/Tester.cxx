/***************************************************************************
                          Tester.cxx  -  description
                             -------------------
    begin                : Mon Apr 23 2001
    copyright            : (C) 2000 by moyse
    email                : e.moyse@qmw.ac.uk
 ***************************************************************************/
//
//    updated: June 20, M. Wielers
//             move to Storegate
// 
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// ================================================
// Tester class Implementation
// ================================================
//
//

// This algorithm includes
#include "TrigT1CaloSim/Tester.h"
#include <fstream>
#include <algorithm>
// #include "CaloGeoHelpers/CaloSampling.h"
#include "TrigT1CaloEvent/TriggerTower_ClassDEF.h"
#include "TrigT1CaloEvent/CPMTower_ClassDEF.h"
#include "TrigT1CaloEvent/JetElement_ClassDEF.h"
#include "TrigT1CaloEvent/EmTauROI_ClassDEF.h"
#include "TrigT1CaloEvent/JetROI_ClassDEF.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigT1CaloEvent/JEMHits_ClassDEF.h"
#include "TrigT1CaloEvent/JEMEtSums_ClassDEF.h"
#include "TrigT1CaloEvent/JEMRoI_ClassDEF.h"
#include "TrigT1CaloEvent/CPMHits_ClassDEF.h"
#include "TrigT1CaloEvent/CPMRoI_ClassDEF.h"
#include "TrigT1CaloEvent/CMMCPHits_ClassDEF.h"
#include "TrigT1CaloEvent/CMMJetHits_ClassDEF.h"
#include "TrigT1CaloEvent/CMMEtSums_ClassDEF.h"
#include "TrigT1CaloEvent/CPMTobRoI_ClassDEF.h"
#include "TrigT1CaloEvent/CPMCMXData_ClassDEF.h"
#include "TrigT1CaloEvent/JEMTobRoI_ClassDEF.h"
#include "TrigT1CaloEvent/JetCMXData_ClassDEF.h"
#include "TrigT1CaloEvent/CMMRoI.h"
#include "TrigT1CaloEvent/CPCMXTopoData.h"
#include "TrigT1CaloEvent/JetCMXTopoData.h"
#include "TrigT1CaloEvent/CPTopoTOB.h"
#include "TrigT1CaloEvent/JetTopoTOB.h"
#include "TrigT1CaloEvent/EnergyTopoData.h"
#include "TrigT1CaloUtils/ModuleEnergy.h"
#include "TrigT1CaloUtils/CrateEnergy.h"
#include "TrigT1CaloUtils/SystemEnergy.h"
#include "TrigT1Interfaces/EmTauCTP.h"
#include "TrigT1Interfaces/JetCTP.h"
#include "TrigT1Interfaces/EnergyCTP.h"

namespace LVL1{


//--------------------------------
// Constructors and destructors
//--------------------------------

Tester::Tester
  ( const std::string& name, ISvcLocator* pSvcLocator ) 
    : Algorithm( name, pSvcLocator ), 
      m_storeGate("StoreGateSvc", name),
      m_VectorOfEmTauROIs(0),
      m_EmTauSlinkLocation(TrigT1CaloDefs::EmTauSlinkLocation),
      m_EmTauTool("LVL1::L1EmTauTools/L1EmTauTools"),
      m_JetTool("LVL1::L1JetTools/L1JetTools"),
      m_EtTool("LVL1::L1EtTools/L1EtTools")
{
	m_numberOfErrors=0;
	m_numberOfEvents=0;
	m_eventNumber=0;
	m_mode=0;
  //set defaults
  m_TriggerTowerLocation   = TrigT1CaloDefs::TriggerTowerLocation ;
  m_JetElementLocation     = TrigT1CaloDefs::JetElementLocation;
  m_EmTauROILocation       = TrigT1CaloDefs::EmTauROILocation ;
  m_JetROILocation         = TrigT1CaloDefs::JetROILocation ;
  m_actualROIWordLocation  = DEFAULT_actualROIWordLocation ;
  m_jemHitsLocation        = LVL1::TrigT1CaloDefs::JEMHitsLocation ;
  m_jemEtSumsLocation      = LVL1::TrigT1CaloDefs::JEMEtSumsLocation ;
  m_CPMTobRoILocation      = TrigT1CaloDefs::CPMTobRoILocation;
  m_CPMCMXDataLocation     = TrigT1CaloDefs::CPMCMXDataLocation ;
  m_JEMTobRoILocation      = TrigT1CaloDefs::JEMTobRoILocation;
  m_JetCMXDataLocation     = TrigT1CaloDefs::JetCMXDataLocation;
  m_CPCMXTopoDataLocation  = TrigT1CaloDefs::EmTauTopoTobLocation;
  m_JetCMXTopoDataLocation  = TrigT1CaloDefs::JetTopoTobLocation;
  m_EnergyTopoDataLocation  = TrigT1CaloDefs::EnergyTopoDataLocation;
  m_EmTauCTPLocation        = TrigT1CaloDefs::EmTauCTPLocation;
  m_JetCTPLocation          = TrigT1CaloDefs::JetCTPLocation;
  m_EnergyCTPLocation       = TrigT1CaloDefs::EnergyCTPLocation;

  declareProperty("EventStore",m_storeGate,"StoreGate Service");
  declareProperty( "L1EmTauTools", m_EmTauTool, "Tool for EM/Tau trigger simulation");
  declareProperty( "L1JetTools", m_JetTool, "Tool for Jet trigger simulation");
  declareProperty( "L1EtTools", m_EtTool, "Tool for ET trigger simulation");

  // This is how you declare the paramemters to Gaudi so that
  // they can be over-written via the job options file
  declareProperty(  "TriggerTowerLocation",   m_TriggerTowerLocation ) ;
  declareProperty( "JetElementLocation",         m_JetElementLocation ) ;
  declareProperty( "EMTauROILocation",     m_EmTauROILocation ) ;
  declareProperty( "JetROILocation",     m_JetROILocation ) ;
  declareProperty( "ActualROIWordLocation",     m_actualROIWordLocation );
  declareProperty( "Mode", m_mode);
  declareProperty(  "EmTauSlinkLocation",       m_EmTauSlinkLocation );

  for (unsigned int i = 0; i < TrigT1CaloDefs::numOfCPRoIRODs; ++i) {
    m_emTauSlink[i] = 0;
  }
}



// Destructor
Tester::~Tester() {
    MsgStream log( messageService(), name() ) ;
    log << MSG::INFO << "Destructor called" << endreq;
} 


//---------------------------------
// initialise() 
//---------------------------------

StatusCode Tester::initialize()
{

  // We must here instantiate items which can only be made after
  // any job options have been set
  MsgStream log( messageService(), name() ) ;

  StatusCode sc = m_storeGate.retrieve();
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Couldn't connect to " << m_storeGate.typeAndName() 
        << endreq;
  } else {
    log << MSG::DEBUG << "Connected to " << m_storeGate.typeAndName() 
        << endreq;
  }

  return StatusCode::SUCCESS ;
}


//---------------------------------
// finalise() 
//---------------------------------

StatusCode Tester::finalize()
{
   MsgStream log( messageService(), name() ) ;
   log << MSG::INFO << "Finalizing" << endreq;
	 log << MSG::INFO << "TOTAL ERRORS : " <<m_numberOfErrors<<" out of "<<m_numberOfEvents<<" examined events."<< endreq;
	 
	
   for( std::vector<int>::const_iterator it= m_eventsWithErrors.begin(); it < m_eventsWithErrors.end(); ++it){
	   log << MSG::DEBUG<<"Error in event :"<<std::ios::dec<<(*it)<<endreq;
   }

   return StatusCode::SUCCESS ;
}


//----------------------------------------------
// execute() method called once per event
//----------------------------------------------
//


StatusCode Tester::execute( )
{

  //................................
  // make a message logging stream

  MsgStream log( messageService(), name() ) ;
  log << MSG::DEBUG << "Executing" << endreq;


	m_numberOfEvents++;
	m_actualROIWords.erase(m_actualROIWords.begin(), m_actualROIWords.end());
	m_generatedROIWords.erase(m_generatedROIWords.begin(), m_generatedROIWords.end());

  // Dump TriggerTower values.
	if (m_mode&m_dumpTTs) {printTriggerTowerValues();}
  // Dump EmTauROI values.
	if (m_mode&m_dumpEmTauRoIs) {loadEmTauROIs();}
  // try to compare generated ROI words with actual ROI words.
	if (m_mode&m_compareEmTauRoIs) {loadEmTauROIs();// get some ROIs
			                          loadActualROIWord();
			                          compareROIWords();}
  // compare coords from RoIwords and RoIDecoder and original RoI objects
  if (m_mode&m_compRoIWrdCoords) {compareRoIWordCoords();}
  // look for specific RoI and dump its calo cells
  if (m_mode&m_dumpRoICells) {dumpROICells();}

  if (m_mode&m_dumpJEMResults) {dumpJEMResults();}

  if (m_mode&m_testEtTool) {testEtTool();}

  if (m_mode&m_testJetTool) {testJetTool();}
  
  if (m_mode&m_dumpEDM) {dumpEDM();}

  return StatusCode::SUCCESS ;
}

void LVL1::Tester::loadTriggerTowers(){
//  MsgStream log( messageService(), name() ) ;
//
//  typedef DataVector<LVL1::TriggerTower> t_TTCollection ;
//  DataHandle<t_TTCollection> TTVector;
//
//  if( m_storeGate->retrieve(TTVector, m_TriggerTowerLocation).isFailure() ){
//    log << MSG::ERROR
//        << "No TriggerTowers found in TES at "
//        << m_TriggerTowerLocation
//        << endreq ;
//    return;
//  }
//
//  log << MSG::INFO
//      << "Got "
//      << TTVector->size()
//      << " Trigger Towers from TES"
//      << endreq ;
//
//
//  return;
} // end of loadTriggerTowers()

/** prints useful TriggerTower values*/
void LVL1::Tester::printTriggerTowerValues(){
	  MsgStream log( messageService(), name() ) ;

typedef DataVector<LVL1::TriggerTower> t_TTCollection ;
  const t_TTCollection* TTVector;

  if( m_storeGate->retrieve(TTVector, m_TriggerTowerLocation).isFailure() ){
    log << MSG::DEBUG
        << "No TriggerTowers found in TES at "
        << m_TriggerTowerLocation
        << endreq ;
  return;
  }

  log << MSG::INFO
      << "Got "
      << TTVector->size()
      << " Trigger Towers from TES"
      << endreq ;

	// Step over all TTs and print values...
  t_TTCollection::const_iterator it ;

  for( it  = TTVector->begin(); it < TTVector->end(); ++it ){
	  log << MSG::INFO<<"TT has coords ("<<(*it)->phi()<<", "<<(*it)->eta()
		    << " and energies : "<<(*it)->emEnergy()<<", "<<(*it)->hadEnergy()<<" (Em,Had)"<<endreq;
	}//end it for-loop
}//end of printTTs

/** loads the EmTauROIs from the TES.*/
void LVL1::Tester::loadEmTauROIs(){
  MsgStream log( messageService(), name() ) ;

  const t_EmTauROICollection* ROIs;
  StatusCode sc1 = m_storeGate->retrieve(ROIs, m_EmTauROILocation);

  if( ! ROIs ) {
    log << MSG::DEBUG
        << "No ROIs found in TES at "
        << m_EmTauROILocation
        << endreq ;
        return;
  } else {
    log << MSG::DEBUG
        << "Found "<<ROIs->size()
        << " ROI(s)"<<endreq;

    t_EmTauROICollection::const_iterator it ;

    for( it  = ROIs->begin(); it < ROIs->end(); ++it){
       int tempROIword=(*it)->roiWord();
       log << MSG::DEBUG<<"ROI has ROIword : " << std::hex
           <<  tempROIword << std::dec << endreq
           << "eta         : " << (*it)->eta() << endreq
           << "phi         : " << (*it)->phi() << endreq
           << "Core ET     : " << (*it)->energy() << endreq
           << "EM cluster  : " << (*it)->clusterEnergy() << endreq
           << "Tau cluster : " << (*it)->tauClusterEnergy() << endreq
           << "EM isol     : " << (*it)->emRingIsolationEnergy() << endreq
           << "Had isol    : " << (*it)->hadRingIsolationEnergy() << endreq
           << "Had veto    : " << (*it)->hadCoreEnergy() << endreq;
       //m_generatedROIWords.push_back(tempROIword);
    }
  }

  // Now test new EM/Tau implementation
  
  StatusCode sc = m_EmTauTool.retrieve();
  if (sc.isFailure()) {
    log << MSG::ERROR << "Problem retrieving EmTauTool" << endreq;
  }
  else {
    const DataVector<LVL1::TriggerTower>* TTVector;
    if( m_storeGate->retrieve(TTVector, m_TriggerTowerLocation).isSuccess() ) {  
      DataVector<CPAlgorithm>* rois = new DataVector<CPAlgorithm>;
      m_EmTauTool->findRoIs(TTVector,rois);
      DataVector<CPAlgorithm>::iterator cpw = rois->begin();
      for ( ; cpw != rois->end(); cpw++) {
        log << MSG::DEBUG<<"CPAlgorithm has properties : " << std::hex
           << "RoIWord     : " << std::hex << (*cpw)->RoIWord() << std::dec << endreq
           << "eta         : " << (*cpw)->eta() << endreq
           << "phi         : " << (*cpw)->phi() << endreq
           << "Core ET     : " << (*cpw)->Core() << endreq
           << "EM cluster  : " << (*cpw)->EMClus() << endreq
           << "Tau cluster : " << (*cpw)->TauClus() << endreq
           << "EM isol     : " << (*cpw)->EMIsol() << endreq
           << "Had isol    : " << (*cpw)->HadIsol() << endreq
           << "Had veto    : " << (*cpw)->HadVeto() << endreq;
      }
      delete rois;
    }
  }
    
  return;
} // end of loadROIs()

/** loads the Actual ROI words from the TES.*/
void LVL1::Tester::loadActualROIWord(){
	MsgStream log( messageService(), name() ) ;
	char line[60]="";    // to read a line into
  long lineNumber=0;
  int numRows=2;

  log<< MSG::DEBUG <<"Trying to open input file " << m_actualROIWordLocation.c_str() << endreq ;
  std::ifstream in( m_actualROIWordLocation.c_str() );
  if (!in){
    log << MSG::FATAL << "Could not find ROI words in "<<m_actualROIWordLocation.c_str() << endreq;
    return;
  }else{
  	TriggerTowerKey get(0.0, 0.0);


  	log << MSG::DEBUG
      	<< "Event Number : "<<std::ios::dec<<m_eventNumber
	    	<<endreq;

  	while ( lineNumber < ((m_eventNumber-1)*3) ){
    	in.getline(line,50);
    	lineNumber++;
    	}

  	for ( int row = 1 ; row <= numRows ; row++ ){
    	lineNumber++;

    	int input=0;
    	//if ( in.eof( ) )
    	//   return;
    	in  >> input;


			// I'm ignoring the left/right info...
			input &= 0x0000FFFF;

			if (input>0){
    		log << MSG::DEBUG
	      		<< " LineNumber : "<<std::ios::dec<<lineNumber
	      		<< " ROIword : "<<std::ios::dec<<input<<" and in hex : "<<std::ios::hex<<input
	      		<<endreq;
				m_actualROIWords.push_back(input);
			}
  	} // end phi for loop
	}
	return;
}//end of loadActualROIWords

/** Compares generated ROIwords with actual ROIwords and checks to make sure they are the same.*/
void LVL1::Tester::compareROIWords(){
	MsgStream log( messageService(), name() ) ;
	if (m_actualROIWords.size()!=m_generatedROIWords.size()){
		log<< MSG::FATAL <<"Different numbers of ROI words in generated and actual data!" << endreq;
	}else{
		sort(m_actualROIWords.begin(), m_actualROIWords.end());
		sort(m_generatedROIWords.begin(), m_generatedROIWords.end());
		if (equal(m_actualROIWords.begin(), m_actualROIWords.end(),m_generatedROIWords.begin() )){
		}else{
			log<< MSG::INFO <<"ROIWords do not match!!!!!!!!!!!!!!!!!!!!!" << endreq;
			m_numberOfErrors++;
			m_eventsWithErrors.push_back(m_eventNumber);
		}
	}
	return;
}//end of compareROIWords

/** loads the EmTauROIs from the TES.*/
void LVL1::Tester::testJetTool(){
  MsgStream log( messageService(), name() ) ;

  const t_JetROICollection* ROIs;
  StatusCode sc1 = m_storeGate->retrieve(ROIs, m_JetROILocation);
  StatusCode scTool = m_JetTool.retrieve();

  const DataVector<JetElement>* storedJEs;
  std::map<int, JetInput*>* jetInputs = new std::map<int, JetInput*>;
  if (scTool.isSuccess()) {
    StatusCode sc = m_storeGate->retrieve(storedJEs,m_JetElementLocation);
    if ( sc==StatusCode::SUCCESS ) m_JetTool->mapJetInputs(storedJEs,jetInputs);
  }

  if( ! ROIs ) {
    log << MSG::DEBUG
        << "No ROIs found in TES at "
        << m_JetROILocation
        << endreq ;
	delete jetInputs;
        return;
  } else {
    log << MSG::DEBUG
        << "Found "<<ROIs->size()
        << " ROI(s)"<<endreq;

    t_JetROICollection::const_iterator it ;

    for( it  = ROIs->begin(); it < ROIs->end(); ++it){
       int tempROIword=(*it)->roiWord();
       log << MSG::DEBUG<<"ROI has ROIword : " << std::hex
           <<  tempROIword << std::dec << endreq
           << "eta         : " << (*it)->eta() << endreq
           << "phi         : " << (*it)->phi() << endreq
           << "ET 4x4      : " << (*it)->clusterEnergy4() << endreq
           << "ET 6x6      : " << (*it)->clusterEnergy6() << endreq
           << "ET 8x8      : " << (*it)->clusterEnergy8() << endreq;
       if (scTool.isSuccess()) {
         JetAlgorithm test = m_JetTool->findRoI((*it)->eta(), (*it)->phi(), jetInputs);
         log << MSG::DEBUG << "JetAlgorithm gives: " << endreq
             << "ROIword     : " << test.RoIWord() << endreq
             << "isEtMax     : " << test.isEtMax() << endreq
             << "eta         : " << test.eta() << endreq
             << "phi         : " << test.phi() << endreq
             << "ET 4x4      : " << test.ET4x4() << endreq
             << "ET 6x6      : " << test.ET6x6() << endreq
             << "ET 8x8      : " << test.ET8x8() << endreq;
       }
    }
  }

  // Now test new jet trigger implementation
  
  if (scTool.isSuccess()) {
    DataVector<JetAlgorithm>* rois = new DataVector<JetAlgorithm>;
    m_JetTool->findRoIs(storedJEs, rois);
    DataVector<JetAlgorithm>::iterator cpw = rois->begin();
    for ( ; cpw != rois->end(); cpw++) {
        log << MSG::DEBUG<<"JetAlgorithm has properties : " << endreq << std::hex
           << "RoIWord     : " << std::hex << (*cpw)->RoIWord() << std::dec << endreq
           << "eta         : " << (*cpw)->eta() << endreq
           << "phi         : " << (*cpw)->phi() << endreq
           << "ET 4x4      : " << (*cpw)->ET4x4() << endreq
           << "ET 6x6      : " << (*cpw)->ET6x6() << endreq
           << "ET 8x8      : " << (*cpw)->ET8x8() << endreq;
    }
    delete rois;
  }

  delete jetInputs;
  
  return;
} // end of testJetTool()


/** prints useful TriggerTower values*/
void LVL1::Tester::dumpEDM(){
  MsgStream log( messageService(), name() ) ;

  typedef DataVector<LVL1::TriggerTower> t_TTCollection ;
  const t_TTCollection* TTVector;

  if( m_storeGate->retrieve(TTVector, m_TriggerTowerLocation).isFailure() ){
    log << MSG::INFO
        << "No TriggerTowers found in TES at "
        << m_TriggerTowerLocation
        << endreq ;
  }
  else {

    log << MSG::INFO
        << "Got "
        << TTVector->size()
        << " Trigger Towers from TES"
        << endreq ;

        // Step over all TTs and print values...
    t_TTCollection::const_iterator it ;

    for( it  = TTVector->begin(); it < TTVector->end(); ++it ){
          log << MSG::INFO<<"TT has coords ("<<(*it)->phi()<<", "<<(*it)->eta()
                    << " and energies : "<<(*it)->emEnergy()<<", "<<(*it)->hadEnergy()<<" (Em,Had)"<<endreq;
    }//end it for-loop
  }


  // Now do the same for CPMTowers
  const DataVector<LVL1::CPMTower>* CPMTVector;

  if( m_storeGate->retrieve(CPMTVector, TrigT1CaloDefs::CPMTowerLocation).isFailure() ){
    log << MSG::INFO << "No CPMTowers found in TES at "
        << TrigT1CaloDefs::CPMTowerLocation << endreq ;
  }
  else {
 
    log << MSG::INFO << "Got " << CPMTVector->size() << " CPMTowers from TES" << endreq ;

        // Step over all TTs and print values...
    DataVector<LVL1::CPMTower>::const_iterator it2 ;
    for( it2  = CPMTVector->begin(); it2 < CPMTVector->end(); ++it2 ){
          log << MSG::INFO<<"CPMT has coords ("<<(*it2)->phi()<<", "<<(*it2)->eta()
                    << " and energies : "<<(*it2)->emEnergy()<<", "<<(*it2)->hadEnergy()<<" (Em,Had)"<<endreq;
    }//end it for-loop
  }

    
  // Now do the same for JetElements
  const DataVector<LVL1::JetElement>* JEVector;

  if( m_storeGate->retrieve(JEVector, TrigT1CaloDefs::JetElementLocation).isFailure() ){
    log << MSG::INFO << "No JetElements found in TES at "
        << TrigT1CaloDefs::JetElementLocation << endreq ;
  }
  else {
    log << MSG::INFO << "Got " << JEVector->size() << " JetElements from TES" << endreq ;

        // Step over all TTs and print values...
    DataVector<LVL1::JetElement>::const_iterator it3 ;
    for( it3 = JEVector->begin(); it3 < JEVector->end(); ++it3 ){
          log << MSG::INFO<<"JE has coords ("<<(*it3)->phi()<<", "<<(*it3)->eta()
                    << " and energies : "<<(*it3)->emEnergy()<<", "<<(*it3)->hadEnergy()<<" (Em,Had)"<<endreq;
    }//end it for-loop
  }

    
  // Now do the same for CPMHits
  const DataVector<LVL1::CPMHits>* CPMHVector;

  if( m_storeGate->retrieve(CPMHVector, TrigT1CaloDefs::CPMHitsLocation).isFailure() ){
    log << MSG::INFO << "No CPMHits found in TES at "
        << TrigT1CaloDefs::CPMHitsLocation << endreq ;
  }
  else {
  
    log << MSG::INFO << "Got " << CPMHVector->size() << " CPMHits from TES" << endreq ;

        // Step over all TTs and print values...
    DataVector<LVL1::CPMHits>::const_iterator it4 ;
    for( it4 = CPMHVector->begin(); it4 < CPMHVector->end(); ++it4 ){
          log << MSG::INFO<<"CPMHits from crate "<<(*it4)->crate()
                    <<" module "<<(*it4)->module() << " hitwords : "
                    << std::hex << (*it4)->HitWord0()<<", "<<(*it4)->HitWord1()
                    << std::dec <<endreq;
    }//end it for-loop
  }
 
    
  // Now do the same for CPMRoIs
  const DataVector<LVL1::CPMRoI>* CPMRVector;

  if( m_storeGate->retrieve(CPMRVector, TrigT1CaloDefs::CPMRoILocation).isFailure() ){
    log << MSG::INFO << "No CPMRoIs found in TES at "
        << TrigT1CaloDefs::CPMRoILocation << endreq ;
  }
  else {

    log << MSG::INFO << "Got " << CPMRVector->size() << " CPMRoIs from TES" << endreq ;

        // Step over all TTs and print values...
    DataVector<LVL1::CPMRoI>::const_iterator it5 ;
    for( it5 = CPMRVector->begin(); it5 < CPMRVector->end(); ++it5 ){
          log << MSG::INFO << "CPMRoI from crate " << (*it5)->crate()
                    <<" module " << (*it5)->cpm() << " chip " << (*it5)->chip()
                    << " LC " << (*it5)->location()
                    << " word " << std::hex << (*it5)->hits() << std::dec <<endreq;
    }//end it for-loop
  }


    
  // Now do the same for CMMCPHits
  const DataVector<LVL1::CMMCPHits>* CMMCPHVector;

  if( m_storeGate->retrieve(CMMCPHVector, TrigT1CaloDefs::CMMCPHitsLocation).isFailure() ){
    log << MSG::INFO << "No CMMCPHits found in TES at "
        << TrigT1CaloDefs::CMMCPHitsLocation << endreq ;
  }
  else {
  
    log << MSG::INFO << "Got " << CMMCPHVector->size() << " CMMCPHits from TES" << endreq ;

        // Step over all TTs and print values...
    DataVector<LVL1::CMMCPHits>::const_iterator it6;
    for( it6 = CMMCPHVector->begin(); it6 < CMMCPHVector->end(); ++it6 ){
          log << MSG::INFO<<"CMMCPHits from crate "<<(*it6)->crate()
                    <<" dataID "<<(*it6)->dataID() << " hitwords : "
                    << std::hex << (*it6)->HitWord0()<<", "<<(*it6)->HitWord1()
                    << std::dec <<endreq;
    }//end it for-loop
  }

    
  // Now do the same for CMMJetHits
  const DataVector<LVL1::CMMJetHits>* CMMJHVector;

  if( m_storeGate->retrieve(CMMJHVector, TrigT1CaloDefs::CMMJetHitsLocation).isFailure() ){
    log << MSG::INFO << "No CMMJetHits found in TES at "
        << TrigT1CaloDefs::CMMJetHitsLocation << endreq ;
  }
  else {

    log << MSG::INFO << "Got " << CMMJHVector->size() << " CMMJetHits from TES" << endreq ;

        // Step over all TTs and print values...
    DataVector<LVL1::CMMJetHits>::const_iterator it7;
    for( it7 = CMMJHVector->begin(); it7 < CMMJHVector->end(); ++it7 ){
          log << MSG::INFO<<"CMMJetHits from crate "<<(*it7)->crate()
                    <<" dataID "<<(*it7)->dataID() << " hitwords : "
                    << std::hex << (*it7)->Hits()
                    << std::dec <<endreq;
    }//end it for-loop
  }

    
  // Now do the same for CMMEtSums
  const DataVector<LVL1::CMMEtSums>* CMMESVector;

  if( m_storeGate->retrieve(CMMESVector, TrigT1CaloDefs::CMMEtSumsLocation).isFailure() ){
    log << MSG::INFO << "No CMMEtSums found in TES at "
        << TrigT1CaloDefs::CMMEtSumsLocation << endreq ;
  }
  else {

    log << MSG::INFO << "Got " << CMMESVector->size() << " CMMEtSums from TES" << endreq ;

        // Step over all TTs and print values...
    DataVector<LVL1::CMMEtSums>::const_iterator it8;
    for( it8 = CMMESVector->begin(); it8 < CMMESVector->end(); ++it8 ){
          log << MSG::INFO<<"CMMEtSums from crate "<<(*it8)->crate()
                    <<" dataID "<<(*it8)->dataID() << " ET: "
                    << (*it8)->Et() << " Ex: " << (*it8)->Ex() << " Ey: " << (*it8)->Ey()
                    <<endreq;
    }//end it for-loop
  }

    
  // Now do the same for JEMHits
  const DataVector<LVL1::JEMHits>* JEMHVector;

  if( m_storeGate->retrieve(JEMHVector, TrigT1CaloDefs::JEMHitsLocation).isFailure() ){
    log << MSG::INFO << "No JEMHits found in TES at "
        << TrigT1CaloDefs::JEMHitsLocation << endreq ;
  }
  else {

    log << MSG::INFO << "Got " << JEMHVector->size() << " JEMHits from TES" << endreq ;

        // Step over all TTs and print values...
    DataVector<LVL1::JEMHits>::const_iterator it9 ;
    for( it9 = JEMHVector->begin(); it9 < JEMHVector->end(); ++it9 ){
          log << MSG::INFO<<"JEMHits from crate "<<(*it9)->crate()
                    <<" module "<<(*it9)->module() << " hitword : "
                    << std::hex << (*it9)->JetHits()
                    << std::dec <<endreq;
    }//end it for-loop
  }

    
  // Now do the same for JEMEtSums
  const DataVector<LVL1::JEMEtSums>* JEMESVector;

  if( m_storeGate->retrieve(JEMESVector, TrigT1CaloDefs::JEMEtSumsLocation).isFailure() ){
    log << MSG::INFO << "No JEMEtSums found in TES at "
        << TrigT1CaloDefs::JEMEtSumsLocation << endreq ;
  }
  else {

    log << MSG::INFO << "Got " << JEMESVector->size() << " JEMEtSums from TES" << endreq ;

        // Step over all TTs and print values...
    DataVector<LVL1::JEMEtSums>::const_iterator it10;
    for( it10 = JEMESVector->begin(); it10 < JEMESVector->end(); ++it10 ){
          log << MSG::INFO<<"JEMEtSums from crate "<<(*it10)->crate()
                    <<" module "<<(*it10)->module() << " ET: "
                    << (*it10)->Et() << " Ex: " << (*it10)->Ex() << " Ey: " << (*it10)->Ey()
                    <<endreq;
    }//end it for-loop
  }
 
    
  // Now do the same for JEMRoIs
  const DataVector<LVL1::JEMRoI>* JEMRVector;

  if( m_storeGate->retrieve(JEMRVector, TrigT1CaloDefs::JEMRoILocation).isFailure() ){
    log << MSG::INFO << "No JEMRoIs found in TES at "
        << TrigT1CaloDefs::JEMRoILocation << endreq ;
  }
  else {
  
    log << MSG::INFO << "Got " << JEMRVector->size() << " JEMRoIs from TES" << endreq ;

        // Step over all TTs and print values...
    DataVector<LVL1::JEMRoI>::const_iterator it11 ;
    for( it11 = JEMRVector->begin(); it11 < JEMRVector->end(); ++it11 ){
          log << MSG::INFO << "JEMRoI from crate " << (*it11)->crate()
                    <<" module " << (*it11)->jem() << " frame " << (*it11)->frame()
                    << " LC " << (*it11)->location()
                    << " hits " << std::hex << (*it11)->hits() << std::dec <<endreq;
    }//end it for-loop
  }

    
  // Now do the same for CPMTobRoI
  const DataVector<CPMTobRoI>* cpmtobrois;

  if( m_storeGate->retrieve(cpmtobrois, m_CPMTobRoILocation).isFailure() ){
    log << MSG::INFO << "No CPMTobRoI found in TES at "
        << m_CPMTobRoILocation << endreq ;
  }
  else {
  
    log << MSG::INFO << "Got " << cpmtobrois->size() << " CPMTobRoI from TES" << endreq ;

    // print values...
    DataVector<LVL1::CPMTobRoI>::const_iterator itcp ;
    for( itcp = cpmtobrois->begin(); itcp < cpmtobrois->end(); ++itcp ) {
        log << MSG::INFO << std::hex
                   << "RoI Word " << (*itcp)->roiWord()
                   << std::dec << " Crate = "
		   << (*itcp)->crate() << ", Module = " << (*itcp)->cpm()
		   << ", Chip = " << (*itcp)->chip() 
		   << ", Coordinate = " << (*itcp)->location()
		   << endreq;
        log << MSG::INFO << " ET " << (*itcp)->energy()
                   << " Isolation " << std::hex << (*itcp)->isolation() << std::dec
                   << " Type = " << (*itcp)->type()
                   <<endreq;
    }
    
  }
  
    
  // CPM->CMX Data
  const DataVector<CPMCMXData>* emcmx;

  if( m_storeGate->retrieve(emcmx, m_CPMCMXDataLocation).isFailure() ){
    log << MSG::INFO << "No CPMCMXData found in TES at "
        << m_CPMCMXDataLocation << endreq ;
  }
  else {
  
    log << MSG::INFO << "Got " << emcmx->size() << " CPMCMXData from TES" << endreq ;

    // print values...
    DataVector<LVL1::CPMCMXData>::const_iterator item ;
    for( item = emcmx->begin(); item < emcmx->end(); ++item ) {
        log << MSG::INFO
                   << " Crate = " << (*item)->crate() << ", Module = " << (*item)->module()
		   << ", Type = " << (*item)->type() 
		   << endreq;
	std::vector<unsigned int> data = (*item)->DataWords();
        log << MSG::INFO << " Map " << std::hex << (*item)->presenceMap() << std::dec << endreq;
	log << MSG::INFO << " Data : " << std::hex << data[0] << "  " << data[1]
	                 << "  " << data[2] << "  " << data[3] << std::dec << endreq;
    }
    
  }
 
    
    
  // Now do the same for JEMTobRoI
  const DataVector<JEMTobRoI>* jemtobrois;

  if( m_storeGate->retrieve(jemtobrois, m_JEMTobRoILocation).isFailure() ){
    log << MSG::INFO << "No JEMTobRoI found in TES at "
        << m_JEMTobRoILocation << endreq ;
  }
  else {
  
    log << MSG::INFO << "Got " << jemtobrois->size() << " JEMTobRoI from TES" << endreq ;

    // print values...
    DataVector<LVL1::JEMTobRoI>::const_iterator itje ;
    for( itje = jemtobrois->begin(); itje < jemtobrois->end(); ++itje ) {
        log << MSG::INFO << std::hex
                   << "RoI Word " << (*itje)->roiWord()
                   << std::dec << " Crate = "
		   << (*itje)->crate() << ", Module = " << (*itje)->jem()
		   << ", Frame = " << (*itje)->frame() 
		   << ", Coordinate = " << (*itje)->location()
		   << endreq;
        log << MSG::INFO << " ETLarge = " << (*itje)->energyLarge()
                   << " ETSmall = " << (*itje)->energySmall()
                   <<endreq;
    }
    
  }
   
   
  // Jet JEM->CMX Data
  const DataVector<JetCMXData>* jetcmx;

  if( m_storeGate->retrieve(jetcmx, m_JetCMXDataLocation).isFailure() ){
    log << MSG::INFO << "No JetCMXData found in TES at "
        << m_JetCMXDataLocation << endreq ;
  }
  else {
  
    log << MSG::INFO << "Got " << jetcmx->size() << " JetCMXData from TES" << endreq ;

    // print values...
    DataVector<LVL1::JetCMXData>::const_iterator itjetcmx ;
    for( itjetcmx = jetcmx->begin(); itjetcmx < jetcmx->end(); ++itjetcmx ) {
        log << MSG::INFO
                   << " Crate = " << (*itjetcmx)->crate() << ", Module = " << (*itjetcmx)->module()
		   << endreq;
	std::vector<unsigned int> data = (*itjetcmx)->DataWords();
        log << MSG::INFO << " Map " << std::hex << (*itjetcmx)->presenceMap() << std::dec << endreq;
	log << MSG::INFO << " Data : " << std::hex << data[0] << "  " << data[1]
	                 << "  " << data[2] << "  " << data[3] << std::dec << endreq;
    }
    
  }
  
  // CPCMXTopoData
  const DataVector<CPCMXTopoData>* cpcmxtopo;

  if( m_storeGate->retrieve(cpcmxtopo, m_CPCMXTopoDataLocation).isFailure() ){
    log << MSG::INFO << "No CPCMXTopoData found in TES at "
        << m_CPCMXTopoDataLocation << endreq ;
  }
  else {
  
    log << MSG::INFO << "Got " << cpcmxtopo->size() << " CPCMXTopoData from TES" << endreq ;

    // print values...
    DataVector<LVL1::CPCMXTopoData>::const_iterator itcpcmxtopo ;
    for( itcpcmxtopo = cpcmxtopo->begin(); itcpcmxtopo < cpcmxtopo->end(); ++itcpcmxtopo ) {
        log << MSG::INFO
                   << " Crate = " << (*itcpcmxtopo)->crate() << ", CMX = " << (*itcpcmxtopo)->cmx()
		   << ", Overflow = " << (*itcpcmxtopo)->overflow() << endreq;
	std::vector<unsigned int> data = (*itcpcmxtopo)->tobWords();
	std::vector<CPTopoTOB> tobs;
	(*itcpcmxtopo)->tobs(tobs);
	if (tobs.size() == data.size()) {
	   for (unsigned int i = 0; i < data.size(); ++i) {
              log << MSG::INFO << " TOB word " << std::hex << data[i] << std::dec << 
	                          " ET " << tobs[i].et() << ", isolation = 0x" << std::hex << tobs[i].isolation() << std::dec << endreq <<
				  " Coordinate (" << tobs[i].eta() << ", " << tobs[i].phi() << ") => (" <<
				  tobs[i].ieta() << ", " << tobs[i].iphi() << ")" << endreq;
	   }
	}
	else log << MSG::INFO << "MisMatch: " << data.size() << " words but " << tobs.size() << " TOBs" << endreq;
    }
    
  }

  
  // JetCMXTopoData
  const DataVector<JetCMXTopoData>* jetcmxtopo;

  if( m_storeGate->retrieve(jetcmxtopo, m_JetCMXTopoDataLocation).isFailure() ){
    log << MSG::INFO << "No JetCMXTopoData found in TES at "
        << m_JetCMXTopoDataLocation << endreq ;
  }
  else {
  
    log << MSG::INFO << "Got " << jetcmxtopo->size() << " JetCMXTopoData from TES" << endreq ;

    // print values...
    DataVector<LVL1::JetCMXTopoData>::const_iterator itjetcmxtopo ;
    for( itjetcmxtopo = jetcmxtopo->begin(); itjetcmxtopo < jetcmxtopo->end(); ++itjetcmxtopo ) {
        log << MSG::INFO
                   << " Crate = " << (*itjetcmxtopo)->crate() 
		   << ", Overflow = " << (*itjetcmxtopo)->overflow() << endreq;
	std::vector<unsigned int> data = (*itjetcmxtopo)->tobWords();
	std::vector<JetTopoTOB> tobs;
	(*itjetcmxtopo)->tobs(tobs);
	if (tobs.size() == data.size()) {
	   for (unsigned int i = 0; i < data.size(); ++i) {
              log << MSG::INFO << " TOB word " << std::hex << data[i] << std::dec << 
	                          " ETLarge " << tobs[i].etLarge() << ", ETSmall " << tobs[i].etSmall() << endreq <<
				  " Coordinate (" << tobs[i].eta() << ", " << tobs[i].phi() << ") => (" <<
				  tobs[i].ieta() << ", " << tobs[i].iphi() << ")" << endreq;
	   }
	}
	else log << MSG::INFO << "MisMatch: " << data.size() << " words but " << tobs.size() << " TOBs" << endreq;
    }
    
  }

  
  // EnergyTopoData
  const EnergyTopoData* energytopo;

  if( m_storeGate->retrieve(energytopo, m_EnergyTopoDataLocation).isFailure() ){
    log << MSG::INFO << "No EnergyTopoData found in TES at "
        << m_EnergyTopoDataLocation << endreq ;
  }
  else {
  
    // print values...
    log << MSG::INFO << " EnergyTopoData:" << endreq
                     << "Word 0: " << MSG::hex << energytopo->word0() << ", Word 1:  " << energytopo->word1()
		     << ", Word 2: " << energytopo->word2() << MSG::dec << endreq
		     << "Ex: Full " << energytopo->Ex(LVL1::EnergyTopoData::Normal)
		     << ", Restricted " << energytopo->Ex(LVL1::EnergyTopoData::Restricted) 
		     << ", Overflows " << energytopo->ExOverflow(LVL1::EnergyTopoData::Normal) << ", " << energytopo->ExOverflow(LVL1::EnergyTopoData::Restricted)
		     << endreq
		     << "Ey: Full " << energytopo->Ey(LVL1::EnergyTopoData::Normal)
		     << ", Restricted " << energytopo->Ey(LVL1::EnergyTopoData::Restricted) 
		     << ", Overflows " << energytopo->ExOverflow(LVL1::EnergyTopoData::Normal) << ", " << energytopo->ExOverflow(LVL1::EnergyTopoData::Restricted)
		     << endreq
		     << "ET: Full " << energytopo->Et(LVL1::EnergyTopoData::Normal)
		     << ", Restricted " << energytopo->Et(LVL1::EnergyTopoData::Restricted) 
		     << ", Overflows " << energytopo->ExOverflow(LVL1::EnergyTopoData::Normal) << ", " << energytopo->ExOverflow(LVL1::EnergyTopoData::Restricted)
		     << endreq;
    
  }

  
  // CTP Data
  const EmTauCTP* emtauctp;
  if( m_storeGate->retrieve(emtauctp, m_EmTauCTPLocation).isFailure() ){
    log << MSG::INFO << "No EmTauCTP found in TES at "
        << m_EmTauCTPLocation << endreq ;
  }
  else {
    // print values...
    log << MSG::INFO << " EmTauCTP:" << MSG::hex << endreq
                     << "Word 0: " << emtauctp->cableWord0() << ", Word 1:  " << emtauctp->cableWord1()
		     << ", Word 2: " << emtauctp->cableWord2() << ", Word 3: " << emtauctp->cableWord3() << MSG::dec << endreq;   
  }
   
  const JetCTP* jetctp;
  if( m_storeGate->retrieve(jetctp, m_JetCTPLocation).isFailure() ){
    log << MSG::INFO << "No JetCTP found in TES at "
        << m_JetCTPLocation << endreq ;
  }
  else {
    // print values...
    log << MSG::INFO << " JetCTP:" << MSG::hex << endreq
                     << "Word 0: " << jetctp->cableWord0() << ", Word 1:  " << jetctp->cableWord1() << MSG::dec << endreq;   
  }
 
  const EnergyCTP* energyctp;
  if( m_storeGate->retrieve(energyctp, m_EnergyCTPLocation).isFailure() ){
    log << MSG::INFO << "No EnergyCTP found in TES at "
        << m_EnergyCTPLocation << endreq ;
  }
  else {
    // print values...
    log << MSG::INFO << " EnergyCTP:" << MSG::hex << endreq
                     << "Word 0: " << energyctp->cableWord0() << ", Word 1:  " << energyctp->cableWord1() << MSG::dec << endreq;   
  }


  // Now do the same for CMMRoI
  const CMMRoI* cmmroi;

  if( m_storeGate->retrieve(cmmroi, TrigT1CaloDefs::CMMRoILocation).isFailure() ){
    log << MSG::INFO << "No CMMRoI found in TES at "
        << TrigT1CaloDefs::CMMRoILocation << endreq ;
  }
  else {
  
    log << MSG::INFO << "Got CMMRoI from TES" << endreq ;

    // print values...
    log << MSG::INFO << std::hex
                   << "jetEtHits " << cmmroi->jetEtHits()
                   << " sumEtHits " << cmmroi->sumEtHits()
                   << " EtMissHits " << cmmroi->missingEtHits()
                   << std::dec << endreq;
    log << MSG::INFO << " ET " << cmmroi->et()
                   << " Ex: " << cmmroi->ex()
                   << " Ey: " << cmmroi->ey()
                   <<endreq;
  }

        
}//end of dumpEDM


} // end of LVL1 namespace bracket

/** load and lookat SlinkObject. Form lost of EmTauRoIwords from them and check coordinates. */
void LVL1::Tester::examineSlinkObjects(){
  MsgStream log( messageService(), name() ) ;
  /**\todo There must be a better way of doing this, but CERN doesn't seem to have sstream.h*/
  std::string emTauSlinkLocation[4];
  emTauSlinkLocation[0]= m_EmTauSlinkLocation+"0";
  emTauSlinkLocation[1]= m_EmTauSlinkLocation+"1";
  emTauSlinkLocation[2]= m_EmTauSlinkLocation+"2";
  emTauSlinkLocation[3]= m_EmTauSlinkLocation+"3";

  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPRoIRODs;i++){
      // Here we tell load a CTP object from the transient store.
    StatusCode sc1 = m_storeGate->retrieve(m_emTauSlink[i],emTauSlinkLocation[i]);
    if( sc1.isFailure() ) {
      log << MSG::ERROR
          << "No Slink object found in TES at "
          << emTauSlinkLocation[i]
          << endreq ;
          return;
    }
  }
	//dumpSlinks();
  return;
}
/** Compare RoI coords reconstructed from RoI words with coords from original RoI objects */
void LVL1::Tester::compareRoIWordCoords(){
  MsgStream log( messageService(), name() ) ;
  //Not finished yet. Not sure if this is actually needed.
  examineSlinkObjects();
  std::vector<unsigned int>* RoIWords = extractRoIWords();
  for (std::vector<unsigned int>::const_iterator i=RoIWords->begin(); i!=RoIWords->end(); i++){
    log<<MSG::INFO << "Got RoIWord : "<<(*i)<<endreq;
  }
  delete RoIWords;
  return;
}
/** returns a pointer to a vector of RoIwords extracted
from the Slink fragment. Very much a cludge at the moment */
std::vector<unsigned int>* LVL1::Tester::extractRoIWords(){
  MsgStream log( messageService(), name() ) ;
  std::vector<unsigned int>* extractedWords = new std::vector<unsigned int>;

  const unsigned int headerLength=10;
  const unsigned int tailerLength=5;
  for (unsigned int slink=0; slink<(TrigT1CaloDefs::numOfCPRoIRODs);slink++){
    //a blank Slink fragment still has header and tail (=15 words)
    unsigned int numberOfWords=(m_emTauSlink[slink]->size())-headerLength-tailerLength;
    for (unsigned int word=9; word<(headerLength+numberOfWords-1);word++){
      //
      const DataVector<LVL1CTP::SlinkWord>* testOne=m_emTauSlink[slink];
      const LVL1CTP::SlinkWord* test=(*testOne)[word];
      extractedWords->push_back(test->word() );
    }
  }
  return extractedWords;
}
/** dump the cells belonging to an RoI. */
void LVL1::Tester::dumpROICells(){
   MsgStream log( messageService(), name() ) ;
  const t_EmTauROICollection* ROIs;
  StatusCode sc1 = m_storeGate->retrieve(ROIs,
					 m_EmTauROILocation);

  if( ! ROIs ) {
    log << MSG::DEBUG
        << "No ROIs found in TES at "
        << m_EmTauROILocation
        << endreq ;
        return;
  } else {
    log << MSG::DEBUG << "Found "<<ROIs->size() << " ROI(s)"<<endreq;

    t_EmTauROICollection::const_iterator it ;
     // I think these should provide enough of a signature....
      unsigned int emClus =  6;
      unsigned int emIsol =  6;
      unsigned int hadIsol = 12;
      unsigned int hadCore = 17;
//      unsigned int RoICore = 26 ;
      for( it  = ROIs->begin(); it < ROIs->end(); ++it){
        bool matchesSignature=( (*it)->clusterEnergy()==emClus) && ( (*it)->emRingIsolationEnergy()==emIsol) &&
              ( (*it)->hadRingIsolationEnergy()==hadIsol) &&( (*it)->hadCoreEnergy()==hadCore) &&( (*it)->emRingIsolationEnergy()==emIsol);
        // cludge for testing
        matchesSignature=true;
        if (matchesSignature){
          log << MSG::DEBUG<< "Found matching iROI"<<endreq;
          //std::vector<LVL1::TriggerTower*>::const_iterator  ttIter=(*it)->getTowers().begin();
          //log << MSG::DEBUG<< "which contains "<<(*it)->getTowers().size()<<" TriggerTowers"<<endreq;
          /*
          for(ttIter=(*it)->getTowers().begin(); ttIter!= (*it)->getTowers().end();++ttIter){
            std::vector<const CaloCell*>::const_iterator ccIter=(*ttIter)->containedGEANTCells().begin();
            log << MSG::DEBUG<< "One TT contains "<<(*ttIter)->containedGEANTCells().size()<<" CaloCells"<<endreq;
            for( ccIter=(*ttIter)->containedGEANTCells().begin(); ccIter!=(*ttIter)->containedGEANTCells().end();++ccIter){
              CaloSampling::CaloSample  sampl = CaloSampling::getSampling((**ccIter) );
              log << MSG::DEBUG<< "Cell has "<<std::endl<<"-----------"<<std::endl
                   << "Energy : "<<(*ccIter)->energy()
                   << "coords : "<<(*ccIter)->phi()<<", "<<  (*ccIter)->eta()
                   << "and sampling "<<(static_cast<unsigned int>(sampl) ) <<endreq;
            }//endfor
          }//endfor
	  */
        }//endif
      }//endfor
  }
  return;
}                      
/** dump the cells belonging to an RoI. */
void LVL1::Tester::dumpJEMResults(){
   MsgStream log( messageService(), name() ) ;
  log << MSG::INFO << "loadJEMResults" << endreq;

  const JEMHitsCollection* jemHits;
  StatusCode sc1 = m_storeGate->retrieve(jemHits, m_jemHitsLocation);

  if( (sc1==StatusCode::FAILURE) ) {
    log << MSG::INFO
        << "No JEMHits found in Storegate at "
        << m_jemHitsLocation
        << endreq ;      
  }
  else {
    log << MSG::INFO
        << jemHits->size() << " JEMHits found at "
        << m_jemHitsLocation
        << endreq ; 

     JEMHitsCollection::const_iterator it;     
     JetEnergyModuleKey testKey;

     for ( it  = jemHits->begin(); it < jemHits->end(); ++it ){
       unsigned int crate =(*it)->crate();
       unsigned int module=(*it)->module();
       unsigned int hits=(*it)->JetHits();
       log << MSG::INFO
           << "Crate " << crate << " Module " << module
	   << " -> Hits = " << std::ios::hex << hits << std::ios::dec << endreq;
     }
  }

  const JEMEtSumsCollection* jemEtSums;
  sc1 = m_storeGate->retrieve(jemEtSums, m_jemEtSumsLocation);

  if( (sc1==StatusCode::FAILURE) ) {
    log << MSG::INFO
        << "No JEMEtSums found in Storegate at "
        << m_jemEtSumsLocation
        << endreq ;      
  }
  else {
    log << MSG::INFO
        << jemEtSums->size() << " JEMEtSums found at "
        << m_jemEtSumsLocation
        << endreq ;       

     JEMEtSumsCollection::const_iterator it;     
     JetEnergyModuleKey testKey;

     for ( it  = jemEtSums->begin(); it < jemEtSums->end(); ++it ){
       unsigned int crate =(*it)->crate();
       unsigned int module=(*it)->module();
       unsigned int Ex=(*it)->Ex();
       unsigned int Ey=(*it)->Ey();
       unsigned int Et=(*it)->Et();
       log << MSG::INFO
           << "Crate " << crate << " Module " << module
	   << " -> Ex = " << Ex << ", Ey = " << Ey << ", Et = " << Et 
	   << endreq;
     }
  }
  
}
/** test the ETmiss/ETsum tool */
void LVL1::Tester::testEtTool(){
   MsgStream log( messageService(), name() ) ;
   log << MSG::INFO << "testEtTool" << endreq;

   StatusCode sc = m_EtTool.retrieve();
   if (sc.isFailure()) log << MSG::ERROR << "Problem retrieving EtTool" << endreq;

   const DataVector<JetElement>* storedJEs;
   sc = m_storeGate->retrieve(storedJEs,m_JetElementLocation);
   if (sc.isFailure()) log << MSG::ERROR << "Problem retrieving JetElements" << endreq;
   
   DataVector<ModuleEnergy>* modules = new DataVector<ModuleEnergy>;
   m_EtTool->moduleSums(storedJEs, modules);
   DataVector<CrateEnergy>*  crates  = new DataVector<CrateEnergy>;
   m_EtTool->crateSums(modules, crates);
   SystemEnergy result = m_EtTool->systemSums(crates);

   log << MSG::INFO << "Final Ex, Ey, ET = " << result.ex() << ", "
       << result.ey() << ", " << result.et() << endreq;

   log << MSG::INFO << "RoI Words: " << endreq
       << "   Word 0: " << std::hex <<  result.roiWord0() << std::dec << endreq
       << "   Word 1: " << std::hex <<  result.roiWord1() << std::dec << endreq
       << "   Word 2: " << std::hex <<  result.roiWord2() << std::dec << endreq;

   delete modules;
   delete crates;
       
}

