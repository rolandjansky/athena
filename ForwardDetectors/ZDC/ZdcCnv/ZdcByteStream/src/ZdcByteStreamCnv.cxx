/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/ClassID_traits.h"
#include "AthContainers/DataVector.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/StorableConversions.h"
#include "AthenaKernel/errorcheck.h"

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
	m_ByteStreamEventAccess("ByteStreamCnvSvc", m_name),
        m_evtStore("StoreGateSvc", m_name),
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

	ATH_CHECK( Converter::initialize() );
	ATH_CHECK( m_ByteStreamEventAccess.retrieve() );
	ATH_CHECK( m_tool.retrieve() );
	ATH_CHECK( m_evtStore.retrieve() );

	StatusCode sc = m_robDataProvider.retrieve();
	if (sc.isFailure())
	{
                REPORT_MESSAGE (MSG::WARNING) << "ZDC: Failed to retrieve service " << m_robDataProvider;
	}

	return StatusCode::SUCCESS;
}
//==================================================================================================


//==================================================================================================
StatusCode ZdcByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
	/// Create the RDO from bytestream.
	ByteStreamAddress *pBS_Addr;
	pBS_Addr = dynamic_cast<ByteStreamAddress *> (pAddr);
	if (!pBS_Addr)
	{
                REPORT_ERROR (StatusCode::FAILURE) << "ZDC: Can not cast to ByteStreamAddress ";
		return StatusCode::FAILURE;
	}

	const std::string nm = *(pBS_Addr->par());

	if (m_debug) {
          REPORT_MESSAGE (MSG::DEBUG) << "ZDC: Creating Objects " << nm;
        }

	// Get SourceIDs; This is NOT related to the ZDC Identifiers
	//const std::vector<uint32_t>& vID(m_tool->sourceIDs()); // old style
	const std::vector<uint32_t>& vID(m_tool->ppmSourceIDs("temp")); // new style

	// get ROB fragments
	IROBDataProviderSvc::VROBFRAG robFrags;
	m_robDataProvider->getROBData(vID, robFrags);

	// size check
	if (m_debug)
	{
          REPORT_MESSAGE (MSG::DEBUG) << "ZDC: Number of ROB fragments is " << robFrags.size();
	}

	//ZdcDigitsCollection* const ttCollection = new ZdcDigitsCollection;
	auto TTCollection = std::make_unique<xAOD::TriggerTowerContainer>(); // new style
	auto aux = std::make_unique<xAOD::TriggerTowerAuxContainer>();
	TTCollection->setStore(aux.get());

	if (robFrags.size() == 0)
	{
	  pObj = SG::asStorable(std::move(TTCollection));
          ATH_CHECK( m_evtStore->record (std::move(aux), nm + "Aux.") );
	  return StatusCode::SUCCESS;
	}


	ATH_CHECK( m_tool->convert(robFrags, TTCollection.get()) ); // new style

        pObj = SG::asStorable(std::move(TTCollection)); // new style
        ATH_CHECK( m_evtStore->record (std::move(aux), nm + "Aux.") );

	return StatusCode::SUCCESS;
}
//==================================================================================================


//==================================================================================================
/// Create the bytestream from RDOs.

/*
StatusCode ZdcByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)
{
	RawEventWrite* re = m_ByteStreamEventAccess->getRawEvent();

	ZdcDigitsCollection* ttCollection = 0;
	if (!SG::fromStorable(pObj, ttCollection))
	{
                REPORT_ERROR (StatusCode::FAILURE) << "ZDC: Cannot cast to ZdcDigitsCollection";
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
