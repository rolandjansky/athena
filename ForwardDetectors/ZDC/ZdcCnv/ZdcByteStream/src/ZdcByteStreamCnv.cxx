/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "CLIDSvc/tools/ClassID_traits.h"
#include "DataModel/DataVector.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "SGTools/StorableConversions.h"

///Zdc specific includes.
#include "ZdcEvent/ZdcDigits.h"
#include "ZdcEvent/ZdcDigitsCollection.h"
#include "ZdcByteStream/ZdcByteStreamCnv.h"
#include "ZdcByteStream/ZdcByteStreamTool.h"

//==================================================================================================
ZdcByteStreamCnv::ZdcByteStreamCnv(ISvcLocator* svcloc) :
	Converter(ByteStream_StorageType, classID(), svcloc), m_name("ZdcByteStreamCnv"),
			m_tool("ZdcByteStreamTool/ZdcByteStreamTool"), m_robDataProvider("ROBDataProviderSvc",
																				m_name),
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
	return ClassID_traits<ZdcDigitsCollection>::ID();
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
			<< endreq;

	//Gaudi Converter Factory
	StatusCode sc = Converter::initialize();
	if (sc.isFailure())
	{
		m_log << MSG::ERROR << "ZDC: Failed to initialize Converter " << endreq;
		return sc;
	}
	else
	{
		m_log << MSG::DEBUG << "ZDC: Converter initialized " << endreq;
	}


	//Get ByteStreamCnvSvc
	sc = m_ByteStreamEventAccess.retrieve();
	if (sc.isFailure())
	{
		m_log << MSG::ERROR << "ZDC: Failed to retrieve service " << m_ByteStreamEventAccess << endreq;
		return sc;
	}
	else
	{
		m_log << MSG::DEBUG << "ZDC: Retrieved service " << m_ByteStreamEventAccess << endreq;
	}

	// Retrieve Tool
	sc = m_tool.retrieve();
	if (sc.isFailure())
	{
		m_log << MSG::ERROR << "ZDC: Failed to retrieve tool " << m_tool << endreq;
		return sc;
	}
	else m_log << MSG::DEBUG << "ZDC: Retrieved tool " << m_tool << endreq;

	// Get ROBDataProvider
	sc = m_robDataProvider.retrieve();
	if (sc.isFailure())
	{
		m_log << MSG::WARNING << "ZDC: Failed to retrieve service " << m_robDataProvider << endreq;
	}
	else
	{
		m_log << MSG::DEBUG << "ZDC: Retrieved service " << m_robDataProvider << endreq;
	}

	return StatusCode::SUCCESS;
}
//==================================================================================================


//==================================================================================================
StatusCode ZdcByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
	/// Create the RDO from bytestream.
	m_log << MSG::DEBUG << "ZDC: createObj() called" << endreq;
	ByteStreamAddress *pBS_Addr;
	pBS_Addr = dynamic_cast<ByteStreamAddress *> (pAddr);
	if (!pBS_Addr)
	{
		m_log << MSG::ERROR << "ZDC: Can not cast to ByteStreamAddress " << endreq;
		return StatusCode::FAILURE;
	}

	const std::string nm = *(pBS_Addr->par());

	if (m_debug) m_log << MSG::DEBUG << "ZDC: Creating Objects " << nm << endreq;

	// Get SourceIDs; This is NOT related to the ZDC Identifiers
	const std::vector<uint32_t>& vID(m_tool->sourceIDs());

	// get ROB fragments
	IROBDataProviderSvc::VROBFRAG robFrags;
	m_robDataProvider->getROBData(vID, robFrags);

	// size check
	ZdcDigitsCollection* const ttCollection = new ZdcDigitsCollection;
	if (m_debug)
	{
		m_log << MSG::DEBUG << "ZDC: Number of ROB fragments is " << robFrags.size() << endreq;
	}

	if (robFrags.size() == 0)
	{
		pObj = SG::asStorable(ttCollection);
		return StatusCode::SUCCESS;
	}

	StatusCode sc = m_tool->convert(robFrags, ttCollection);
	if (sc.isFailure())
	{
		m_log << MSG::ERROR << "ZDC: Failed to create Objects   " << nm << endreq;
		delete ttCollection;
		return sc;
	}

	pObj = SG::asStorable(ttCollection);

	return sc;
}
//==================================================================================================


//==================================================================================================
/// Create the bytestream from RDOs.
StatusCode ZdcByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)
{
	m_log << MSG::DEBUG << "ZDC: createRep() called" << endreq;

	RawEventWrite* re = m_ByteStreamEventAccess->getRawEvent();

	ZdcDigitsCollection* ttCollection = 0;
	if (!SG::fromStorable(pObj, ttCollection))
	{
		m_log << MSG::ERROR << "ZDC: Cannot cast to ZdcDigitsCollection" << endreq;
		return StatusCode::FAILURE;
	}

	const std::string nm = pObj->registry()->name();
	ByteStreamAddress* addr = new ByteStreamAddress(classID(), nm, "");
	pAddr = addr;

	// Convert to ByteStream
	return m_tool->convert(ttCollection, re);
}
//==================================================================================================
