/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRegistry.h"

#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamData/RawEvent.h"
#include "ByteStreamData/ROBData.h"

#include "SGTools/StorableConversions.h"

// Trigger include(s):
#include "L1TopoRDO/L1TopoRDOCollection.h"
#include "L1TopoRDO/Helpers.h"

// Local include(s):
#include "L1TopoByteStreamCnv.h"
#include "L1TopoSrcIdMap.h"

/**
 * The constructor sets up all the ToolHandle and ServiceHandle objects and
 * initialises the
 * base class in the correct way.
 */
L1TopoByteStreamCnv::L1TopoByteStreamCnv(ISvcLocator* svcloc)
    : Converter(storageType(), classID(), svcloc),
      AthMessaging(svcloc != 0 ? msgSvc() : 0, "L1TopoByteStreamCnv"),
      m_tool("L1TopoByteStreamTool"),
      m_srcIdMap(0),
      m_robDataProvider("ROBDataProviderSvc", "L1TopoByteStreamCnv"),
      m_ByteStreamEventAccess("ByteStreamCnvSvc", "L1TopoByteStreamCnv") {}

/**
 * The destructor actually does some cleanup, it deletes the L1TopoSrcIdMap
 * object that is created in the initialize() function.
 */
L1TopoByteStreamCnv::~L1TopoByteStreamCnv() {
  if (m_srcIdMap) {
    delete m_srcIdMap;
    m_srcIdMap = 0;
  }
}

/**
 * Function telling the framework the Class ID of the object that this converter
 * is for (L1TopoRDOCollection).
 */
const CLID& L1TopoByteStreamCnv::classID() {
  return ClassID_traits<L1TopoRDOCollection>::ID();
}

long L1TopoByteStreamCnv::storageType() {
  return ByteStreamAddress::storageType();
}

/**
 * Init method gets all necessary services etc.
 */
StatusCode L1TopoByteStreamCnv::initialize() {
  //
  // Initialise the base class:
  //
  StatusCode sc = Converter::initialize();
  if (sc.isFailure()) {
    return sc;
  }

  MsgStream log(msgSvc(), "L1TopoByteStreamCnv");
  log << MSG::DEBUG << "L1TopoByteStreamCnv in initialize() " << endmsg;

  //
  // Get ByteStreamCnvSvc:
  //
  sc = m_ByteStreamEventAccess.retrieve();
  if (sc.isFailure()) {
    log << MSG::FATAL << "Can't get ByteStreamEventAccess interface" << endmsg;
    return sc;
  } else {
    log << MSG::DEBUG << "Connected to ByteStreamEventAccess interface"
        << endmsg;
  }

  //
  // Get L1TopoByteStreamTool:
  //
  sc = m_tool.retrieve();
  if (sc.isFailure()) {
    log << MSG::FATAL << "Can't get L1TopoByteStreamTool" << endmsg;
    return sc;
  } else {
    log << MSG::DEBUG << "Connected to L1TopoByteStreamTool" << endmsg;
  }

  //
  // Get ROBDataProvider:
  //
  sc = m_robDataProvider.retrieve();
  if (sc.isFailure()) {
    log << MSG::WARNING << "Can't get ROBDataProviderSvc" << endmsg;
    // return is disabled for Write BS which does not requre ROBDataProviderSvc
  } else {
    log << MSG::DEBUG << "Connected to ROBDataProviderSvc" << endmsg;
  }

  //
  // Create L1TopoSrcIdMap:
  //
  m_srcIdMap = new L1TopoSrcIdMap();

  return StatusCode::SUCCESS;
}

/**
 * This function creates the L1TopoRDO collection from the BS data. It requests
 * the ROB fragments with the ROB IDs of the L1Topo DAQ modules and gives these
 * fragments to the L1TopoByteStreamTool for conversion.
 */
StatusCode L1TopoByteStreamCnv::createObj(IOpaqueAddress* pAddr,
                                          DataObject*& pObj) {
  ATH_MSG_DEBUG("createObj() called");
  // -------------------------------------------------------------------------
  ByteStreamAddress* pBS_Addr = dynamic_cast<ByteStreamAddress*>(pAddr);
  CHECK(pBS_Addr != nullptr);
  // -------------------------------------------------------------------------
  const std::string nm = *(pBS_Addr->par());
  ATH_MSG_DEBUG("Creating Objects " << nm);

  // Create an empty collection. This is a DataVector<L1TopoRDO> which is like a
  // vector of pointers to L1TopoRDO and it owns the L1TopoRDOs from a memory
  // cleanup point of view
  L1TopoRDOCollection* result = new L1TopoRDOCollection();
  StatusCode sc = m_tool->convert(nm, result);
  if (sc.isFailure()) {
    return sc;
  }
  ATH_MSG_DEBUG("Converted  " << result->size() << " ROBs");
  pObj = SG::asStorable(result);

  return StatusCode::SUCCESS;
}

/**
 * This function receives a L1TopoRDO object as input, and adds the ROB fragment
 * of the L1Topo to the current raw event using the IByteStreamEventAccess
 * interface.
 */
StatusCode L1TopoByteStreamCnv::createRep(DataObject* pObj,
                                          IOpaqueAddress*& pAddr) {
  MsgStream log(msgSvc(), "L1TopoByteStreamCnv");

  log << MSG::DEBUG << "createRep() called" << endmsg;

  RawEventWrite* re = m_ByteStreamEventAccess->getRawEvent();

  L1TopoRDOCollection* result;
  if (!SG::fromStorable(pObj, result)) {
    log << MSG::ERROR << " Cannot cast to L1TopoRDOCollection" << endmsg;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << " Found " << result->size()
        << " L1TopoRDOs to convert to ROBs" << endmsg;
  }

  ByteStreamAddress* addr =
      new ByteStreamAddress(classID(), pObj->registry()->name(), "");

  pAddr = addr;

  // Convert to ByteStream
  for (L1TopoRDOCollection::const_iterator it = result->begin();
       it != result->end(); ++it) {
    StatusCode sc = m_tool->convert(*it, re);
    if (sc.isFailure()) {
      log << MSG::ERROR << " Failed to create ROB for L1TopoRDO:  " << **it
          << endmsg;
      return sc;
    }
  }
  return StatusCode::SUCCESS;
}
