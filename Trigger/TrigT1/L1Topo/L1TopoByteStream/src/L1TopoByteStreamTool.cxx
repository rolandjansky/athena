/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <exception>
#include <bitset>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"
#include "ByteStreamData/ROBData.h"

// Trigger include(s):
#include "L1TopoRDO/L1TopoRDO.h"
#include "L1TopoRDO/L1TopoRDOCollection.h"
#include "L1TopoRDO/ModuleID.h"
#include "L1TopoRDO/Helpers.h"

// Local include(s):
#include "L1TopoByteStreamTool.h"

/// Unique interface ID of the tool that identifies it to the framweork
static const InterfaceID IID_IL1TopoByteStreamTool("L1TopoByteStreamTool", 1,
                                                   0);

/**
 * This function is needed by the framework to know what kind of tool
 * this is.
 */
const InterfaceID& L1TopoByteStreamTool::interfaceID() {
  return IID_IL1TopoByteStreamTool;
}

/**
 * The constructor takes care of correctly constructing the base class and
 * declaring the tool's interface to the framework.
 */
L1TopoByteStreamTool::L1TopoByteStreamTool(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent)
    : AthAlgTool(type, name, parent),
      m_srcIdMap(0),
      m_robDataProvider("ROBDataProviderSvc", "L1TopoByteStreamTool") {
  declareInterface<L1TopoByteStreamTool>(this);
  declareProperty("ROBSourceIDs", m_sourceIDs,
                  "ROB fragment source identifiers - overrides decodeDAQROBs "
                  "and decodeROIROBs");
  declareProperty(
      "decodeDAQROBs", m_doDAQROBs = true,
      "add standard DAQ ROBs to list of ROBs to decode; default true");
  declareProperty(
      "decodeROIROBs", m_doROIROBs = false,
      "add standard ROI ROBs to list of ROBs to decode; default false");
}

/**
 * The destructor doesn't need to do anything.
 */
L1TopoByteStreamTool::~L1TopoByteStreamTool() {}

/**
 * The function creates a L1TopoSrcIdMap object that is used in the conversion
 * and initialises the base class.
 */
StatusCode L1TopoByteStreamTool::initialize() {
  m_srcIdMap = new L1TopoSrcIdMap();
  ATH_MSG_DEBUG(m_sourceIDs);
  if (!m_sourceIDs.empty()) {
    ATH_MSG_INFO(
        "ROBSourceIDs property is set: this overrides decodeDAQROBs and "
        "decodeROIROBs");
  }
  ATH_MSG_DEBUG(m_doDAQROBs);
  ATH_MSG_DEBUG(m_doROIROBs);
  ATH_MSG_DEBUG("using sourceIDs " << std::hex << std::showbase << sourceIDs()
                                   << std::dec << std::noshowbase);

  CHECK(m_robDataProvider.retrieve());
  return AlgTool::initialize();
}

/**
 * The function deletes the L1TopoSrcIdMap object and finalises the base class.
 */
StatusCode L1TopoByteStreamTool::finalize() {
  delete m_srcIdMap;
  return AlgTool::finalize();
}

/**
 * Return list of L1Topo source IDs to use for conversion.
 * By default the list of DAQ IDs are generated
 * Will generate list according to algorithm herein, if not already provided via
 * properties.
 * Used by L1TopoByteStreamCnv and L1TopoByteStreamTool.
 * The boolean properties decodeDAQROBs and decodeROIROBs are used to determine
 * which
 * ROB source IDs are added to the list.
 */
const std::vector<uint32_t>& L1TopoByteStreamTool::sourceIDs() {
  if (m_sourceIDs.empty()) {
    // iterate over link numbers: 0 is DAQ link, 1 is ROIB, 2 for New ROIB
    for (unsigned int slink : {0, 1, 2}) {
      unsigned int roiOrDaq = 0;
      if (slink > 0) {  // 0 means DAQ-type link, 1 means ROI-type link
        roiOrDaq = 1;
      }
      if (roiOrDaq == 0 && !m_doDAQROBs) {
        continue;  // skip DAQ ROBs
      } else if (roiOrDaq == 1 && !m_doROIROBs) {
        continue;  // skip ROI ROBs
      }
      for (unsigned int module :
           {0, 1}) {  // could be upgraded to 3 modules eventually?
        const uint32_t moduleId =
            L1Topo::ModuleID(slink, module, roiOrDaq).id();
        eformat::helper::SourceIdentifier sourceId(eformat::TDAQ_CALO_TOPO_PROC,
                                                   moduleId);
        m_sourceIDs.push_back(sourceId.code());
      }
    }
  }
  return m_sourceIDs;
}

/**
 * Conversion from RDO to eformat::ROBFragment.
 * This is called from the L1TopoByteStreamCnv::createRep method.
 */
StatusCode L1TopoByteStreamTool::convert(const L1TopoRDO* result,
                                         RawEventWrite* re) {
  ATH_MSG_DEBUG("executing convert() from RDO to ROBFragment");

  // Clear Event Assembler
  m_fea.clear();

  // Reset lumi-block number to 1
  m_fea.setDetEvtType(1);
  // Set L1Apos to center of readout window
  // uint16_t minorVersion = ( result->getNumberOfBunches() - 1u ) / 2u;
  // minorVersion &= ctpVersion.getL1APositionMask();
  // minorVersion <<= ctpVersion.getL1APositionShift();
  uint16_t minorVersion = 0;
  m_fea.setRodMinorVersion(minorVersion);

  FullEventAssembler<L1TopoSrcIdMap>::RODDATA* theROD;

  // Source ID of L1Topo
  // NB needs to change to using list of modules - as it is, it will just
  // produce one with an unconventional ID of 0x91000000.
  const uint32_t rodId = m_srcIdMap->getRodID();
  ATH_MSG_DEBUG(" ROD ID:" << MSG::hex << rodId);
  const std::vector<uint32_t> rodIds = sourceIDs();
  ATH_MSG_DEBUG(" ROD IDs:" << MSG::hex << rodIds << " NOT YET IMPLEMENTED");

  // get the ROD data container to be filled
  theROD = m_fea.getRodData(rodId);

  ATH_MSG_VERBOSE(" Dumping L1Topo data words:");
  // fill Data Words
  const std::vector<uint32_t>& vDataWords = result->getDataWords();
  std::vector<uint32_t>::const_iterator it = vDataWords.begin();
  std::vector<uint32_t>::const_iterator end = vDataWords.end();
  for (; it != end; ++it) {
    theROD->push_back(*it);
    ATH_MSG_VERBOSE("     0x" << MSG::hex << std::setw(8) << (*it));
  }

  // Now fill full event
  ATH_MSG_DEBUG("Now filling the event with the L1Topo fragment");
  m_fea.fill(re, msg());

  return StatusCode::SUCCESS;
}

StatusCode L1TopoByteStreamTool::convert(const std::string& sgKey,
                                         L1TopoRDOCollection* result) {
  std::vector<uint32_t> vID;
  vID = sourceIDs();

  //
  // Get the ROB fragment:
  //
  IROBDataProviderSvc::VROBFRAG robFrags;
  m_robDataProvider->getROBData(vID, robFrags);

  if (robFrags.size() == 0) {
    ATH_MSG_WARNING("No ROB fragments found");
    return StatusCode::SUCCESS;
  }

  for (auto it : robFrags) {
    L1TopoRDO* rdo = nullptr;
    StatusCode sc = convert(ROBData(it).getROBFragment(), rdo);
    if (sc.isFailure()) {
      ATH_MSG_ERROR(" Failed to create Objects: " << sgKey);
      return sc;
    } else {
      result->push_back(rdo);  // append a copy of the pointer to the
                               // collection, i.e. takes ownership of the new
                               // L1TopoRDO.
    }
  }
  return StatusCode::SUCCESS;
}

/**
 * Conversion from eformat::ROBFragment to RDO.
 * This is called from the L1TopoByteStreamCnv::createObj method.
 */
// change to VROBFRAG and use IROBDataProviderSvc::VROBFRAG::const_iterator to
// loop over them
StatusCode L1TopoByteStreamTool::convert(const ROBF* rob, L1TopoRDO*& result) {
  ATH_MSG_DEBUG("executing convert() from ROBFragment to RDO");

  const std::vector<uint32_t> l1TopoRodIds = sourceIDs();
  uint32_t rodId = rob->rob_source_id();

  ATH_MSG_DEBUG("expected ROD sub-detector ID: " << MSG::hex << l1TopoRodIds
                                                 << " ID found: " << MSG::hex
                                                 << rodId << MSG::dec);

  bool idMatch = false;
  for (auto id : l1TopoRodIds) {
    if (rodId == id) {
      idMatch = true;
    }
  }
  if (idMatch) {
    // Need to check S-link trailer for errors - this would be propagated to
    // status word, see below.
    // First use ROB & ROD error check methods
    bool error_rob(false);
    bool error_rod(false);
    try {
      if (rob->check_rob()) {
        ATH_MSG_VERBOSE("ROB fragment checked ok");
      }
    } catch (std::exception const& ex) {
      ATH_MSG_WARNING("ROB fragment not valid: " << ex.what());
      error_rob = true;
    }
    try {
      if (rob->check_rod()) {
        ATH_MSG_VERBOSE("ROD fragment checked ok");
      }
    } catch (std::exception const& ex) {
      ATH_MSG_WARNING("ROD fragment not valid: " << ex.what());
      error_rod = true;
    }

    // Print some header info
    ATH_MSG_VERBOSE(
        MSG::hex << " \n"
                 << "  rod_version           0x" << rob->rod_version() << " \n"
                 << "  rod_run_no            0x" << MSG::dec
                 << rob->rod_run_no() << " \n"
                 << "  rod_lvl1_id           0x" << MSG::hex
                 << rob->rod_lvl1_id() << " \n"
                 << "  rod_bc_id             0x" << rob->rod_bc_id() << " \n"
                 << "  rod_lvl1_trigger_type 0x" << rob->rod_lvl1_trigger_type()
                 << " \n"
                 << "  nchildren             0x" << rob->nchildren() << " \n"
                 << MSG::dec);

    // print and check status words
    OFFLINE_FRAGMENTS_NAMESPACE::PointerType it_status = rob->rod_status();
    const uint32_t nstatus = rob->rod_nstatus();
    ATH_MSG_VERBOSE("Number of status words: " << nstatus);
    std::vector<uint32_t> vStatusWords;
    vStatusWords.reserve(nstatus);
    ATH_MSG_VERBOSE("Dumping ROD status words:");
    for (uint32_t i = 0; i < nstatus; ++i, ++it_status) {
      vStatusWords.push_back(static_cast<uint32_t>(*it_status));
      ATH_MSG_VERBOSE("     0x" << MSG::hex << std::setfill('0') << std::setw(8)
                                << *it_status << MSG::dec);
    }
    // for definition of first status word see:
    // - bits 00-15: https://edms.cern.ch/document/445840/5.0a section 5.8
    // - bits 16-31: bits 16-31:
    // https://twiki.cern.ch/twiki/bin/viewauth/Atlas/ROBINFragmentErrors
    // Can do something more specific eventually.
    bool error_status(false);
    if (vStatusWords.size() == 0) {
      ATH_MSG_WARNING("ROD has no status word");
    }
    if (vStatusWords.size() > 0 && vStatusWords.at(0) != 0) {
      ATH_MSG_WARNING("Non-zero first status word, payload may not be valid");
      error_status = true;
    }

    // print and interpret data words and save them for RDO
    OFFLINE_FRAGMENTS_NAMESPACE::PointerType it_data = rob->rod_data();
    const uint32_t ndata = rob->rod_ndata();
    ATH_MSG_VERBOSE("Number of data words: " << ndata);

    ATH_MSG_VERBOSE("Dumping L1Topo data words:");
    std::vector<uint32_t> vDataWords;
    vDataWords.reserve(ndata);
    for (uint32_t i = 0; i < ndata; ++i, ++it_data) {
      vDataWords.push_back(static_cast<uint32_t>(*it_data));
      ATH_MSG_VERBOSE("     0x" << MSG::hex << std::setfill('0') << std::setw(8)
                                << *it_data << MSG::dec);
    }

    // create L1Topo RDO
    result = new L1TopoRDO();
    result->setDataWords(std::move(vDataWords));
    result->setStatusWords(std::move(vStatusWords));
    if (error_status) {
      result->setError(L1Topo::Error::SLINK_STATUS_ERROR);
    }
    if (error_rob) {
      result->setError(L1Topo::Error::ROB_ERROR);
    }
    if (error_rod) {
      result->setError(L1Topo::Error::ROD_ERROR);
    }
    result->setSourceID(rodId);

    return StatusCode::SUCCESS;
  }

  ATH_MSG_ERROR("Wrong ROD ID found in the L1Topo ROB fragment!");
  return StatusCode::FAILURE;
}
