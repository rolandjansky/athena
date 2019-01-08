/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          Tester.cxx  -  description
                             -------------------
    begin                : Mon Apr 23 2001
    email                : e.moyse@qmw.ac.uk
 ***************************************************************************/
//
//    updated: June 20, M. Wielers
//             move to Storegate
// 
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
    : AthAlgorithm( name, pSvcLocator ), 
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


//---------------------------------
// initialise() 
//---------------------------------

StatusCode Tester::initialize()
{
  if (m_mode&m_dumpEmTauRoIs || m_mode&m_compareEmTauRoIs) {
    ATH_CHECK( m_EmTauTool.retrieve() );
  } else {
    m_EmTauTool.disable();
  }
  // The following are never used?
  m_JetTool.disable();
  m_EtTool.disable();

  return StatusCode::SUCCESS ;
}


//---------------------------------
// finalise() 
//---------------------------------

StatusCode Tester::finalize()
{
   ATH_MSG_INFO( "Finalizing" );
	 ATH_MSG_INFO( "TOTAL ERRORS : " <<m_numberOfErrors<<" out of "<<m_numberOfEvents<<" examined events.");
	 
	
   for( std::vector<int>::const_iterator it= m_eventsWithErrors.begin(); it < m_eventsWithErrors.end(); ++it){
	   ATH_MSG_DEBUG("Error in event :"<<std::ios::dec<<(*it));
   }

   return StatusCode::SUCCESS ;
}


//----------------------------------------------
// execute() method called once per event
//----------------------------------------------
//


StatusCode Tester::execute( )
{

  ATH_MSG_DEBUG ( "Executing" );


	m_numberOfEvents++;
	m_actualROIWords.erase(m_actualROIWords.begin(), m_actualROIWords.end());
	m_generatedROIWords.erase(m_generatedROIWords.begin(), m_generatedROIWords.end());

  // Dump TriggerTower values.
	if (m_mode&m_dumpTTs) {printTriggerTowerValues();}
  // Dump EmTauROI values.
	if (m_mode&m_dumpEmTauRoIs) {loadEmTauROIs();}
  // try to compare generated ROI words with actual ROI words.
	if (m_mode&m_compareEmTauRoIs) {
          loadEmTauROIs();      // get some ROIs
          loadActualROIWord();
          compareROIWords();
        }
  // compare coords from RoIwords and RoIDecoder and original RoI objects
  if (m_mode&m_compRoIWrdCoords) {compareRoIWordCoords();}
  // look for specific RoI and dump its calo cells
  if (m_mode&m_dumpRoICells) {dumpROICells();}

  if (m_mode&m_dumpJEMResults) {dumpJEMResults();}
  
  if (m_mode&m_dumpEDM) {dumpEDM();}

  return StatusCode::SUCCESS ;
}

void LVL1::Tester::loadTriggerTowers(){
//
//  typedef DataVector<LVL1::TriggerTower> t_TTCollection ;
//  DataHandle<t_TTCollection> TTVector;
//
//  if( evtStore()->retrieve(TTVector, m_TriggerTowerLocation).isFailure() ){
//    log << MSG::ERROR
//        << "No TriggerTowers found in TES at "
//        << m_TriggerTowerLocation
//        );
//    return;
//  }
//
//  log << MSG::INFO
//      << "Got "
//      << TTVector->size()
//      << " Trigger Towers from TES"
//      );
//
//
//  return;
} // end of loadTriggerTowers()

/** prints useful TriggerTower values*/
void LVL1::Tester::printTriggerTowerValues(){
typedef DataVector<LVL1::TriggerTower> t_TTCollection ;
  const t_TTCollection* TTVector;

  if( evtStore()->retrieve(TTVector, m_TriggerTowerLocation).isFailure() ){
    ATH_MSG_DEBUG ( "No TriggerTowers found in TES at "
                    << m_TriggerTowerLocation ) ;
  return;
  }

  ATH_MSG_INFO( "Got " << TTVector->size()
      << " Trigger Towers from TES" );

	// Step over all TTs and print values...
  t_TTCollection::const_iterator it ;

  for( it  = TTVector->begin(); it < TTVector->end(); ++it ){
	  ATH_MSG_INFO("TT has coords ("<<(*it)->phi()<<", "<<(*it)->eta()
		    << " and energies : "<<(*it)->emEnergy()<<", "<<(*it)->hadEnergy()<<" (Em,Had)");
	}//end it for-loop
}//end of printTTs

/** loads the EmTauROIs from the TES.*/
void LVL1::Tester::loadEmTauROIs(){

  const t_EmTauROICollection* ROIs;
  StatusCode sc1 = evtStore()->retrieve(ROIs, m_EmTauROILocation);

  if( ! ROIs ) {
    ATH_MSG_DEBUG( "No ROIs found in TES at "
            << m_EmTauROILocation  );
    return;
  } else {
    ATH_MSG_DEBUG("Found "<<ROIs->size() << " ROI(s)");

    t_EmTauROICollection::const_iterator it ;

    for( it  = ROIs->begin(); it < ROIs->end(); ++it){
       int tempROIword=(*it)->roiWord();
       ATH_MSG_DEBUG("ROI has ROIword : " << std::hex
           <<  tempROIword << std::dec << endmsg
           << "eta         : " << (*it)->eta() << endmsg
           << "phi         : " << (*it)->phi() << endmsg
           << "Core ET     : " << (*it)->energy() << endmsg
           << "EM cluster  : " << (*it)->clusterEnergy() << endmsg
           << "Tau cluster : " << (*it)->tauClusterEnergy() << endmsg
           << "EM isol     : " << (*it)->emRingIsolationEnergy() << endmsg
           << "Had isol    : " << (*it)->hadRingIsolationEnergy() << endmsg
           << "Had veto    : " << (*it)->hadCoreEnergy() );
       //m_generatedROIWords.push_back(tempROIword);
    }
  }

  // Now test new EM/Tau implementation
  
  StatusCode sc = m_EmTauTool.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Problem retrieving EmTauTool" );
  }
  else {
    const DataVector<LVL1::TriggerTower>* TTVector;
    if( evtStore()->retrieve(TTVector, m_TriggerTowerLocation).isSuccess() ) {  
      DataVector<CPAlgorithm>* rois = new DataVector<CPAlgorithm>;
      m_EmTauTool->findRoIs(TTVector,rois);
      DataVector<CPAlgorithm>::iterator cpw = rois->begin();
      for ( ; cpw != rois->end(); cpw++) {
        ATH_MSG_DEBUG("CPAlgorithm has properties : " << std::hex
           << "RoIWord     : " << std::hex << (*cpw)->RoIWord() << std::dec << endmsg
           << "eta         : " << (*cpw)->eta() << endmsg
           << "phi         : " << (*cpw)->phi() << endmsg
           << "Core ET     : " << (*cpw)->Core() << endmsg
           << "EM cluster  : " << (*cpw)->EMClus() << endmsg
           << "Tau cluster : " << (*cpw)->TauClus() << endmsg
           << "EM isol     : " << (*cpw)->EMIsol() << endmsg
           << "Had isol    : " << (*cpw)->HadIsol() << endmsg
           << "Had veto    : " << (*cpw)->HadVeto() );
      }
      delete rois;
    }
  }
    
  return;
} // end of loadROIs()

/** loads the Actual ROI words from the TES.*/
void LVL1::Tester::loadActualROIWord(){
	char line[60]="";    // to read a line into
  long lineNumber=0;
  int numRows=2;

  ATH_MSG_DEBUG("Trying to open input file " << m_actualROIWordLocation.c_str() );
  std::ifstream in( m_actualROIWordLocation.c_str() );
  if (!in){
    ATH_MSG_FATAL( "Could not find ROI words in "<<m_actualROIWordLocation.c_str() );
    return;
  }else{
  	TriggerTowerKey get(0.0, 0.0);
  	ATH_MSG_DEBUG ( "Event Number : "<<std::ios::dec<<m_eventNumber );

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
    		ATH_MSG_DEBUG(
			   " LineNumber : "<<std::ios::dec<<lineNumber
	      		<< " ROIword : "<<std::ios::dec<<input<<" and in hex : "<<std::ios::hex<<input
	      		);
				m_actualROIWords.push_back(input);
			}
  	} // end phi for loop
	}
	return;
}//end of loadActualROIWords

/** Compares generated ROIwords with actual ROIwords and checks to make sure they are the same.*/
void LVL1::Tester::compareROIWords(){
	if (m_actualROIWords.size()!=m_generatedROIWords.size()){
		ATH_MSG_FATAL( "Different numbers of ROI words in generated and actual data!" );
	}else{
		sort(m_actualROIWords.begin(), m_actualROIWords.end());
		sort(m_generatedROIWords.begin(), m_generatedROIWords.end());
		if (equal(m_actualROIWords.begin(), m_actualROIWords.end(),m_generatedROIWords.begin() )){
		}else{
			ATH_MSG_INFO("ROIWords do not match!!!!!!!!!!!!!!!!!!!!!" );
			m_numberOfErrors++;
			m_eventsWithErrors.push_back(m_eventNumber);
		}
	}
	return;
}//end of compareROIWords


/** prints useful TriggerTower values*/
void LVL1::Tester::dumpEDM(){
  typedef DataVector<LVL1::TriggerTower> t_TTCollection ;
  const t_TTCollection* TTVector;

  if( evtStore()->retrieve(TTVector, m_TriggerTowerLocation).isFailure() ){
    ATH_MSG_INFO ( "No TriggerTowers found in TES at "
                   << m_TriggerTowerLocation ) ;
  }
  else {

    ATH_MSG_INFO( "Got " << TTVector->size()
                  << " Trigger Towers from TES");

        // Step over all TTs and print values...
    t_TTCollection::const_iterator it ;

    for( it  = TTVector->begin(); it < TTVector->end(); ++it ){
          ATH_MSG_INFO("TT has coords ("<<(*it)->phi()<<", "<<(*it)->eta()
                    << " and energies : "<<(*it)->emEnergy()<<", "<<(*it)->hadEnergy()<<" (Em,Had)");
    }//end it for-loop
  }


  // Now do the same for CPMTowers
  const DataVector<LVL1::CPMTower>* CPMTVector;

  if( evtStore()->retrieve(CPMTVector, TrigT1CaloDefs::CPMTowerLocation).isFailure() ){
    ATH_MSG_INFO ( "No CPMTowers found in TES at "
                   << TrigT1CaloDefs::CPMTowerLocation ) ;
  }
  else {
 
    ATH_MSG_INFO( "Got " << CPMTVector->size() << " CPMTowers from TES" );

        // Step over all TTs and print values...
    DataVector<LVL1::CPMTower>::const_iterator it2 ;
    for( it2  = CPMTVector->begin(); it2 < CPMTVector->end(); ++it2 ){
          ATH_MSG_INFO("CPMT has coords ("<<(*it2)->phi()<<", "<<(*it2)->eta()
                    << " and energies : "<<(*it2)->emEnergy()<<", "<<(*it2)->hadEnergy()<<" (Em,Had)");
    }//end it for-loop
  }

    
  // Now do the same for JetElements
  const DataVector<LVL1::JetElement>* JEVector;

  if( evtStore()->retrieve(JEVector, TrigT1CaloDefs::JetElementLocation).isFailure() ){
    ATH_MSG_INFO ( "No JetElements found in TES at "
                   << TrigT1CaloDefs::JetElementLocation ) ;
  }
  else {
    ATH_MSG_INFO( "Got " << JEVector->size() << " JetElements from TES" );

        // Step over all TTs and print values...
    DataVector<LVL1::JetElement>::const_iterator it3 ;
    for( it3 = JEVector->begin(); it3 < JEVector->end(); ++it3 ){
          ATH_MSG_INFO("JE has coords ("<<(*it3)->phi()<<", "<<(*it3)->eta()
                    << " and energies : "<<(*it3)->emEnergy()<<", "<<(*it3)->hadEnergy()<<" (Em,Had)");
    }//end it for-loop
  }

    
  // Now do the same for CPMHits
  const DataVector<LVL1::CPMHits>* CPMHVector;

  if( evtStore()->retrieve(CPMHVector, TrigT1CaloDefs::CPMHitsLocation).isFailure() ){
    ATH_MSG_INFO ( "No CPMHits found in TES at "
                   << TrigT1CaloDefs::CPMHitsLocation ) ;
  }
  else {
  
    ATH_MSG_INFO( "Got " << CPMHVector->size() << " CPMHits from TES" );

        // Step over all TTs and print values...
    DataVector<LVL1::CPMHits>::const_iterator it4 ;
    for( it4 = CPMHVector->begin(); it4 < CPMHVector->end(); ++it4 ){
          ATH_MSG_INFO("CPMHits from crate "<<(*it4)->crate()
                    <<" module "<<(*it4)->module() << " hitwords : "
                    << std::hex << (*it4)->HitWord0()<<", "<<(*it4)->HitWord1()
                    << std::dec );
    }//end it for-loop
  }
 
    
  // Now do the same for CPMRoIs
  const DataVector<LVL1::CPMRoI>* CPMRVector;

  if( evtStore()->retrieve(CPMRVector, TrigT1CaloDefs::CPMRoILocation).isFailure() ){
    ATH_MSG_INFO ( "No CPMRoIs found in TES at "
                   << TrigT1CaloDefs::CPMRoILocation ) ;
  }
  else {

    ATH_MSG_INFO( "Got " << CPMRVector->size() << " CPMRoIs from TES" );

        // Step over all TTs and print values...
    DataVector<LVL1::CPMRoI>::const_iterator it5 ;
    for( it5 = CPMRVector->begin(); it5 < CPMRVector->end(); ++it5 ){
          ATH_MSG_INFO( "CPMRoI from crate " << (*it5)->crate()
                    <<" module " << (*it5)->cpm() << " chip " << (*it5)->chip()
                    << " LC " << (*it5)->location()
                    << " word " << std::hex << (*it5)->hits() << std::dec );
    }//end it for-loop
  }


    
  // Now do the same for CMMCPHits
  const DataVector<LVL1::CMMCPHits>* CMMCPHVector;

  if( evtStore()->retrieve(CMMCPHVector, TrigT1CaloDefs::CMMCPHitsLocation).isFailure() ){
    ATH_MSG_INFO ( "No CMMCPHits found in TES at "
                   << TrigT1CaloDefs::CMMCPHitsLocation ) ;
  }
  else {
  
    ATH_MSG_INFO( "Got " << CMMCPHVector->size() << " CMMCPHits from TES" );

        // Step over all TTs and print values...
    DataVector<LVL1::CMMCPHits>::const_iterator it6;
    for( it6 = CMMCPHVector->begin(); it6 < CMMCPHVector->end(); ++it6 ){
          ATH_MSG_INFO("CMMCPHits from crate "<<(*it6)->crate()
                    <<" dataID "<<(*it6)->dataID() << " hitwords : "
                    << std::hex << (*it6)->HitWord0()<<", "<<(*it6)->HitWord1()
                    << std::dec );
    }//end it for-loop
  }

    
  // Now do the same for CMMJetHits
  const DataVector<LVL1::CMMJetHits>* CMMJHVector;

  if( evtStore()->retrieve(CMMJHVector, TrigT1CaloDefs::CMMJetHitsLocation).isFailure() ){
    ATH_MSG_INFO ( "No CMMJetHits found in TES at "
                   << TrigT1CaloDefs::CMMJetHitsLocation ) ;
  }
  else {

    ATH_MSG_INFO( "Got " << CMMJHVector->size() << " CMMJetHits from TES" );

        // Step over all TTs and print values...
    DataVector<LVL1::CMMJetHits>::const_iterator it7;
    for( it7 = CMMJHVector->begin(); it7 < CMMJHVector->end(); ++it7 ){
          ATH_MSG_INFO("CMMJetHits from crate "<<(*it7)->crate()
                    <<" dataID "<<(*it7)->dataID() << " hitwords : "
                    << std::hex << (*it7)->Hits()
                    << std::dec );
    }//end it for-loop
  }

    
  // Now do the same for CMMEtSums
  const DataVector<LVL1::CMMEtSums>* CMMESVector;

  if( evtStore()->retrieve(CMMESVector, TrigT1CaloDefs::CMMEtSumsLocation).isFailure() ){
    ATH_MSG_INFO ( "No CMMEtSums found in TES at "
                   << TrigT1CaloDefs::CMMEtSumsLocation ) ;
  }
  else {

    ATH_MSG_INFO( "Got " << CMMESVector->size() << " CMMEtSums from TES" );

        // Step over all TTs and print values...
    DataVector<LVL1::CMMEtSums>::const_iterator it8;
    for( it8 = CMMESVector->begin(); it8 < CMMESVector->end(); ++it8 ){
          ATH_MSG_INFO("CMMEtSums from crate "<<(*it8)->crate()
                    <<" dataID "<<(*it8)->dataID() << " ET: "
                    << (*it8)->Et() << " Ex: " << (*it8)->Ex() << " Ey: " << (*it8)->Ey()
                    );
    }//end it for-loop
  }

    
  // Now do the same for JEMHits
  const DataVector<LVL1::JEMHits>* JEMHVector;

  if( evtStore()->retrieve(JEMHVector, TrigT1CaloDefs::JEMHitsLocation).isFailure() ){
    ATH_MSG_INFO ( "No JEMHits found in TES at "
                   << TrigT1CaloDefs::JEMHitsLocation ) ;
  }
  else {

    ATH_MSG_INFO( "Got " << JEMHVector->size() << " JEMHits from TES" );

        // Step over all TTs and print values...
    DataVector<LVL1::JEMHits>::const_iterator it9 ;
    for( it9 = JEMHVector->begin(); it9 < JEMHVector->end(); ++it9 ){
          ATH_MSG_INFO("JEMHits from crate "<<(*it9)->crate()
                    <<" module "<<(*it9)->module() << " hitword : "
                    << std::hex << (*it9)->JetHits()
                    << std::dec );
    }//end it for-loop
  }

    
  // Now do the same for JEMEtSums
  const DataVector<LVL1::JEMEtSums>* JEMESVector;

  if( evtStore()->retrieve(JEMESVector, TrigT1CaloDefs::JEMEtSumsLocation).isFailure() ){
    ATH_MSG_INFO ( "No JEMEtSums found in TES at "
                   << TrigT1CaloDefs::JEMEtSumsLocation ) ;
  }
  else {

    ATH_MSG_INFO( "Got " << JEMESVector->size() << " JEMEtSums from TES" );

        // Step over all TTs and print values...
    DataVector<LVL1::JEMEtSums>::const_iterator it10;
    for( it10 = JEMESVector->begin(); it10 < JEMESVector->end(); ++it10 ){
          ATH_MSG_INFO("JEMEtSums from crate "<<(*it10)->crate()
                    <<" module "<<(*it10)->module() << " ET: "
                    << (*it10)->Et() << " Ex: " << (*it10)->Ex() << " Ey: " << (*it10)->Ey()
                    );
    }//end it for-loop
  }
 
    
  // Now do the same for JEMRoIs
  const DataVector<LVL1::JEMRoI>* JEMRVector;

  if( evtStore()->retrieve(JEMRVector, TrigT1CaloDefs::JEMRoILocation).isFailure() ){
    ATH_MSG_INFO ( "No JEMRoIs found in TES at "
                   << TrigT1CaloDefs::JEMRoILocation ) ;
  }
  else {
  
    ATH_MSG_INFO( "Got " << JEMRVector->size() << " JEMRoIs from TES" );

        // Step over all TTs and print values...
    DataVector<LVL1::JEMRoI>::const_iterator it11 ;
    for( it11 = JEMRVector->begin(); it11 < JEMRVector->end(); ++it11 ){
          ATH_MSG_INFO( "JEMRoI from crate " << (*it11)->crate()
                    <<" module " << (*it11)->jem() << " frame " << (*it11)->frame()
                    << " LC " << (*it11)->location()
                    << " hits " << std::hex << (*it11)->hits() << std::dec );
    }//end it for-loop
  }

    
  // Now do the same for CPMTobRoI
  const DataVector<CPMTobRoI>* cpmtobrois;

  if( evtStore()->retrieve(cpmtobrois, m_CPMTobRoILocation).isFailure() ){
    ATH_MSG_INFO ( "No CPMTobRoI found in TES at "
                   << m_CPMTobRoILocation ) ;
  }
  else {
  
    ATH_MSG_INFO( "Got " << cpmtobrois->size() << " CPMTobRoI from TES" );

    // print values...
    DataVector<LVL1::CPMTobRoI>::const_iterator itcp ;
    for( itcp = cpmtobrois->begin(); itcp < cpmtobrois->end(); ++itcp ) {
        ATH_MSG_INFO( std::hex
                   << "RoI Word " << (*itcp)->roiWord()
                   << std::dec << " Crate = "
		   << (*itcp)->crate() << ", Module = " << (*itcp)->cpm()
		   << ", Chip = " << (*itcp)->chip() 
		   << ", Coordinate = " << (*itcp)->location()
		   );
        ATH_MSG_INFO( " ET " << (*itcp)->energy()
                   << " Isolation " << std::hex << (*itcp)->isolation() << std::dec
                   << " Type = " << (*itcp)->type()
                   );
    }
    
  }
  
    
  // CPM->CMX Data
  const DataVector<CPMCMXData>* emcmx;

  if( evtStore()->retrieve(emcmx, m_CPMCMXDataLocation).isFailure() ){
    ATH_MSG_INFO ( "No CPMCMXData found in TES at "
                   << m_CPMCMXDataLocation ) ;
  }
  else {
  
    ATH_MSG_INFO( "Got " << emcmx->size() << " CPMCMXData from TES" );

    // print values...
    DataVector<LVL1::CPMCMXData>::const_iterator item ;
    for( item = emcmx->begin(); item < emcmx->end(); ++item ) {
        ATH_MSG_INFO( " Crate = " << (*item)->crate() << ", Module = " << (*item)->module()
		          << ", Type = " << (*item)->type() );
	std::vector<unsigned int> data = (*item)->DataWords();
        ATH_MSG_INFO( " Map " << std::hex << (*item)->presenceMap() << std::dec );
	ATH_MSG_INFO( " Data : " << std::hex << data[0] << "  " << data[1]
	                 << "  " << data[2] << "  " << data[3] << std::dec );
    }
    
  }
 
   
    
  // Now do the same for JEMTobRoI
  const DataVector<JEMTobRoI>* jemtobrois;

  if( evtStore()->retrieve(jemtobrois, m_JEMTobRoILocation).isFailure() ){
    ATH_MSG_INFO ( "No JEMTobRoI found in TES at " << m_JEMTobRoILocation ) ;
  }
  else {
  
    ATH_MSG_INFO( "Got " << jemtobrois->size() << " JEMTobRoI from TES" );

    // print values...
    DataVector<LVL1::JEMTobRoI>::const_iterator itje ;
    for( itje = jemtobrois->begin(); itje < jemtobrois->end(); ++itje ) {
        ATH_MSG_INFO( std::hex
                   << "RoI Word " << (*itje)->roiWord()
                   << std::dec << " Crate = "
		   << (*itje)->crate() << ", Module = " << (*itje)->jem()
		   << ", Frame = " << (*itje)->frame() 
		   << ", Coordinate = " << (*itje)->location()
		   );
        ATH_MSG_INFO( " ETLarge = " << (*itje)->energyLarge()
                   << " ETSmall = " << (*itje)->energySmall()
                   );
    }
    
  }
   
   
  // Jet JEM->CMX Data
  const DataVector<JetCMXData>* jetcmx;

  if( evtStore()->retrieve(jetcmx, m_JetCMXDataLocation).isFailure() ){
    ATH_MSG_INFO ( "No JetCMXData found in TES at "
                   << m_JetCMXDataLocation ) ;
  }
  else {
  
    ATH_MSG_INFO( "Got " << jetcmx->size() << " JetCMXData from TES" );

    // print values...
    DataVector<LVL1::JetCMXData>::const_iterator itjetcmx ;
    for( itjetcmx = jetcmx->begin(); itjetcmx < jetcmx->end(); ++itjetcmx ) {
        ATH_MSG_INFO(" Crate = " << (*itjetcmx)->crate() << ", Module = " << (*itjetcmx)->module() );
	std::vector<unsigned int> data = (*itjetcmx)->DataWords();
        ATH_MSG_INFO( " Map " << std::hex << (*itjetcmx)->presenceMap() << std::dec );
	ATH_MSG_INFO( " Data : " << std::hex << data[0] << "  " << data[1]
	                 << "  " << data[2] << "  " << data[3] << std::dec );
    }
    
  }
  
  // CPCMXTopoData
  const DataVector<CPCMXTopoData>* cpcmxtopo;

  if( evtStore()->retrieve(cpcmxtopo, m_CPCMXTopoDataLocation).isFailure() ){
    ATH_MSG_INFO ( "No CPCMXTopoData found in TES at "
                   << m_CPCMXTopoDataLocation ) ;
  }
  else {
  
    ATH_MSG_INFO( "Got " << cpcmxtopo->size() << " CPCMXTopoData from TES" );

    // print values...
    DataVector<LVL1::CPCMXTopoData>::const_iterator itcpcmxtopo ;
    for( itcpcmxtopo = cpcmxtopo->begin(); itcpcmxtopo < cpcmxtopo->end(); ++itcpcmxtopo ) {
        ATH_MSG_INFO( " Crate = " << (*itcpcmxtopo)->crate() << ", CMX = " << (*itcpcmxtopo)->cmx()
		      << ", Overflow = " << (*itcpcmxtopo)->overflow() );
	std::vector<unsigned int> data = (*itcpcmxtopo)->tobWords();
	std::vector<CPTopoTOB> tobs;
	(*itcpcmxtopo)->tobs(tobs);
	if (tobs.size() == data.size()) {
	   for (unsigned int i = 0; i < data.size(); ++i) {
              ATH_MSG_INFO( " TOB word " << std::hex << data[i] << std::dec << 
	                          " ET " << tobs[i].et() << ", isolation = 0x" << std::hex << tobs[i].isolation() << std::dec << endmsg <<
				  " Coordinate (" << tobs[i].eta() << ", " << tobs[i].phi() << ") => (" <<
				  tobs[i].ieta() << ", " << tobs[i].iphi() << ")" );
	   }
	}
	else ATH_MSG_INFO( "MisMatch: " << data.size() << " words but " << tobs.size() << " TOBs" );
    }
    
  }

  
  // JetCMXTopoData
  const DataVector<JetCMXTopoData>* jetcmxtopo;

  if( evtStore()->retrieve(jetcmxtopo, m_JetCMXTopoDataLocation).isFailure() ){
    ATH_MSG_INFO ( "No JetCMXTopoData found in TES at "
                   << m_JetCMXTopoDataLocation ) ;
  }
  else {
  
    ATH_MSG_INFO( "Got " << jetcmxtopo->size() << " JetCMXTopoData from TES" );

    // print values...
    DataVector<LVL1::JetCMXTopoData>::const_iterator itjetcmxtopo ;
    for( itjetcmxtopo = jetcmxtopo->begin(); itjetcmxtopo < jetcmxtopo->end(); ++itjetcmxtopo ) {
        ATH_MSG_INFO(" Crate = " << (*itjetcmxtopo)->crate() 
		     << ", Overflow = " << (*itjetcmxtopo)->overflow() );
	std::vector<unsigned int> data = (*itjetcmxtopo)->tobWords();
	std::vector<JetTopoTOB> tobs;
	(*itjetcmxtopo)->tobs(tobs);
	if (tobs.size() == data.size()) {
	   for (unsigned int i = 0; i < data.size(); ++i) {
              ATH_MSG_INFO( " TOB word " << std::hex << data[i] << std::dec << 
	                          " ETLarge " << tobs[i].etLarge() << ", ETSmall " << tobs[i].etSmall() << endmsg <<
				  " Coordinate (" << tobs[i].eta() << ", " << tobs[i].phi() << ") => (" <<
				  tobs[i].ieta() << ", " << tobs[i].iphi() << ")" );
	   }
	}
	else ATH_MSG_INFO( "MisMatch: " << data.size() << " words but " << tobs.size() << " TOBs" );
    }
    
  }

  
  // EnergyTopoData
  const EnergyTopoData* energytopo;

  if( evtStore()->retrieve(energytopo, m_EnergyTopoDataLocation).isFailure() ){
    ATH_MSG_INFO ( "No EnergyTopoData found in TES at "
                   << m_EnergyTopoDataLocation ) ;
  }
  else {
  
    // print values...
    ATH_MSG_INFO( " EnergyTopoData:"
                     << "Word 0: " << MSG::hex << energytopo->word0() << ", Word 1:  " << energytopo->word1()
		     << ", Word 2: " << energytopo->word2() << MSG::dec
		     << "Ex: Full " << energytopo->Ex(LVL1::EnergyTopoData::Normal)
		     << ", Restricted " << energytopo->Ex(LVL1::EnergyTopoData::Restricted) 
		     << ", Overflows " << energytopo->ExOverflow(LVL1::EnergyTopoData::Normal) << ", " << energytopo->ExOverflow(LVL1::EnergyTopoData::Restricted)
		     << "Ey: Full " << energytopo->Ey(LVL1::EnergyTopoData::Normal)
		     << ", Restricted " << energytopo->Ey(LVL1::EnergyTopoData::Restricted) 
		     << ", Overflows " << energytopo->ExOverflow(LVL1::EnergyTopoData::Normal) << ", " << energytopo->ExOverflow(LVL1::EnergyTopoData::Restricted)
		     << "ET: Full " << energytopo->Et(LVL1::EnergyTopoData::Normal)
		     << ", Restricted " << energytopo->Et(LVL1::EnergyTopoData::Restricted) 
		     << ", Overflows " << energytopo->ExOverflow(LVL1::EnergyTopoData::Normal) << ", " << energytopo->ExOverflow(LVL1::EnergyTopoData::Restricted)
		     );
    
  }

  
  // CTP Data
  const EmTauCTP* emtauctp = nullptr;
  if( evtStore()->retrieve(emtauctp, m_EmTauCTPLocation).isFailure() ){
    ATH_MSG_INFO ( "No EmTauCTP found in TES at "
                   << m_EmTauCTPLocation ) ;
  }
  else {
    // print values...
    ATH_MSG_INFO( " EmTauCTP:" << MSG::hex 
                     << "Word 0: " << emtauctp->cableWord0() << ", Word 1:  " << emtauctp->cableWord1()
		     << ", Word 2: " << emtauctp->cableWord2() << ", Word 3: " << emtauctp->cableWord3() << MSG::dec );   
  }
   
  const JetCTP* jetctp  = nullptr;
  if( evtStore()->retrieve(jetctp, m_JetCTPLocation).isFailure() ){
    ATH_MSG_INFO ( "No JetCTP found in TES at "
                   << m_JetCTPLocation ) ;
  }
  else {
    // print values...
    ATH_MSG_INFO( " JetCTP:" << MSG::hex << endmsg
                     << "Word 0: " << jetctp->cableWord0() << ", Word 1:  " << jetctp->cableWord1() << MSG::dec );   
  }
 
  const EnergyCTP* energyctp = nullptr;
  if( evtStore()->retrieve(energyctp, m_EnergyCTPLocation).isFailure() ){
    ATH_MSG_INFO ( "No EnergyCTP found in TES at "
                   << m_EnergyCTPLocation ) ;
  }
  else {
    // print values...
    ATH_MSG_INFO( " EnergyCTP:" << MSG::hex << endmsg
                     << "Word 0: " << energyctp->cableWord0() << ", Word 1:  " << energyctp->cableWord1() << MSG::dec );   
  }


  // Now do the same for CMMRoI
  const CMMRoI* cmmroi = nullptr;

  if( evtStore()->retrieve(cmmroi, TrigT1CaloDefs::CMMRoILocation).isFailure() ){
    ATH_MSG_INFO ( "No CMMRoI found in TES at "
                   << TrigT1CaloDefs::CMMRoILocation ) ;
  }
  else {
  
    ATH_MSG_INFO( "Got CMMRoI from TES" );

    // print values...
    ATH_MSG_INFO( std::hex
                   << "jetEtHits " << cmmroi->jetEtHits()
                   << " sumEtHits " << cmmroi->sumEtHits()
                   << " EtMissHits " << cmmroi->missingEtHits()
                   << std::dec );
    ATH_MSG_INFO( " ET " << cmmroi->et()
                   << " Ex: " << cmmroi->ex()
                   << " Ey: " << cmmroi->ey()
                   );
  }

        
}//end of dumpEDM


} // end of LVL1 namespace bracket

/** load and lookat SlinkObject. Form lost of EmTauRoIwords from them and check coordinates. */
void LVL1::Tester::examineSlinkObjects(){
  /**\todo There must be a better way of doing this, but CERN doesn't seem to have sstream.h*/
  std::string emTauSlinkLocation[4];
  emTauSlinkLocation[0]= m_EmTauSlinkLocation+"0";
  emTauSlinkLocation[1]= m_EmTauSlinkLocation+"1";
  emTauSlinkLocation[2]= m_EmTauSlinkLocation+"2";
  emTauSlinkLocation[3]= m_EmTauSlinkLocation+"3";

  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPRoIRODs;i++){
      // Here we tell load a CTP object from the transient store.
    StatusCode sc1 = evtStore()->retrieve(m_emTauSlink[i],emTauSlinkLocation[i]);
    if ( sc1.isFailure() ) {
        ATH_MSG_ERROR( "No Slink object found in TES at "
                       << emTauSlinkLocation[i] );
        return;
    }
  }
	//dumpSlinks();
  return;
}
/** Compare RoI coords reconstructed from RoI words with coords from original RoI objects */
void LVL1::Tester::compareRoIWordCoords(){
  //Not finished yet. Not sure if this is actually needed.
  examineSlinkObjects();
  std::vector<unsigned int>* RoIWords = extractRoIWords();
  for (std::vector<unsigned int>::const_iterator i=RoIWords->begin(); i!=RoIWords->end(); i++){
    ATH_MSG_INFO("Got RoIWord : "<<(*i));
  }
  delete RoIWords;
  return;
}
/** returns a pointer to a vector of RoIwords extracted
from the Slink fragment. Very much a cludge at the moment */
std::vector<unsigned int>* LVL1::Tester::extractRoIWords(){
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
  const t_EmTauROICollection* ROIs = nullptr;
  StatusCode sc1 = evtStore()->retrieve(ROIs,
					 m_EmTauROILocation);

  if( ! ROIs ) {
    ATH_MSG_DEBUG("No ROIs found in TES at "
                  << m_EmTauROILocation );
        return;
  } else {
    ATH_MSG_DEBUG( "Found "<<ROIs->size() << " ROI(s)");

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
          ATH_MSG_DEBUG( "Found matching iROI");
          //std::vector<LVL1::TriggerTower*>::const_iterator  ttIter=(*it)->getTowers().begin();
          //ATH_MSG_DEBUG( "which contains "<<(*it)->getTowers().size()<<" TriggerTowers");
          /*
          for(ttIter=(*it)->getTowers().begin(); ttIter!= (*it)->getTowers().end();++ttIter){
            std::vector<const CaloCell*>::const_iterator ccIter=(*ttIter)->containedGEANTCells().begin();
            ATH_MSG_DEBUG( "One TT contains "<<(*ttIter)->containedGEANTCells().size()<<" CaloCells");
            for( ccIter=(*ttIter)->containedGEANTCells().begin(); ccIter!=(*ttIter)->containedGEANTCells().end();++ccIter){
              CaloSampling::CaloSample  sampl = CaloSampling::getSampling((**ccIter) );
              ATH_MSG_DEBUG( "Cell has "<<std::endl<<"-----------"<<std::endl
                   << "Energy : "<<(*ccIter)->energy()
                   << "coords : "<<(*ccIter)->phi()<<", "<<  (*ccIter)->eta()
                   << "and sampling "<<(static_cast<unsigned int>(sampl) ) );
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

  ATH_MSG_INFO( "loadJEMResults" );

  const JEMHitsCollection* jemHits;
  StatusCode sc1 = evtStore()->retrieve(jemHits, m_jemHitsLocation);

  if( (sc1==StatusCode::FAILURE) ) {
    ATH_MSG_INFO( "No JEMHits found in Storegate at "
        << m_jemHitsLocation);      
  }
  else {
    ATH_MSG_INFO( jemHits->size() << " JEMHits found at "
        << m_jemHitsLocation); 

     JEMHitsCollection::const_iterator it;     
     JetEnergyModuleKey testKey;

     for ( it  = jemHits->begin(); it < jemHits->end(); ++it ){
       unsigned int crate =(*it)->crate();
       unsigned int module=(*it)->module();
       unsigned int hits=(*it)->JetHits();
       ATH_MSG_INFO( "Crate " << crate << " Module " << module
	   << " -> Hits = " << std::ios::hex << hits << std::ios::dec );
     }
  }

  const JEMEtSumsCollection* jemEtSums;
  sc1 = evtStore()->retrieve(jemEtSums, m_jemEtSumsLocation);

  if( (sc1==StatusCode::FAILURE) ) {
    ATH_MSG_INFO("No JEMEtSums found in Storegate at "
        << m_jemEtSumsLocation);      
  }
  else {
    ATH_MSG_INFO(jemEtSums->size() << " JEMEtSums found at "
        << m_jemEtSumsLocation);       

     JEMEtSumsCollection::const_iterator it;     
     JetEnergyModuleKey testKey;

     for ( it  = jemEtSums->begin(); it < jemEtSums->end(); ++it ){
       unsigned int crate =(*it)->crate();
       unsigned int module=(*it)->module();
       unsigned int Ex=(*it)->Ex();
       unsigned int Ey=(*it)->Ey();
       unsigned int Et=(*it)->Et();
       ATH_MSG_INFO("Crate " << crate << " Module " << module
	   << " -> Ex = " << Ex << ", Ey = " << Ey << ", Et = " << Et );
     }
  }
  
}

