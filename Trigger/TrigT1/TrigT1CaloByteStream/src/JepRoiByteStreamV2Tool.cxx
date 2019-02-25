/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <numeric>
#include <set>
#include <utility>

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "ByteStreamCnvSvcBase/FullEventAssembler.h"

#include "TrigT1CaloEvent/CMXEtSums.h"
#include "TrigT1CaloEvent/CMXRoI.h"
#include "TrigT1CaloEvent/JEMTobRoI.h"
#include "TrigT1CaloEvent/JEPRoIBSCollectionV2.h"

#include "CmxSubBlock.h"
#include "JemRoiSubBlockV2.h"
#include "L1CaloErrorByteStreamTool.h"
#include "L1CaloSrcIdMap.h"
#include "L1CaloSubBlock.h"
#include "L1CaloUserHeader.h"

#include "JepRoiByteStreamV2Tool.h"

namespace LVL1BS {

// Interface ID

static const InterfaceID IID_IJepRoiByteStreamV2Tool("JepRoiByteStreamV2Tool",
    1, 1);

const InterfaceID& JepRoiByteStreamV2Tool::interfaceID()
{
  return IID_IJepRoiByteStreamV2Tool;
}

// Constructor

JepRoiByteStreamV2Tool::JepRoiByteStreamV2Tool(const std::string& type,
    const std::string& name,
    const IInterface*  parent)
  : AthAlgTool(type, name, parent),
    m_robDataProvider("ROBDataProviderSvc", name),
    m_errorTool("LVL1BS::L1CaloErrorByteStreamTool/L1CaloErrorByteStreamTool"),
    m_crates(2), m_modules(16), m_frames(8), m_maxRoiWords(6),
    m_srcIdMap(0), m_subBlock(0), m_rodStatus(0), m_fea(0)
{
  declareInterface<JepRoiByteStreamV2Tool>(this);

  declareProperty("ErrorTool", m_errorTool,
                  "Tool to collect errors for monitoring");
  declareProperty("CrateOffsetHw",  m_crateOffsetHw = 12,
                  "Offset of JEP crate numbers in bytestream");
  declareProperty("CrateOffsetSw",  m_crateOffsetSw = 0,
                  "Offset of JEP crate numbers in RDOs");

  // Properties for reading bytestream only
  declareProperty("ROBSourceIDs",       m_sourceIDs,
                  "ROB fragment source identifiers");
  declareProperty("ROBSourceIDsRoIB",   m_sourceIDsRoIB,
                  "ROB fragment source identifiers");

  // Properties for writing bytestream only
  declareProperty("DataVersion",    m_version       = 2,                   //<<== CHECK
                  "Format version number in sub-block header");
  declareProperty("DataFormat",     m_dataFormat    = 1,
                  "Format identifier (0-1) in sub-block header");
  declareProperty("SlinksPerCrate", m_slinks        = 1,
                  "The number of S-Links per crate");
  declareProperty("CrateMin",       m_crateMin = 0,
                  "Minimum crate number, allows partial output");
  declareProperty("CrateMax",       m_crateMax = m_crates - 1,
                  "Maximum crate number, allows partial output");

}

// Destructor

JepRoiByteStreamV2Tool::~JepRoiByteStreamV2Tool()
{
}

// Initialize


StatusCode JepRoiByteStreamV2Tool::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endmsg;

  StatusCode sc = m_errorTool.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to retrieve tool " << m_errorTool << endmsg;
    return sc;
  } else msg(MSG::INFO) << "Retrieved tool " << m_errorTool << endmsg;

  m_subDetector = eformat::TDAQ_CALO_JET_PROC_ROI;
  m_srcIdMap    = new L1CaloSrcIdMap();
  m_subBlock    = new JemRoiSubBlockV2();
  m_rodStatus   = new std::vector<uint32_t>(2);
  m_fea         = new FullEventAssembler<L1CaloSrcIdMap>();
  return StatusCode::SUCCESS;
}

// Finalize

StatusCode JepRoiByteStreamV2Tool::finalize()
{
  delete m_fea;
  delete m_rodStatus;
  delete m_subBlock;
  delete m_srcIdMap;
  return StatusCode::SUCCESS;
}

// Conversion bytestream to JEM RoI

StatusCode JepRoiByteStreamV2Tool::convert(
    const std::string& sgKey,
    DataVector<LVL1::JEMTobRoI> *const collection)
{
 const std::vector<uint32_t>& vID(sourceIDs(sgKey));
  // // get ROB fragments
  IROBDataProviderSvc::VROBFRAG robFrags;
  m_robDataProvider->getROBData(vID, robFrags, "JepRoiByteStreamV2Tool");
  ATH_MSG_DEBUG("Number of ROB fragments:" << robFrags.size());
  return convert(robFrags, collection);
}

StatusCode JepRoiByteStreamV2Tool::convert(
  const IROBDataProviderSvc::VROBFRAG& robFrags,
  DataVector<LVL1::JEMTobRoI>* const jeCollection)
{
  m_jeCollection = jeCollection;
  return convertBs(robFrags, JEM_ROI);
}

// Conversion bytestream to CMX RoI

StatusCode JepRoiByteStreamV2Tool::convert(
    const std::string& sgKey,
    LVL1::CMXRoI* cmCollection)
{
  const std::vector<uint32_t>& vID(sourceIDs(sgKey));
  // // get ROB fragments
  IROBDataProviderSvc::VROBFRAG robFrags;
  m_robDataProvider->getROBData(vID, robFrags, "JepRoiByteStreamV2Tool");
  ATH_MSG_DEBUG("Number of ROB fragments:" << robFrags.size());
  return convert(robFrags, cmCollection);
}

StatusCode JepRoiByteStreamV2Tool::convert(
  const IROBDataProviderSvc::VROBFRAG& robFrags,
  LVL1::CMXRoI* cmCollection)
{
  m_cmCollection = cmCollection;
  return convertBs(robFrags, CMX_ROI);
}

// Conversion of JEP container to bytestream

StatusCode JepRoiByteStreamV2Tool::convert(
  const LVL1::JEPRoIBSCollectionV2* const jep,
  RawEventWrite* const re)
{
  const bool debug = msgLvl(MSG::DEBUG);
  if (debug) msg(MSG::DEBUG);

  // Clear the event assembler

  m_fea->clear();
  const uint16_t minorVersion = m_srcIdMap->minorVersion();
  m_fea->setRodMinorVersion(minorVersion);
  m_rodStatusMap.clear();

  // Pointer to ROD data vector

  FullEventAssembler<L1CaloSrcIdMap>::RODDATA* theROD = 0;

  // Set up the container maps

  const bool neutralFormat = m_dataFormat == L1CaloSubBlock::NEUTRAL;
  setupJemRoiMap(jep->JemRoi());
  JemRoiMap::const_iterator mapIter    = m_roiMap.begin();
  JemRoiMap::const_iterator mapIterEnd = m_roiMap.end();
  if (neutralFormat) {
    setupCmxEtMap(jep->CmxSums());
  }

  // Loop over JEM RoI data

  const int modulesPerSlink = m_modules / m_slinks;
  for (int crate = m_crateMin; crate <= m_crateMax; ++crate) {
    const int hwCrate = crate + m_crateOffsetHw;

    for (int module = 0; module < m_modules; ++module) {

      // Pack required number of modules per slink

      if (module % modulesPerSlink == 0) {
        const int daqOrRoi = 1;
        const int slink = module / modulesPerSlink;
        if (debug) {
          msg() << "Treating crate " << hwCrate
                << " slink " << slink << endmsg
                << "Data Version/Format: " << m_version
                << " " << m_dataFormat << endmsg;
        }
        const uint32_t rodIdJem = m_srcIdMap->getRodID(hwCrate, slink, daqOrRoi,
                                  m_subDetector);
        theROD = m_fea->getRodData(rodIdJem);
        if (neutralFormat) {
          const L1CaloUserHeader userHeader;
          theROD->push_back(userHeader.header());
        }
        m_rodStatusMap.insert(make_pair(rodIdJem, m_rodStatus));
      }
      if (debug) msg() << "JEM Module " << module << endmsg;
      if (!theROD) break; // for coverity, shouldn't happen

      // Create a sub-block (Neutral format only)

      if (neutralFormat) {
        m_subBlock->clear();
        m_subBlock->setRoiHeader(m_version, hwCrate, module);
      }

      // Find JEM RoIs for this module

      for (; mapIter != mapIterEnd; ++mapIter) {
        const LVL1::JEMTobRoI* const roi = mapIter->second;
        if (roi->crate() < crate)  continue;
        if (roi->crate() > crate)  break;
        if (roi->jem()   < module) continue;
        if (roi->jem()   > module) break;
        if (roi->energyLarge() || roi->energySmall()) {
          if (neutralFormat) m_subBlock->fillRoi(*roi);
          else theROD->push_back(roi->roiWord());
        }
      }

      // Pack and write the sub-block

      if (neutralFormat) {
        if ( !m_subBlock->pack()) {
          msg(MSG::ERROR) << "JEM RoI sub-block packing failed" << endmsg;
          return StatusCode::FAILURE;
        }
        if (debug) {
          msg() << "JEM RoI sub-block data words: "
                << m_subBlock->dataWords() << endmsg;
        }
        m_subBlock->write(theROD);
      }
    }
    if (!theROD) break; // for coverity, shouldn't happen

    // Append CMX RoIs to last S-Link of the system crate

    if (crate != m_crates - 1) continue;

    // Create sub-blocks for Neutral format

    if (neutralFormat) {
      const int timeslices = 1;
      const int slice = 0;

      // CMX-Energy

      CmxEnergySubBlock subBlock;
      const int cmxEnergyVersion = 3;                                      //<<== CHECK
      subBlock.setCmxHeader(cmxEnergyVersion, m_dataFormat, slice, hwCrate,
                            CmxSubBlock::SYSTEM, CmxSubBlock::CMX_ENERGY,
                            CmxSubBlock::LEFT, timeslices);
      int maxSource = static_cast<int>(LVL1::CMXEtSums::MAX_SOURCE);
      for (int source = 0; source < maxSource; ++source) {
        const LVL1::CMXEtSums* const sums = findCmxSums(crate, source);
        if ( sums ) {
          const unsigned int ex = sums->Ex();
          const unsigned int ey = sums->Ey();
          const unsigned int et = sums->Et();
          const int exErr = sums->ExError();
          const int eyErr = sums->EyError();
          const int etErr = sums->EtError();
          if (source < m_modules) {
            subBlock.setSubsums(slice, source, ex, ey, et, exErr, eyErr, etErr);
          } else {
            CmxEnergySubBlock::SourceType srcType = CmxEnergySubBlock::MAX_SOURCE_TYPE;
            CmxEnergySubBlock::SumType    sumType = CmxEnergySubBlock::MAX_SUM_TYPE;
            CmxEnergySubBlock::HitsType   hitType = CmxEnergySubBlock::MAX_HITS_TYPE;
            energySubBlockTypes(source, srcType, sumType, hitType);
            if (srcType != CmxEnergySubBlock::MAX_SOURCE_TYPE) {
              subBlock.setSubsums(slice, srcType, sumType, ex, ey, et,
                                  exErr, eyErr, etErr);
            } else if (hitType != CmxEnergySubBlock::MAX_HITS_TYPE) {
              subBlock.setEtHits(slice, hitType, sumType, et);
            }
          }
        }
      }
      if ( !subBlock.pack()) {
        msg(MSG::ERROR) << "CMX-Energy sub-block packing failed" << endmsg;
        return StatusCode::FAILURE;
      }
      if (debug) {
        msg() << "CMX-Energy sub-block data words: "
              << subBlock.dataWords() << endmsg;
      }
      subBlock.write(theROD);

    } else {

      // Standard format

      const LVL1::CMXRoI* const roi = jep->CmxRoi();
      if ( roi ) {
        // CMX-Energy RoIs are not zero-supressed unless all are zero
        for (int word = 0; word < m_maxRoiWords; ++word) {
          theROD->push_back(roi->roiWord(word));
        }
      }
    }
  }

  // Fill the raw event

  m_fea->fill(re, msg());

  // Set ROD status words

  //L1CaloRodStatus::setStatus(re, m_rodStatusMap, m_srcIdMap);

  return StatusCode::SUCCESS;
}

// Return reference to vector with all possible Source Identifiers

const std::vector<uint32_t>& JepRoiByteStreamV2Tool::sourceIDs(
  const std::string& sgKey)
{
  const std::string flag("RoIB");
  const std::string::size_type pos = sgKey.find(flag);
  const bool roiDaq =
    (pos == std::string::npos || pos != sgKey.length() - flag.length());
  const bool empty  = (roiDaq) ? m_sourceIDs.empty() : m_sourceIDsRoIB.empty();
  if (empty) {
    const int maxCrates = m_crates + m_crateOffsetHw;
    const int maxSlinks = m_srcIdMap->maxSlinks();
    for (int hwCrate = m_crateOffsetHw; hwCrate < maxCrates; ++hwCrate) {
      for (int slink = 0; slink < maxSlinks; ++slink) {
        const int daqOrRoi = 1;
        const uint32_t rodId = m_srcIdMap->getRodID(hwCrate, slink, daqOrRoi,
                               m_subDetector);
        const uint32_t robId = m_srcIdMap->getRobID(rodId);
        if (roiDaq) {
          if (slink < 2) m_sourceIDs.push_back(robId);
        } else if (slink >= 2) m_sourceIDsRoIB.push_back(robId);
      }
    }
  }
  return (roiDaq) ? m_sourceIDs : m_sourceIDsRoIB;
}

// Convert bytestream to given container type

StatusCode JepRoiByteStreamV2Tool::convertBs(
  const IROBDataProviderSvc::VROBFRAG& robFrags,
  const CollectionType collection)
{
  const bool debug = msgLvl(MSG::DEBUG);
  if (debug) msg(MSG::DEBUG);

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
    if (m_srcIdMap->getRobID(sourceID) != robid           ||
        m_srcIdMap->subDet(sourceID)   != m_subDetector   ||
        m_srcIdMap->daqOrRoi(sourceID) != 1               ||
        (m_srcIdMap->slink(sourceID) != 0 && m_srcIdMap->slink(sourceID) != 2) ||
        m_srcIdMap->crate(sourceID)    <  m_crateOffsetHw ||
        m_srcIdMap->crate(sourceID)    >= m_crateOffsetHw + m_crates) {
      m_errorTool->rodError(robid, L1CaloSubBlock::ERROR_ROD_ID);
      if (debug) {
        msg() << "Wrong source identifier in data: "
              << MSG::hex << sourceID << MSG::dec << endmsg;
      }
      continue;
    }

    // Check minor version
    const int minorVersion = (*rob)->rod_version() & 0xffff;
    if (minorVersion <= m_srcIdMap->minorVersionPreLS1()) {
      if (debug) msg() << "Skipping pre-LS1 data" << endmsg;
      continue;
    }
    const int rodCrate = m_srcIdMap->crate(sourceID);
    if (debug) {
      msg() << "Treating crate " << rodCrate
            << " slink " << m_srcIdMap->slink(sourceID) << endmsg;
    }

    // First word may be User Header
    if (L1CaloUserHeader::isValid(*payload)) {
      L1CaloUserHeader userHeader(*payload);
      userHeader.setVersion(minorVersion);
      const int headerWords = userHeader.words();
      if (headerWords != 1) {
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
        const int slice = 0;
        if (CmxSubBlock::cmxBlock(*payload)) {
          // CMXs
          if (CmxSubBlock::cmxType(*payload) == CmxSubBlock::CMX_ENERGY) {
            CmxEnergySubBlock subBlock;
            payload = subBlock.read(payload, payloadEnd);
            if (collection == CMX_ROI) {
              if (subBlock.dataWords() && !subBlock.unpack()) {
                if (debug) {
                  std::string errMsg(subBlock.unpackErrorMsg());
                  msg() << "CMX-Energy sub-block unpacking failed: "
                        << errMsg << endmsg;
                }
                rodErr = m_subBlock->unpackErrorCode();
                break;
              }
              const LVL1::CMXRoI roi(
                subBlock.energy(slice, CmxEnergySubBlock::TOTAL,
                                CmxEnergySubBlock::STANDARD,
                                CmxEnergySubBlock::ENERGY_EX),
                subBlock.energy(slice, CmxEnergySubBlock::TOTAL,
                                CmxEnergySubBlock::STANDARD,
                                CmxEnergySubBlock::ENERGY_EY),
                subBlock.energy(slice, CmxEnergySubBlock::TOTAL,
                                CmxEnergySubBlock::STANDARD,
                                CmxEnergySubBlock::ENERGY_ET),
                subBlock.error(slice, CmxEnergySubBlock::TOTAL,
                               CmxEnergySubBlock::STANDARD,
                               CmxEnergySubBlock::ENERGY_EX),
                subBlock.error(slice, CmxEnergySubBlock::TOTAL,
                               CmxEnergySubBlock::STANDARD,
                               CmxEnergySubBlock::ENERGY_EY),
                subBlock.error(slice, CmxEnergySubBlock::TOTAL,
                               CmxEnergySubBlock::STANDARD,
                               CmxEnergySubBlock::ENERGY_ET),
                subBlock.hits(slice, CmxEnergySubBlock::SUM_ET,
                              CmxEnergySubBlock::STANDARD),
                subBlock.hits(slice, CmxEnergySubBlock::MISSING_ET,
                              CmxEnergySubBlock::STANDARD),
                subBlock.hits(slice, CmxEnergySubBlock::MISSING_ET_SIG,
                              CmxEnergySubBlock::STANDARD),
                subBlock.energy(slice, CmxEnergySubBlock::TOTAL,
                                CmxEnergySubBlock::RESTRICTED_WEIGHTED,
                                CmxEnergySubBlock::ENERGY_EX),
                subBlock.energy(slice, CmxEnergySubBlock::TOTAL,
                                CmxEnergySubBlock::RESTRICTED_WEIGHTED,
                                CmxEnergySubBlock::ENERGY_EY),
                subBlock.energy(slice, CmxEnergySubBlock::TOTAL,
                                CmxEnergySubBlock::RESTRICTED_WEIGHTED,
                                CmxEnergySubBlock::ENERGY_ET),
                subBlock.error(slice, CmxEnergySubBlock::TOTAL,
                               CmxEnergySubBlock::RESTRICTED_WEIGHTED,
                               CmxEnergySubBlock::ENERGY_EX),
                subBlock.error(slice, CmxEnergySubBlock::TOTAL,
                               CmxEnergySubBlock::RESTRICTED_WEIGHTED,
                               CmxEnergySubBlock::ENERGY_EY),
                subBlock.error(slice, CmxEnergySubBlock::TOTAL,
                               CmxEnergySubBlock::RESTRICTED_WEIGHTED,
                               CmxEnergySubBlock::ENERGY_ET),
                subBlock.hits(slice, CmxEnergySubBlock::SUM_ET,
                              CmxEnergySubBlock::RESTRICTED_WEIGHTED),
                subBlock.hits(slice, CmxEnergySubBlock::MISSING_ET,
                              CmxEnergySubBlock::RESTRICTED_WEIGHTED));
              for (int word = 0; word < m_maxRoiWords; ++word) {
                m_cmCollection->setRoiWord(roi.roiWord(word));
              }
            }
          }
        } else {
          // JEM RoI
          JemRoiSubBlockV2 subBlock;
          payload = subBlock.read(payload, payloadEnd);
          if (collection == JEM_ROI) {
            if (subBlock.dataWords() && !subBlock.unpack()) {
              if (debug) {
                std::string errMsg(subBlock.unpackErrorMsg());
                msg() << "JEM RoI sub-block unpacking failed: "
                      << errMsg << endmsg;
              }
              rodErr = m_subBlock->unpackErrorCode();
              break;
            }
            for (int frame = 0; frame < m_frames; ++frame) {
              const LVL1::JEMTobRoI roi = subBlock.roi(frame);
              if (roi.energyLarge() || roi.energySmall()) {
                m_jeCollection->push_back(new LVL1::JEMTobRoI(roi));
              }
            }
          }
        }
      } else {
        // Just RoI word
        LVL1::JEMTobRoI jroi;
        LVL1::CMXRoI croi;
        if (jroi.setRoiWord(*payload)) {
          if (collection == JEM_ROI) {
            if (jroi.crate() != rodCrate - m_crateOffsetHw) {
              if (debug) msg() << "Inconsistent RoI crate number: "
                                 << jroi.crate() << endmsg;
              rodErr = L1CaloSubBlock::ERROR_CRATE_NUMBER;
              break;
            }
            const uint32_t location = (*payload) & 0xfff80000;
            if (dupRoiCheck.insert(location).second) {
              if (jroi.energyLarge() || jroi.energySmall()) {
                m_jeCollection->push_back(new LVL1::JEMTobRoI(*payload));
              }
            } else {
              if (debug) msg() << "Duplicate RoI word "
                                 << MSG::hex << *payload << MSG::dec << endmsg;
              rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
              break;
            }
          }
        } else if (croi.setRoiWord(*payload)) {
          if (collection == CMX_ROI) {
            const uint32_t roiType = (*payload) & 0xfc000000;
   
            if (dupRoiCheck.insert(roiType).second) {
              m_cmCollection->setRoiWord(*payload);
            } else {
              if (debug) msg() << "Duplicate RoI word "
                                 << MSG::hex << *payload << MSG::dec << endmsg;
              rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
              break;
            }
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

// Find CMX energy sums for given crate, source

const LVL1::CMXEtSums* JepRoiByteStreamV2Tool::findCmxSums(const int crate,
    const int source)
{
  const LVL1::CMXEtSums* sums = 0;
  CmxSumsMap::const_iterator mapIter;
  mapIter = m_cmxEtMap.find(crate * 100 + source);
  if (mapIter != m_cmxEtMap.end()) sums = mapIter->second;
  return sums;
}

// Set up JEM RoIs map

void JepRoiByteStreamV2Tool::setupJemRoiMap(const JemRoiCollection*
    const jeCollection)
{
  m_roiMap.clear();
  if (jeCollection) {
    JemRoiCollection::const_iterator pos  = jeCollection->begin();
    JemRoiCollection::const_iterator pose = jeCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::JEMTobRoI* const roi = *pos;
      const uint32_t key = roi->roiWord();
      m_roiMap.insert(std::make_pair(key, roi));
    }
  }
}

// Set up CMX energy sums map

void JepRoiByteStreamV2Tool::setupCmxEtMap(const CmxSumsCollection*
    const etCollection)
{
  m_cmxEtMap.clear();
  if (etCollection) {
    CmxSumsCollection::const_iterator pos  = etCollection->begin();
    CmxSumsCollection::const_iterator pose = etCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CMXEtSums* const sums = *pos;
      const int crate = sums->crate() - m_crateOffsetSw;
      const int key   = crate * 100 + sums->source();
      m_cmxEtMap.insert(std::make_pair(key, sums));
    }
  }
}

// Get energy subBlock types from CMXEtSums source type

void JepRoiByteStreamV2Tool::energySubBlockTypes(const int source,
    CmxEnergySubBlock::SourceType& srcType,
    CmxEnergySubBlock::SumType&    sumType,
    CmxEnergySubBlock::HitsType&   hitType)
{
  switch (source) {
  case LVL1::CMXEtSums::REMOTE_STANDARD:
    srcType = CmxEnergySubBlock::REMOTE;
    sumType = CmxEnergySubBlock::STANDARD;
    break;
  case LVL1::CMXEtSums::REMOTE_RESTRICTED:
    srcType = CmxEnergySubBlock::REMOTE;
    sumType = CmxEnergySubBlock::RESTRICTED_WEIGHTED;
    break;
  case LVL1::CMXEtSums::LOCAL_STANDARD:
    srcType = CmxEnergySubBlock::LOCAL;
    sumType = CmxEnergySubBlock::STANDARD;
    break;
  case LVL1::CMXEtSums::LOCAL_RESTRICTED:
    srcType = CmxEnergySubBlock::LOCAL;
    sumType = CmxEnergySubBlock::RESTRICTED_WEIGHTED;
    break;
  case LVL1::CMXEtSums::TOTAL_STANDARD:
    srcType = CmxEnergySubBlock::TOTAL;
    sumType = CmxEnergySubBlock::STANDARD;
    break;
  case LVL1::CMXEtSums::TOTAL_RESTRICTED:
    srcType = CmxEnergySubBlock::TOTAL;
    sumType = CmxEnergySubBlock::RESTRICTED_WEIGHTED;
    break;
  case LVL1::CMXEtSums::SUM_ET_STANDARD:
    hitType = CmxEnergySubBlock::SUM_ET;
    sumType = CmxEnergySubBlock::STANDARD;
    break;
  case LVL1::CMXEtSums::SUM_ET_RESTRICTED:
    hitType = CmxEnergySubBlock::SUM_ET;
    sumType = CmxEnergySubBlock::RESTRICTED_WEIGHTED;
    break;
  case LVL1::CMXEtSums::MISSING_ET_STANDARD:
    hitType = CmxEnergySubBlock::MISSING_ET;
    sumType = CmxEnergySubBlock::STANDARD;
    break;
  case LVL1::CMXEtSums::MISSING_ET_RESTRICTED:
    hitType = CmxEnergySubBlock::MISSING_ET;
    sumType = CmxEnergySubBlock::RESTRICTED_WEIGHTED;
    break;
  case LVL1::CMXEtSums::MISSING_ET_SIG_STANDARD:
    hitType = CmxEnergySubBlock::MISSING_ET_SIG;
    sumType = CmxEnergySubBlock::STANDARD;
    break;
  default:
    break;
  }
}

} // end namespace
