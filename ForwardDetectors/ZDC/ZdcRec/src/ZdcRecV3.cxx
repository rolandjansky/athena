/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZdcRecV3.cxx
 *
 *  Created on: Sept 11, 2016 (never forget)
 *      Author: Peter.Steinberg@bnl.gov
 */


#include <memory>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
//#include "Identifier/Identifier.h"

#include "xAODForward/ZdcModuleAuxContainer.h"
#include "xAODForward/ZdcModuleContainer.h"
#include "ZdcRec/ZdcRecV3.h"
#include "ZdcRec/ZdcRecChannelToolV2.h"
#include "xAODForward/ZdcModuleToString.h"
#include "ZdcAnalysis/ZdcAnalysisTool.h"
#include "ZdcByteStream/ZdcToString.h"

//==================================================================================================
ZdcRecV3::ZdcRecV3(const std::string& name, ISvcLocator* pSvcLocator) :

	AthAlgorithm(name, pSvcLocator),
	m_storeGate("StoreGateSvc", name),
	m_ownPolicy(static_cast<int> (SG::OWN_ELEMENTS)),
	m_ttContainerName("ZdcTriggerTowers"),
	m_zdcModuleContainerName("ZdcModules"),
	m_zdcModuleAuxContainerName("ZdcModulesAux."),
	m_ttContainer(0),
	m_eventCount(0),
	m_complainContain(1),
	m_complainRetrieve(1),
        m_ChannelTool("ZdcRecChannelToolV2"),
	m_zdcTool("ZDC::ZdcAnalysisTool/ZdcAnalysisTool")
{
	declareProperty("OwnPolicy",m_ownPolicy) ;

	declareProperty("DigitsContainerName", m_ttContainerName, "ZdcTriggerTowers");
	declareProperty("ZdcModuleContainerName",    m_zdcModuleContainerName,    "ZdcModules");
	declareProperty("ZdcModuleAuxContainerName",    m_zdcModuleAuxContainerName,    "ZdcModulesAux.");
}
//==================================================================================================

//==================================================================================================
ZdcRecV3::~ZdcRecV3() {}
//==================================================================================================

//==================================================================================================
StatusCode ZdcRecV3::initialize()
{
	MsgStream mLog(msgSvc(), name());

	// Look up the Storegate service
	StatusCode sc = m_storeGate.retrieve();
	if (sc.isFailure())
	{
		mLog << MSG::FATAL << "--> ZDC: Unable to retrieve pointer to StoreGateSvc" << endmsg;
		return sc;
	}


	// Reconstruction Tool
	StatusCode scTool = m_ChannelTool.retrieve();
	if (scTool.isFailure())
	{
		mLog << MSG::WARNING << "--> ZDC: Could not retrieve " << m_ChannelTool << endmsg;
		return StatusCode::FAILURE;
	}
	mLog << MSG::DEBUG << "--> ZDC: SUCCESS retrieving " << m_ChannelTool << endmsg;

	// Reconstruction Tool
	StatusCode zdcTool = m_zdcTool.retrieve();
	if (zdcTool.isFailure())
	{
		mLog << MSG::WARNING << "--> ZDC: Could not retrieve " << m_zdcTool << endmsg;
		return StatusCode::FAILURE;
	}
	mLog << MSG::DEBUG << "--> ZDC: SUCCESS retrieving " << m_zdcTool << endmsg;

	// Container output name
	//TODO: change MESSAGE !!
	mLog << MSG::DEBUG << " Output Container Name " << m_zdcModuleContainerName << endmsg;
	if (m_ownPolicy == SG::OWN_ELEMENTS)
		mLog << MSG::DEBUG << "...will OWN its cells." << endmsg;
	else
		mLog << MSG::DEBUG << "...will VIEW its cells." << endmsg;


	mLog << MSG::DEBUG << "--> ZDC: ZdcRecV3 initialization complete" << endmsg;

	return StatusCode::SUCCESS;
}
//==================================================================================================

//==================================================================================================
StatusCode ZdcRecV3::execute()
{

  MsgStream mLog(msgSvc(), name());
	mLog << MSG::DEBUG
	     << "--> ZDC: ZdcRecV3 execute starting on "
	     << m_eventCount
	     << "th event"
		 << endmsg;

	m_eventCount++;

	//Look for the container presence
	bool dg = m_storeGate->contains<xAOD::TriggerTowerContainer>( m_ttContainerName);
	if (!dg) {
	  if (m_complainContain) mLog << MSG::WARNING << "--> ZDC: StoreGate does not contain " << m_ttContainerName << endmsg;
	  m_complainContain = 0;
	  return StatusCode::SUCCESS;
	}

	// Look up the Digits "TriggerTowerContainer" in Storegate
	StatusCode digitsLookupSC = m_storeGate->retrieve(m_ttContainer, m_ttContainerName);
	if (digitsLookupSC.isFailure())
	{
	  if (m_complainRetrieve)
	    mLog << MSG::WARNING
		 << "--> ZDC: Could not retrieve "
		 << m_ttContainerName
		 << " from StoreGate"
		 << endmsg;
	  m_complainRetrieve = 0;
	  return StatusCode::SUCCESS;
	}

	if (digitsLookupSC.isSuccess() && !m_ttContainer)
	{
		mLog << MSG::ERROR
			 << "--> ZDC: Storegate returned zero pointer for "
			 << m_ttContainerName
			 << endmsg;
		return StatusCode::SUCCESS;
	}

    	//Create the containers to hold the reconstructed information (you just pass the pointer and the converter does the work)	
	std::unique_ptr<xAOD::ZdcModuleContainer> moduleContainer( new xAOD::ZdcModuleContainer());
	std::unique_ptr<xAOD::ZdcModuleAuxContainer> moduleAuxContainer( new xAOD::ZdcModuleAuxContainer() );
	moduleContainer->setStore( moduleAuxContainer.get() );

	// rearrange ZDC channels and perform fast reco on all channels (including non-big tubes)
	int ncha = m_ChannelTool->convertTT2ZM(m_ttContainer, moduleContainer.get() );
	
	msg( MSG::DEBUG ) << "Channel tool returns " << ncha << endmsg;
	msg( MSG::DEBUG ) << ZdcModuleToString(*moduleContainer) << endmsg;

	// re-reconstruct big tubes
 
	ATH_CHECK( evtStore()->record( std::move(moduleContainer), m_zdcModuleContainerName) );
	ATH_CHECK( evtStore()->record( std::move(moduleAuxContainer), m_zdcModuleAuxContainerName) );

	ATH_CHECK( m_zdcTool->reprocessZdc() ); // pulls the modules out of the event store

	return StatusCode::SUCCESS;

}
//==================================================================================================

//==================================================================================================
StatusCode ZdcRecV3::finalize()
{

  MsgStream mLog(msgSvc(),name());

  mLog << MSG::DEBUG
		  << "--> ZDC: ZdcRecV3 finalize complete"
		  << endmsg;

  return StatusCode::SUCCESS;

}
//==================================================================================================

