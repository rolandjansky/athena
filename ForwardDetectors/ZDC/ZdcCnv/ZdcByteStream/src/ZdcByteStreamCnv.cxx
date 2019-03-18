/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * ZdcByteStreamCnv.cxx
 *
 *  Created on: May 27, 2009
 *      Author: leite
 *      Implementations to unpack the ZDC data from event fragments using
 *      bytestream implementations. This very much based on T1 code, but
 *      it's also very similar to other systems handling.
 *
 *      This follows a very specific order:
 *       ZdcByteStreamCnv::initialize
 *
 *       TODO: Make standandard the message option (debug, info etc)
 *
 */

#include <vector>
#include <stdint.h>

#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"
#include "ByteStreamData/ROBData.h"
#include "AthenaKernel/ClassID_traits.h"
#include "AthContainers/DataVector.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/StorableConversions.h"

///Zdc specific includes.
#include "ZdcEvent/ZdcDigits.h"
#include "ZdcEvent/ZdcDigitsCollection.h"
#include "ZdcByteStream/ZdcByteStreamCnv.h"
#include "ZdcByteStream/ZdcByteStreamTool.h"
#include "ZdcByteStream/ZdcByteStreamReadV1V2Tool.h"

//L1Calo include
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODTrigL1Calo/TriggerTowerAuxContainer.h"

//==================================================================================================
ZdcByteStreamCnv::ZdcByteStreamCnv(ISvcLocator* svcloc) :
    Converter(storageType(), classID(), svcloc), m_name("ZdcByteStreamCnv"),
	//m_tool("ZdcByteStreamTool/ZdcByteStreamTool"), // old style
	m_tool("ZdcByteStreamReadV1V2Tool/ZdcByteStreamReadV1V2Tool"), // new style
	m_robDataProvider("ROBDataProviderSvc",m_name),
	m_ByteStreamEventAccess("ByteStreamCnvSvc", m_name), m_log(msgSvc(), m_name),
	m_debug(false)
{
}
//==================================================================================================


//==================================================================================================
ZdcByteStreamCnv::~ZdcByteStreamCnv()
{
}
//==================================================================================================


//==================================================================================================
const CLID& ZdcByteStreamCnv::classID()
{
  //std::cout << "In ZdcByteStreamCnv::classID()" << std::endl;
  return ClassID_traits<ZdcDigitsCollection>::ID();
  //return ClassID_traits<xAOD::TriggerTowerContainer>::ID();
}
//==================================================================================================

//==================================================================================================
long ZdcByteStreamCnv::storageType()
{
  return ByteStreamAddress::storageType();
}
//==================================================================================================


//==================================================================================================
StatusCode ZdcByteStreamCnv::initialize()
{
	/**
	 *  Get all Athena services here:
	 *
	 *   - Gaudi Kernel Converter
	 *   - ByteStreamCnvSvc
	 *   - ZdcByteStreamTool
	 *   - ROBDataProvider
	 */

	m_debug = msgSvc()->outputLevel(m_name) <= MSG::DEBUG;
	m_log << MSG::DEBUG << "Initializing " << m_name << " - package version " << PACKAGE_VERSION
			<< endmsg;

	//Gaudi Converter Factory
	StatusCode sc = Converter::initialize();
	if (sc.isFailure())
	{
		m_log << MSG::ERROR << "ZDC: Failed to initialize Converter " << endmsg;
		return sc;
	}
	else
	{
		m_log << MSG::DEBUG << "ZDC: Converter initialized " << endmsg;
	}


	//Get ByteStreamCnvSvc
	sc = m_ByteStreamEventAccess.retrieve();
	if (sc.isFailure())
	{
		m_log << MSG::ERROR << "ZDC: Failed to retrieve service " << m_ByteStreamEventAccess << endmsg;
		return sc;
	}
	else
	{
		m_log << MSG::DEBUG << "ZDC: Retrieved service " << m_ByteStreamEventAccess << endmsg;
	}

	// Retrieve Tool
	sc = m_tool.retrieve();
	if (sc.isFailure())
	{
		m_log << MSG::ERROR << "ZDC: Failed to retrieve tool " << m_tool << endmsg;
		return sc;
	}
	else m_log << MSG::DEBUG << "ZDC: Retrieved tool " << m_tool << endmsg;

	// Get ROBDataProvider
	sc = m_robDataProvider.retrieve();
	if (sc.isFailure())
	{
		m_log << MSG::WARNING << "ZDC: Failed to retrieve service " << m_robDataProvider << endmsg;
	}
	else
	{
		m_log << MSG::DEBUG << "ZDC: Retrieved service " << m_robDataProvider << endmsg;
	}

	return StatusCode::SUCCESS;
}
//==================================================================================================


//==================================================================================================
StatusCode ZdcByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
	/// Create the RDO from bytestream.
	m_log << MSG::DEBUG << "ZDC: createObj() called" << endmsg;
	ByteStreamAddress *pBS_Addr;
	pBS_Addr = dynamic_cast<ByteStreamAddress *> (pAddr);
	if (!pBS_Addr)
	{
		m_log << MSG::ERROR << "ZDC: Can not cast to ByteStreamAddress " << endmsg;
		return StatusCode::FAILURE;
	}

	const std::string nm = *(pBS_Addr->par());

	if (m_debug) m_log << MSG::DEBUG << "ZDC: Creating Objects " << nm << endmsg;

	// Get SourceIDs; This is NOT related to the ZDC Identifiers
	//const std::vector<uint32_t>& vID(m_tool->sourceIDs()); // old style
	const std::vector<uint32_t>& vID(m_tool->ppmSourceIDs("temp")); // new style

	// get ROB fragments
	IROBDataProviderSvc::VROBFRAG robFrags;
	m_robDataProvider->getROBData(vID, robFrags);

	// size check
	if (m_debug)
	{
		m_log << MSG::DEBUG << "ZDC: Number of ROB fragments is " << robFrags.size() << endmsg;
	}

	//ZdcDigitsCollection* const ttCollection = new ZdcDigitsCollection;
	xAOD::TriggerTowerContainer* const TTCollection = new xAOD::TriggerTowerContainer; // new style
	xAOD::TriggerTowerAuxContainer* aux = new xAOD::TriggerTowerAuxContainer();
	TTCollection->setStore(aux);

	if (robFrags.size() == 0)
	{
	  //pObj = SG::asStorable(ttCollection);
	  pObj = SG::asStorable(TTCollection);
	  return StatusCode::SUCCESS;
	}


	//StatusCode sc = m_tool->convert(robFrags, ttCollection); // old style
	StatusCode sc = m_tool->convert(robFrags, TTCollection); // new style

	if (sc.isFailure())
	{
		m_log << MSG::ERROR << "ZDC: Failed to create Objects   " << nm << endmsg;
		//delete ttCollection;
		delete TTCollection; // new style
		return sc;
	}

        //pObj = SG::asStorable(ttCollection); // old style
        pObj = SG::asStorable(TTCollection); // new style

	return sc;
}
//==================================================================================================


//==================================================================================================
/// Create the bytestream from RDOs.

/*
StatusCode ZdcByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)
{
	m_log << MSG::DEBUG << "ZDC: createRep() called" << endmsg;

	RawEventWrite* re = m_ByteStreamEventAccess->getRawEvent();

	ZdcDigitsCollection* ttCollection = 0;
	if (!SG::fromStorable(pObj, ttCollection))
	{
		m_log << MSG::ERROR << "ZDC: Cannot cast to ZdcDigitsCollection" << endmsg;
		return StatusCode::FAILURE;
	}

	const std::string nm = pObj->registry()->name();
	ByteStreamAddress* addr = new ByteStreamAddress(classID(), nm, "");
	pAddr = addr;

	// Convert to ByteStream
	return m_tool->convert(ttCollection, re);
}
*/

//==================================================================================================
