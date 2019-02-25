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
#include "TrigT1CaloEvent/JEMHits.h"
#include "TrigT1CaloEvent/JEMEtSums.h"
#include "TrigT1CaloEvent/JEPBSCollection.h"
#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1CaloUtils/JetElementKey.h"
#include "TrigT1CaloMappingToolInterfaces/IL1CaloMappingTool.h"

#include "CmmEnergySubBlock.h"
#include "CmmJetSubBlock.h"
#include "CmmSubBlock.h"
#include "JemJetElement.h"
#include "JemSubBlock.h"
#include "L1CaloErrorByteStreamTool.h"
#include "L1CaloSrcIdMap.h"
#include "L1CaloSubBlock.h"
#include "L1CaloUserHeader.h"
#include "ModifySlices.h"

#include "JepByteStreamTool.h"

namespace LVL1BS {

// Interface ID

static const InterfaceID IID_IJepByteStreamTool("JepByteStreamTool", 1, 1);

const InterfaceID& JepByteStreamTool::interfaceID()
{
  return IID_IJepByteStreamTool;
}

// Constructor

JepByteStreamTool::JepByteStreamTool(const std::string& type,
                                     const std::string& name,
                                     const IInterface*  parent)
  : AthAlgTool(type, name, parent),
    m_jemMaps("LVL1::JemMappingTool/JemMappingTool"),
    m_errorTool("LVL1BS::L1CaloErrorByteStreamTool/L1CaloErrorByteStreamTool"),
    m_channels(44), m_crates(2), m_modules(16), m_coreOverlap(0),
    m_subDetector(eformat::TDAQ_CALO_JET_PROC_DAQ),
    m_srcIdMap(0), m_elementKey(0),
    m_jemSubBlock(0), m_cmmEnergySubBlock(0), m_cmmJetSubBlock(0),
    m_rodStatus(0), m_fea(0)
{
  declareInterface<JepByteStreamTool>(this);

  declareProperty("JemMappingTool", m_jemMaps,
                  "Crate/Module/Channel to Eta/Phi/Layer mapping tool");

  declareProperty("CrateOffsetHw",  m_crateOffsetHw = 12,
                  "Offset of JEP crate numbers in bytestream");
  declareProperty("CrateOffsetSw",  m_crateOffsetSw = 0,
                  "Offset of JEP crate numbers in RDOs");
  declareProperty("SlinksPerCrate", m_slinks        = 4,
                  "The number of S-Links per crate");

  // Properties for reading bytestream only
  declareProperty("ROBSourceIDs",       m_sourceIDs,
                  "ROB fragment source identifiers");

  // Properties for writing bytestream only
  declareProperty("DataVersion",    m_version     = 1,
                  "Format version number in sub-block header");
  declareProperty("DataFormat",     m_dataFormat  = 1,
                  "Format identifier (0-1) in sub-block header");
  declareProperty("SimulSlices",    m_dfltSlices  = 1,
                  "The number of slices in the simulation");
  declareProperty("ForceSlices",    m_forceSlices = 0,
                  "If >0, the number of slices in bytestream");

}

// Destructor

JepByteStreamTool::~JepByteStreamTool()
{
}

// Initialize


StatusCode JepByteStreamTool::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endmsg;

  StatusCode sc = m_jemMaps.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to retrieve tool " << m_jemMaps << endmsg;
    return sc;
  } else msg(MSG::INFO) << "Retrieved tool " << m_jemMaps << endmsg;

  sc = m_errorTool.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to retrieve tool " << m_errorTool << endmsg;
    return sc;
  } else msg(MSG::INFO) << "Retrieved tool " << m_errorTool << endmsg;

  m_srcIdMap          = new L1CaloSrcIdMap();
  m_elementKey        = new LVL1::JetElementKey();
  m_jemSubBlock       = new JemSubBlock();
  m_cmmEnergySubBlock = new CmmEnergySubBlock();
  m_cmmJetSubBlock    = new CmmJetSubBlock();
  m_rodStatus         = new std::vector<uint32_t>(2);
  m_fea               = new FullEventAssembler<L1CaloSrcIdMap>();
  return StatusCode::SUCCESS;
}

// Finalize

StatusCode JepByteStreamTool::finalize()
{
  delete m_fea;
  delete m_rodStatus;
  delete m_cmmJetSubBlock;
  delete m_cmmEnergySubBlock;
  delete m_jemSubBlock;
  delete m_elementKey;
  delete m_srcIdMap;
  return StatusCode::SUCCESS;
}

// Conversion bytestream to jet elements

StatusCode JepByteStreamTool::convert(
  const IROBDataProviderSvc::VROBFRAG& robFrags,
  DataVector<LVL1::JetElement>* const jeCollection)
{
  JetElementData data (jeCollection);
  return convertBs(robFrags, data);
}

// Conversion bytestream to jet hits

StatusCode JepByteStreamTool::convert(
  const IROBDataProviderSvc::VROBFRAG& robFrags,
  DataVector<LVL1::JEMHits>* const hitCollection)
{
  JetHitsData data (hitCollection);
  return convertBs(robFrags, data);
}

// Conversion bytestream to energy sums

StatusCode JepByteStreamTool::convert(
  const IROBDataProviderSvc::VROBFRAG& robFrags,
  DataVector<LVL1::JEMEtSums>* const etCollection)
{
  EnergySumsData data (etCollection);
  return convertBs(robFrags, data);
}

// Conversion bytestream to CMM hits

StatusCode JepByteStreamTool::convert(
  const IROBDataProviderSvc::VROBFRAG& robFrags,
  DataVector<LVL1::CMMJetHits>* const hitCollection)
{
  CmmHitsData data (hitCollection);
  return convertBs(robFrags, data);
}

// Conversion bytestream to CMM energy sums

StatusCode JepByteStreamTool::convert(
  const IROBDataProviderSvc::VROBFRAG& robFrags,
  DataVector<LVL1::CMMEtSums>* const etCollection)
{
  CmmSumsData data (etCollection);
  return convertBs(robFrags, data);
}

// Conversion of JEP container to bytestream

StatusCode JepByteStreamTool::convert(const LVL1::JEPBSCollection* const jep,
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

  setupJeMap(jep->JetElements());
  setupHitsMap(jep->JetHits());
  setupEtMap(jep->EnergySums());
  setupCmmHitsMap(jep->CmmHits());
  setupCmmEtMap(jep->CmmSums());

  // Loop over data

  const bool neutralFormat = m_dataFormat == L1CaloSubBlock::NEUTRAL;
  const int modulesPerSlink = m_modules / m_slinks;
  int timeslices       = 1;
  int trigJem          = 0;
  int timeslicesNew    = 1;
  int trigJemNew       = 0;
  for (int crate = 0; crate < m_crates; ++crate) {
    const int hwCrate = crate + m_crateOffsetHw;

    for (int module = 0; module < m_modules; ++module) {

      // Pack required number of modules per slink

      if (module % modulesPerSlink == 0) {
        const int daqOrRoi = 0;
        const int slink = module / modulesPerSlink;
        if (debug) {
          msg() << "Treating crate " << hwCrate
                << " slink " << slink << endmsg;
        }
        // Get number of JEM slices and triggered slice offset
        // for this slink
        if ( ! slinkSlices(crate, module, modulesPerSlink,
                           timeslices, trigJem)) {
          msg(MSG::ERROR) << "Inconsistent number of slices or "
                          << "triggered slice offsets in data for crate "
                          << hwCrate << " slink " << slink << endmsg;
          return StatusCode::FAILURE;
        }
        timeslicesNew = (m_forceSlices) ? m_forceSlices : timeslices;
        trigJemNew    = ModifySlices::peak(trigJem, timeslices, timeslicesNew);
        if (debug) {
          msg() << "Data Version/Format: " << m_version
                << " " << m_dataFormat << endmsg
                << "Slices/offset: " << timeslices << " " << trigJem;
          if (timeslices != timeslicesNew) {
            msg() << " modified to " << timeslicesNew << " " << trigJemNew;
          }
          msg() << endmsg;
        }
        L1CaloUserHeader userHeader;
        userHeader.setJem(trigJemNew);
        const uint32_t rodIdJem = m_srcIdMap->getRodID(hwCrate, slink, daqOrRoi,
                                  m_subDetector);
        theROD = m_fea->getRodData(rodIdJem);
        theROD->push_back(userHeader.header());
        m_rodStatusMap.insert(make_pair(rodIdJem, m_rodStatus));
      }
      if (debug) msg() << "Module " << module << endmsg;

      // Create a sub-block for each slice (except Neutral format)

      m_jemBlocks.clear();
      for (int slice = 0; slice < timeslicesNew; ++slice) {
        JemSubBlock* const subBlock = new JemSubBlock();
        subBlock->setJemHeader(m_version, m_dataFormat, slice,
                               hwCrate, module, timeslicesNew);
        m_jemBlocks.push_back(subBlock);
        if (neutralFormat) break;
      }

      // Find jet elements corresponding to each eta/phi pair and fill
      // sub-blocks

      for (int chan = 0; chan < m_channels; ++chan) {
        double eta = 0.;
        double phi = 0.;
        int layer = 0;
        if (m_jemMaps->mapping(crate, module, chan, eta, phi, layer)) {
          const LVL1::JetElement* const je = findJetElement(eta, phi);
          if (je ) {
            std::vector<int> emData;
            std::vector<int> hadData;
            std::vector<int> emErrors;
            std::vector<int> hadErrors;
            ModifySlices::data(je->emEnergyVec(),  emData,    timeslicesNew);
            ModifySlices::data(je->hadEnergyVec(), hadData,   timeslicesNew);
            ModifySlices::data(je->emErrorVec(),   emErrors,  timeslicesNew);
            ModifySlices::data(je->hadErrorVec(),  hadErrors, timeslicesNew);
            for (int slice = 0; slice < timeslicesNew; ++slice) {
              const LVL1::DataError emErrBits(emErrors[slice]);
              const LVL1::DataError hadErrBits(hadErrors[slice]);
              const int index = ( neutralFormat ) ? 0 : slice;
              JemSubBlock* const subBlock = m_jemBlocks[index];
              const JemJetElement jetEle(chan, emData[slice], hadData[slice],
                                         emErrBits.get(LVL1::DataError::Parity),
                                         hadErrBits.get(LVL1::DataError::Parity),
                                         emErrBits.get(LVL1::DataError::LinkDown) +
                                         (hadErrBits.get(LVL1::DataError::LinkDown) << 1));
              subBlock->fillJetElement(slice, jetEle);
            }
          }
        }
      }

      // Add jet hits and energy subsums

      const LVL1::JEMHits* const hits = findJetHits(crate, module);
      if (hits) {
        std::vector<unsigned int> vec;
        ModifySlices::data(hits->JetHitsVec(), vec, timeslicesNew);
        for (int slice = 0; slice < timeslicesNew; ++slice) {
          const int index = ( neutralFormat ) ? 0 : slice;
          JemSubBlock* const subBlock = m_jemBlocks[index];
          subBlock->setJetHits(slice, vec[slice]);
        }
      }
      const LVL1::JEMEtSums* const et = findEnergySums(crate, module);
      if (et) {
        std::vector<unsigned int> exVec;
        std::vector<unsigned int> eyVec;
        std::vector<unsigned int> etVec;
        ModifySlices::data(et->ExVec(), exVec, timeslicesNew);
        ModifySlices::data(et->EyVec(), eyVec, timeslicesNew);
        ModifySlices::data(et->EtVec(), etVec, timeslicesNew);
        for (int slice = 0; slice < timeslicesNew; ++slice) {
          const int index = ( neutralFormat ) ? 0 : slice;
          JemSubBlock* const subBlock = m_jemBlocks[index];
          subBlock->setEnergySubsums(slice, exVec[slice], eyVec[slice],
                                     etVec[slice]);
        }
      }

      // Pack and write the sub-blocks

      DataVector<JemSubBlock>::iterator pos;
      for (pos = m_jemBlocks.begin(); pos != m_jemBlocks.end(); ++pos) {
        JemSubBlock* const subBlock = *pos;
        if ( !subBlock->pack()) {
          msg(MSG::ERROR) << "JEM sub-block packing failed" << endmsg;
          return StatusCode::FAILURE;
        }
        if (debug) {
          msg() << "JEM sub-block data words: "
                << subBlock->dataWords() << endmsg;
        }
        subBlock->write(theROD);
      }
    }

    // Append CMMs to last S-Link of the crate

    // Create a sub-block for each slice (except Neutral format)

    m_cmmEnergyBlocks.clear();
    m_cmmJetBlocks.clear();
    const int summing = (crate == m_crates - 1) ? CmmSubBlock::SYSTEM
                        : CmmSubBlock::CRATE;
    for (int slice = 0; slice < timeslicesNew; ++slice) {
      CmmEnergySubBlock* const enBlock = new CmmEnergySubBlock();
      const int cmmEnergyVersion = 2; // with Missing-ET-Sig
      enBlock->setCmmHeader(cmmEnergyVersion, m_dataFormat, slice, hwCrate,
                            summing, CmmSubBlock::CMM_ENERGY,
                            CmmSubBlock::LEFT, timeslicesNew);
      m_cmmEnergyBlocks.push_back(enBlock);
      CmmJetSubBlock* const jetBlock = new CmmJetSubBlock();
      jetBlock->setCmmHeader(m_version, m_dataFormat, slice, hwCrate,
                             summing, CmmSubBlock::CMM_JET,
                             CmmSubBlock::RIGHT, timeslicesNew);
      m_cmmJetBlocks.push_back(jetBlock);
      if (neutralFormat) break;
    }

    // CMM-Energy

    int maxDataID = static_cast<int>(LVL1::CMMEtSums::MAXID);
    for (int dataID = 0; dataID < maxDataID; ++dataID) {
      int source = dataID;
      if (dataID >= m_modules) {
        if (summing == CmmSubBlock::CRATE &&
            dataID != LVL1::CMMEtSums::LOCAL) continue;
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
        std::vector<unsigned int> ex;
        std::vector<unsigned int> ey;
        std::vector<unsigned int> et;
        std::vector<int> exErr;
        std::vector<int> eyErr;
        std::vector<int> etErr;
        ModifySlices::data(sums->ExVec(), ex, timeslicesNew);
        ModifySlices::data(sums->EyVec(), ey, timeslicesNew);
        ModifySlices::data(sums->EtVec(), et, timeslicesNew);
        ModifySlices::data(sums->ExErrorVec(), exErr, timeslicesNew);
        ModifySlices::data(sums->EyErrorVec(), eyErr, timeslicesNew);
        ModifySlices::data(sums->EtErrorVec(), etErr, timeslicesNew);
        for (int slice = 0; slice < timeslicesNew; ++slice) {
          const LVL1::DataError exErrBits(exErr[slice]);
          const LVL1::DataError eyErrBits(eyErr[slice]);
          const LVL1::DataError etErrBits(etErr[slice]);
          int exError = exErrBits.get(LVL1::DataError::Parity);
          int eyError = eyErrBits.get(LVL1::DataError::Parity);
          int etError = etErrBits.get(LVL1::DataError::Parity);
          if (dataID == LVL1::CMMEtSums::LOCAL ||
              dataID == LVL1::CMMEtSums::REMOTE ||
              dataID == LVL1::CMMEtSums::TOTAL) {
            exError = (exError << 1) + exErrBits.get(LVL1::DataError::Overflow);
            eyError = (eyError << 1) + eyErrBits.get(LVL1::DataError::Overflow);
            etError = (etError << 1) + etErrBits.get(LVL1::DataError::Overflow);
          }
          const int index = ( neutralFormat ) ? 0 : slice;
          CmmEnergySubBlock* const subBlock = m_cmmEnergyBlocks[index];
          if (dataID == LVL1::CMMEtSums::MISSING_ET_MAP) {
            subBlock->setMissingEtHits(slice, et[slice]);
          } else if (dataID == LVL1::CMMEtSums::SUM_ET_MAP) {
            subBlock->setSumEtHits(slice, et[slice]);
          } else if (dataID == LVL1::CMMEtSums::MISSING_ET_SIG_MAP) {
            subBlock->setMissingEtSigHits(slice, et[slice]);
          } else {
            subBlock->setSubsums(slice, source,
                                 ex[slice], ey[slice], et[slice],
                                 exError, eyError, etError);
          }
        }
      }
    }
    DataVector<CmmEnergySubBlock>::iterator pos;
    pos = m_cmmEnergyBlocks.begin();
    for (; pos != m_cmmEnergyBlocks.end(); ++pos) {
      CmmEnergySubBlock* const subBlock = *pos;
      if ( !subBlock->pack()) {
        msg(MSG::ERROR) << "CMM-Energy sub-block packing failed" << endmsg;
        return StatusCode::FAILURE;
      }
      if (debug) {
        msg() << "CMM-Energy sub-block data words: "
              << subBlock->dataWords() << endmsg;
      }
      subBlock->write(theROD);
    }

    // CMM-Jet

    maxDataID = static_cast<int>(LVL1::CMMJetHits::MAXID);
    for (int dataID = 0; dataID < maxDataID; ++dataID) {
      int source = dataID;
      if (dataID >= m_modules) {
        if (summing == CmmSubBlock::CRATE &&
            dataID != LVL1::CMMJetHits::LOCAL_MAIN &&
            dataID != LVL1::CMMJetHits::LOCAL_FORWARD) continue;
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
        std::vector<unsigned int> hits;
        std::vector<int> errs;
        ModifySlices::data(ch->HitsVec(),  hits, timeslicesNew);
        ModifySlices::data(ch->ErrorVec(), errs, timeslicesNew);
        for (int slice = 0; slice < timeslicesNew; ++slice) {
          const LVL1::DataError errBits(errs[slice]);
          const int index = ( neutralFormat ) ? 0 : slice;
          CmmJetSubBlock* const subBlock = m_cmmJetBlocks[index];
          if (dataID == LVL1::CMMJetHits::ET_MAP) {
            subBlock->setJetEtMap(slice, hits[slice]);
          } else {
            subBlock->setJetHits(slice, source, hits[slice],
                                 errBits.get(LVL1::DataError::Parity));
          }
        }
      }
    }
    DataVector<CmmJetSubBlock>::iterator jos;
    jos = m_cmmJetBlocks.begin();
    for (; jos != m_cmmJetBlocks.end(); ++jos) {
      CmmJetSubBlock* const subBlock = *jos;
      if ( !subBlock->pack()) {
        msg(MSG::ERROR) << "CMM-Jet sub-block packing failed" << endmsg;
        return StatusCode::FAILURE;
      }
      if (debug) {
        msg() << "CMM-Jet sub-block data words: "
              << subBlock->dataWords() << endmsg;
      }
      subBlock->write(theROD);
    }
  }

  // Fill the raw event

  m_fea->fill(re, msg());

  // Set ROD status words

  //L1CaloRodStatus::setStatus(re, m_rodStatusMap, m_srcIdMap);

  return StatusCode::SUCCESS;
}

// Return reference to vector with all possible Source Identifiers

const std::vector<uint32_t>& JepByteStreamTool::sourceIDs(
  const std::string& sgKey)
{
  // Check if overlap jet element channels wanted
  const std::string flag("Overlap");
  const std::string::size_type pos = sgKey.find(flag);
  m_coreOverlap =
    (pos == std::string::npos || pos != sgKey.length() - flag.length()) ? 0 : 1;

  if (m_sourceIDs.empty()) {
    const int maxCrates = m_crates + m_crateOffsetHw;
    const int maxSlinks = m_srcIdMap->maxSlinks();
    for (int hwCrate = m_crateOffsetHw; hwCrate < maxCrates; ++hwCrate) {
      for (int slink = 0; slink < maxSlinks; ++slink) {
        const int daqOrRoi = 0;
        const uint32_t rodId = m_srcIdMap->getRodID(hwCrate, slink, daqOrRoi,
                               m_subDetector);
        const uint32_t robId = m_srcIdMap->getRobID(rodId);
        m_sourceIDs.push_back(robId);
      }
    }
  }
  return m_sourceIDs;
}

// Convert bytestream to given container type

StatusCode JepByteStreamTool::convertBs(
  const IROBDataProviderSvc::VROBFRAG& robFrags,
  JepByteStreamToolData& data)
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
        m_srcIdMap->daqOrRoi(sourceID) != 0               ||
        m_srcIdMap->slink(sourceID)    >= m_slinks        ||
        m_srcIdMap->crate(sourceID)    <  m_crateOffsetHw ||
        m_srcIdMap->crate(sourceID)    >= m_crateOffsetHw + m_crates) {
      m_errorTool->rodError(robid, L1CaloSubBlock::ERROR_ROD_ID);
      if (debug) {
        msg() << "Wrong source identifier in data: ROD "
              << MSG::hex << sourceID << "  ROB " << robid
              << MSG::dec << endmsg;
      }
      continue;
    }
    const int rodCrate = m_srcIdMap->crate(sourceID);
    if (debug) {
      msg() << "Treating crate " << rodCrate
            << " slink " << m_srcIdMap->slink(sourceID) << endmsg;
    }

    // First word should be User Header
    if ( !L1CaloUserHeader::isValid(*payload) ) {
      m_errorTool->rodError(robid, L1CaloSubBlock::ERROR_USER_HEADER);
      if (debug) msg() << "Invalid or missing user header" << endmsg;
      continue;
    }
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
    // triggered slice offsets
    int trigJem = userHeader.jem();
    int trigCmm = userHeader.jepCmm();
    if (debug) {
      msg() << "Minor format version number: " << MSG::hex
            << minorVersion << MSG::dec << endmsg
            << "JEM triggered slice offset: " << trigJem << endmsg
            << "CMM triggered slice offset: " << trigCmm << endmsg;
    }
    if (trigJem != trigCmm) {
      const int newTrig = (trigJem > trigCmm) ? trigJem : trigCmm;
      trigJem = newTrig;
      trigCmm = newTrig;
      if (debug) msg() << "Changed both offsets to " << newTrig << endmsg;
    }

    // Loop over sub-blocks

    m_rodErr = L1CaloSubBlock::ERROR_NONE;
    while (payload != payloadEnd) {

      if (L1CaloSubBlock::wordType(*payload) != L1CaloSubBlock::HEADER) {
        if (debug) msg() << "Unexpected data sequence" << endmsg;
        m_rodErr = L1CaloSubBlock::ERROR_MISSING_HEADER;
        break;
      }
      if (CmmSubBlock::cmmBlock(*payload)) {
        // CMMs
        if (CmmSubBlock::cmmType(*payload) == CmmSubBlock::CMM_JET) {
          m_cmmJetSubBlock->clear();
          payload = m_cmmJetSubBlock->read(payload, payloadEnd);
          if (m_cmmJetSubBlock->crate() != rodCrate) {
            if (debug) msg() << "Inconsistent crate number in ROD source ID"
                               << endmsg;
            m_rodErr = L1CaloSubBlock::ERROR_CRATE_NUMBER;
            break;
          }
          if (data.m_collection == CMM_HITS) {
            decodeCmmJet(m_cmmJetSubBlock, trigCmm, static_cast<CmmHitsData&>(data));
            if (m_rodErr != L1CaloSubBlock::ERROR_NONE) {
              if (debug) msg() << "decodeCmmJet failed" << endmsg;
              break;
            }
          }
        } else if (CmmSubBlock::cmmType(*payload) == CmmSubBlock::CMM_ENERGY) {
          m_cmmEnergySubBlock->clear();
          payload = m_cmmEnergySubBlock->read(payload, payloadEnd);
          if (m_cmmEnergySubBlock->crate() != rodCrate) {
            if (debug) msg() << "Inconsistent crate number in ROD source ID"
                               << endmsg;
            m_rodErr = L1CaloSubBlock::ERROR_CRATE_NUMBER;
            break;
          }
          if (data.m_collection == CMM_SUMS) {
            decodeCmmEnergy(m_cmmEnergySubBlock, trigCmm, static_cast<CmmSumsData&>(data));
            if (m_rodErr != L1CaloSubBlock::ERROR_NONE) {
              if (debug) msg() << "decodeCmmEnergy failed" << endmsg;
              break;
            }
          }
        } else {
          if (debug) msg() << "Invalid CMM type in module field" << endmsg;
          m_rodErr = L1CaloSubBlock::ERROR_MODULE_NUMBER;
          break;
        }
      } else {
        // JEM
        m_jemSubBlock->clear();
        payload = m_jemSubBlock->read(payload, payloadEnd);
        if (m_jemSubBlock->crate() != rodCrate) {
          if (debug) msg() << "Inconsistent crate number in ROD source ID"
                             << endmsg;
          m_rodErr = L1CaloSubBlock::ERROR_CRATE_NUMBER;
          break;
        }
        if (data.m_collection == JET_ELEMENTS || data.m_collection == JET_HITS ||
            data.m_collection == ENERGY_SUMS) {
          decodeJem(m_jemSubBlock, trigJem, data);
          if (m_rodErr != L1CaloSubBlock::ERROR_NONE) {
            if (debug) msg() << "decodeJem failed" << endmsg;
            break;
          }
        }
      }
    }
    if (m_rodErr != L1CaloSubBlock::ERROR_NONE)
      m_errorTool->rodError(robid, m_rodErr);
  }

  return StatusCode::SUCCESS;
}

// Unpack CMM-Energy sub-block

void JepByteStreamTool::decodeCmmEnergy(CmmEnergySubBlock* subBlock,
                                        int trigCmm,
                                        CmmSumsData& data)
{
  const bool debug = msgLvl(MSG::DEBUG);
  if (debug) msg(MSG::DEBUG);

  const int hwCrate    = subBlock->crate();
  const int module     = subBlock->cmmPosition();
  const int firmware   = subBlock->cmmFirmware();
  const int summing    = subBlock->cmmSumming();
  const int timeslices = subBlock->timeslices();
  const int sliceNum   = subBlock->slice();
  if (debug) {
    msg() << "CMM-Energy: Crate " << hwCrate
          << "  Module "          << module
          << "  Firmware "        << firmware
          << "  Summing "         << summing
          << "  Total slices "    << timeslices
          << "  Slice "           << sliceNum    << endmsg;
  }
  if (timeslices <= trigCmm) {
    if (debug) msg() << "Triggered CMM slice from header "
                       << "inconsistent with number of slices: "
                       << trigCmm << ", " << timeslices << endmsg;
    m_rodErr = L1CaloSubBlock::ERROR_SLICES;
    return;
  }
  if (timeslices <= sliceNum) {
    if (debug) msg() << "Total slices inconsistent with slice number: "
                       << timeslices << ", " << sliceNum << endmsg;
    m_rodErr = L1CaloSubBlock::ERROR_SLICES;
    return;
  }
  // Unpack sub-block
  if (subBlock->dataWords() && !subBlock->unpack()) {
    if (debug) {
      std::string errMsg(subBlock->unpackErrorMsg());
      msg() << "CMM-Energy sub-block unpacking failed: " << errMsg << endmsg;
    }
    m_rodErr = subBlock->unpackErrorCode();
    return;
  }

  // Retrieve required data

  const bool neutralFormat = subBlock->format() == L1CaloSubBlock::NEUTRAL;
  const int crate    = hwCrate - m_crateOffsetHw;
  const int swCrate  = crate   + m_crateOffsetSw;
  const int maxSid   = static_cast<int>(CmmEnergySubBlock::MAX_SOURCE_ID);
  LVL1::DataError derr;
  derr.set(LVL1::DataError::SubStatusWord, subBlock->subStatus());
  const int ssError = derr.error();
  const int sliceBeg = ( neutralFormat ) ? 0          : sliceNum;
  const int sliceEnd = ( neutralFormat ) ? timeslices : sliceNum + 1;
  for (int slice = sliceBeg; slice < sliceEnd; ++slice) {

    // Energy sums

    for (int source = 0; source < maxSid; ++source) {
      int dataID = source;
      if (source >= m_modules) {
        if (summing == CmmSubBlock::CRATE &&
            source != CmmEnergySubBlock::LOCAL) continue;
        // coverity[mixed_enums : FALSE]
        // coverity[switch_on_enum : FALSE]
        // coverity[first_enum_type : FALSE]  
        switch (source) {
        case CmmEnergySubBlock::LOCAL:
          dataID = LVL1::CMMEtSums::LOCAL;
          break;
        case CmmEnergySubBlock::REMOTE:
          dataID = LVL1::CMMEtSums::REMOTE;
          break;
        case CmmEnergySubBlock::TOTAL:
          dataID = LVL1::CMMEtSums::TOTAL;
          break;
        default:
          continue;
        }
      }
      const unsigned int ex = subBlock->ex(slice, source);
      const unsigned int ey = subBlock->ey(slice, source);
      const unsigned int et = subBlock->et(slice, source);
      int exErr = subBlock->exError(slice, source);
      int eyErr = subBlock->eyError(slice, source);
      int etErr = subBlock->etError(slice, source);
      LVL1::DataError exErrBits(ssError);
      LVL1::DataError eyErrBits(ssError);
      LVL1::DataError etErrBits(ssError);
      // coverity[mixed_enums : FALSE]
      // coverity[switch_on_enum : FALSE]
      // coverity[first_enum_type : FALSE]
      if (dataID == LVL1::CMMEtSums::LOCAL ||
          dataID == LVL1::CMMEtSums::REMOTE ||
          dataID == LVL1::CMMEtSums::TOTAL) {
        exErrBits.set(LVL1::DataError::Overflow, exErr);
        exErrBits.set(LVL1::DataError::Parity,   exErr >> 1);
        eyErrBits.set(LVL1::DataError::Overflow, eyErr);
        eyErrBits.set(LVL1::DataError::Parity,   eyErr >> 1);
        etErrBits.set(LVL1::DataError::Overflow, etErr);
        etErrBits.set(LVL1::DataError::Parity,   etErr >> 1);
      } else {
        exErrBits.set(LVL1::DataError::Parity, exErr);
        eyErrBits.set(LVL1::DataError::Parity, eyErr);
        etErrBits.set(LVL1::DataError::Parity, etErr);
      }
      exErr = exErrBits.error();
      eyErr = eyErrBits.error();
      etErr = etErrBits.error();
      if (ex || ey || et || exErr || eyErr || etErr) {
        LVL1::CMMEtSums* sums = findCmmSums(data, crate, dataID);
        if ( ! sums ) {   // create new CMM energy sums
          m_exVec.assign(timeslices, 0);
          m_eyVec.assign(timeslices, 0);
          m_etVec.assign(timeslices, 0);
          m_exErrVec.assign(timeslices, 0);
          m_eyErrVec.assign(timeslices, 0);
          m_etErrVec.assign(timeslices, 0);
          m_exVec[slice] = ex;
          m_eyVec[slice] = ey;
          m_etVec[slice] = et;
          m_exErrVec[slice] = exErr;
          m_eyErrVec[slice] = eyErr;
          m_etErrVec[slice] = etErr;
          auto sumsp = 
            std::make_unique<LVL1::CMMEtSums>(swCrate, dataID, m_etVec, m_exVec, m_eyVec,
                                              m_etErrVec, m_exErrVec, m_eyErrVec, trigCmm);
          const int key = crate * 100 + dataID;
          data.m_cmmEtMap.insert(std::make_pair(key, sumsp.get()));
          data.m_cmmEtCollection->push_back(std::move(sumsp));
        } else {
          m_exVec = sums->ExVec();
          m_eyVec = sums->EyVec();
          m_etVec = sums->EtVec();
          m_exErrVec = sums->ExErrorVec();
          m_eyErrVec = sums->EyErrorVec();
          m_etErrVec = sums->EtErrorVec();
          const int nsl = m_exVec.size();
          if (timeslices != nsl) {
            if (debug) msg() << "Inconsistent number of slices in sub-blocks"
                               << endmsg;
            m_rodErr = L1CaloSubBlock::ERROR_SLICES;
            return;
          }
          if (m_exVec[slice] != 0 || m_eyVec[slice] != 0 || m_etVec[slice] != 0 ||
              m_exErrVec[slice] != 0 || m_eyErrVec[slice] != 0 ||
              m_etErrVec[slice] != 0) {
            if (debug) msg() << "Duplicate data for slice " << slice << endmsg;
            m_rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
            return;
          }
          m_exVec[slice] = ex;
          m_eyVec[slice] = ey;
          m_etVec[slice] = et;
          m_exErrVec[slice] = exErr;
          m_eyErrVec[slice] = eyErr;
          m_etErrVec[slice] = etErr;
          sums->addEx(m_exVec, m_exErrVec);
          sums->addEy(m_eyVec, m_eyErrVec);
          sums->addEt(m_etVec, m_etErrVec);
        }
      }
    }

    // Hit maps - store as Et

    if (summing == CmmSubBlock::SYSTEM) {
      const unsigned int missEt = subBlock->missingEtHits(slice);
      if ( missEt || ssError ) {
        const int dataID = LVL1::CMMEtSums::MISSING_ET_MAP;
        LVL1::CMMEtSums* map = findCmmSums(data, crate, dataID);
        if ( ! map ) {
          m_etVec.assign(timeslices, 0);
          m_etErrVec.assign(timeslices, 0);
          m_etVec[slice]    = missEt;
          m_etErrVec[slice] = ssError;
          auto mapp =
            std::make_unique<LVL1::CMMEtSums>(swCrate, dataID,
                                              m_etVec, m_etVec, m_etVec,
                                              m_etErrVec, m_etErrVec, m_etErrVec, trigCmm);
          const int key = crate * 100 + dataID;
          data.m_cmmEtMap.insert(std::make_pair(key, mapp.get()));
          data.m_cmmEtCollection->push_back(std::move(mapp));
        } else {
          m_etVec    = map->EtVec();
          m_etErrVec = map->EtErrorVec();
          const int nsl = m_etVec.size();
          if (timeslices != nsl) {
            if (debug) msg() << "Inconsistent number of slices in sub-blocks"
                               << endmsg;
            m_rodErr = L1CaloSubBlock::ERROR_SLICES;
            return;
          }
          if (m_etVec[slice] != 0 || m_etErrVec[slice] != 0) {
            if (debug) msg() << "Duplicate data for slice " << slice << endmsg;
            m_rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
            return;
          }
          m_etVec[slice]     = missEt;
          m_etErrVec[slice]  = ssError;
          map->addEx(m_etVec, m_etErrVec);
          map->addEy(m_etVec, m_etErrVec);
          map->addEt(m_etVec, m_etErrVec);
        }
      }
      const unsigned int sumEt = subBlock->sumEtHits(slice);
      if ( sumEt || ssError ) {
        const int dataID = LVL1::CMMEtSums::SUM_ET_MAP;
        LVL1::CMMEtSums* map = findCmmSums(data, crate, dataID);
        if ( ! map ) {
          m_etVec.assign(timeslices, 0);
          m_etErrVec.assign(timeslices, 0);
          m_etVec[slice]    = sumEt;
          m_etErrVec[slice] = ssError;
          auto mapp =
            std::make_unique<LVL1::CMMEtSums>(swCrate, dataID,
                                              m_etVec, m_etVec, m_etVec,
                                              m_etErrVec, m_etErrVec, m_etErrVec, trigCmm);
          const int key = crate * 100 + dataID;
          data.m_cmmEtMap.insert(std::make_pair(key, mapp.get()));
          data.m_cmmEtCollection->push_back(std::move(mapp));
        } else {
          m_etVec    = map->EtVec();
          m_etErrVec = map->EtErrorVec();
          const int nsl = m_etVec.size();
          if (timeslices != nsl) {
            if (debug) msg() << "Inconsistent number of slices in sub-blocks"
                               << endmsg;
            m_rodErr = L1CaloSubBlock::ERROR_SLICES;
            return;
          }
          if (m_etVec[slice] != 0 || m_etErrVec[slice] != 0) {
            if (debug) msg() << "Duplicate data for slice " << slice << endmsg;
            m_rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
            return;
          }
          m_etVec[slice]    = sumEt;
          m_etErrVec[slice] = ssError;
          map->addEx(m_etVec, m_etErrVec);
          map->addEy(m_etVec, m_etErrVec);
          map->addEt(m_etVec, m_etErrVec);
        }
      }
      if (subBlock->version() > 1) {
        const unsigned int missEtSig = subBlock->missingEtSigHits(slice);
        if ( missEtSig || ssError ) {
          const int dataID = LVL1::CMMEtSums::MISSING_ET_SIG_MAP;
          LVL1::CMMEtSums* map = findCmmSums(data, crate, dataID);
          if ( ! map ) {
            m_etVec.assign(timeslices, 0);
            m_etErrVec.assign(timeslices, 0);
            m_etVec[slice]    = missEtSig;
            m_etErrVec[slice] = ssError;
            auto mapp = 
              std::make_unique<LVL1::CMMEtSums>(swCrate, dataID,
                                                m_etVec, m_etVec, m_etVec,
                                                m_etErrVec, m_etErrVec, m_etErrVec, trigCmm);
            const int key = crate * 100 + dataID;
            data.m_cmmEtMap.insert(std::make_pair(key, mapp.get()));
            data.m_cmmEtCollection->push_back(std::move(mapp));
          } else {
            m_etVec    = map->EtVec();
            m_etErrVec = map->EtErrorVec();
            const int nsl = m_etVec.size();
            if (timeslices != nsl) {
              if (debug) msg() << "Inconsistent number of slices in sub-blocks"
                                 << endmsg;
              m_rodErr = L1CaloSubBlock::ERROR_SLICES;
              return;
            }
            if (m_etVec[slice] != 0 || m_etErrVec[slice] != 0) {
              if (debug) msg() << "Duplicate data for slice "
                                 << slice << endmsg;
              m_rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
              return;
            }
            m_etVec[slice]    = missEtSig;
            m_etErrVec[slice] = ssError;
            map->addEx(m_etVec, m_etErrVec);
            map->addEy(m_etVec, m_etErrVec);
            map->addEt(m_etVec, m_etErrVec);
          }
        }
      }
    }
  }

  return;
}

// Unpack CMM-Jet sub-block

void JepByteStreamTool::decodeCmmJet(CmmJetSubBlock* subBlock, int trigCmm,
                                     CmmHitsData& data)
{
  const bool debug = msgLvl(MSG::DEBUG);
  if (debug) msg(MSG::DEBUG);

  const int hwCrate    = subBlock->crate();
  const int module     = subBlock->cmmPosition();
  const int firmware   = subBlock->cmmFirmware();
  const int summing    = subBlock->cmmSumming();
  const int timeslices = subBlock->timeslices();
  const int sliceNum   = subBlock->slice();
  if (debug) {
    msg() << "CMM-Jet: Crate " << hwCrate
          << "  Module "       << module
          << "  Firmware "     << firmware
          << "  Summing "      << summing
          << "  Total slices " << timeslices
          << "  Slice "        << sliceNum    << endmsg;
  }
  if (timeslices <= trigCmm) {
    if (debug) msg() << "Triggered CMM slice from header "
                       << "inconsistent with number of slices: "
                       << trigCmm << ", " << timeslices << endmsg;
    m_rodErr = L1CaloSubBlock::ERROR_SLICES;
    return;
  }
  if (timeslices <= sliceNum) {
    if (debug) msg() << "Total slices inconsistent with slice number: "
                       << timeslices << ", " << sliceNum << endmsg;
    m_rodErr = L1CaloSubBlock::ERROR_SLICES;
    return;
  }
  // Unpack sub-block
  if (subBlock->dataWords() && !subBlock->unpack()) {
    if (debug) {
      std::string errMsg(subBlock->unpackErrorMsg());
      msg() << "CMM-Jet sub-block unpacking failed: " << errMsg << endmsg;
    }
    m_rodErr = subBlock->unpackErrorCode();
    return;
  }

  // Retrieve required data

  const bool neutralFormat = subBlock->format() == L1CaloSubBlock::NEUTRAL;
  const int crate    = hwCrate - m_crateOffsetHw;
  const int swCrate  = crate   + m_crateOffsetSw;
  const int maxSid   = static_cast<int>(CmmJetSubBlock::MAX_SOURCE_ID);
  LVL1::DataError derr;
  derr.set(LVL1::DataError::SubStatusWord, subBlock->subStatus());
  const int ssError = derr.error();
  const int sliceBeg = ( neutralFormat ) ? 0          : sliceNum;
  const int sliceEnd = ( neutralFormat ) ? timeslices : sliceNum + 1;
  for (int slice = sliceBeg; slice < sliceEnd; ++slice) {

    // Jet hit counts

    for (int source = 0; source < maxSid; ++source) {
      int dataID = source;
      if (source >= m_modules) {
        if (summing == CmmSubBlock::CRATE &&
            source != CmmJetSubBlock::LOCAL_MAIN     &&
            source != CmmJetSubBlock::LOCAL_FORWARD) continue;
        switch (source) {
        case CmmJetSubBlock::LOCAL_MAIN:
          dataID = LVL1::CMMJetHits::LOCAL_MAIN;
          break;
        case CmmJetSubBlock::REMOTE_MAIN:
          dataID = LVL1::CMMJetHits::REMOTE_MAIN;
          break;
        case CmmJetSubBlock::TOTAL_MAIN:
          dataID = LVL1::CMMJetHits::TOTAL_MAIN;
          break;
        case CmmJetSubBlock::LOCAL_FORWARD:
          dataID = LVL1::CMMJetHits::LOCAL_FORWARD;
          break;
        case CmmJetSubBlock::REMOTE_FORWARD:
          dataID = LVL1::CMMJetHits::REMOTE_FORWARD;
          break;
        case CmmJetSubBlock::TOTAL_FORWARD:
          dataID = LVL1::CMMJetHits::TOTAL_FORWARD;
          break;
        default:
          continue;
        }
      }
      const unsigned int hits = subBlock->jetHits(slice, source);
      LVL1::DataError errBits(ssError);
      errBits.set(LVL1::DataError::Parity,
                  subBlock->jetHitsError(slice, source));
      const int err = errBits.error();
      if (hits || err) {
        LVL1::CMMJetHits* jh = findCmmHits(data, crate, dataID);
        if ( ! jh ) {   // create new CMM hits
          m_hitsVec.assign(timeslices, 0);
          m_errVec.assign(timeslices, 0);
          m_hitsVec[slice] = hits;
          m_errVec[slice]  = err;
          auto jhp =
            std::make_unique<LVL1::CMMJetHits>(swCrate, dataID, m_hitsVec, m_errVec, trigCmm);
          const int key = crate * 100 + dataID;
          data.m_cmmHitsMap.insert(std::make_pair(key, jhp.get()));
          data.m_cmmHitCollection->push_back(std::move(jhp));
        } else {
          m_hitsVec = jh->HitsVec();
          m_errVec  = jh->ErrorVec();
          const int nsl = m_hitsVec.size();
          if (timeslices != nsl) {
            if (debug) msg() << "Inconsistent number of slices in sub-blocks"
                               << endmsg;
            m_rodErr = L1CaloSubBlock::ERROR_SLICES;
            return;
          }
          if (m_hitsVec[slice] != 0 || m_errVec[slice] != 0) {
            if (debug) msg() << "Duplicate data for slice " << slice << endmsg;
            m_rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
            return;
          }
          m_hitsVec[slice] = hits;
          m_errVec[slice]  = err;
	  jh->addHits(m_hitsVec, m_errVec);
        }
      }
    }

    // Hit map - store as hits

    if (summing == CmmSubBlock::SYSTEM) {
      const unsigned int etMap = subBlock->jetEtMap(slice);
      if ( etMap || ssError ) {
        const int dataID = LVL1::CMMJetHits::ET_MAP;
        LVL1::CMMJetHits* map = findCmmHits(data, crate, dataID);
        if ( ! map ) {
          m_hitsVec.assign(timeslices, 0);
          m_errVec.assign(timeslices, 0);
          m_hitsVec[slice] = etMap;
          m_errVec[slice]  = ssError;
          auto mapp =
            std::make_unique<LVL1::CMMJetHits>(swCrate, dataID, m_hitsVec, m_errVec, trigCmm);
          const int key = crate * 100 + dataID;
          data.m_cmmHitsMap.insert(std::make_pair(key, mapp.get()));
          data.m_cmmHitCollection->push_back(std::move(mapp));
        } else {
          m_hitsVec = map->HitsVec();
          m_errVec  = map->ErrorVec();
          const int nsl = m_hitsVec.size();
          if (timeslices != nsl) {
            if (debug) msg() << "Inconsistent number of slices in sub-blocks"
                               << endmsg;
            m_rodErr = L1CaloSubBlock::ERROR_SLICES;
            return;
          }
          if (m_hitsVec[slice] != 0 || m_errVec[slice] != 0) {
            if (debug) msg() << "Duplicate data for slice " << slice << endmsg;
            m_rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
            return;
          }
          m_hitsVec[slice] = etMap;
          m_errVec[slice]  = ssError;
	  map->addHits(m_hitsVec, m_errVec);
        }
      }
    }
  }

  return;
}

// Unpack JEM sub-block

void JepByteStreamTool::decodeJem(JemSubBlock* subBlock, int trigJem,
                                  JepByteStreamToolData& data)
{
  const bool debug   = msgLvl(MSG::DEBUG);
  const bool verbose = msgLvl(MSG::VERBOSE);
  if (debug) msg(MSG::DEBUG);

  const int hwCrate    = subBlock->crate();
  const int module     = subBlock->module();
  const int timeslices = subBlock->timeslices();
  const int sliceNum   = subBlock->slice();
  if (debug) {
    msg() << "JEM: Crate "     << hwCrate
          << "  Module "       << module
          << "  Total slices " << timeslices
          << "  Slice "        << sliceNum    << endmsg;
  }
  if (timeslices <= trigJem) {
    if (debug) msg() << "Triggered JEM slice from header "
                       << "inconsistent with number of slices: "
                       << trigJem << ", " << timeslices << endmsg;
    m_rodErr = L1CaloSubBlock::ERROR_SLICES;
    return;
  }
  if (timeslices <= sliceNum) {
    if (debug) msg() << "Total slices inconsistent with slice number: "
                       << timeslices << ", " << sliceNum << endmsg;
    m_rodErr = L1CaloSubBlock::ERROR_SLICES;
    return;
  }
  // Unpack sub-block
  if (subBlock->dataWords() && !subBlock->unpack()) {
    if (debug) {
      std::string errMsg(subBlock->unpackErrorMsg());
      msg() << "JEM sub-block unpacking failed: " << errMsg << endmsg;
    }
    m_rodErr = subBlock->unpackErrorCode();
    return;
  }

  // Retrieve required data

  const bool neutralFormat = subBlock->format() == L1CaloSubBlock::NEUTRAL;
  const int crate    = hwCrate - m_crateOffsetHw;
  const int swCrate  = crate   + m_crateOffsetSw;
  LVL1::DataError derr;
  derr.set(LVL1::DataError::SubStatusWord, subBlock->subStatus());
  const int ssError = derr.error();
  std::vector<int> dummy(timeslices);
  const int sliceBeg = ( neutralFormat ) ? 0          : sliceNum;
  const int sliceEnd = ( neutralFormat ) ? timeslices : sliceNum + 1;
  for (int slice = sliceBeg; slice < sliceEnd; ++slice) {

    if (data.m_collection == JET_ELEMENTS) {
      JetElementData& jedata = static_cast<JetElementData&>(data);

      // Loop over jet element channels and fill jet elements

      for (int chan = 0; chan < m_channels; ++chan) {
        const JemJetElement jetEle(subBlock->jetElement(slice, chan));
        if (jetEle.data() || ssError) {
          double eta = 0.;
          double phi = 0.;
          int layer = 0;
          if (m_jemMaps->mapping(crate, module, chan, eta, phi, layer)) {
            if (layer == m_coreOverlap) {
              LVL1::JetElement* je = findJetElement(jedata, eta, phi);
              if ( ! je ) {   // create new jet element
                const unsigned int key = m_elementKey->jeKey(phi, eta);
                auto jep = 
                  std::make_unique<LVL1::JetElement>(phi, eta, dummy, dummy, key,
                                                     dummy, dummy, dummy, trigJem);
                je = jep.get();
                jedata.m_jeMap.insert(std::make_pair(key, jep.get()));
                jedata.m_jeCollection->push_back(std::move(jep));
              } else {
                const std::vector<int>& emEnergy(je->emEnergyVec());
                const std::vector<int>& hadEnergy(je->hadEnergyVec());
                const std::vector<int>& emError(je->emErrorVec());
                const std::vector<int>& hadError(je->hadErrorVec());
                const int nsl = emEnergy.size();
                if (timeslices != nsl) {
                  if (debug) {
                    msg() << "Inconsistent number of slices in sub-blocks"
                          << endmsg;
                  }
                  m_rodErr = L1CaloSubBlock::ERROR_SLICES;
                  return;
                }
                if (emEnergy[slice] != 0 || hadEnergy[slice] != 0 ||
                    emError[slice]  != 0 || hadError[slice]  != 0) {
                  if (debug) msg() << "Duplicate data for slice "
                                     << slice << endmsg;
                  m_rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
                  return;
                }
              }
              LVL1::DataError emErrBits(ssError);
              LVL1::DataError hadErrBits(ssError);
              const int linkError = jetEle.linkError();
              emErrBits.set(LVL1::DataError::Parity, jetEle.emParity());
              emErrBits.set(LVL1::DataError::LinkDown, linkError);
              hadErrBits.set(LVL1::DataError::Parity, jetEle.hadParity());
              hadErrBits.set(LVL1::DataError::LinkDown, linkError >> 1);
	      je->addSlice(slice, jetEle.emData(), jetEle.hadData(),
                           emErrBits.error(), hadErrBits.error(),
                           linkError);
            }
          } else if (verbose && jetEle.data()) {
            msg(MSG::VERBOSE) << "Non-zero data but no channel mapping for channel "
                              << chan << endmsg;
            msg(MSG::DEBUG);
          }
        } else if (verbose) {
          msg(MSG::VERBOSE) << "No jet element data for channel "
                            << chan << " slice " << slice << endmsg;
          msg(MSG::DEBUG);
        }
      }
    } else if (data.m_collection == JET_HITS) {
      JetHitsData& jhdata = static_cast<JetHitsData&>(data);

      // Get jet hits

      const unsigned int hits = subBlock->jetHits(slice);
      if (hits) {
        LVL1::JEMHits* jh = findJetHits(jhdata, crate, module);
        if ( ! jh ) {   // create new jet hits
          m_hitsVec.assign(timeslices, 0);
          m_hitsVec[slice] = hits;
          auto jhp =
            std::make_unique<LVL1::JEMHits>(swCrate, module, m_hitsVec, trigJem);
          jhdata.m_hitsMap.insert(std::make_pair(crate * m_modules + module, jhp.get()));
          jhdata.m_hitCollection->push_back(std::move(jhp));
        } else {
          m_hitsVec = jh->JetHitsVec();
          const int nsl = m_hitsVec.size();
          if (timeslices != nsl) {
            if (debug) {
              msg() << "Inconsistent number of slices in sub-blocks"
                    << endmsg;
            }
            m_rodErr = L1CaloSubBlock::ERROR_SLICES;
            return;
          }
          if (m_hitsVec[slice] != 0) {
            if (debug) msg() << "Duplicate data for slice "
                               << slice << endmsg;
            m_rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
            return;
          }
          m_hitsVec[slice] = hits;
	  jh->addJetHits(m_hitsVec);
        }
      } else if (verbose) {
        msg(MSG::VERBOSE) << "No jet hits data for crate/module/slice "
                          << hwCrate << "/" << module << "/" << slice
                          << endmsg;
        msg(MSG::DEBUG);
      }
    } else if (data.m_collection == ENERGY_SUMS) {
      EnergySumsData& sumdata = static_cast<EnergySumsData&>(data);

      // Get energy subsums

      const unsigned int ex = subBlock->ex(slice);
      const unsigned int ey = subBlock->ey(slice);
      const unsigned int et = subBlock->et(slice);
      if (ex | ey | et) {
        LVL1::JEMEtSums* sums = findEnergySums(sumdata, crate, module);
        if ( ! sums ) {   // create new energy sums
          m_exVec.assign(timeslices, 0);
          m_eyVec.assign(timeslices, 0);
          m_etVec.assign(timeslices, 0);
          m_exVec[slice] = ex;
          m_eyVec[slice] = ey;
          m_etVec[slice] = et;
          auto sumsp =
            std::make_unique<LVL1::JEMEtSums>(swCrate, module, m_etVec, m_exVec, m_eyVec,
                                              trigJem);
          sumdata.m_etMap.insert(std::make_pair(crate * m_modules + module, sumsp.get()));
          sumdata.m_etCollection->push_back(std::move(sumsp));
        } else {
          m_exVec = sums->ExVec();
          m_eyVec = sums->EyVec();
          m_etVec = sums->EtVec();
          const int nsl = m_exVec.size();
          if (timeslices != nsl) {
            if (debug) {
              msg() << "Inconsistent number of slices in sub-blocks"
                    << endmsg;
            }
            m_rodErr = L1CaloSubBlock::ERROR_SLICES;
            return;
          }
          if (m_exVec[slice] != 0 || m_eyVec[slice] != 0 || m_etVec[slice] != 0) {
            if (debug) msg() << "Duplicate data for slice "
                               << slice << endmsg;
            m_rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
            return;
          }
          m_exVec[slice] = ex;
          m_eyVec[slice] = ey;
          m_etVec[slice] = et;
          sums->addEx(m_exVec);
          sums->addEy(m_eyVec);
          sums->addEt(m_etVec);
        }
      } else if (verbose) {
        msg(MSG::VERBOSE) << "No energy sums data for crate/module/slice "
                          << hwCrate << "/" << module << "/" << slice
                          << endmsg;
        msg(MSG::DEBUG);
      }
    }
  }
  return;
}

// Find a jet element given eta, phi

const 
LVL1::JetElement* JepByteStreamTool::findJetElement(const double eta,
                                                    const double phi) const
{
  const unsigned int key = m_elementKey->jeKey(phi, eta);
  ConstJetElementMap::const_iterator mapIter = m_jeMap.find(key);
  if (mapIter != m_jeMap.end()) return mapIter->second;
  return nullptr;
}

LVL1::JetElement* JepByteStreamTool::findJetElement(const JetElementData& data,
                                                    const double eta,
                                                    const double phi) const
{
  const unsigned int key = m_elementKey->jeKey(phi, eta);
  JetElementMap::const_iterator mapIter = data.m_jeMap.find(key);
  if (mapIter != data.m_jeMap.end()) return mapIter->second;
  return nullptr;
}

// Find jet hits for given crate, module

const
LVL1::JEMHits* JepByteStreamTool::findJetHits(const int crate,
                                              const int module) const
{
  ConstJetHitsMap::const_iterator mapIter = m_hitsMap.find(crate * m_modules + module);
  if (mapIter != m_hitsMap.end()) return mapIter->second;
  return nullptr;
}

LVL1::JEMHits* JepByteStreamTool::findJetHits(const JetHitsData& data,
                                              const int crate,
                                              const int module) const
{
  JetHitsMap::const_iterator mapIter = data.m_hitsMap.find(crate * m_modules + module);
  if (mapIter != data.m_hitsMap.end()) return mapIter->second;
  return nullptr;
}

// Find energy sums for given crate, module

const
LVL1::JEMEtSums* JepByteStreamTool::findEnergySums(const int crate,
                                                   const int module) const
{
  ConstEnergySumsMap::const_iterator mapIter = m_etMap.find(crate * m_modules + module);
  if (mapIter != m_etMap.end()) return mapIter->second;
  return nullptr;
}

LVL1::JEMEtSums* JepByteStreamTool::findEnergySums(const EnergySumsData& data,
                                                   const int crate,
                                                   const int module) const
{
  EnergySumsMap::const_iterator mapIter = data.m_etMap.find(crate * m_modules + module);
  if (mapIter != data.m_etMap.end()) return mapIter->second;
  return nullptr;
}

// Find CMM hits for given crate, dataID

const
LVL1::CMMJetHits* JepByteStreamTool::findCmmHits(const int crate,
                                                 const int dataID) const
{
  ConstCmmHitsMap::const_iterator mapIter = m_cmmHitsMap.find(crate * 100 + dataID);
  if (mapIter != m_cmmHitsMap.end()) return mapIter->second;
  return nullptr;
}

LVL1::CMMJetHits* JepByteStreamTool::findCmmHits(const CmmHitsData& data,
                                                 const int crate,
                                                 const int dataID) const
{
  CmmHitsMap::const_iterator mapIter = data.m_cmmHitsMap.find(crate * 100 + dataID);
  if (mapIter != data.m_cmmHitsMap.end()) return mapIter->second;
  return nullptr;
}

// Find CMM energy sums for given crate, module, dataID

const
LVL1::CMMEtSums* JepByteStreamTool::findCmmSums(const int crate,
                                                const int dataID) const
{
  ConstCmmSumsMap::const_iterator mapIter = m_cmmEtMap.find(crate * 100 + dataID);
  if (mapIter != m_cmmEtMap.end()) return mapIter->second;
  return nullptr;
}

LVL1::CMMEtSums* JepByteStreamTool::findCmmSums(const CmmSumsData& data,
                                                const int crate,
                                                const int dataID) const
{
  CmmSumsMap::const_iterator mapIter = data.m_cmmEtMap.find(crate * 100 + dataID);
  if (mapIter != data.m_cmmEtMap.end()) return mapIter->second;
  return nullptr;
}

// Set up jet element map

void JepByteStreamTool::setupJeMap(const JetElementCollection*
                                   const jeCollection)
{
  m_jeMap.clear();
  if (jeCollection) {
    JetElementCollection::const_iterator pos  = jeCollection->begin();
    JetElementCollection::const_iterator pose = jeCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::JetElement* const je = *pos;
      const unsigned int key = m_elementKey->jeKey(je->phi(), je->eta());
      m_jeMap.insert(std::make_pair(key, je));
    }
  }
}

// Set up jet hits map

void JepByteStreamTool::setupHitsMap(const JetHitsCollection*
                                     const hitCollection)
{
  m_hitsMap.clear();
  if (hitCollection) {
    JetHitsCollection::const_iterator pos  = hitCollection->begin();
    JetHitsCollection::const_iterator pose = hitCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::JEMHits* const hits = *pos;
      const int crate = hits->crate() - m_crateOffsetSw;
      const int key   = m_modules * crate + hits->module();
      m_hitsMap.insert(std::make_pair(key, hits));
    }
  }
}

// Set up energy sums map

void JepByteStreamTool::setupEtMap(const EnergySumsCollection*
                                   const etCollection)
{
  m_etMap.clear();
  if (etCollection) {
    EnergySumsCollection::const_iterator pos  = etCollection->begin();
    EnergySumsCollection::const_iterator pose = etCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::JEMEtSums* const sums = *pos;
      const int crate = sums->crate() - m_crateOffsetSw;
      const int key   = m_modules * crate + sums->module();
      m_etMap.insert(std::make_pair(key, sums));
    }
  }
}

// Set up CMM hits map

void JepByteStreamTool::setupCmmHitsMap(const CmmHitsCollection*
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

void JepByteStreamTool::setupCmmEtMap(const CmmSumsCollection*
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

// Get number of slices and triggered slice offset for next slink

bool JepByteStreamTool::slinkSlices(const int crate, const int module,
                                    const int modulesPerSlink, int& timeslices, int& trigJem)
{
  int slices = -1;
  int trigJ  = m_dfltSlices / 2;
  for (int mod = module; mod < module + modulesPerSlink; ++mod) {
    for (int chan = 0; chan < m_channels; ++chan) {
      double eta = 0.;
      double phi = 0.;
      int layer = 0;
      if ( !m_jemMaps->mapping(crate, mod, chan, eta, phi, layer)) continue;
      const LVL1::JetElement* const je = findJetElement(eta, phi);
      if ( !je ) continue;
      const int numdat = 5;
      std::vector<int> sums(numdat);
      std::vector<int> sizes(numdat);
      sums[0] = std::accumulate((je->emEnergyVec()).begin(),
                                (je->emEnergyVec()).end(), 0);
      sums[1] = std::accumulate((je->hadEnergyVec()).begin(),
                                (je->hadEnergyVec()).end(), 0);
      sums[2] = std::accumulate((je->emErrorVec()).begin(),
                                (je->emErrorVec()).end(), 0);
      sums[3] = std::accumulate((je->hadErrorVec()).begin(),
                                (je->hadErrorVec()).end(), 0);
      sums[4] = std::accumulate((je->linkErrorVec()).begin(),
                                (je->linkErrorVec()).end(), 0);
      sizes[0] = (je->emEnergyVec()).size();
      sizes[1] = (je->hadEnergyVec()).size();
      sizes[2] = (je->emErrorVec()).size();
      sizes[3] = (je->hadErrorVec()).size();
      sizes[4] = (je->linkErrorVec()).size();
      const int peak = je->peak();
      for (int i = 0; i < numdat; ++i) {
        if (sums[i] == 0) continue;
        if (slices < 0) {
          slices = sizes[i];
          trigJ  = peak;
        } else if (slices != sizes[i] || trigJ != peak) return false;
      }
    }
    const LVL1::JEMHits* const hits = findJetHits(crate, mod);
    if (hits) {
      const unsigned int sum = std::accumulate((hits->JetHitsVec()).begin(),
                               (hits->JetHitsVec()).end(), 0);
      if (sum) {
        const int size = (hits->JetHitsVec()).size();
        const int peak = hits->peak();
        if (slices < 0) {
          slices = size;
          trigJ  = peak;
        } else if (slices != size || trigJ != peak) return false;
      }
    }
    const LVL1::JEMEtSums* const et = findEnergySums(crate, mod);
    if (et) {
      const int numdat = 3;
      std::vector<unsigned int> sums(numdat);
      std::vector<int> sizes(numdat);
      sums[0] = std::accumulate((et->ExVec()).begin(),
                                (et->ExVec()).end(), 0);
      sums[1] = std::accumulate((et->EyVec()).begin(),
                                (et->EyVec()).end(), 0);
      sums[2] = std::accumulate((et->EtVec()).begin(),
                                (et->EtVec()).end(), 0);
      sizes[0] = (et->ExVec()).size();
      sizes[1] = (et->EyVec()).size();
      sizes[2] = (et->EtVec()).size();
      const int peak = et->peak();
      for (int i = 0; i < numdat; ++i) {
        if (sums[i] == 0) continue;
        if (slices < 0) {
          slices = sizes[i];
          trigJ  = peak;
        } else if (slices != sizes[i] || trigJ != peak) return false;
      }
    }
  }
  // CMM last slink of crate
  if (module / modulesPerSlink == m_slinks - 1) {
    const int maxDataID1 = LVL1::CMMJetHits::MAXID;
    const int maxDataID2 = LVL1::CMMEtSums::MAXID;
    const int maxDataID  = (maxDataID1 > maxDataID2) ? maxDataID1 : maxDataID2;
    for (int dataID = 0; dataID < maxDataID; ++dataID) {
      const int numdat = 6;
      std::vector<unsigned int> sums(numdat);
      std::vector<int> sizes(numdat);
      const LVL1::CMMJetHits* hits = 0;
      if (dataID < maxDataID1) hits = findCmmHits(crate, dataID);
      if (hits) {
        sums[0] = std::accumulate((hits->HitsVec()).begin(),
                                  (hits->HitsVec()).end(), 0);
        sums[1] = std::accumulate((hits->ErrorVec()).begin(),
                                  (hits->ErrorVec()).end(), 0);
        sizes[0] = (hits->HitsVec()).size();
        sizes[1] = (hits->ErrorVec()).size();
        const int peak = hits->peak();
        for (int i = 0; i < 2; ++i) {
          if (sums[i] == 0) continue;
          if (slices < 0) {
            slices = sizes[i];
            trigJ  = peak;
          } else if (slices != sizes[i] || trigJ != peak) return false;
        }
      }
      const LVL1::CMMEtSums* et = 0;
      if (dataID < maxDataID2) et = findCmmSums(crate, dataID);
      if (et) {
        sums[0] = std::accumulate((et->ExVec()).begin(),
                                  (et->ExVec()).end(), 0);
        sums[1] = std::accumulate((et->EyVec()).begin(),
                                  (et->EyVec()).end(), 0);
        sums[2] = std::accumulate((et->EtVec()).begin(),
                                  (et->EtVec()).end(), 0);
        sums[3] = std::accumulate((et->ExErrorVec()).begin(),
                                  (et->ExErrorVec()).end(), 0);
        sums[4] = std::accumulate((et->EyErrorVec()).begin(),
                                  (et->EyErrorVec()).end(), 0);
        sums[5] = std::accumulate((et->EtErrorVec()).begin(),
                                  (et->EtErrorVec()).end(), 0);
        sizes[0] = (et->ExVec()).size();
        sizes[1] = (et->EyVec()).size();
        sizes[2] = (et->EtVec()).size();
        sizes[3] = (et->ExErrorVec()).size();
        sizes[4] = (et->EyErrorVec()).size();
        sizes[5] = (et->EtErrorVec()).size();
        const int peak = et->peak();
        for (int i = 0; i < numdat; ++i) {
          if (sums[i] == 0) continue;
          if (slices < 0) {
            slices = sizes[i];
            trigJ  = peak;
          } else if (slices != sizes[i] || trigJ != peak) return false;
        }
      }
    }
  }
  if (slices < 0) slices = m_dfltSlices;
  timeslices = slices;
  trigJem    = trigJ;
  return true;
}

} // end namespace
