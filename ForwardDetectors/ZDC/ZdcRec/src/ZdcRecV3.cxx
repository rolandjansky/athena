/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"
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
	//m_storeGate("StoreGateSvc", name),
	m_ownPolicy(static_cast<int> (SG::OWN_ELEMENTS)),
	//m_ttContainerName("ZdcTriggerTowers"),
	//m_zdcModuleContainerName("ZdcModules"),
	//m_zdcModuleAuxContainerName("ZdcModulesAux."),
	//m_ttContainer(0),
	m_eventCount(0),
	m_complainContain(1),
	m_complainRetrieve(1)
	//m_zdcTool("ZDC::ZdcAnalysisTool/ZdcAnalysisTool")
{
	declareProperty("OwnPolicy",m_ownPolicy) ;

	//declareProperty("DigitsContainerName", m_ttContainerName, "ZdcTriggerTowers");
	//declareProperty("ZdcModuleContainerName",    m_zdcModuleContainerName,    "ZdcModules");
	//declareProperty("ZdcModuleAuxContainerName",    m_zdcModuleAuxContainerName,    "ZdcModulesAux.");
}
//==================================================================================================

//==================================================================================================
ZdcRecV3::~ZdcRecV3() {}
//==================================================================================================

//==================================================================================================
StatusCode ZdcRecV3::initialize()
{
	MsgStream mLog(msgSvc(), name());

	/*
	// Look up the Storegate service
	StatusCode sc = m_storeGate.retrieve();
	if (sc.isFailure())
	{
		mLog << MSG::FATAL << "--> ZDC: Unable to retrieve pointer to StoreGateSvc" << endmsg;
		return sc;
	}
	*/

	// Reconstruction Tool
	ATH_CHECK( m_ChannelTool.retrieve() );

	// Reconstruction Tool
	ATH_CHECK( m_zdcTool.retrieve() );

	ATH_CHECK( m_zdcModuleContainerName.initialize() );
	ATH_CHECK( m_ttContainerName.initialize(SG::AllowEmpty) );

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

  const EventContext& ctx = Gaudi::Hive::currentContext();
  ATH_MSG_DEBUG ("--> ZDC: ZdcRecV3 execute starting on "
                 << ctx.evt()
                 << "th event");

  //Look for the container presence
  if (m_ttContainerName.empty()) {
    return StatusCode::SUCCESS;
  }

  // Look up the Digits "TriggerTowerContainer" in Storegate
  SG::ReadHandle<xAOD::TriggerTowerContainer> ttContainer    (m_ttContainerName, ctx);
  
  //Create the containers to hold the reconstructed information (you just pass the pointer and the converter does the work)	
  std::unique_ptr<xAOD::ZdcModuleContainer> moduleContainer( new xAOD::ZdcModuleContainer());
  std::unique_ptr<xAOD::ZdcModuleAuxContainer> moduleAuxContainer( new xAOD::ZdcModuleAuxContainer() );
  moduleContainer->setStore( moduleAuxContainer.get() );
  
  // rearrange ZDC channels and perform fast reco on all channels (including non-big tubes)
  int ncha = m_ChannelTool->convertTT2ZM(ttContainer.get(), moduleContainer.get() );
  ATH_MSG_DEBUG("m_ChannelTool->convertTT2ZM returned " << ncha << " channels");
  //msg( MSG::DEBUG ) << ZdcModuleToString(*moduleContainer) << endmsg;
  
  // re-reconstruct big tubes 
  ATH_CHECK( m_zdcTool->recoZdcModules(*moduleContainer.get()) ); // passes by reference

  SG::WriteHandle<xAOD::ZdcModuleContainer> moduleContainerH (m_zdcModuleContainerName, ctx);
  ATH_CHECK( moduleContainerH.record (std::move(moduleContainer),
				      std::move(moduleAuxContainer)) );

  return StatusCode::SUCCESS;

}
//==================================================================================================

//==================================================================================================
StatusCode ZdcRecV3::finalize()
{

  ATH_MSG_DEBUG( "--> ZDC: ZdcRecV3 finalize complete" );

  return StatusCode::SUCCESS;

}
//==================================================================================================

