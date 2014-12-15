/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <stdint.h>

#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include "ByteStreamData/RawEvent.h"
#include "ByteStreamData/ROBData.h"

#include "DataModel/DataVector.h"

#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"

#include "SGTools/ClassID_traits.h"
#include "SGTools/StorableConversions.h"

#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODTrigL1Calo/TriggerTowerAuxContainer.h"

#include "PpmByteStreamV2Cnv.h"
#include "PpmByteStreamV2Tool.h"

namespace LVL1BS {

PpmByteStreamV2Cnv::PpmByteStreamV2Cnv(ISvcLocator* svcloc) :
		Converter(ByteStream_StorageType, classID(), svcloc),
		AthMessaging(svcloc != 0 ? msgSvc() : 0, "PpmByteStreamV2Cnv"),
		m_name("PpmByteStreamV2Cnv"),
		m_tool("LVL1BS::PpmByteStreamV2Tool/PpmByteStreamV2Tool"),
		m_robDataProvider("ROBDataProviderSvc", m_name),
		m_ByteStreamEventAccess("ByteStreamCnvSvc", m_name){
}

PpmByteStreamV2Cnv::~PpmByteStreamV2Cnv() {
}

// CLID

const CLID& PpmByteStreamV2Cnv::classID() {
	return ClassID_traits < xAOD::TriggerTowerContainer > ::ID();
}

//  Init method gets all necessary services etc.

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode PpmByteStreamV2Cnv::initialize() {
	ATH_MSG_DEBUG(
			"Initializing " << m_name << " - package version "
					<< PACKAGE_VERSION
	);

	StatusCode sc = Converter::initialize();
	if (sc.isFailure()) {
		return sc;
	}

	//Get ByteStreamCnvSvc
	sc = m_ByteStreamEventAccess.retrieve();
	if (sc.isFailure()) {
		ATH_MSG_ERROR("Failed to retrieve service " << m_ByteStreamEventAccess);
		return sc;
	} else {
		ATH_MSG_DEBUG("Retrieved service " << m_ByteStreamEventAccess);
	}

	// Retrieve Tool
	sc = m_tool.retrieve();
	if (sc.isFailure()) {
		ATH_MSG_ERROR("Failed to retrieve tool " << m_tool);
		return sc;
	} else {
		ATH_MSG_DEBUG("Retrieved tool " << m_tool);
	}
	// Get ROBDataProvider
	sc = m_robDataProvider.retrieve();
	if (sc.isFailure()) {
		ATH_MSG_WARNING("Failed to retrieve service " << m_robDataProvider);
		// return is disabled for Write BS which does not require ROBDataProviderSvc
		// return sc ;
	} else {
		ATH_MSG_DEBUG("Retrieved service " << m_robDataProvider);
	}

	return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.

StatusCode PpmByteStreamV2Cnv::createObj(IOpaqueAddress* pAddr,
		DataObject*& pObj) {
	ATH_MSG_DEBUG("createObj() called");
	// -------------------------------------------------------------------------
	ByteStreamAddress *pBS_Addr;
	pBS_Addr = dynamic_cast<ByteStreamAddress *>(pAddr);
	if (!pBS_Addr) {
		ATH_MSG_DEBUG("Can not cast to ByteStreamAddress");;
		return StatusCode::FAILURE;
	}
	// -------------------------------------------------------------------------
	const std::string nm = *(pBS_Addr->par());
	ATH_MSG_DEBUG("Creating Objects " << nm);
	// -------------------------------------------------------------------------
	// // get SourceIDs
	const std::vector<uint32_t>& vID(m_tool->sourceIDs(nm));
	// // get ROB fragments
	IROBDataProviderSvc::VROBFRAG robFrags;
	m_robDataProvider->getROBData(vID, robFrags);
	// -------------------------------------------------------------------------
	// size check
	xAOD::TriggerTowerContainer* const ttCollection =
			new xAOD::TriggerTowerContainer(SG::VIEW_ELEMENTS);

	ATH_MSG_DEBUG("Number of ROB fragments is " << robFrags.size());

	if (!robFrags.size()) {
		pObj = SG::asStorable(ttCollection);
		return StatusCode::SUCCESS;
	}
	// -------------------------------------------------------------------------
	StatusCode sc = m_tool->convert(robFrags, ttCollection);
	if (sc.isFailure()) {
		ATH_MSG_ERROR("Failed to create Objects");
		delete ttCollection;
		return sc;
	}

	pObj = SG::asStorable(ttCollection);
	return sc;
}

// createRep should create the bytestream from RDOs.

StatusCode PpmByteStreamV2Cnv::createRep(DataObject* pObj,
		IOpaqueAddress*& pAddr) {
	//TODO: (Sasha) implement createRep
	ATH_MSG_DEBUG("createRep() called");

	RawEventWrite* re = m_ByteStreamEventAccess->getRawEvent();

	xAOD::TriggerTowerContainer* ttCollection = 0;
	if(!SG::fromStorable(pObj, ttCollection )) {
	   ATH_MSG_ERROR("Cannot cast to xAOD::TriggerTowerContainer");
	   return StatusCode::FAILURE;
	}

	const std::string nm = pObj->registry()->name();

	ByteStreamAddress* addr = new ByteStreamAddress( classID(), nm, "" );
	pAddr = addr;

	// Convert to ByteStream
	return m_tool->convert( ttCollection, re );
}

} // end namespace
