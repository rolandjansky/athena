/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include <numeric>
#include <set>
#include <utility>

#include "ByteStreamCnvSvcBase/FullEventAssembler.h"

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigT1CaloEvent/CPMRoI.h"

#include "CpmRoiSubBlockV1.h"
#include "L1CaloErrorByteStreamTool.h"
#include "L1CaloUserHeader.h"

#include "CpmRoiByteStreamV1Tool.h"

namespace LVL1BS {

// Interface ID

static const InterfaceID IID_ICpmRoiByteStreamV1Tool("CpmRoiByteStreamV1Tool",
                                                                        1, 1);

const InterfaceID& CpmRoiByteStreamV1Tool::interfaceID()
{
  return IID_ICpmRoiByteStreamV1Tool;
}

// Constructor

CpmRoiByteStreamV1Tool::CpmRoiByteStreamV1Tool(const std::string& type,
                                               const std::string& name,
                                               const IInterface*  parent)
  : AthAlgTool(type, name, parent),
    m_errorTool("LVL1BS::L1CaloErrorByteStreamTool/L1CaloErrorByteStreamTool"),
    m_crates(4), m_modules(14),
    m_subDetector (eformat::TDAQ_CALO_CLUSTER_PROC_ROI)
{
  declareInterface<CpmRoiByteStreamV1Tool>(this);

  declareProperty("ErrorTool", m_errorTool,
                  "Tool to collect errors for monitoring");
  declareProperty("CrateOffsetHw",  m_crateOffsetHw = 8,
                  "Offset of CP crate numbers in bytestream");
  declareProperty("CrateOffsetSw",  m_crateOffsetSw = 0,
                  "Offset of CP crate numbers in RDOs");

  // Properties for reading bytestream only
  declareProperty("ROBSourceIDs",       m_sourceIDsProp,
                  "ROB fragment source identifiers");
  declareProperty("ROBSourceIDsRoIB",   m_sourceIDsRoIBProp,
                  "ROB fragment source identifiers for RoIBs");

  // Properties for writing bytestream only
  declareProperty("DataVersion",    m_version       = 1,
                  "Format version number in sub-block header");
  declareProperty("DataFormat",     m_dataFormat    = 1,
                  "Format identifier (0-1) in sub-block header");
  declareProperty("SlinksPerCrate", m_slinks        = 1,
                  "The number of S-Links per crate");
  declareProperty("CrateMin",       m_crateMin = 0,
                  "Minimum crate number, allows partial output");
  declareProperty("CrateMax",       m_crateMax = m_crates-1,
                  "Maximum crate number, allows partial output");

}

// Destructor

CpmRoiByteStreamV1Tool::~CpmRoiByteStreamV1Tool()
{
}

// Initialize


StatusCode CpmRoiByteStreamV1Tool::initialize()
{
  ATH_MSG_INFO ("Initializing " << name());

  ATH_CHECK( m_errorTool.retrieve() );
  ATH_CHECK( m_byteStreamCnvSvc.retrieve() );

  return StatusCode::SUCCESS;
}

// Convert ROB fragments to CPM RoIs

StatusCode CpmRoiByteStreamV1Tool::convert(
                            const IROBDataProviderSvc::VROBFRAG& robFrags,
                            DataVector<LVL1::CPMRoI>* const roiCollection) const
{
  const bool debug = msgLvl(MSG::DEBUG);
  if (debug) msg(MSG::DEBUG);

  // Sub-block for neutral format
  CpmRoiSubBlockV1 subBlock;

  // Loop over ROB fragments

  int robCount = 0;
  std::set<uint32_t> dupCheck;
  std::set<uint32_t> dupRoiCheck;
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

    // Unpack ROD data (slinks)

    RODPointer payloadBeg;
    RODPointer payload;
    RODPointer payloadEnd;
    (*rob)->rod_data(payloadBeg);
    payloadEnd = payloadBeg + (*rob)->rod_ndata();
    payload = payloadBeg;
    if (payload == payloadEnd) {
      if (debug) msg() << "ROB fragment empty" << endmsg;
      continue;
    }

    // Check identifier
    const uint32_t sourceID = (*rob)->rod_source_id();
    if (m_srcIdMap.getRobID(sourceID) != robid           ||
        m_srcIdMap.subDet(sourceID)   != m_subDetector   ||
        m_srcIdMap.daqOrRoi(sourceID) != 1               ||
       (m_srcIdMap.slink(sourceID) != 0 && m_srcIdMap.slink(sourceID) != 2) ||
	m_srcIdMap.crate(sourceID)    <  m_crateOffsetHw ||
	m_srcIdMap.crate(sourceID)    >= m_crateOffsetHw + m_crates) {
      m_errorTool->rodError(robid, L1CaloSubBlock::ERROR_ROD_ID);
      if (debug) {
        msg() << "Wrong source identifier in data: "
              << MSG::hex << sourceID << MSG::dec << endmsg;
      }
      continue;
    }

    // Check minor version
    const int minorVersion = (*rob)->rod_version() & 0xffff;
    if (minorVersion > m_srcIdMap.minorVersionPreLS1()) {
      if (debug) msg() << "Skipping post-LS1 data" << endmsg;
      continue;
    }
    const int rodCrate = m_srcIdMap.crate(sourceID);
    if (debug) {
      msg() << "Treating crate " << rodCrate 
            << " slink " << m_srcIdMap.slink(sourceID) << endmsg;
    }

    // First word may be User Header
    if (L1CaloUserHeader::isValid(*payload)) {
      L1CaloUserHeader userHeader(*payload);
      userHeader.setVersion(minorVersion);
      const int headerWords = userHeader.words();
      if (headerWords != 1 ) {
        m_errorTool->rodError(robid, L1CaloSubBlock::ERROR_USER_HEADER);
        if (debug) msg() << "Unexpected number of user header words: "
                         << headerWords << endmsg;
        continue;
      }
      for (int i = 0; i < headerWords; ++i) ++payload;
    }

    // Loop over sub-blocks if there are any

    unsigned int rodErr = L1CaloSubBlock::ERROR_NONE;
    while (payload != payloadEnd) {
      
      if (L1CaloSubBlock::wordType(*payload) == L1CaloSubBlock::HEADER) {
        subBlock.clear();
        payload = subBlock.read(payload, payloadEnd);
        if (debug) {
          msg() << "CPM RoI sub-block: Crate " << subBlock.crate()
                << "  Module " << subBlock.module() << endmsg;
        }
        // Unpack sub-block
        if (subBlock.dataWords() && !subBlock.unpack()) {
          if (debug) {
            std::string errMsg(subBlock.unpackErrorMsg());
	    msg() << "CPM RoI sub-block unpacking failed: " << errMsg << endmsg;
	  }
	  rodErr = subBlock.unpackErrorCode();
	  break;
        }
	const int numChips = 8;
	const int numLocs  = 2;
	for (int chip = 0; chip < numChips; ++chip) {
	  for (int loc = 0; loc < numLocs; ++loc) {
	    const LVL1::CPMRoI roi = subBlock.roi(chip, loc);
            if (roi.hits() || roi.error()) {
              roiCollection->push_back(new LVL1::CPMRoI(roi));
            }
          }
        }
      } else {
        // Just RoI word
	LVL1::CPMRoI roi;
	if (roi.setRoiWord(*payload)) {
          if (roi.crate() != rodCrate - m_crateOffsetHw) {
	    if (debug) msg() << "Inconsistent RoI crate number: "
	                     << roi.crate() << endmsg;
            rodErr = L1CaloSubBlock::ERROR_CRATE_NUMBER;
	    break;
          }
	  if (roi.cpm() == 0 || roi.cpm() > m_modules) {
	    if (debug) msg() << "Invalid CPM number: "
	                     << roi.cpm() << endmsg;
            rodErr = L1CaloSubBlock::ERROR_MODULE_NUMBER;
	    break;
          }
	  const uint32_t location = (*payload) & 0xfffc0000;
          if (dupRoiCheck.insert(location).second) {
	    if (roi.hits() || roi.error()) {
	      roiCollection->push_back(new LVL1::CPMRoI(*payload));
	    }
	  } else {
	    if (debug) msg() << "Duplicate RoI word "
	                     << MSG::hex << *payload << MSG::dec << endmsg;
            rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
	    break;
	  }
	} else {
	  if (debug) msg() << "Invalid RoI word "
	                   << MSG::hex << *payload << MSG::dec << endmsg;
          rodErr = L1CaloSubBlock::ERROR_ROI_TYPE;
	  break;
        }
	++payload;
      }
    }
    if (rodErr != L1CaloSubBlock::ERROR_NONE)
                                        m_errorTool->rodError(robid, rodErr);
  }

  return StatusCode::SUCCESS;
}

// Convert CPM RoI to bytestream

StatusCode CpmRoiByteStreamV1Tool::convert(
           const DataVector<LVL1::CPMRoI>* const roiCollection) const
{
  const bool debug = msgLvl(MSG::DEBUG);
  if (debug) msg(MSG::DEBUG);

  // Get the event assembler

  FullEventAssembler<L1CaloSrcIdMap>* fea = nullptr;
  ATH_CHECK( m_byteStreamCnvSvc->getFullEventAssembler (fea,
                                                        "CpmRoiByteStreamV1") );
  uint16_t minorVersion = m_srcIdMap.minorVersionPreLS1();
  fea->setRodMinorVersion(minorVersion);

  // Pointer to ROD data vector

  FullEventAssembler<L1CaloSrcIdMap>::RODDATA* theROD = 0;

  // Sub-block for neutral format
  CpmRoiSubBlockV1 subBlock;

  // Set up the container map

  // CPM RoI map
  CpmRoiMap roiMap;
  setupCpmRoiMap(roiCollection, roiMap);
  CpmRoiMap::const_iterator mapIter    = roiMap.begin();
  CpmRoiMap::const_iterator mapIterEnd = roiMap.end();

  // Loop over data

  const bool neutralFormat = m_dataFormat == L1CaloSubBlock::NEUTRAL;
  const int  modulesPerSlink = m_modules / m_slinks;
  for (int crate = m_crateMin; crate <= m_crateMax; ++crate) {
    const int hwCrate = crate + m_crateOffsetHw;

    // CPM modules are numbered 1 to m_modules
    for (int module=1; module <= m_modules; ++module) {
      const int mod = module - 1;

      // Pack required number of modules per slink

      if (mod%modulesPerSlink == 0) {
	const int daqOrRoi = 1;
	const int slink = (m_slinks == 2) ? 2*(mod/modulesPerSlink)
	                                  : mod/modulesPerSlink;
        if (debug) {
          msg() << "Treating crate " << hwCrate
                << " slink " << slink << endmsg
	        << "Data Version/Format: " << m_version
	        << " " << m_dataFormat << endmsg;
        }
	const uint32_t rodIdCpm = m_srcIdMap.getRodID(hwCrate, slink, daqOrRoi,
	                                                        m_subDetector);
	theROD = fea->getRodData(rodIdCpm);
	if (neutralFormat) {
          const L1CaloUserHeader userHeader;
	  theROD->push_back(userHeader.header());
        }
      }
      if (debug) msg() << "Module " << module << endmsg;

      // Create a sub-block (Neutral format only)

      if (neutralFormat) {
        subBlock.clear();
	subBlock.setRoiHeader(m_version, hwCrate, module);
      }

      // Find CPM RoIs for this module

      for (; mapIter != mapIterEnd; ++mapIter) {
        const LVL1::CPMRoI* const roi = mapIter->second;
	if (roi->crate() < crate)  continue;
	if (roi->crate() > crate)  break;
	if (roi->cpm()   < module) continue;
	if (roi->cpm()   > module) break;
	if (roi->hits() || roi->error()) {
	  if (neutralFormat) subBlock.fillRoi(*roi);
          else theROD->push_back(roi->roiWord());
        }
      }
      
      // Pack and write the sub-block

      if (neutralFormat) {
        if ( !subBlock.pack()) {
          msg(MSG::ERROR) << "CPMRoI sub-block packing failed" << endmsg;
 	  return StatusCode::FAILURE;
        }
	if (debug) {
	  msg() << "CPMRoI sub-block data words: "
	        << subBlock.dataWords() << endmsg;
        }
        subBlock.write(theROD);
      }
    }
  }

  return StatusCode::SUCCESS;
}

// Return reference to vector with all possible Source Identifiers

std::vector<uint32_t> CpmRoiByteStreamV1Tool::makeSourceIDs (bool roiDaq) const
{
  std::vector<uint32_t> sourceIDs;

  if (roiDaq) {
    sourceIDs = m_sourceIDsProp;
  }
  else {
    sourceIDs = m_sourceIDsRoIBProp;
  }

  if (sourceIDs.empty()) {
        const int maxCrates = m_crates + m_crateOffsetHw;
        const int maxSlinks = m_srcIdMap.maxSlinks();
        for (int hwCrate = m_crateOffsetHw; hwCrate < maxCrates; ++hwCrate)
        {
            for (int slink = 0; slink < maxSlinks; ++slink)
            {
                const int daqOrRoi = 1;
                const uint32_t rodId = m_srcIdMap.getRodID(hwCrate, slink, daqOrRoi,
                                       m_subDetector);
                const uint32_t robId = m_srcIdMap.getRobID(rodId);
                if (roiDaq)
                {
                    if (slink < 2) sourceIDs.push_back(robId);
                }
                else if (slink >= 2) sourceIDs.push_back(robId);
            }
        }

  }
  return sourceIDs;
}

const std::vector<uint32_t>& CpmRoiByteStreamV1Tool::sourceIDs(
                                                      const std::string& sgKey) const
{
  const std::string flag("RoIB");
  const std::string::size_type pos = sgKey.find(flag);
  const bool roiDaq =
           (pos == std::string::npos || pos != sgKey.length() - flag.length());

  if (roiDaq) {
    static const std::vector<uint32_t> sourceIDs = makeSourceIDs(roiDaq);
    return sourceIDs;
  }
  else {
    static const std::vector<uint32_t> sourceIDsRoIB = makeSourceIDs(roiDaq);
    return sourceIDsRoIB;
  }
}

// Set up CPM RoI map

void CpmRoiByteStreamV1Tool::setupCpmRoiMap(const CpmRoiCollection*
                                                          const roiCollection,
                                            CpmRoiMap& roiMap) const
{
  roiMap.clear();
  if (roiCollection) {
    CpmRoiCollection::const_iterator pos  = roiCollection->begin();
    CpmRoiCollection::const_iterator pose = roiCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CPMRoI* const roi = *pos;
      const uint32_t key = roi->roiWord();
      roiMap.insert(std::make_pair(key, roi));
    }
  }
}

} // end namespace
