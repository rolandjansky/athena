/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZdcRecV2.cxx
 *
 *  Created on: Nov 24, 2009
 *      Author: leite
 */


#include <memory>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
//#include "Identifier/Identifier.h"

#include "xAODForward/ZdcModuleAuxContainer.h"
#include "xAODForward/ZdcModuleContainer.h"
#include "ZdcRec/ZdcRecV2.h"
#include "ZdcRec/ZdcRecChannelToolV2.h"
#include "xAODForward/ZdcModuleToString.h"

#include "ZdcByteStream/ZdcToString.h"

//==================================================================================================
ZdcRecV2::ZdcRecV2(const std::string& name, ISvcLocator* pSvcLocator) :

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
        m_ChannelTool("ZdcRecChannelToolV2")

{
	declareProperty("OwnPolicy",m_ownPolicy) ;

	declareProperty("DigitsContainerName", m_ttContainerName, "ZdcTriggerTowers");
	declareProperty("ZdcModuleContainerName",    m_zdcModuleContainerName,    "ZdcModules");
	declareProperty("ZdcModuleAuxContainerName",    m_zdcModuleAuxContainerName,    "ZdcModulesAux.");
}
//==================================================================================================

//==================================================================================================
ZdcRecV2::~ZdcRecV2() {}
//==================================================================================================

//==================================================================================================
StatusCode ZdcRecV2::initialize()
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
	
	// Container output name
	//TODO: change MESSAGE !!
	mLog << MSG::DEBUG << " Output Container Name " << m_zdcModuleContainerName << endmsg;
	if (m_ownPolicy == SG::OWN_ELEMENTS)
		mLog << MSG::DEBUG << "...will OWN its cells." << endmsg;
	else
		mLog << MSG::DEBUG << "...will VIEW its cells." << endmsg;


	mLog << MSG::DEBUG << "--> ZDC: ZdcRecV2 initialization complete" << endmsg;

	return StatusCode::SUCCESS;
}
//==================================================================================================

//==================================================================================================
StatusCode ZdcRecV2::execute()
{

  MsgStream mLog(msgSvc(), name());
	mLog << MSG::DEBUG
	     << "--> ZDC: ZdcRecV2 execute starting on "
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

	int ncha = m_ChannelTool->convertTT2ZM(m_ttContainer, moduleContainer.get() );
	msg( MSG::DEBUG ) << "Channel tool returns " << ncha << endmsg;

	msg( MSG::DEBUG ) << ZdcModuleToString(*moduleContainer) << endmsg;
	//msg( MSG::INFO ) << ZdcModuleToString(*moduleContainer) << endmsg;

	ATH_CHECK( evtStore()->record( std::move(moduleContainer), m_zdcModuleContainerName) );
	ATH_CHECK( evtStore()->record( std::move(moduleAuxContainer), m_zdcModuleAuxContainerName) );

	return StatusCode::SUCCESS;

}
//==================================================================================================

//==================================================================================================
StatusCode ZdcRecV2::finalize()
{

  MsgStream mLog(msgSvc(),name());

  mLog << MSG::DEBUG
		  << "--> ZDC: ZdcRecV2 finalize complete"
		  << endmsg;

  return StatusCode::SUCCESS;

}
//==================================================================================================

