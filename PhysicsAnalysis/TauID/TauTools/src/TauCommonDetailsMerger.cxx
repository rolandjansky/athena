/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///@file TauCommonDetailsMerger.cxx
///Implementation of TauCommonDetailsMerger
///
///Package: PhysicsAnalysis/TauID/TauTools
///
///@author Thomas Burgess <tburgess-at-cern-ch>
///
///@date 2009-03-30

#include "TauTools/TauCommonDetailsMerger.h"
#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauDetailsContainer.h"
#include <typeinfo>

//////////////////////////////////////////////////////////////////////////////////////
/// Constructor

TauCommonDetailsMerger::TauCommonDetailsMerger(
    const std::string& name,
    ISvcLocator* svcLocator) :
    AthAlgorithm(name, svcLocator),
    m_tauContainerName("TauRecContainer"),
    m_tauDetailsContainerName("TauRecDetailsContainer"),
    m_tauExtraDetailsContainerName("TauRecExtraDetailsContainer"),
    m_skipExtraDetails(false)
{
    declareProperty( "TauContainerName",
		     m_tauContainerName,
		     "Tau container name in storegate" );
    declareProperty( "TauDetailsContainerName",
		     m_tauDetailsContainerName,
		     "Tau common details container name in storegate" );
    declareProperty( "TauExtraDetailsContainerName",
		     m_tauExtraDetailsContainerName,
		     "Tau common extra details container name in storegate" );
    declareProperty("SkipExtraDetails", m_skipExtraDetails);
}

/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

TauCommonDetailsMerger::~TauCommonDetailsMerger()
{
}

////////////////////////////////////////////////////////////////////////////////////
/// initialize

StatusCode TauCommonDetailsMerger::initialize()
{
    if (msgLvl(MSG::VERBOSE)) 
	msg(MSG::VERBOSE) << "Initializing TauCommonDetailsMerger" << endreq;

    return StatusCode::SUCCESS;
}

///////////////////////////////
/// Execute - on event by event

StatusCode TauCommonDetailsMerger::execute()
{   
    //Get tau jets from store gate
    const Analysis::TauJetContainer* taus = 0;
    if ( evtStore()->retrieve(taus,m_tauContainerName).isFailure() ) {
	msg(MSG::WARNING) 
	    << "Failed to get tau container '"
	    << m_tauContainerName
	    << "' from StoreGate skipping merge for this event"
	    << endreq;
	return StatusCode::SUCCESS;
    } else if (msgLvl(MSG::VERBOSE))
	msg(MSG::VERBOSE)
	    << "Retrieved tau container '"<< m_tauContainerName
	    <<"' from StoreGate"<<endreq;
   

    // Loop over tau jets and see if details need merging or not
    bool hasValidTau = false;
    bool hasExtraDetail = false;
    bool hasCommonDetail = false;
    bool hasCommonExtraDetail = false;
    for (Analysis::TauJetContainer::const_iterator itau = taus->begin();
	 itau != taus->end(); ++itau) {
 	Analysis::TauJet* tau = const_cast<Analysis::TauJet*>(*itau);
	if (tau==0) continue;
	// if there are no details, skip event
	if ( tau->nDetails() == 0 ){
	  msg(MSG::WARNING) 
	    << "Found tau with no details, skipping this event without merging"
	    << endreq;
	  return StatusCode::SUCCESS;

	}
	// if there are details, loop over them
	for (int iDetail = 0; iDetail < tau->nDetails();iDetail++) {
	  // In release 14/15 AODs there are often bad element
	  // links from tau to details. Protect against this.
	  if (!tau->detailElementLink(iDetail).isValid()){
	    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Bad Element Link to Tau Detail"<< endreq;
	    continue;
	  }
          
          // from this point on, we are sure we have at least on tau with at least one valid container
          hasValidTau=true;

	  // Figure out if detail is commonDetails or ExtraDetails.
	  std::string::size_type commonLoc = tau->detailName(iDetail).find("TauRecDetailsContainer");
	  std::string::size_type extraLoc = tau->detailName(iDetail).find("TauRecExtraDetailsContainer");

	  if ( commonLoc != std::string::npos ) {
	    try{ // "try" to make safe dynamic cast
	      const Analysis::TauCommonDetails * commonDetails = 
	        dynamic_cast<const Analysis::TauCommonDetails*>(tau->detail(iDetail));
	      if (commonDetails){
		hasCommonDetail = true;		
	      }
	    }
	    catch (...) {
	      if (msgLvl(MSG::VERBOSE)) {
		msg(MSG::VERBOSE) << "Dynamic Cast Exception Caught" << endreq;
	      }
	    } 
	  }
	  // it is an ExtraDetails
	  else if ( extraLoc != std::string::npos ) {
	    hasExtraDetail = true;
	    try{
	      const Analysis::TauCommonExtraDetails * commonExtraDetails = 
		dynamic_cast<const Analysis::TauCommonExtraDetails*>(tau->detail(iDetail));
	      if (commonExtraDetails){
		hasCommonExtraDetail = true;
	      }
	    }
	    catch (...) {
	      if (msgLvl(MSG::VERBOSE)) {
		msg(MSG::VERBOSE) << "Dynamic Cast Exception Caught" << endreq;
	      }
	    }
	  }
	} // end loop over details
    } // end loop over taus
    ////////////////////
    // Don't merge at all if not needed or not wanted 
    bool skipExtraDetails = m_skipExtraDetails;
    if ( !hasValidTau || (hasCommonDetail && hasCommonExtraDetail) || (hasCommonDetail && skipExtraDetails) || (hasCommonDetail && !hasExtraDetail) ){
      msg(MSG::VERBOSE) 
	<< "There are no details which need to be merged, skipping event" 
	<< endreq;
      return StatusCode::SUCCESS;
    }
    ///////////////
    // OK, we must have a release <=14 file with details which need merging.
    //Find the old details in storegate
    const Analysis::TauDetailsContainer* tauDetails = 0;
    if (evtStore()->retrieve(tauDetails,m_tauDetailsContainerName).isFailure()){
	if (msgLvl(MSG::VERBOSE)) 
	    msg(MSG::VERBOSE) 
		<< "Failed to get tau details container '"
		<< m_tauDetailsContainerName
		<<"' from StoreGate"<<endreq;
    } else if (msgLvl(MSG::VERBOSE)) 
	msg(MSG::VERBOSE) 
	    << "Got tau details container '"
	    << m_tauDetailsContainerName
	    <<"' from StoreGate"<<endreq;
    
    //...and for extra details
    const Analysis::TauDetailsContainer* tauExtraDetails = 0;
    if (!skipExtraDetails) {
	if (evtStore()->retrieve(
		tauExtraDetails,m_tauExtraDetailsContainerName).isFailure()){
	    if (msgLvl(MSG::VERBOSE)) 
		msg(MSG::VERBOSE) 
		    << "Failed to get tau extra details container '"
		    << m_tauExtraDetailsContainerName
		    <<"' from StoreGate! Will skip extra details in this event"
		    << endreq;
	    skipExtraDetails=true;
	} else if (msgLvl(MSG::VERBOSE)) 
	    msg(MSG::VERBOSE) 
		<< "Got tau extra details container '"
		<< m_tauExtraDetailsContainerName
		<<"' from StoreGate"<<endreq;
    }
    //Skip event if there are no details in StoreGate
    if ( (tauDetails==0) && (tauExtraDetails==0)) {
	if (msgLvl(MSG::VERBOSE)) 
	    msg(MSG::VERBOSE) 
		<< "There are no details to merge, skipping event" 
		<< endreq;
	return StatusCode::SUCCESS;
    }
    //Make temporary containers for merged details
    Analysis::TauDetailsContainer* tauMergedDetails = 
	new Analysis::TauDetailsContainer();
    Analysis::TauDetailsContainer* tauMergedExtraDetails = 
	(skipExtraDetails)?0:new Analysis::TauDetailsContainer();
    //Loop over tau jets and merge details
    //(Null details are ok MergeToCommonDetails ignores them)
    for (Analysis::TauJetContainer::const_iterator itau = taus->begin();
	 itau != taus->end(); ++itau) {
	Analysis::TauJet* tau = const_cast<Analysis::TauJet*>(*itau);
	if (tau==0) continue;
	if (msgLvl(MSG::VERBOSE))
	    msg(MSG::VERBOSE) 
		<< "calling TauJet::MergeToCommonDetails" << endreq;
	tau->MergeToCommonDetails(
	    tauMergedDetails,tauMergedExtraDetails);
    }
    
    //Remove old details
    if (tauDetails!=0) {
	if (evtStore()->remove(tauDetails).isSuccess()) {
	    if (msgLvl(MSG::VERBOSE))
		msg(MSG::VERBOSE) 
		    << "removed old tau details container " 
		    <<m_tauDetailsContainerName 
		    << "' from StoreGate" <<endreq;	    
	} else {
	    msg(MSG::WARNING) 
		<< "Failed to remove old tau details container " 
		<<m_tauDetailsContainerName 
		<< "' from StoreGate" <<endreq;	    
	}
    }
    //... and for extra details
    if ((!skipExtraDetails) && (tauExtraDetails!=0)) {
	if (evtStore()->remove(tauExtraDetails).isSuccess()){
	    if (msgLvl(MSG::VERBOSE))
		msg(MSG::VERBOSE) 
		    << "removed old tau details container '" 
		    << m_tauExtraDetailsContainerName 
		    << "' from StoreGate" <<endreq;	    
	} else {
	    msg(MSG::WARNING) 
		<< "Failed to remove old extra tau details container " 
		<<m_tauDetailsContainerName 
		<< "' from StoreGate" <<endreq;	    
	}	
    }

    //Record new details in StoreGate
    if ( tauMergedDetails!=0) {
	if (evtStore()->record(
		tauMergedDetails,m_tauDetailsContainerName).isFailure()){
	    msg(MSG::WARNING)
		<< "Failed to store tau details container '"
		<< m_tauDetailsContainerName << "' in StoreGate" 
		<< endreq;	    
	} else if (msgLvl(MSG::VERBOSE)) {
	    msg(MSG::VERBOSE)
		<< "Recorded tau details container '" 
		<< m_tauDetailsContainerName
		<< "' in StoreGate" << endreq;
	} 
    }
    //... and for extra details
    if ( (!skipExtraDetails) && (tauMergedExtraDetails!=0) ) {
	if (evtStore()->record(
		tauMergedExtraDetails,
		m_tauExtraDetailsContainerName).isFailure()) {
	    msg(MSG::WARNING)
		<< "Failed to store tau extra details container '"
		<< m_tauExtraDetailsContainerName << "' in StoreGate" 
		<< endreq;	    
	} else if (msgLvl(MSG::VERBOSE)) {
	    msg(MSG::VERBOSE)
		<< "Recorded tau extra details container '" 
		<< m_tauExtraDetailsContainerName
		<< "' in StoreGate" << endreq;
	}   
    }
 
    return StatusCode::SUCCESS;
}

//////////////////////////////
/// Finalize - delete any memory allocation from the heap

StatusCode TauCommonDetailsMerger::finalize()
{
    return StatusCode::SUCCESS;
}
