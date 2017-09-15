/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * HFORSelectionTool.cxx
 *
 *  Created on: 15 de mai de 2016
 *      Author: Marco Leite (leite@cern.ch)
 *
 *	    This tool implements the Heavy Flavor Overlap removal as a SelectionTool. This only mkaes sense to
 *	    use in AlpgenPythia6 samples where light and HF samples are mixed together (so the same event topology is
 *	    not double counted.
 *
 *      See https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/HforTool for the Run1 version of the tool
 *
 *      The heavy lifting will be done by HFOR_Truth class. This should be isolated from the normal user and all
 *      interfacing and setup is done through the tool framework .
 *
 *      TODO: List
 *      TODO: 1) Improve documentation around the code
 *      TODO: 2) Provide a "tap" method to debug the HFOR_Truth class (eg histograms of deltaR, quark masses etc.)
 *      TODO: 3) Implement selection based on other discrimination methods. This requires further studies
 *
 */
#include <string>
#include "boost/current_function.hpp"

#include "PathResolver/PathResolver.h"

#include "xAODEventInfo/EventInfo.h"
#include "HFORTools/HFORSelectionTool.h"
#include "HFORTools/HFOR_Truth.h"


//==============================================================================
HFORSelectionTool::HFORSelectionTool( const std::string& name )
  : asg::AsgMetadataTool( name ),
    m_sampleType (HFORType::noType),
    m_decisionType (HFORType::noType),
    m_sampleRunNumber (0),
    m_sampleName ("unknown"),
    m_evtCounterAll (0),
    m_evtCounterKilled (0) {
  declareProperty( "MatchCone",     m_matchCone = 0.4 );
  declareProperty( "runConfigFile", m_runConfigFile = "HFORTools/mc15_AlpgenPythia_2016.cfg" ) ;
  declareProperty( "HFORStrategy", m_HFORStrategy  = "DRBased" );
  declareProperty( "TruthJetCollectionName", m_truthJetCollectionName  = "AntiKt4TruthWZJets" );
}
//==============================================================================

//==============================================================================
// During the initialization, the toll checks for the strategy to be used
// (jet based or angular overlap removal) and loads the configuration file
// with the list of the samples, setting the present analysis sample accordingly
//==============================================================================
StatusCode HFORSelectionTool::initialize() {

  // Greet the user:
  ATH_MSG_INFO( BOOST_CURRENT_FUNCTION << ": Starting tool initialization ..." );
  ATH_MSG_INFO( BOOST_CURRENT_FUNCTION << ": Using matching cone deltaR = " << m_matchCone );
  ATH_MSG_INFO(" ") ;
  ATH_MSG_INFO( BOOST_CURRENT_FUNCTION << " ************************************ " );
  ATH_MSG_INFO( BOOST_CURRENT_FUNCTION << " ** THIS TOOL IS UNDER DEVELOPMENT ** " );
  ATH_MSG_INFO( BOOST_CURRENT_FUNCTION << " ************************************ " );
  ATH_MSG_INFO(" ") ;

  //Initialize the Overlap Removal engine
  m_hforTruth = HFOR_Truth() ;

  if(m_HFORStrategy == "DRBased"){
    m_hforTruth.setAngularBasedHFOR();
    ATH_MSG_INFO( BOOST_CURRENT_FUNCTION << " ** Angular-Based HFOR ** " );
  } else if(m_HFORStrategy == "JetBased"){
    m_hforTruth.setJetBasedHFOR();
    ATH_MSG_INFO( BOOST_CURRENT_FUNCTION << " ** Jet-Based HFOR ** " );
  } else {
    ATH_MSG_ERROR( BOOST_CURRENT_FUNCTION << " ** HFORStrategy is not known! ** " );
    return StatusCode::FAILURE;
  }

  //FIXME: Check the return of this also ..
  std::string filename = PathResolverFindCalibFile(m_runConfigFile);
  m_hforTruth.readRunConfig(filename) ;

  ATH_MSG_INFO( BOOST_CURRENT_FUNCTION << ": Initialization done.");
  return StatusCode::SUCCESS ;
  m_isConfigured = false ;

}
//==============================================================================

//==============================================================================
// Make sure we clean the the configuration information, forcing the call to
// setSampleType when the next file is opened.
//==============================================================================
StatusCode HFORSelectionTool::beginInputFile() {

  //ATH_MSG_DEBUG( BOOST_CURRENT_FUNCTION << " File change detected, will check the sample type " ) ;
  //ATH_CHECK( setSampleType() ) ;
  m_isConfigured = false ;

  return  StatusCode::SUCCESS ;
}
//==============================================================================

//==============================================================================
// At the end of processing, we may want to access the bookkeeping information
// of how many events are killed/passed. Also makes sure we unconfigure for
// the next file forcing the call to setSampleType when the next file is opened.
//==============================================================================
StatusCode HFORSelectionTool::endInputFile() {

  ATH_MSG_DEBUG( BOOST_CURRENT_FUNCTION << " End Of File detected ");
  m_isConfigured = false ;

  if (m_evtCounterAll != 0) {
    ATH_MSG_DEBUG(" ") ;
    ATH_MSG_DEBUG("****************************************************************************") ;
    ATH_MSG_DEBUG(BOOST_CURRENT_FUNCTION << ": Sample type   : "  << m_sampleName ) ;
    ATH_MSG_DEBUG(BOOST_CURRENT_FUNCTION << ": Total  Events : " << m_evtCounterAll );
    ATH_MSG_DEBUG(BOOST_CURRENT_FUNCTION << ": Killed Events : " << m_evtCounterKilled <<
                  " (" << 100*float(m_evtCounterKilled)/float(m_evtCounterAll)  << "%)");
    ATH_MSG_DEBUG("****************************************************************************") ;
    ATH_MSG_DEBUG(" ") ;
  }

  //Reset the counters
  m_evtCounterAll = 0;
  m_evtCounterKilled = 0;

  return StatusCode::SUCCESS ;
}

//==============================================================================

//==============================================================================
// This method will read the Truth information (particle and/or truthJet
// container and decide if this event should be killed or kept in face of the
// overlap found by the HFOR_Truth method. It does nothing if the file is
// not an AlpgenPythia6 sample. Also keeps track of how many events passed or
// killed
//==============================================================================
bool HFORSelectionTool::isSelected()  {

  if (! m_isConfigured) {
    ATH_CHECK( setSampleType() ) ;
    m_isConfigured = true ;
  }

  //This toll will tell if this event is to removed (returns 0) or kept
  //(returns 1)
  //In case of error (container not present) it returns -1

  bool selected = true ;

  m_decisionType = HFORType::noType ;

  // Only run this function on Monte Carlo
  const xAOD::EventInfo* eventInfo = nullptr;
  ATH_CHECK( evtStore()->retrieve(eventInfo, "EventInfo") );
  const bool isSim = eventInfo->eventType(xAOD::EventInfo::EventType::IS_SIMULATION);
  if ( !isSim ) {
    ATH_MSG_DEBUG (BOOST_CURRENT_FUNCTION << "It is a data event... nothing to be done...");
    return selected;
  }


  //===========================
  // Truth Event
  //===========================
  std::string truthCollectionName  = "TruthEvents" ;
  const xAOD::TruthEventContainer* truthEvent = nullptr ;
  ATH_CHECK ( evtStore()->retrieve(truthEvent, truthCollectionName ) ) ;

  //=========================================
  // truth JET info (not needed for DR-HFOR)
  //=========================================
  const xAOD::JetContainer* jets = nullptr ;
  ATH_CHECK ( evtStore()->retrieve(jets, m_truthJetCollectionName ) ) ;

  //=========================================================
  // Truth Event DeltaR based and Jet Based switched in init
  //=========================================================
  m_decisionType  =  m_hforTruth.findOverlap(*truthEvent, jets) ;


  ATH_MSG_DEBUG( BOOST_CURRENT_FUNCTION << " Event Type " << (unsigned int) m_decisionType) ;

  if (m_decisionType == HFORType::kill) selected = false ;
  else selected = true ;

  //BookKeeping of how many kills/passed we have.
  //TODO: create a dictionary with key as sample type so we can count per type,
  //and per run. This allows for debugging
  m_evtCounterAll++ ;
  if (! selected) {
    m_evtCounterKilled++ ;
  }

  return selected ;

}
//==============================================================================

//==============================================================================
// Extract the MC sample inforation from EventInfo container and using the
// configuration file classifies the sample. Given the fact that some derivation
// change the run number, we use the mcChannelNumber, failing back to run number
// for truth derivations as in these the mcChannelNumber is 0. Heavy Flavor
// Overlap removal technique  will depend on it. To avoid any surprises, it is
// good to check it at every file change. Due to the definition of tool
// Property, this cannot be a Property of the tool (it will require
// re-initialization of the tool).
//==============================================================================
StatusCode HFORSelectionTool::setSampleType()  {

  //Check what type of MC sample we have.

  //=============
  // Event Info
  //=============
  const xAOD::EventInfo* eventInfo = nullptr;
  ATH_CHECK( evtStore()->retrieve( eventInfo, "EventInfo" ) );

  // Only run this function on Monte Carlo
  const bool isSim = eventInfo->eventType(xAOD::EventInfo::EventType::IS_SIMULATION);
  if ( !isSim ) {
    ATH_MSG_DEBUG (BOOST_CURRENT_FUNCTION << "It is a data event... nothing to be done...");
    return StatusCode::SUCCESS;
  }


  //Derivations changes runNumber due to pileup reweight,
  //while  mcChannelNumber is 0 in Truth derivations
  m_sampleRunNumber = eventInfo->mcChannelNumber() ;
  if (m_sampleRunNumber == 0 ) {
    ATH_MSG_WARNING(BOOST_CURRENT_FUNCTION << "mcChannelNumber is 0, falling back to runNumber: " << eventInfo->runNumber() ) ;
    m_sampleRunNumber = eventInfo->runNumber() ;
  }

  m_hforTruth.setSampleType(m_sampleRunNumber) ;

  //Some bookkeeping
  m_sampleType = m_hforTruth.getSampleType() ;
  m_sampleName = m_hforTruth.getSampleName() ;

  if (m_sampleType == HFORType::noType) {
    ATH_MSG_INFO(BOOST_CURRENT_FUNCTION <<
                    ": This MC (Run " << m_sampleRunNumber <<
                    " ) is not an mc15 Alpgen+Pythia6 sample - this tool will not do anything") ;
  }
  else {
    ATH_MSG_INFO(BOOST_CURRENT_FUNCTION <<
                 ": Good ! This is a valid Alpgen+Pythia6 sample ( " << m_sampleName << " )" ) ;
    ATH_MSG_INFO(BOOST_CURRENT_FUNCTION << ": MC Channel Number = " << eventInfo->mcChannelNumber()
                 << " ,  MC Run Number = "  << eventInfo->runNumber() );
  }

  //This routine never fails (unless it cannot get the container)
  //It may not identify the sample, in this case it will do nothing
  return StatusCode::SUCCESS;
}

//==============================================================================

//==============================================================================
// Getter to access the sample type
//==============================================================================
HFORType HFORSelectionTool::getSampleType() {
  if (! m_isConfigured){
    if (setSampleType().isFailure()) {
      ATH_MSG_ERROR("Did not configure correctly - results could be incorrect");
    }
    m_isConfigured = true;
  }
  //Return an enum object with the type of the sample
  return m_sampleType ;
}
//==============================================================================

//==============================================================================
// Getter to access the sample name
//==============================================================================
std::string HFORSelectionTool::getSampleName() {
  if (! m_isConfigured){
    if (setSampleType().isFailure()) {
      ATH_MSG_ERROR("Did not configure correctly - results could be incorrect");
    }
    m_isConfigured = true;
  }
//Return a string with the type of the sample (bb, cc, c, light or unknown)
  return m_sampleName ;
}
//==============================================================================

//==============================================================================
// Return a string with the LAST type of classification of this event (bb, cc,
// c, light, kill or unknown)
//==============================================================================
HFORType HFORSelectionTool::getDecisionType() {
  //TODO: Provide a method to return a string (like "isBB" etc.)
  return m_decisionType ;
}
//==============================================================================

