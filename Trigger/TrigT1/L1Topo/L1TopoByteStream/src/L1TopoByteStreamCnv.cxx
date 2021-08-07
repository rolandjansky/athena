/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "GaudiKernel/IRegistry.h"

#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamData/RawEvent.h"
#include "ByteStreamData/ROBData.h"

#include "AthenaKernel/StorableConversions.h"

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
    : AthConstConverter(storageType(), classID(), svcloc, "L1TopoByteStreamCnv"),
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

  ATH_MSG_DEBUG("L1TopoByteStreamCnv in initialize() ");

  //
  // Get ByteStreamCnvSvc:
  //
  sc = m_ByteStreamEventAccess.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Can't get ByteStreamEventAccess interface");
    return sc;
  } else {
    ATH_MSG_DEBUG("Connected to ByteStreamEventAccess interface");
  }

  //
  // Get L1TopoByteStreamTool:
  //
  sc = m_tool.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Can't get L1TopoByteStreamTool");
    return sc;
  } else {
    ATH_MSG_DEBUG("Connected to L1TopoByteStreamTool");
  }

  //
  // Get ROBDataProvider:
  //
  sc = m_robDataProvider.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_WARNING("Can't get ROBDataProviderSvc");
    // return is disabled for Write BS which does not requre ROBDataProviderSvc
  } else {
    ATH_MSG_DEBUG("Connected to ROBDataProviderSvc");
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
StatusCode L1TopoByteStreamCnv::createObjConst(IOpaqueAddress* pAddr,
                                               DataObject*& pObj) const {
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
StatusCode L1TopoByteStreamCnv::createRepConst(DataObject* pObj,
                                               IOpaqueAddress*& pAddr) const {
  ATH_MSG_DEBUG("createRep() called");

  RawEventWrite* re = m_ByteStreamEventAccess->getRawEvent();

  L1TopoRDOCollection* result;
  if (!SG::fromStorable(pObj, result)) {
    ATH_MSG_ERROR(" Cannot cast to L1TopoRDOCollection");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG(" Found " << result->size() << " L1TopoRDOs to convert to ROBs");
  }

  ByteStreamAddress* addr =
      new ByteStreamAddress(classID(), pObj->registry()->name(), "");

  pAddr = addr;

  // Convert to ByteStream
  for (L1TopoRDOCollection::const_iterator it = result->begin();
       it != result->end(); ++it) {
    StatusCode sc = m_tool->convert(*it, re);
    if (sc.isFailure()) {
      ATH_MSG_ERROR(" Failed to create ROB for L1TopoRDO:  " << **it);
      return sc;
    }
  }
  return StatusCode::SUCCESS;
}
