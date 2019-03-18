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

#include "TrigT1CaloEvent/CMMJetHits.h"
#include "TrigT1CaloEvent/CMMEtSums.h"
#include "TrigT1CaloEvent/CMMRoI.h"
#include "TrigT1CaloEvent/JEMRoI.h"
#include "TrigT1CaloEvent/JEPRoIBSCollection.h"

#include "CmmEnergySubBlock.h"
#include "CmmJetSubBlock.h"
#include "CmmSubBlock.h"
#include "JemRoiSubBlock.h"
#include "L1CaloErrorByteStreamTool.h"
#include "L1CaloSrcIdMap.h"
#include "L1CaloSubBlock.h"
#include "L1CaloUserHeader.h"

#include "JepRoiByteStreamTool.h"

namespace LVL1BS {

// Interface ID

static const InterfaceID IID_IJepRoiByteStreamTool("JepRoiByteStreamTool",
    1, 1);

const InterfaceID& JepRoiByteStreamTool::interfaceID()
{
  return IID_IJepRoiByteStreamTool;
}

// Constructor

JepRoiByteStreamTool::JepRoiByteStreamTool(const std::string& type,
    const std::string& name,
    const IInterface*  parent)
  : AthAlgTool(type, name, parent),
    m_errorTool("LVL1BS::L1CaloErrorByteStreamTool/L1CaloErrorByteStreamTool"),
    m_crates(2), m_modules(16), m_srcIdMap(0), m_subBlock(0), m_rodStatus(0),
    m_fea(0)
{
  declareInterface<JepRoiByteStreamTool>(this);

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
  declareProperty("DataVersion",    m_version       = 1,
                  "Format version number in sub-block header");
  declareProperty("DataFormat",     m_dataFormat    = 1,
                  "Format identifier (0-1) in sub-block header");
  declareProperty("SlinksPerCrate", m_slinks        = 1,
                  "The number of S-Links per crate");

}

// Destructor

JepRoiByteStreamTool::~JepRoiByteStreamTool()
{
}

// Initialize


StatusCode JepRoiByteStreamTool::initialize()
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
  m_subBlock    = new JemRoiSubBlock();
  m_rodStatus   = new std::vector<uint32_t>(2);
  m_fea         = new FullEventAssembler<L1CaloSrcIdMap>();
  return StatusCode::SUCCESS;
}

// Finalize

StatusCode JepRoiByteStreamTool::finalize()
{
  delete m_fea;
  delete m_rodStatus;
  delete m_subBlock;
  delete m_srcIdMap;
  return StatusCode::SUCCESS;
}

// Conversion bytestream to JEM RoI

StatusCode JepRoiByteStreamTool::convert(
  const IROBDataProviderSvc::VROBFRAG& robFrags,
  DataVector<LVL1::JEMRoI>* const jeCollection)
{
  m_jeCollection = jeCollection;
  return convertBs(robFrags, JEM_ROI);
}

// Conversion bytestream to CMM RoI

StatusCode JepRoiByteStreamTool::convert(
  const IROBDataProviderSvc::VROBFRAG& robFrags,
  LVL1::CMMRoI* const cmCollection)
{
  m_cmCollection = cmCollection;
  return convertBs(robFrags, CMM_ROI);
}

// Conversion of JEP container to bytestream

StatusCode JepRoiByteStreamTool::convert(
  const LVL1::JEPRoIBSCollection* const jep,
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
    setupCmmHitsMap(jep->CmmHits());
    setupCmmEtMap(jep->CmmSums());
  }

  // Loop over JEM RoI data

  const int modulesPerSlink = m_modules / m_slinks;
  for (int crate = 0; crate < m_crates; ++crate) {
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
        const LVL1::JEMRoI* const roi = mapIter->second;
        if (roi->crate() < crate)  continue;
        if (roi->crate() > crate)  break;
        if (roi->jem()   < module) continue;
        if (roi->jem()   > module) break;
        if (roi->hits() || roi->error()) {
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

    // Append CMM RoIs to last S-Link of the system crate

    if (crate != m_crates - 1) continue;

    // Create sub-blocks for Neutral format

    if (neutralFormat) {
      const int timeslices = 1;
      const int slice = 0;

      // CMM-Energy

      CmmEnergySubBlock enBlock;
      const int cmmEnergyVersion = 2; // with Missing-ET-Sig
      enBlock.setCmmHeader(cmmEnergyVersion, m_dataFormat, slice, hwCrate,
                           CmmSubBlock::SYSTEM, CmmSubBlock::CMM_ENERGY,
                           CmmSubBlock::LEFT, timeslices);
      int maxDataID = static_cast<int>(LVL1::CMMEtSums::MAXID);
      for (int dataID = 0; dataID < maxDataID; ++dataID) {
        int source = dataID;
        if (dataID >= m_modules) {
          // coverity[mixed_enums : FALSE]
          // coverity[switch_on_enum : FALSE]
          // coverity[first_enum_type : FALSE]
          switch (dataID) {
          case LVL1::CMMEtSums::LOCAL:
            source = CmmEnergySubBlock::LOCAL;
            break;
          case LVL1::CMMEtSums::REMOTE:
            source = CmmEnergySubBlock::REMOTE;
            break;
          case LVL1::CMMEtSums::TOTAL:
            source = CmmEnergySubBlock::TOTAL;
            break;
          case LVL1::CMMEtSums::MISSING_ET_MAP:
          case LVL1::CMMEtSums::SUM_ET_MAP:
          case LVL1::CMMEtSums::MISSING_ET_SIG_MAP:
            break;
          default:
            continue;
          }
        }
        const LVL1::CMMEtSums* const sums = findCmmSums(crate, dataID);
        if ( sums ) {
          const unsigned int ex = sums->Ex();
          const unsigned int ey = sums->Ey();
          const unsigned int et = sums->Et();
          const int exErr = sums->ExError();
          const int eyErr = sums->EyError();
          const int etErr = sums->EtError();
          if (dataID == LVL1::CMMEtSums::MISSING_ET_MAP) {
            enBlock.setMissingEtHits(slice, et);
          } else if (dataID == LVL1::CMMEtSums::SUM_ET_MAP) {
            enBlock.setSumEtHits(slice, et);
          } else if (dataID == LVL1::CMMEtSums::MISSING_ET_SIG_MAP) {
            enBlock.setMissingEtSigHits(slice, et);
          } else {
            enBlock.setSubsums(slice, source, ex, ey, et, exErr, eyErr, etErr);
          }
        }
      }
      if ( !enBlock.pack()) {
        msg(MSG::ERROR) << "CMM-Energy sub-block packing failed" << endmsg;
        return StatusCode::FAILURE;
      }
      if (debug) {
        msg() << "CMM-Energy sub-block data words: "
              << enBlock.dataWords() << endmsg;
      }
      enBlock.write(theROD);

      // CMM-Jet

      CmmJetSubBlock jetBlock;
      jetBlock.setCmmHeader(m_version, m_dataFormat, slice, hwCrate,
                            CmmSubBlock::SYSTEM, CmmSubBlock::CMM_JET,
                            CmmSubBlock::RIGHT, timeslices);
      maxDataID = static_cast<int>(LVL1::CMMJetHits::MAXID);
      for (int dataID = 0; dataID < maxDataID; ++dataID) {
        int source = dataID;
        if (dataID >= m_modules) {
          // coverity[mixed_enums : FALSE]
          // coverity[switch_on_enum : FALSE]
          // coverity[first_enum_type : FALSE]
          switch (dataID) {
          case LVL1::CMMJetHits::LOCAL_MAIN:
            source = CmmJetSubBlock::LOCAL_MAIN;
            break;
          case LVL1::CMMJetHits::REMOTE_MAIN:
            source = CmmJetSubBlock::REMOTE_MAIN;
            break;
          case LVL1::CMMJetHits::TOTAL_MAIN:
            source = CmmJetSubBlock::TOTAL_MAIN;
            break;
          case LVL1::CMMJetHits::LOCAL_FORWARD:
            source = CmmJetSubBlock::LOCAL_FORWARD;
            break;
          case LVL1::CMMJetHits::REMOTE_FORWARD:
            source = CmmJetSubBlock::REMOTE_FORWARD;
            break;
          case LVL1::CMMJetHits::TOTAL_FORWARD:
            source = CmmJetSubBlock::TOTAL_FORWARD;
            break;
          case LVL1::CMMJetHits::ET_MAP:
            break;
          default:
            continue;
          }
        }
        const LVL1::CMMJetHits* const ch = findCmmHits(crate, dataID);
        if ( ch ) {
          const unsigned int hits = ch->Hits();
          const int          errs = ch->Error();
          if (dataID == LVL1::CMMJetHits::ET_MAP) {
            jetBlock.setJetEtMap(slice, hits);
          } else {
            jetBlock.setJetHits(slice, source, hits, errs);
          }
        }
      }
      if ( !jetBlock.pack()) {
        msg(MSG::ERROR) << "CMM-Jet sub-block packing failed" << endmsg;
        return StatusCode::FAILURE;
      }
      if (debug) {
        msg() << "CMM-Jet sub-block data words: "
              << jetBlock.dataWords() << endmsg;
      }
      jetBlock.write(theROD);

    } else {

      // Standard format

      const LVL1::CMMRoI* const roi = jep->CmmRoi();
      if ( roi ) {
        // Make sure word IDs are correct
        const LVL1::CMMRoI roid(roi->jetEtHits(), roi->sumEtHits(),
                                roi->missingEtHits(), roi->missingEtSigHits(),
                                roi->ex(), roi->ey(), roi->et(),
                                roi->jetEtError(), roi->sumEtError(),
                                roi->missingEtError(), roi->missingEtSigError(),
                                roi->exError(), roi->eyError(), roi->etError());
        if (roid.jetEtHits() || roid.jetEtError()) {
          theROD->push_back(roid.jetEtRoiWord());
        }
        // CMM-Energy RoIs are not zero-supressed unless all are zero
        if (roid.sumEtHits() || roid.missingEtHits() ||
            roid.missingEtSigHits() || roid.ex() || roid.ey() || roid.et() ||
            roid.sumEtError() || roid.missingEtError() ||
            roid.missingEtSigError() || roid.exError() || roid.eyError() ||
            roid.etError()) {
          theROD->push_back(roid.energyRoiWord0());
          theROD->push_back(roid.energyRoiWord1());
          theROD->push_back(roid.energyRoiWord2());
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

const std::vector<uint32_t>& JepRoiByteStreamTool::sourceIDs(
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

StatusCode JepRoiByteStreamTool::convertBs(
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
    const int rodCrate = m_srcIdMap->crate(sourceID);
    if (debug) {
      msg() << "Treating crate " << rodCrate
            << " slink " << m_srcIdMap->slink(sourceID) << endmsg;
    }

    // First word may be User Header
    if (L1CaloUserHeader::isValid(*payload)) {
      L1CaloUserHeader userHeader(*payload);
      const int minorVersion = (*rob)->rod_version() & 0xffff;
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
        if (CmmSubBlock::cmmBlock(*payload)) {
          // CMMs
          if (CmmSubBlock::cmmType(*payload) == CmmSubBlock::CMM_JET) {
            CmmJetSubBlock subBlock;
            payload = subBlock.read(payload, payloadEnd);
            if (collection == CMM_ROI) {
              if (subBlock.dataWords() && !subBlock.unpack()) {
                if (debug) {
                  std::string errMsg(subBlock.unpackErrorMsg());
                  msg() << "CMM-Jet sub-block unpacking failed: "
                        << errMsg << endmsg;
                }
                rodErr = m_subBlock->unpackErrorCode();
                break;
              }
              const LVL1::CMMRoI roi(subBlock.jetEtMap(slice),
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
              m_cmCollection->setRoiWord(roi.jetEtRoiWord());
            }
          } else {
            CmmEnergySubBlock subBlock;
            payload = subBlock.read(payload, payloadEnd);
            if (collection == CMM_ROI) {
              if (subBlock.dataWords() && !subBlock.unpack()) {
                if (debug) {
                  std::string errMsg(subBlock.unpackErrorMsg());
                  msg() << "CMM-Energy sub-block unpacking failed: "
                        << errMsg << endmsg;
                }
                rodErr = m_subBlock->unpackErrorCode();
                break;
              }
              const LVL1::CMMRoI roi(0, subBlock.sumEtHits(slice),
                                     subBlock.missingEtHits(slice),
                                     subBlock.missingEtSigHits(slice),
                                     subBlock.ex(slice, CmmEnergySubBlock::TOTAL),
                                     subBlock.ey(slice, CmmEnergySubBlock::TOTAL),
                                     subBlock.et(slice, CmmEnergySubBlock::TOTAL),
                                     0, 0, 0, 0,
                                     subBlock.exError(slice, CmmEnergySubBlock::TOTAL),
                                     subBlock.eyError(slice, CmmEnergySubBlock::TOTAL),
                                     subBlock.etError(slice, CmmEnergySubBlock::TOTAL));
              m_cmCollection->setRoiWord(roi.energyRoiWord0());
              m_cmCollection->setRoiWord(roi.energyRoiWord1());
              m_cmCollection->setRoiWord(roi.energyRoiWord2());
            }
          }
        } else {
          // JEM RoI
          JemRoiSubBlock subBlock;
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
            for (int frame = 0; frame < 8; ++frame) {
              for (int forward = 0; forward < 2; ++forward) {
                const LVL1::JEMRoI roi = subBlock.roi(frame, forward);
                if (roi.hits() || roi.error()) {
                  m_jeCollection->push_back(new LVL1::JEMRoI(roi));
                }
              }
            }
          }
        }
      } else {
        // Just RoI word
        LVL1::JEMRoI jroi;
        LVL1::CMMRoI croi;
        if (jroi.setRoiWord(*payload)) {
          if (collection == JEM_ROI) {
            if (jroi.crate() != rodCrate - m_crateOffsetHw) {
              if (debug) msg() << "Inconsistent RoI crate number: "
                                 << jroi.crate() << endmsg;
              rodErr = L1CaloSubBlock::ERROR_CRATE_NUMBER;
              break;
            }
            const uint32_t location = (*payload) & 0xfffc0000;
            if (dupRoiCheck.insert(location).second) {
              if (jroi.hits() || jroi.error()) {
                m_jeCollection->push_back(new LVL1::JEMRoI(*payload));
              }
            } else {
              if (debug) msg() << "Duplicate RoI word "
                                 << MSG::hex << *payload << MSG::dec << endmsg;
              rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
              break;
            }
          }
        } else if (croi.setRoiWord(*payload)) {
          if (collection == CMM_ROI) {
            uint32_t roiType = (*payload) & 0xf0000000;
            if ((roiType & 0xe0000000) == 0xa0000000) roiType = 0xa0000000;
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

// Find CMM hits for given crate, dataID

const LVL1::CMMJetHits* JepRoiByteStreamTool::findCmmHits(const int crate,
    const int dataID)
{
  const LVL1::CMMJetHits* hits = 0;
  CmmHitsMap::const_iterator mapIter;
  mapIter = m_cmmHitsMap.find(crate * 100 + dataID);
  if (mapIter != m_cmmHitsMap.end()) hits = mapIter->second;
  return hits;
}

// Find CMM energy sums for given crate, module, dataID

const LVL1::CMMEtSums* JepRoiByteStreamTool::findCmmSums(const int crate,
    const int dataID)
{
  const LVL1::CMMEtSums* sums = 0;
  CmmSumsMap::const_iterator mapIter;
  mapIter = m_cmmEtMap.find(crate * 100 + dataID);
  if (mapIter != m_cmmEtMap.end()) sums = mapIter->second;
  return sums;
}

// Set up JEM RoIs map

void JepRoiByteStreamTool::setupJemRoiMap(const JemRoiCollection*
    const jeCollection)
{
  m_roiMap.clear();
  if (jeCollection) {
    JemRoiCollection::const_iterator pos  = jeCollection->begin();
    JemRoiCollection::const_iterator pose = jeCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::JEMRoI* const roi = *pos;
      const uint32_t key = roi->roiWord();
      m_roiMap.insert(std::make_pair(key, roi));
    }
  }
}

// Set up CMM hits map

void JepRoiByteStreamTool::setupCmmHitsMap(const CmmHitsCollection*
    const hitCollection)
{
  m_cmmHitsMap.clear();
  if (hitCollection) {
    CmmHitsCollection::const_iterator pos  = hitCollection->begin();
    CmmHitsCollection::const_iterator pose = hitCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CMMJetHits* const hits = *pos;
      const int crate = hits->crate() - m_crateOffsetSw;
      const int key   = crate * 100 + hits->dataID();
      m_cmmHitsMap.insert(std::make_pair(key, hits));
    }
  }
}

// Set up CMM energy sums map

void JepRoiByteStreamTool::setupCmmEtMap(const CmmSumsCollection*
    const etCollection)
{
  m_cmmEtMap.clear();
  if (etCollection) {
    CmmSumsCollection::const_iterator pos  = etCollection->begin();
    CmmSumsCollection::const_iterator pose = etCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CMMEtSums* const sums = *pos;
      const int crate = sums->crate() - m_crateOffsetSw;
      const int key   = crate * 100 + sums->dataID();
      m_cmmEtMap.insert(std::make_pair(key, sums));
    }
  }
}

} // end namespace
