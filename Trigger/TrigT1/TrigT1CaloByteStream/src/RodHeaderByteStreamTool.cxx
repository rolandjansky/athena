/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    m_errorTool("LVL1BS::L1CaloErrorByteStreamTool/L1CaloErrorByteStreamTool"),
    m_srcIdMap(0)
{
  declareInterface<RodHeaderByteStreamTool>(this);

  declareProperty("ErrorTool", m_errorTool,
                  "Tool to collect errors for monitoring");
  declareProperty("ROBSourceIDs",        m_sourceIDs,
                  "ROB fragment source identifiers - All except RoIB");
  declareProperty("ROBSourceIDsPP",      m_sourceIDsPP,
                  "ROB fragment source identifiers - PP only");
  declareProperty("ROBSourceIDsCP",      m_sourceIDsCP,
                  "ROB fragment source identifiers - CP DAQ only");
  declareProperty("ROBSourceIDsJEP",     m_sourceIDsJEP,
                  "ROB fragment source identifiers - JEP DAQ only");
  declareProperty("ROBSourceIDsCPRoI",   m_sourceIDsCPRoI,
                  "ROB fragment source identifiers - CP RoI only");
  declareProperty("ROBSourceIDsJEPRoI",  m_sourceIDsJEPRoI,
                  "ROB fragment source identifiers - JEP RoI only");
  declareProperty("ROBSourceIDsCPRoIB",  m_sourceIDsCPRoIB,
                  "ROB fragment source identifiers - CP RoIB only");
  declareProperty("ROBSourceIDsJEPRoIB", m_sourceIDsJEPRoIB,
                  "ROB fragment source identifiers - JEP RoIB only");

}

// Destructor

RodHeaderByteStreamTool::~RodHeaderByteStreamTool()
{
}

// Initialize


StatusCode RodHeaderByteStreamTool::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endmsg;

  StatusCode sc = m_errorTool.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to retrieve tool " << m_errorTool << endmsg;
    return sc;
  } else msg(MSG::INFO) << "Retrieved tool " << m_errorTool << endmsg;

  m_srcIdMap = new L1CaloSrcIdMap();
  return StatusCode::SUCCESS;
}

// Finalize

StatusCode RodHeaderByteStreamTool::finalize()
{
  delete m_srcIdMap;
  return StatusCode::SUCCESS;
}

// Conversion bytestream to RODHeaders

// Conversion bytestream to CMX-CP TOBs
StatusCode RodHeaderByteStreamTool::convert(
  const std::string& sgKey,
  DataVector<LVL1::RODHeader>* const rhCollection)
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
  DataVector<LVL1::RODHeader>* const rhCollection)
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
  const std::string& sgKey)
{
  const bool pp      = isAppended(sgKey, "PP");
  const bool cp      = isAppended(sgKey, "CP");
  const bool jep     = isAppended(sgKey, "JEP");
  const bool cpRoi   = isAppended(sgKey, "CPRoI");
  const bool jepRoi  = isAppended(sgKey, "JEPRoI");
  const bool cpRoib  = isAppended(sgKey, "CPRoIB");
  const bool jepRoib = isAppended(sgKey, "JEPRoIB");
  const bool all = !(pp || cp || jep || cpRoi || jepRoi || cpRoib || jepRoib);
  if (all     && !m_sourceIDs.empty())        return m_sourceIDs;
  if (pp      && !m_sourceIDsPP.empty())      return m_sourceIDsPP;
  if (cp      && !m_sourceIDsCP.empty())      return m_sourceIDsCP;
  if (jep     && !m_sourceIDsJEP.empty())     return m_sourceIDsJEP;
  if (cpRoi   && !m_sourceIDsCPRoI.empty())   return m_sourceIDsCPRoI;
  if (jepRoi  && !m_sourceIDsJEPRoI.empty())  return m_sourceIDsJEPRoI;
  if (cpRoib  && !m_sourceIDsCPRoIB.empty())  return m_sourceIDsCPRoIB;
  if (jepRoib && !m_sourceIDsJEPRoIB.empty()) return m_sourceIDsJEPRoIB;
  // PP
  if (all || pp) {
    std::vector<int> slinks(4);
    for (int i = 1; i < 4; ++i) slinks[i] = i;
    fillRobIds(all, 8, 0, slinks, 0, eformat::TDAQ_CALO_PREPROC,
               m_sourceIDsPP);
    if (pp) return m_sourceIDsPP;
  }
  // CP
  if (all || cp) {
    std::vector<int> slinks(2);
    slinks[1] = 2;
    fillRobIds(all, 4, 8, slinks, 0, eformat::TDAQ_CALO_CLUSTER_PROC_DAQ,
               m_sourceIDsCP);
    if (cp) return m_sourceIDsCP;
  }
  // CP RoI
  if (all || cpRoi) {
    const std::vector<int> slinks(1);
    fillRobIds(all, 4, 8, slinks, 1, eformat::TDAQ_CALO_CLUSTER_PROC_ROI,
               m_sourceIDsCPRoI);
    if (cpRoi) return m_sourceIDsCPRoI;
  }
  // JEP
  if (all || jep) {
    std::vector<int> slinks(4);
    for (int i = 1; i < 4; ++i) slinks[i] = i;
    fillRobIds(all, 2, 12, slinks, 0, eformat::TDAQ_CALO_JET_PROC_DAQ,
               m_sourceIDsJEP);
    if (jep) return m_sourceIDsJEP;
  }
  // JEP RoI
  if (all || jepRoi) {
    const std::vector<int> slinks(1);
    fillRobIds(all, 2, 12, slinks, 1, eformat::TDAQ_CALO_JET_PROC_ROI,
               m_sourceIDsJEPRoI);
    if (jepRoi) return m_sourceIDsJEPRoI;
  }
  // Don't include RoIBs (LVL2) in complete set
  // CP RoIB
  if (cpRoib) {
    const std::vector<int> slinks(1, 2);
    fillRobIds(false, 4, 8, slinks, 1, eformat::TDAQ_CALO_CLUSTER_PROC_ROI,
               m_sourceIDsCPRoIB);
    return m_sourceIDsCPRoIB;
  }
  // JEP RoIB
  if (jepRoib) {
    const std::vector<int> slinks(1, 2);
    fillRobIds(false, 2, 12, slinks, 1, eformat::TDAQ_CALO_JET_PROC_ROI,
               m_sourceIDsJEPRoIB);
    return m_sourceIDsJEPRoIB;
  }
  return m_sourceIDs;
}

// Fill vector with ROB IDs for given sub-detector

void RodHeaderByteStreamTool::fillRobIds(const bool all, const int numCrates,
    const int crateOffset,
    const std::vector<int>& slinks,
    const int daqOrRoi,
    const eformat::SubDetector subdet,
    std::vector<uint32_t>& detSourceIDs)
{
  if (all && !detSourceIDs.empty()) {
    std::copy(detSourceIDs.begin(), detSourceIDs.end(),
              std::back_inserter(m_sourceIDs));
  } else {
    for (int crate = 0; crate < numCrates; ++crate) {
      const int numSlinks = slinks.size();
      for (int i = 0; i < numSlinks; ++i) {
        const uint32_t rodId = m_srcIdMap->getRodID(crate + crateOffset,
                               slinks[i], daqOrRoi, subdet);
        const uint32_t robId = m_srcIdMap->getRobID(rodId);
        if (all) m_sourceIDs.push_back(robId);
        else     detSourceIDs.push_back(robId);
      }
    }
  }
}

// Return true if StoreGate key ends in given string

bool RodHeaderByteStreamTool::isAppended(const std::string& sgKey,
    const std::string& flag) const
{

  return ::hasEnding(sgKey, flag) || ::hasEnding(sgKey, flag + "Aux.");
}

} // end namespace
