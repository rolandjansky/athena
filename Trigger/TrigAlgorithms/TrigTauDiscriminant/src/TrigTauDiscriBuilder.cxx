/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigTauDiscriBuilder.cxx
 * PACKAGE:  Trigger/TrigAlgorithms/TrigTauDiscriminant
 *
 * AUTHOR:   M. Morgenstern
 *  *   
 *           Run a subset of the offline TauDiscriminant routines in the EF 
 *********************************************************************/

#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauDefs.h"
//#include "TauDiscriminant/TauDiscriToolBase.h"
#include "TrigTauDiscriminant/TrigTauDiscriBuilder.h"
#include "tauRecTools/ITauToolBase.h"

ATLAS_NO_CHECK_FILE_THREAD_SAFETY;  // legacy trigger code

using namespace std;

/////////////////////////////////////////////////////////////////
// Invokes base class constructor.
TrigTauDiscriBuilder::TrigTauDiscriBuilder(const std::string& name,ISvcLocator* pSvcLocator):
		  HLT::FexAlgo(name, pSvcLocator),
		  m_tools(this)
{
	declareProperty("Tools", m_tools, "List of TrigTauDiscriminantTools");

	declareMonitoredVariable("EF_LLHScore",m_LLHScore);
	declareMonitoredVariable("EF_BDTScore",m_BDTScore);
}

/////////////////////////////////////////////////////////////////
TrigTauDiscriBuilder::~TrigTauDiscriBuilder()
{ }

/////////////////////////////////////////////////////////////////
HLT::ErrorCode TrigTauDiscriBuilder::hltInitialize()
{

	msg() << MSG::INFO << "TrigTauDiscriBuilder::initialize()" << endmsg;

	///////////////////
	// Allocate Tools //
	////////////////////

	// check tool names
	if ( m_tools.begin() == m_tools.end() ) {
		msg() << MSG::ERROR << " no tools given for this algorithm." << endmsg;
		return HLT::BAD_JOB_SETUP;
	}

	// find tools
	//-------------------------------------------------------------------------
	ToolHandleArray<ITauToolBase> ::iterator p_itT = m_tools.begin();
	ToolHandleArray<ITauToolBase> ::iterator p_itTE = m_tools.end();
	msg() << MSG::INFO << "List of tools in execution sequence:" << endmsg;
	msg() << MSG::INFO << "------------------------------------" << endmsg;

	for(; p_itT != p_itTE; ++p_itT ) {
		StatusCode p_sc = p_itT->retrieve();
		if( p_sc.isFailure() ) {
			msg() << MSG::WARNING << "Cannot find tool named <";
			msg() << *p_itT << ">" << endmsg;
			return HLT::BAD_JOB_SETUP;
		}
		else {
			//add to manager
			//if( (*p_itT)->prepare(*this->manager).isFailure() ) {
			//	msg() << MSG::FATAL << "Initialization failed in tool " << p_itT->name() << endmsg;
			//	return HLT::BAD_JOB_SETUP;
			//}
			//add to timer
			//else {
		        msg() << MSG::INFO << "REGTEST ";
			msg() <<" add timer for tool "<< ( *p_itT )->type() <<" "<< ( *p_itT )->name() << endmsg;
			if(  doTiming() ) m_mytimers.push_back(addTimer((*p_itT)->name())) ;
			//}
		}
	}
	msg() << MSG::INFO << " " << endmsg;
	msg() << MSG::INFO << "------------------------------------" << endmsg;

	return HLT::OK;
}

/////////////////////////////////////////////////////////////////
HLT::ErrorCode TrigTauDiscriBuilder::hltFinalize()
{
	msg() << MSG::DEBUG << "Finalizing TrigTauDiscriBuilder" << endmsg;
	return HLT::OK;
}

/////////////////////////////////////////////////////////////////
// ATHENA EXECUTE METHOD:
HLT::ErrorCode TrigTauDiscriBuilder::hltExecute(const HLT::TriggerElement* /*inputTE*/, HLT::TriggerElement* outputTE){

	msg() << MSG::DEBUG << "Executing TrigTauDiscriBuilder" << endmsg;

	xAOD::TauJetContainer* tau_container = 0;
	const xAOD::TauJetContainer* const_tau_container(0);
	HLT::ErrorCode hltStatus = getFeature(outputTE, const_tau_container, "TrigTauRecMerged");
	tau_container = const_cast<xAOD::TauJetContainer*>(const_tau_container);

	if (hltStatus!=HLT::OK || ! tau_container) {
		msg() << MSG::DEBUG << "No input tau container found!" << endmsg;
		return HLT::OK;
	}

	// Update event-based variables
	//if (!this->manager->updateEvent()) {
	//	msg() << MSG::WARNING << "Updating event-based variables in TauDetailsManager failed! Do not trust discriminant outputs!" << endmsg;
	//	return HLT::OK;
	//}

	xAOD::TauJetContainer::iterator tau_it(tau_container->begin());
	xAOD::TauJetContainer::iterator tau_end(tau_container->end());
	// Loop over tau's:
	for (; tau_it != tau_end; ++tau_it) {
	        //if (!this->manager->update(**tau_it)) {
		//	msg() << MSG::WARNING << "Updating tau-based variables in TauDetailsManager failed! Do not trust discriminant outputs!" << endmsg;
		//	return HLT::OK;
		//}
		//msg() << MSG::VERBOSE << *this->manager << endmsg;

		//-----------------------------------------------------------------
		// Process the candidate
		//-----------------------------------------------------------------
		ToolHandleArray<ITauToolBase>::iterator tool_it(this->m_tools.begin());
		ToolHandleArray<ITauToolBase>::iterator tool_end(this->m_tools.end());

		//-----------------------------------------------------------------
		// Loop stops when Failure indicated by one of the tools
		//-----------------------------------------------------------------
		std::vector<TrigTimer* >::iterator itimer =  m_mytimers.begin();
		for(; tool_it != tool_end; ++tool_it ) {
			msg() << MSG::VERBOSE << "Invoking tool " << tool_it->name() << endmsg;
			if ( doTiming() && itimer != m_mytimers.end() ) {  (*itimer)->start();}

			StatusCode sc = (*tool_it)->execute( **tau_it);
			if( sc.isFailure() ) {
				msg() << MSG::FATAL << "Execute failed in tool " << tool_it->name() << endmsg;
				return HLT::ERROR;
			}
			itimer++;
			if ( doTiming() && itimer != m_mytimers.end() ) (*itimer)->stop();

		}
		//m_LLHScore = (*tau_it)->discriminant(xAOD::TauJetParameters::Likelihood);
		m_BDTScore = (*tau_it)->discriminant(xAOD::TauJetParameters::BDTJetScore);
	}

	return HLT::OK;
}





