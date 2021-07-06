/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include <algorithm>
#include <set>

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigT1CaloEvent/RODHeader.h"

#include "L1CaloErrorByteStreamTool.h"
#include "L1CaloSrcIdMap.h"
#include "L1CaloSubBlock.h"

#include "RodHeaderByteStreamTool.h"


namespace {
bool hasEnding(std::string const &fullString, std::string const &ending) {
  if (fullString.length() >= ending.length()) {
      return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
  } else {
      return false;
  }
}
}


namespace LVL1BS {

// Interface ID

static const InterfaceID IID_IRodHeaderByteStreamTool("RodHeaderByteStreamTool", 1, 1);

const InterfaceID& RodHeaderByteStreamTool::interfaceID()
{
  return IID_IRodHeaderByteStreamTool;
}

// Constructor

RodHeaderByteStreamTool::RodHeaderByteStreamTool(const std::string& type,
    const std::string& name,
    const IInterface*  parent)
  : AthAlgTool(type, name, parent),
    m_robDataProvider("ROBDataProviderSvc", name),
    m_errorTool("LVL1BS::L1CaloErrorByteStreamTool/L1CaloErrorByteStreamTool")
{
  declareInterface<RodHeaderByteStreamTool>(this);

  declareProperty("ErrorTool", m_errorTool,
                  "Tool to collect errors for monitoring");
  declareProperty("ROBSourceIDs",        m_sourceIDsProp,
                  "ROB fragment source identifiers - All except RoIB");
  declareProperty("ROBSourceIDsPP",      m_sourceIDsPPProp,
                  "ROB fragment source identifiers - PP only");
  declareProperty("ROBSourceIDsCP",      m_sourceIDsCPProp,
                  "ROB fragment source identifiers - CP DAQ only");
  declareProperty("ROBSourceIDsJEP",     m_sourceIDsJEPProp,
                  "ROB fragment source identifiers - JEP DAQ only");
  declareProperty("ROBSourceIDsCPRoI",   m_sourceIDsCPRoIProp,
                  "ROB fragment source identifiers - CP RoI only");
  declareProperty("ROBSourceIDsJEPRoI",  m_sourceIDsJEPRoIProp,
                  "ROB fragment source identifiers - JEP RoI only");
  declareProperty("ROBSourceIDsCPRoIB",  m_sourceIDsCPRoIBProp,
                  "ROB fragment source identifiers - CP RoIB only");
  declareProperty("ROBSourceIDsJEPRoIB", m_sourceIDsJEPRoIBProp,
                  "ROB fragment source identifiers - JEP RoIB only");
}

// Destructor

RodHeaderByteStreamTool::~RodHeaderByteStreamTool()
{
}

// Initialize


StatusCode RodHeaderByteStreamTool::initialize()
{
  ATH_MSG_INFO( "Initializing " << name() );

  ATH_CHECK( m_errorTool.retrieve() );

  return StatusCode::SUCCESS;
}

// Finalize

StatusCode RodHeaderByteStreamTool::finalize()
{
  return StatusCode::SUCCESS;
}

// Conversion bytestream to RODHeaders

// Conversion bytestream to CMX-CP TOBs
StatusCode RodHeaderByteStreamTool::convert(
  const std::string& sgKey,
  DataVector<LVL1::RODHeader>* const rhCollection) const
{
  const std::vector<uint32_t>& vID(sourceIDs(sgKey));
  // // get ROB fragments
  IROBDataProviderSvc::VROBFRAG robFrags;
  m_robDataProvider->getROBData(vID, robFrags, "RodHeaderByteStreamTool");
  ATH_MSG_DEBUG("Number of ROB fragments:" << robFrags.size());
  return convert(robFrags, rhCollection);
}

StatusCode RodHeaderByteStreamTool::convert(
  const IROBDataProviderSvc::VROBFRAG& robFrags,
  DataVector<LVL1::RODHeader>* const rhCollection) const
{
  const bool debug = msgLvl(MSG::DEBUG);
  if (debug) msg(MSG::DEBUG);

  // Loop over ROB fragments

  int robCount = 0;
  std::set<uint32_t> dupCheck;
  ROBIterator rob    = robFrags.begin();
  ROBIterator robEnd = robFrags.end();
  for (; rob != robEnd; ++rob) {

    if (debug) {
      ++robCount;
      msg() << "Treating ROB fragment " << robCount << endmsg;
    }

    // Skip fragments with ROB status errors

    uint32_t robid = (*rob)->source_id();
    if ((*rob)->nstatus() > 0) {
      ROBPointer robData;
      (*rob)->status(robData);
      if (*robData != 0) {
        m_errorTool->robError(robid, *robData);
        if (debug) msg() << "ROB status error - skipping fragment" << endmsg;
        continue;
      }
    }

    // Skip duplicate fragments

    if (!dupCheck.insert(robid).second) {
      m_errorTool->rodError(robid, L1CaloSubBlock::ERROR_DUPLICATE_ROB);
      if (debug) msg() << "Skipping duplicate ROB fragment" << endmsg;
      continue;
    }

    // Unpack ROD header info

    const uint32_t version  = (*rob)->rod_version();
    const uint32_t sourceId = (*rob)->rod_source_id();
    const uint32_t run      = (*rob)->rod_run_no();
    const uint32_t lvl1Id   = (*rob)->rod_lvl1_id();
    const uint32_t bcId     = (*rob)->rod_bc_id();
    const uint32_t trigType = (*rob)->rod_lvl1_trigger_type();
    const uint32_t detType  = (*rob)->rod_detev_type();
    const uint32_t nData    = (*rob)->rod_ndata();

    // Unpack status words

    std::vector<uint32_t> statusWords;
    unsigned int nstatus = (*rob)->rod_nstatus();
    if (nstatus <= 2) {
      RODPointer status;
      RODPointer statusEnd;
      (*rob)->rod_status(status);
      statusEnd = status + nstatus;
      for (; status != statusEnd; ++status) statusWords.push_back(*status);
    } else { // Likely corruption
      m_errorTool->rodError(robid, L1CaloSubBlock::ERROR_ROD_NSTATUS);
      continue;
    }

    // Save

    rhCollection->push_back(new LVL1::RODHeader(version, sourceId, run, lvl1Id,
                            bcId, trigType, detType, statusWords, nData));
    if (debug) {
      msg() << MSG::hex
            << "ROD Header version/sourceId/run/lvl1Id/bcId/trigType/detType/nData: "
            << version << "/" << sourceId << "/" << run << "/" << lvl1Id << "/"
            << bcId << "/" << trigType << "/" << detType << "/" << nData
            << endmsg << "ROD Status Words:";
      std::vector<uint32_t>::const_iterator pos  = statusWords.begin();
      std::vector<uint32_t>::const_iterator pose = statusWords.end();
      for (; pos != pose; ++pos) msg() << " " << *pos;
      msg() << MSG::dec << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

// Return reference to vector with all possible Source Identifiers

const std::vector<uint32_t>& RodHeaderByteStreamTool::sourceIDs(
  const std::string& sgKey) const
{
  if (isAppended(sgKey, "PP")) {
    static const std::vector<int> slinksPP { 0, 1, 2, 3 };
    static const std::vector<uint32_t> sourceIDsPP =
      makeRobIds(8, 0, slinksPP, 0, eformat::TDAQ_CALO_PREPROC,
                 m_sourceIDsPPProp);
    return sourceIDsPP;
  }

  if (isAppended(sgKey, "CP")) {
    static const std::vector<int> slinksCP { 0, 2 };
    static const std::vector<uint32_t> sourceIDsCP =
      makeRobIds(4, 8, slinksCP, 0, eformat::TDAQ_CALO_CLUSTER_PROC_DAQ,
                 m_sourceIDsCPProp);
    return sourceIDsCP;
  }

  if (isAppended(sgKey, "CPRoI")) {
    static const std::vector<int> slinksCPRoI { 0 };
    static const std::vector<uint32_t> sourceIDsCPRoI =
      makeRobIds(4, 8, slinksCPRoI, 1, eformat::TDAQ_CALO_CLUSTER_PROC_ROI,
                 m_sourceIDsCPRoIProp);
    return sourceIDsCPRoI;
  }

  if (isAppended(sgKey, "JEP")) {
    static const std::vector<int> slinksJEP { 0, 1, 2, 3 };
    static const std::vector<uint32_t> sourceIDsJEP =
      makeRobIds(2, 12, slinksJEP, 0, eformat::TDAQ_CALO_JET_PROC_DAQ,
                 m_sourceIDsJEPProp);
    return sourceIDsJEP;
  }

  if (isAppended(sgKey, "JEPRoI")) {
    static const std::vector<int> slinksJEPRoI { 0 };
    static const std::vector<uint32_t> sourceIDsJEPRoI =
      makeRobIds(2, 12, slinksJEPRoI, 1, eformat::TDAQ_CALO_JET_PROC_ROI,
                 m_sourceIDsJEPRoIProp);
    return sourceIDsJEPRoI;
  }

  if (isAppended(sgKey, "CPRoIB")) {
    static const std::vector<int> slinksCPRoIB { 2 };
    static const std::vector<uint32_t> sourceIDsCPRoIB =
      makeRobIds(4, 8, slinksCPRoIB, 1, eformat::TDAQ_CALO_CLUSTER_PROC_ROI,
                 m_sourceIDsCPRoIBProp);
    return sourceIDsCPRoIB;
  }

  if (isAppended(sgKey, "JEPRoIB")) {
    static const std::vector<int> slinksJEPRoIB { 2 };
    static const std::vector<uint32_t> sourceIDsJEPRoIB =
      makeRobIds(2, 12, slinksJEPRoIB, 1, eformat::TDAQ_CALO_JET_PROC_ROI,
                 m_sourceIDsJEPRoIBProp);
    return sourceIDsJEPRoIB;
  }

  // all
  static const std::vector<uint32_t> sourceIDs = makeAllRobIds();
  return sourceIDs;
}

// Fill vector with ROB IDs for given sub-detector

std::vector<uint32_t> RodHeaderByteStreamTool::makeAllRobIds() const
{
  std::vector<uint32_t> robIds;

  auto append = [&robIds] (const std::vector<uint32_t>& v)
                { robIds.insert (robIds.end(), v.begin(), v.end()); };

  append (sourceIDs ("PP"));
  append (sourceIDs ("CP"));
  append (sourceIDs ("CPRoI"));
  append (sourceIDs ("JEP"));
  append (sourceIDs ("JEPRoI"));

  // Don't include RoIBs (LVL2) in complete set
  return robIds;
}


std::vector<uint32_t> RodHeaderByteStreamTool::makeRobIds(const int numCrates,
    const int crateOffset,
    const std::vector<int>& slinks,
    const int daqOrRoi,
    const eformat::SubDetector subdet,
    const std::vector<uint32_t>& prop) const
{
  std::vector<uint32_t> robs;

  if (!prop.empty()) {
    robs = prop;
  }
  else {
    for (int crate = 0; crate < numCrates; ++crate) {
      const int numSlinks = slinks.size();
      for (int i = 0; i < numSlinks; ++i) {
        const uint32_t rodId = m_srcIdMap.getRodID(crate + crateOffset,
                                                    slinks[i], daqOrRoi, subdet);
        const uint32_t robId = m_srcIdMap.getRobID(rodId);
        robs.push_back (robId);
      }
    }
  }
  return robs;
}

// Return true if StoreGate key ends in given string

bool RodHeaderByteStreamTool::isAppended(const std::string& sgKey,
    const std::string& flag) const
{

  return ::hasEnding(sgKey, flag) || ::hasEnding(sgKey, flag + "Aux.");
}

} // end namespace
