/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "TrigT1CaloEvent/JEPBSCollectionV1.h"
#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1CaloUtils/JetElementKey.h"
#include "TrigT1CaloToolInterfaces/IL1CaloMappingTool.h"

#include "CmmEnergySubBlock.h"
#include "CmmJetSubBlock.h"
#include "CmmSubBlock.h"
#include "JemJetElement.h"
#include "JemSubBlockV1.h"
#include "L1CaloErrorByteStreamTool.h"
#include "L1CaloSrcIdMap.h"
#include "L1CaloSubBlock.h"
#include "L1CaloUserHeader.h"
#include "ModifySlices.h"

#include "JepByteStreamV1Tool.h"

namespace LVL1BS {

// Interface ID

static const InterfaceID IID_IJepByteStreamV1Tool("JepByteStreamV1Tool", 1, 1);

const InterfaceID& JepByteStreamV1Tool::interfaceID()
{
  return IID_IJepByteStreamV1Tool;
}

// Constructor

JepByteStreamV1Tool::JepByteStreamV1Tool(const std::string& type,
    const std::string& name,
    const IInterface*  parent)
  : AthAlgTool(type, name, parent),
    m_jemMaps("LVL1::JemMappingTool/JemMappingTool"),
    m_errorTool("LVL1BS::L1CaloErrorByteStreamTool/L1CaloErrorByteStreamTool"),
    m_channels(44), m_crates(2), m_modules(16), 
    m_subDetector(eformat::TDAQ_CALO_JET_PROC_DAQ)
{
  declareInterface<JepByteStreamV1Tool>(this);

  declareProperty("JemMappingTool", m_jemMaps,
                  "Crate/Module/Channel to Eta/Phi/Layer mapping tool");
  declareProperty("ErrorTool", m_errorTool,
                  "Tool to collect errors for monitoring");

  declareProperty("CrateOffsetHw",  m_crateOffsetHw = 12,
                  "Offset of JEP crate numbers in bytestream");
  declareProperty("CrateOffsetSw",  m_crateOffsetSw = 0,
                  "Offset of JEP crate numbers in RDOs");
  declareProperty("SlinksPerCrate", m_slinks        = 4,
                  "The number of S-Links per crate");

  // Properties for reading bytestream only
  declareProperty("ROBSourceIDs",       m_sourceIDsProp,
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
  declareProperty("CrateMin",       m_crateMin = 0,
                  "Minimum crate number, allows partial output");
  declareProperty("CrateMax",       m_crateMax = m_crates - 1,
                  "Maximum crate number, allows partial output");

}

// Destructor

JepByteStreamV1Tool::~JepByteStreamV1Tool()
{
}

// Initialize


StatusCode JepByteStreamV1Tool::initialize()
{
  ATH_MSG_INFO ("Initializing " << name());

  ATH_CHECK( m_jemMaps.retrieve() );
  ATH_CHECK(  m_errorTool.retrieve() );
  ATH_CHECK( m_byteStreamCnvSvc.retrieve() );

  return StatusCode::SUCCESS;
}

// Finalize

StatusCode JepByteStreamV1Tool::finalize()
{
  return StatusCode::SUCCESS;
}

// Conversion bytestream to jet elements

StatusCode JepByteStreamV1Tool::convert(
  const std::string& sgKey,
  const IROBDataProviderSvc::VROBFRAG& robFrags,
  DataVector<LVL1::JetElement>* const jeCollection) const
{
  JetElementData data (jeCollection);
  return convertBs(sgKey, robFrags, data);
}

// Conversion bytestream to jet hits

StatusCode JepByteStreamV1Tool::convert(
  const std::string& sgKey,
  const IROBDataProviderSvc::VROBFRAG& robFrags,
  DataVector<LVL1::JEMHits>* const hitCollection) const
{
  JetHitsData data (hitCollection);
  return convertBs(sgKey, robFrags, data);
}

// Conversion bytestream to energy sums

StatusCode JepByteStreamV1Tool::convert(
  const std::string& sgKey,
  const IROBDataProviderSvc::VROBFRAG& robFrags,
  DataVector<LVL1::JEMEtSums>* const etCollection) const
{
  EnergySumsData data (etCollection);
  return convertBs(sgKey, robFrags, data);
}

// Conversion bytestream to CMM hits

StatusCode JepByteStreamV1Tool::convert(
  const std::string& sgKey,
  const IROBDataProviderSvc::VROBFRAG& robFrags,
  DataVector<LVL1::CMMJetHits>* const hitCollection) const
{
  CmmHitsData data (hitCollection);
  return convertBs(sgKey, robFrags, data);
}

// Conversion bytestream to CMM energy sums

StatusCode JepByteStreamV1Tool::convert(
  const std::string& sgKey,
  const IROBDataProviderSvc::VROBFRAG& robFrags,
  DataVector<LVL1::CMMEtSums>* const etCollection) const
{
  CmmSumsData data (etCollection);
  return convertBs(sgKey, robFrags, data);
}

// Conversion of JEP container to bytestream

StatusCode JepByteStreamV1Tool::convert(const LVL1::JEPBSCollectionV1* const jep) const
{
  const bool debug = msgLvl(MSG::DEBUG);
  if (debug) msg(MSG::DEBUG);

  // Get the event assembler
  FullEventAssembler<L1CaloSrcIdMap>* fea = nullptr;
  ATH_CHECK( m_byteStreamCnvSvc->getFullEventAssembler (fea,
                                                        "JepByteStreamV1") );
  const uint16_t minorVersion = m_srcIdMap.minorVersionPreLS1();
  fea->setRodMinorVersion(minorVersion);

  // Pointer to ROD data vector

  FullEventAssembler<L1CaloSrcIdMap>::RODDATA* theROD = 0;

  // Jet element key provider
  LVL1::JetElementKey elementKey;

  // Set up the container maps

  // Jet element map
  ConstJetElementMap jeMap;
  setupJeMap(jep->JetElements(), jeMap, elementKey);

  // Jet hits map
  ConstJetHitsMap hitsMap;
  setupHitsMap(jep->JetHits(), hitsMap);

  // Energy sums map
  ConstEnergySumsMap etMap;
  setupEtMap(jep->EnergySums(), etMap);

  // CMM hits map
  ConstCmmHitsMap cmmHitsMap;
  setupCmmHitsMap(jep->CmmHits(), cmmHitsMap);

  // CMM energy sums map
  ConstCmmSumsMap cmmEtMap;
  setupCmmEtMap(jep->CmmSums(), cmmEtMap);

  // Loop over data

  const bool neutralFormat = m_dataFormat == L1CaloSubBlock::NEUTRAL;
  const int modulesPerSlink = m_modules / m_slinks;
  int timeslices       = 1;
  int trigJem          = 0;
  int timeslicesNew    = 1;
  int trigJemNew       = 0;
  for (int crate = m_crateMin; crate <= m_crateMax; ++crate) {
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
                           timeslices, trigJem,
                           jeMap,
                           hitsMap,
                           etMap,
                           cmmHitsMap,
                           cmmEtMap,
                           elementKey))
        {
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
        const uint32_t rodIdJem = m_srcIdMap.getRodID(hwCrate, slink, daqOrRoi,
                                  m_subDetector);
        theROD = fea->getRodData(rodIdJem);
        theROD->push_back(userHeader.header());
      }
      if (debug) msg() << "Module " << module << endmsg;

      // Create a sub-block for each slice (except Neutral format)

      // Vector for current JEM sub-blocks
      DataVector<JemSubBlockV1> jemBlocks;

      for (int slice = 0; slice < timeslicesNew; ++slice) {
        JemSubBlockV1* const subBlock = new JemSubBlockV1();
        subBlock->setJemHeader(m_version, m_dataFormat, slice,
                               hwCrate, module, timeslicesNew);
        jemBlocks.push_back(subBlock);
        if (neutralFormat) break;
      }

      // Find jet elements corresponding to each eta/phi pair and fill
      // sub-blocks

      for (int chan = 0; chan < m_channels; ++chan) {
        double eta = 0.;
        double phi = 0.;
        int layer = 0;
        if (m_jemMaps->mapping(crate, module, chan, eta, phi, layer)) {
          const LVL1::JetElement* const je = findJetElement(eta, phi, jeMap, elementKey);
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
              JemSubBlockV1* const subBlock = jemBlocks[index];
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

      const LVL1::JEMHits* const hits = findJetHits(crate, module, hitsMap);
      if (hits) {
        std::vector<unsigned int> vec;
        ModifySlices::data(hits->JetHitsVec(), vec, timeslicesNew);
        for (int slice = 0; slice < timeslicesNew; ++slice) {
          const int index = ( neutralFormat ) ? 0 : slice;
          JemSubBlockV1* const subBlock = jemBlocks[index];
          subBlock->setJetHits(slice, vec[slice]);
        }
      }
      const LVL1::JEMEtSums* const et = findEnergySums(crate, module, etMap);
      if (et) {
        std::vector<unsigned int> exVec;
        std::vector<unsigned int> eyVec;
        std::vector<unsigned int> etVec;
        ModifySlices::data(et->ExVec(), exVec, timeslicesNew);
        ModifySlices::data(et->EyVec(), eyVec, timeslicesNew);
        ModifySlices::data(et->EtVec(), etVec, timeslicesNew);
        for (int slice = 0; slice < timeslicesNew; ++slice) {
          const int index = ( neutralFormat ) ? 0 : slice;
          JemSubBlockV1* const subBlock = jemBlocks[index];
          subBlock->setEnergySubsums(slice, exVec[slice], eyVec[slice],
                                     etVec[slice]);
        }
      }

      // Pack and write the sub-blocks

      DataVector<JemSubBlockV1>::iterator pos;
      for (pos = jemBlocks.begin(); pos != jemBlocks.end(); ++pos) {
        JemSubBlockV1* const subBlock = *pos;
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

    // Vector for current CMM-Energy sub-blocks
    DataVector<CmmEnergySubBlock> cmmEnergyBlocks;
    // Vector for current CMM-Jet sub-blocks
    DataVector<CmmJetSubBlock> cmmJetBlocks;

    const int summing = (crate == m_crates - 1) ? CmmSubBlock::SYSTEM
                        : CmmSubBlock::CRATE;
    for (int slice = 0; slice < timeslicesNew; ++slice) {
      CmmEnergySubBlock* const enBlock = new CmmEnergySubBlock();
      const int cmmEnergyVersion = 2; // with Missing-ET-Sig
      enBlock->setCmmHeader(cmmEnergyVersion, m_dataFormat, slice, hwCrate,
                            summing, CmmSubBlock::CMM_ENERGY,
                            CmmSubBlock::LEFT, timeslicesNew);
      cmmEnergyBlocks.push_back(enBlock);
      CmmJetSubBlock* const jetBlock = new CmmJetSubBlock();
      jetBlock->setCmmHeader(m_version, m_dataFormat, slice, hwCrate,
                             summing, CmmSubBlock::CMM_JET,
                             CmmSubBlock::RIGHT, timeslicesNew);
      cmmJetBlocks.push_back(jetBlock);
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
      const LVL1::CMMEtSums* const sums = findCmmSums(crate, dataID, cmmEtMap);
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
          CmmEnergySubBlock* const subBlock = cmmEnergyBlocks[index];
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
    pos = cmmEnergyBlocks.begin();
    for (; pos != cmmEnergyBlocks.end(); ++pos) {
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
      const LVL1::CMMJetHits* const ch = findCmmHits(crate, dataID, cmmHitsMap);
      if ( ch ) {
        std::vector<unsigned int> hits;
        std::vector<int> errs;
        ModifySlices::data(ch->HitsVec(),  hits, timeslicesNew);
        ModifySlices::data(ch->ErrorVec(), errs, timeslicesNew);
        for (int slice = 0; slice < timeslicesNew; ++slice) {
          const LVL1::DataError errBits(errs[slice]);
          const int index = ( neutralFormat ) ? 0 : slice;
          CmmJetSubBlock* const subBlock = cmmJetBlocks[index];
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
    jos = cmmJetBlocks.begin();
    for (; jos != cmmJetBlocks.end(); ++jos) {
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

  return StatusCode::SUCCESS;
}

// Return reference to vector with all possible Source Identifiers

std::vector<uint32_t> JepByteStreamV1Tool::makeSourceIDs() const
{
  std::vector<uint32_t> sourceIDs;

  if (!m_sourceIDsProp.empty()) {
    sourceIDs = m_sourceIDsProp;
  }
  else {
    const int maxCrates = m_crates + m_crateOffsetHw;
    const int maxSlinks = m_srcIdMap.maxSlinks();
    for (int hwCrate = m_crateOffsetHw; hwCrate < maxCrates; ++hwCrate)
    {
      for (int slink = 0; slink < maxSlinks; ++slink)
      {
        const int daqOrRoi = 0;
        const uint32_t rodId = m_srcIdMap.getRodID(hwCrate, slink, daqOrRoi,
                                                   m_subDetector);
        const uint32_t robId = m_srcIdMap.getRobID(rodId);
        sourceIDs.push_back(robId);
      }
    }
  }
  return sourceIDs;
}

const std::vector<uint32_t>& JepByteStreamV1Tool::sourceIDs() const
{
  static const std::vector<uint32_t> sourceIDs = makeSourceIDs();
  return sourceIDs;
}

// Convert bytestream to given container type

StatusCode JepByteStreamV1Tool::convertBs(
  const std::string& sgKey,
  const IROBDataProviderSvc::VROBFRAG& robFrags,
  JepByteStreamToolData& data) const
{
  LocalData ld;

  // Check if overlap jet element channels wanted
  const std::string flag("Overlap");
  const std::string::size_type pos = sgKey.find(flag);
  ld.coreOverlap =
    (pos == std::string::npos || pos != sgKey.length() - flag.length()) ? 0 : 1;

  const bool debug = msgLvl(MSG::DEBUG);
  if (debug) msg(MSG::DEBUG);

  // JemSubBlock for unpacking
  JemSubBlockV1 jemSubBlock;
  // CmmEnergySubBlock for unpacking
  CmmEnergySubBlock cmmEnergySubBlock;
  // CmmJetSubBlock for unpacking
  CmmJetSubBlock cmmJetSubBlock;

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
    if (m_srcIdMap.getRobID(sourceID) != robid           ||
        m_srcIdMap.subDet(sourceID)   != m_subDetector   ||
        m_srcIdMap.daqOrRoi(sourceID) != 0               ||
        m_srcIdMap.slink(sourceID)    >= m_slinks        ||
        m_srcIdMap.crate(sourceID)    <  m_crateOffsetHw ||
        m_srcIdMap.crate(sourceID)    >= m_crateOffsetHw + m_crates) {
      m_errorTool->rodError(robid, L1CaloSubBlock::ERROR_ROD_ID);
      if (debug) {
        msg() << "Wrong source identifier in data: ROD "
              << MSG::hex << sourceID << "  ROB " << robid
              << MSG::dec << endmsg;
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

    // First word should be User Header
    if ( !L1CaloUserHeader::isValid(*payload) ) {
      m_errorTool->rodError(robid, L1CaloSubBlock::ERROR_USER_HEADER);
      if (debug) msg() << "Invalid or missing user header" << endmsg;
      continue;
    }
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

    ld.rodErr = L1CaloSubBlock::ERROR_NONE;
    while (payload != payloadEnd) {

      if (L1CaloSubBlock::wordType(*payload) != L1CaloSubBlock::HEADER) {
        if (debug) msg() << "Unexpected data sequence" << endmsg;
        ld.rodErr = L1CaloSubBlock::ERROR_MISSING_HEADER;
        break;
      }
      if (CmmSubBlock::cmmBlock(*payload)) {
        // CMMs
        if (CmmSubBlock::cmmType(*payload) == CmmSubBlock::CMM_JET) {
          cmmJetSubBlock.clear();
          payload = cmmJetSubBlock.read(payload, payloadEnd);
          if (cmmJetSubBlock.crate() != rodCrate) {
            if (debug) msg() << "Inconsistent crate number in ROD source ID"
                               << endmsg;
            ld.rodErr = L1CaloSubBlock::ERROR_CRATE_NUMBER;
            break;
          }
          if (data.m_collection == CMM_HITS) {
            decodeCmmJet(&cmmJetSubBlock, trigCmm, static_cast<CmmHitsData&>(data), ld);
            if (ld.rodErr != L1CaloSubBlock::ERROR_NONE) {
              if (debug) msg() << "decodeCmmJet failed" << endmsg;
              break;
            }
          }
        } else if (CmmSubBlock::cmmType(*payload) == CmmSubBlock::CMM_ENERGY) {
          cmmEnergySubBlock.clear();
          payload = cmmEnergySubBlock.read(payload, payloadEnd);
          if (cmmEnergySubBlock.crate() != rodCrate) {
            if (debug) msg() << "Inconsistent crate number in ROD source ID"
                               << endmsg;
            ld.rodErr = L1CaloSubBlock::ERROR_CRATE_NUMBER;
            break;
          }
          if (data.m_collection == CMM_SUMS) {
            decodeCmmEnergy(&cmmEnergySubBlock, trigCmm, static_cast<CmmSumsData&>(data), ld);
            if (ld.rodErr != L1CaloSubBlock::ERROR_NONE) {
              if (debug) msg() << "decodeCmmEnergy failed" << endmsg;
              break;
            }
          }
        } else {
          if (debug) msg() << "Invalid CMM type in module field" << endmsg;
          ld.rodErr = L1CaloSubBlock::ERROR_MODULE_NUMBER;
          break;
        }
      } else {
        // JEM
        jemSubBlock.clear();
        payload = jemSubBlock.read(payload, payloadEnd);
        if (jemSubBlock.crate() != rodCrate) {
          if (debug) msg() << "Inconsistent crate number in ROD source ID"
                             << endmsg;
          ld.rodErr = L1CaloSubBlock::ERROR_CRATE_NUMBER;
          break;
        }
        if (data.m_collection == JET_ELEMENTS || data.m_collection == JET_HITS ||
            data.m_collection == ENERGY_SUMS) {
          decodeJem(&jemSubBlock, trigJem, data, ld);
          if (ld.rodErr != L1CaloSubBlock::ERROR_NONE) {
            if (debug) msg() << "decodeJem failed" << endmsg;
            break;
          }
        }
      }
    }
    if (ld.rodErr != L1CaloSubBlock::ERROR_NONE)
      m_errorTool->rodError(robid, ld.rodErr);
  }

  return StatusCode::SUCCESS;
}

// Unpack CMM-Energy sub-block

void JepByteStreamV1Tool::decodeCmmEnergy(CmmEnergySubBlock* subBlock,
                                          int trigCmm,
                                          CmmSumsData& data,
                                          LocalData& ld) const
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
    ld.rodErr = L1CaloSubBlock::ERROR_SLICES;
    return;
  }
  if (timeslices <= sliceNum) {
    if (debug) msg() << "Total slices inconsistent with slice number: "
                       << timeslices << ", " << sliceNum << endmsg;
    ld.rodErr = L1CaloSubBlock::ERROR_SLICES;
    return;
  }
  // Unpack sub-block
  if (subBlock->dataWords() && !subBlock->unpack()) {
    if (debug) {
      std::string errMsg(subBlock->unpackErrorMsg());
      msg() << "CMM-Energy sub-block unpacking failed: " << errMsg << endmsg;
    }
    ld.rodErr = subBlock->unpackErrorCode();
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
          ld.exVec.assign(timeslices, 0);
          ld.eyVec.assign(timeslices, 0);
          ld.etVec.assign(timeslices, 0);
          ld.exErrVec.assign(timeslices, 0);
          ld.eyErrVec.assign(timeslices, 0);
          ld.etErrVec.assign(timeslices, 0);
          ld.exVec[slice] = ex;
          ld.eyVec[slice] = ey;
          ld.etVec[slice] = et;
          ld.exErrVec[slice] = exErr;
          ld.eyErrVec[slice] = eyErr;
          ld.etErrVec[slice] = etErr;
          auto sumsp =
            std::make_unique<LVL1::CMMEtSums>(swCrate, dataID, ld.etVec, ld.exVec, ld.eyVec,
                                              ld.etErrVec, ld.exErrVec, ld.eyErrVec, trigCmm);
          const int key = crate * 100 + dataID;
          data.m_cmmEtMap.insert(std::make_pair(key, sumsp.get()));
          data.m_cmmEtCollection->push_back(std::move(sumsp));
        } else {
          ld.exVec = sums->ExVec();
          ld.eyVec = sums->EyVec();
          ld.etVec = sums->EtVec();
          ld.exErrVec = sums->ExErrorVec();
          ld.eyErrVec = sums->EyErrorVec();
          ld.etErrVec = sums->EtErrorVec();
          const int nsl = ld.exVec.size();
          if (timeslices != nsl) {
            if (debug) msg() << "Inconsistent number of slices in sub-blocks"
                               << endmsg;
            ld.rodErr = L1CaloSubBlock::ERROR_SLICES;
            return;
          }
          if (ld.exVec[slice] != 0 || ld.eyVec[slice] != 0 || ld.etVec[slice] != 0 ||
              ld.exErrVec[slice] != 0 || ld.eyErrVec[slice] != 0 ||
              ld.etErrVec[slice] != 0) {
            if (debug) msg() << "Duplicate data for slice " << slice << endmsg;
            ld.rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
            return;
          }
          ld.exVec[slice] = ex;
          ld.eyVec[slice] = ey;
          ld.etVec[slice] = et;
          ld.exErrVec[slice] = exErr;
          ld.eyErrVec[slice] = eyErr;
          ld.etErrVec[slice] = etErr;
          sums->addEx(ld.exVec, ld.exErrVec);
          sums->addEy(ld.eyVec, ld.eyErrVec);
          sums->addEt(ld.etVec, ld.etErrVec);
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
          ld.etVec.assign(timeslices, 0);
          ld.etErrVec.assign(timeslices, 0);
          ld.etVec[slice]    = missEt;
          ld.etErrVec[slice] = ssError;
          auto mapp =
            std::make_unique<LVL1::CMMEtSums>(swCrate, dataID,
                                              ld.etVec, ld.etVec, ld.etVec,
                                              ld.etErrVec, ld.etErrVec, ld.etErrVec, trigCmm);
          const int key = crate * 100 + dataID;
          data.m_cmmEtMap.insert(std::make_pair(key, mapp.get()));
          data.m_cmmEtCollection->push_back(std::move(mapp));
        } else {
          ld.etVec    = map->EtVec();
          ld.etErrVec = map->EtErrorVec();
          const int nsl = ld.etVec.size();
          if (timeslices != nsl) {
            if (debug) msg() << "Inconsistent number of slices in sub-blocks"
                               << endmsg;
            ld.rodErr = L1CaloSubBlock::ERROR_SLICES;
            return;
          }
          if (ld.etVec[slice] != 0 || ld.etErrVec[slice] != 0) {
            if (debug) msg() << "Duplicate data for slice " << slice << endmsg;
            ld.rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
            return;
          }
          ld.etVec[slice]     = missEt;
          ld.etErrVec[slice]  = ssError;
          map->addEx(ld.etVec, ld.etErrVec);
          map->addEy(ld.etVec, ld.etErrVec);
          map->addEt(ld.etVec, ld.etErrVec);
        }
      }
      const unsigned int sumEt = subBlock->sumEtHits(slice);
      if ( sumEt || ssError ) {
        const int dataID = LVL1::CMMEtSums::SUM_ET_MAP;
        LVL1::CMMEtSums* map = findCmmSums(data, crate, dataID);
        if ( ! map ) {
          ld.etVec.assign(timeslices, 0);
          ld.etErrVec.assign(timeslices, 0);
          ld.etVec[slice]    = sumEt;
          ld.etErrVec[slice] = ssError;
          auto mapp =
            std::make_unique<LVL1::CMMEtSums>(swCrate, dataID,
                                              ld.etVec, ld.etVec, ld.etVec,
                                              ld.etErrVec, ld.etErrVec, ld.etErrVec, trigCmm);
          const int key = crate * 100 + dataID;
          data.m_cmmEtMap.insert(std::make_pair(key, mapp.get()));
          data.m_cmmEtCollection->push_back(std::move(mapp));
        } else {
          ld.etVec    = map->EtVec();
          ld.etErrVec = map->EtErrorVec();
          const int nsl = ld.etVec.size();
          if (timeslices != nsl) {
            if (debug) msg() << "Inconsistent number of slices in sub-blocks"
                               << endmsg;
            ld.rodErr = L1CaloSubBlock::ERROR_SLICES;
            return;
          }
          if (ld.etVec[slice] != 0 || ld.etErrVec[slice] != 0) {
            if (debug) msg() << "Duplicate data for slice " << slice << endmsg;
            ld.rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
            return;
          }
          ld.etVec[slice]    = sumEt;
          ld.etErrVec[slice] = ssError;
          map->addEx(ld.etVec, ld.etErrVec);
          map->addEy(ld.etVec, ld.etErrVec);
          map->addEt(ld.etVec, ld.etErrVec);
        }
      }
      if (subBlock->version() > 1) {
        const unsigned int missEtSig = subBlock->missingEtSigHits(slice);
        if ( missEtSig || ssError ) {
          const int dataID = LVL1::CMMEtSums::MISSING_ET_SIG_MAP;
          LVL1::CMMEtSums* map = findCmmSums(data, crate, dataID);
          if ( ! map ) {
            ld.etVec.assign(timeslices, 0);
            ld.etErrVec.assign(timeslices, 0);
            ld.etVec[slice]    = missEtSig;
            ld.etErrVec[slice] = ssError;
            auto mapp =
              std::make_unique<LVL1::CMMEtSums>(swCrate, dataID,
                                                ld.etVec, ld.etVec, ld.etVec,
                                                ld.etErrVec, ld.etErrVec, ld.etErrVec, trigCmm);
            const int key = crate * 100 + dataID;
            data.m_cmmEtMap.insert(std::make_pair(key, mapp.get()));
            data.m_cmmEtCollection->push_back(std::move(mapp));
          } else {
            ld.etVec    = map->EtVec();
            ld.etErrVec = map->EtErrorVec();
            const int nsl = ld.etVec.size();
            if (timeslices != nsl) {
              if (debug) msg() << "Inconsistent number of slices in sub-blocks"
                                 << endmsg;
              ld.rodErr = L1CaloSubBlock::ERROR_SLICES;
              return;
            }
            if (ld.etVec[slice] != 0 || ld.etErrVec[slice] != 0) {
              if (debug) msg() << "Duplicate data for slice "
                                 << slice << endmsg;
              ld.rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
              return;
            }
            ld.etVec[slice]    = missEtSig;
            ld.etErrVec[slice] = ssError;
            map->addEx(ld.etVec, ld.etErrVec);
            map->addEy(ld.etVec, ld.etErrVec);
            map->addEt(ld.etVec, ld.etErrVec);
          }
        }
      }
    }
  }

  return;
}

// Unpack CMM-Jet sub-block

void JepByteStreamV1Tool::decodeCmmJet(CmmJetSubBlock* subBlock, int trigCmm,
                                       CmmHitsData& data,
                                       LocalData& ld) const
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
    ld.rodErr = L1CaloSubBlock::ERROR_SLICES;
    return;
  }
  if (timeslices <= sliceNum) {
    if (debug) msg() << "Total slices inconsistent with slice number: "
                       << timeslices << ", " << sliceNum << endmsg;
    ld.rodErr = L1CaloSubBlock::ERROR_SLICES;
    return;
  }
  // Unpack sub-block
  if (subBlock->dataWords() && !subBlock->unpack()) {
    if (debug) {
      std::string errMsg(subBlock->unpackErrorMsg());
      msg() << "CMM-Jet sub-block unpacking failed: " << errMsg << endmsg;
    }
    ld.rodErr = subBlock->unpackErrorCode();
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
        // coverity[mixed_enums : FALSE]
        // coverity[switch_on_enum : FALSE]
        // coverity[first_enum_type : FALSE]
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
          ld.hitsVec.assign(timeslices, 0);
          ld.errVec.assign(timeslices, 0);
          ld.hitsVec[slice] = hits;
          ld.errVec[slice]  = err;
          auto jhp =
            std::make_unique<LVL1::CMMJetHits>(swCrate, dataID, ld.hitsVec, ld.errVec, trigCmm);
          const int key = crate * 100 + dataID;
          data.m_cmmHitsMap.insert(std::make_pair(key, jhp.get()));
          data.m_cmmHitCollection->push_back(std::move(jhp));
        } else {
          ld.hitsVec = jh->HitsVec();
          ld.errVec  = jh->ErrorVec();
          const int nsl = ld.hitsVec.size();
          if (timeslices != nsl) {
            if (debug) msg() << "Inconsistent number of slices in sub-blocks"
                               << endmsg;
            ld.rodErr = L1CaloSubBlock::ERROR_SLICES;
            return;
          }
          if (ld.hitsVec[slice] != 0 || ld.errVec[slice] != 0) {
            if (debug) msg() << "Duplicate data for slice " << slice << endmsg;
            ld.rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
            return;
          }
          ld.hitsVec[slice] = hits;
          ld.errVec[slice]  = err;
          jh->addHits(ld.hitsVec, ld.errVec);
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
          ld.hitsVec.assign(timeslices, 0);
          ld.errVec.assign(timeslices, 0);
          ld.hitsVec[slice] = etMap;
          ld.errVec[slice]  = ssError;
          auto mapp =
            std::make_unique<LVL1::CMMJetHits>(swCrate, dataID, ld.hitsVec, ld.errVec, trigCmm);
          const int key = crate * 100 + dataID;
          data.m_cmmHitsMap.insert(std::make_pair(key, mapp.get()));
          data.m_cmmHitCollection->push_back(std::move(mapp));
        } else {
          ld.hitsVec = map->HitsVec();
          ld.errVec  = map->ErrorVec();
          const int nsl = ld.hitsVec.size();
          if (timeslices != nsl) {
            if (debug) msg() << "Inconsistent number of slices in sub-blocks"
                               << endmsg;
            ld.rodErr = L1CaloSubBlock::ERROR_SLICES;
            return;
          }
          if (ld.hitsVec[slice] != 0 || ld.errVec[slice] != 0) {
            if (debug) msg() << "Duplicate data for slice " << slice << endmsg;
            ld.rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
            return;
          }
          ld.hitsVec[slice] = etMap;
          ld.errVec[slice]  = ssError;
          map->addHits(ld.hitsVec, ld.errVec);
        }
      }
    }
  }

  return;
}

// Unpack JEM sub-block

void JepByteStreamV1Tool::decodeJem(JemSubBlockV1* subBlock, int trigJem,
                                    JepByteStreamToolData& data,
                                    LocalData& ld) const
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
    ld.rodErr = L1CaloSubBlock::ERROR_SLICES;
    return;
  }
  if (timeslices <= sliceNum) {
    if (debug) msg() << "Total slices inconsistent with slice number: "
                       << timeslices << ", " << sliceNum << endmsg;
    ld.rodErr = L1CaloSubBlock::ERROR_SLICES;
    return;
  }
  // Unpack sub-block
  if (subBlock->dataWords() && !subBlock->unpack()) {
    if (debug) {
      std::string errMsg(subBlock->unpackErrorMsg());
      msg() << "JEM sub-block unpacking failed: " << errMsg << endmsg;
    }
    ld.rodErr = subBlock->unpackErrorCode();
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
            if (layer == ld.coreOverlap) {
	      LVL1::JetElement* je = findJetElement(jedata, eta, phi, ld.elementKey);
              if ( ! je ) {   // create new jet element
                const unsigned int key = ld.elementKey.jeKey(phi, eta);
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
                  ld.rodErr = L1CaloSubBlock::ERROR_SLICES;
                  return;
                }
                if (emEnergy[slice] != 0 || hadEnergy[slice] != 0 ||
                    emError[slice]  != 0 || hadError[slice]  != 0) {
                  if (debug) msg() << "Duplicate data for slice "
                                     << slice << endmsg;
                  ld.rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
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
          ld.hitsVec.assign(timeslices, 0);
          ld.hitsVec[slice] = hits;
          auto jhp =
            std::make_unique<LVL1::JEMHits>(swCrate, module, ld.hitsVec, trigJem);
          jhdata.m_hitsMap.insert(std::make_pair(crate * m_modules + module, jhp.get()));
          jhdata.m_hitCollection->push_back(std::move(jhp));
        } else {
          ld.hitsVec = jh->JetHitsVec();
          const int nsl = ld.hitsVec.size();
          if (timeslices != nsl) {
            if (debug) {
              msg() << "Inconsistent number of slices in sub-blocks"
                    << endmsg;
            }
            ld.rodErr = L1CaloSubBlock::ERROR_SLICES;
            return;
          }
          if (ld.hitsVec[slice] != 0) {
            if (debug) msg() << "Duplicate data for slice "
                               << slice << endmsg;
            ld.rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
            return;
          }
          ld.hitsVec[slice] = hits;
          jh->addJetHits(ld.hitsVec);
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
          ld.exVec.assign(timeslices, 0);
          ld.eyVec.assign(timeslices, 0);
          ld.etVec.assign(timeslices, 0);
          ld.exVec[slice] = ex;
          ld.eyVec[slice] = ey;
          ld.etVec[slice] = et;
          auto sumsp =
            std::make_unique<LVL1::JEMEtSums>(swCrate, module, ld.etVec, ld.exVec, ld.eyVec,
                                              trigJem);
          sumdata.m_etMap.insert(std::make_pair(crate * m_modules + module, sumsp.get()));
          sumdata.m_etCollection->push_back(std::move(sumsp));
        } else {
          ld.exVec = sums->ExVec();
          ld.eyVec = sums->EyVec();
          ld.etVec = sums->EtVec();
          const int nsl = ld.exVec.size();
          if (timeslices != nsl) {
            if (debug) {
              msg() << "Inconsistent number of slices in sub-blocks"
                    << endmsg;
            }
            ld.rodErr = L1CaloSubBlock::ERROR_SLICES;
            return;
          }
          if (ld.exVec[slice] != 0 || ld.eyVec[slice] != 0 || ld.etVec[slice] != 0) {
            if (debug) msg() << "Duplicate data for slice "
                               << slice << endmsg;
            ld.rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
            return;
          }
          ld.exVec[slice] = ex;
          ld.eyVec[slice] = ey;
          ld.etVec[slice] = et;
          sums->addEx(ld.exVec);
          sums->addEy(ld.eyVec);
          sums->addEt(ld.etVec);
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
LVL1::JetElement* JepByteStreamV1Tool::findJetElement(const double eta,
                                                      const double phi,
                                                      const ConstJetElementMap& jeMap,
                                                      LVL1::JetElementKey& elementKey) const
{
  const unsigned int key = elementKey.jeKey(phi, eta);
  ConstJetElementMap::const_iterator mapIter = jeMap.find(key);
  if (mapIter != jeMap.end()) return mapIter->second;
  return nullptr;
}

LVL1::JetElement* JepByteStreamV1Tool::findJetElement(const JetElementData& data,
                                                      const double eta,
                                                      const double phi,
                                                      LVL1::JetElementKey& elementKey) const
{
  const unsigned int key = elementKey.jeKey(phi, eta);
  JetElementMap::const_iterator mapIter = data.m_jeMap.find(key);
  if (mapIter != data.m_jeMap.end()) return mapIter->second;
  return nullptr;
}

// Find jet hits for given crate, module

const
LVL1::JEMHits* JepByteStreamV1Tool::findJetHits(const int crate,
                                                const int module,
                                                const ConstJetHitsMap& hitsMap) const
{
  ConstJetHitsMap::const_iterator mapIter = hitsMap.find(crate * m_modules + module);
  if (mapIter != hitsMap.end()) return mapIter->second;
  return nullptr;
}

LVL1::JEMHits* JepByteStreamV1Tool::findJetHits(const JetHitsData& data,
                                                const int crate,
                                                const int module) const
{
  JetHitsMap::const_iterator mapIter = data.m_hitsMap.find(crate * m_modules + module);
  if (mapIter != data.m_hitsMap.end()) return mapIter->second;
  return nullptr;
}

// Find energy sums for given crate, module

const
LVL1::JEMEtSums* JepByteStreamV1Tool::findEnergySums(const int crate,
                                                     const int module,
                                                     const ConstEnergySumsMap& etMap) const
{
  ConstEnergySumsMap::const_iterator mapIter = etMap.find(crate * m_modules + module);
  if (mapIter != etMap.end()) return mapIter->second;
  return nullptr;
}

LVL1::JEMEtSums* JepByteStreamV1Tool::findEnergySums(const EnergySumsData& data,
                                                     const int crate,
                                                     const int module) const
{
  EnergySumsMap::const_iterator mapIter = data.m_etMap.find(crate * m_modules + module);
  if (mapIter != data.m_etMap.end()) return mapIter->second;
  return nullptr;
}

// Find CMM hits for given crate, dataID

const
LVL1::CMMJetHits* JepByteStreamV1Tool::findCmmHits(const int crate,
                                                   const int dataID,
                                                   const ConstCmmHitsMap& cmmHitsMap) const
{
  ConstCmmHitsMap::const_iterator mapIter = cmmHitsMap.find(crate * 100 + dataID);
  if (mapIter != cmmHitsMap.end()) return mapIter->second;
  return nullptr;
}

LVL1::CMMJetHits* JepByteStreamV1Tool::findCmmHits(const CmmHitsData& data,
                                                   const int crate,
                                                   const int dataID) const
{
  CmmHitsMap::const_iterator mapIter = data.m_cmmHitsMap.find(crate * 100 + dataID);
  if (mapIter != data.m_cmmHitsMap.end()) return mapIter->second;
  return nullptr;
}

// Find CMM energy sums for given crate, module, dataID

const
LVL1::CMMEtSums* JepByteStreamV1Tool::findCmmSums(const int crate,
                                                  const int dataID,
                                                  const ConstCmmSumsMap& cmmEtMap) const
{
  ConstCmmSumsMap::const_iterator mapIter = cmmEtMap.find(crate * 100 + dataID);
  if (mapIter != cmmEtMap.end()) return mapIter->second;
  return nullptr;
}

LVL1::CMMEtSums* JepByteStreamV1Tool::findCmmSums(const CmmSumsData& data,
                                                  const int crate,
                                                  const int dataID) const
{
  CmmSumsMap::const_iterator mapIter = data.m_cmmEtMap.find(crate * 100 + dataID);
  if (mapIter != data.m_cmmEtMap.end()) return mapIter->second;
  return nullptr;
}

// Set up jet element map

void JepByteStreamV1Tool::setupJeMap(const JetElementCollection*
                                     const jeCollection,
                                     ConstJetElementMap& jeMap,
                                     LVL1::JetElementKey& elementKey) const
{
  jeMap.clear();
  if (jeCollection) {
    JetElementCollection::const_iterator pos  = jeCollection->begin();
    JetElementCollection::const_iterator pose = jeCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::JetElement* je = *pos;
      const unsigned int key = elementKey.jeKey(je->phi(), je->eta());
      jeMap.insert(std::make_pair(key, je));
    }
  }
}


// Set up jet hits map

void JepByteStreamV1Tool::setupHitsMap(const JetHitsCollection*
                                       const hitCollection,
                                       ConstJetHitsMap& hitsMap) const
{
  hitsMap.clear();
  if (hitCollection) {
    JetHitsCollection::const_iterator pos  = hitCollection->begin();
    JetHitsCollection::const_iterator pose = hitCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::JEMHits* const hits = *pos;
      const int crate = hits->crate() - m_crateOffsetSw;
      const int key   = m_modules * crate + hits->module();
      hitsMap.insert(std::make_pair(key, hits));
    }
  }
}

// Set up energy sums map

void JepByteStreamV1Tool::setupEtMap(const EnergySumsCollection*
                                     const etCollection,
                                     ConstEnergySumsMap& etMap) const
{
  etMap.clear();
  if (etCollection) {
    EnergySumsCollection::const_iterator pos  = etCollection->begin();
    EnergySumsCollection::const_iterator pose = etCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::JEMEtSums* const sums = *pos;
      const int crate = sums->crate() - m_crateOffsetSw;
      const int key   = m_modules * crate + sums->module();
      etMap.insert(std::make_pair(key, sums));
    }
  }
}

// Set up CMM hits map

void JepByteStreamV1Tool::setupCmmHitsMap(const CmmHitsCollection*
                                          const hitCollection,
                                          ConstCmmHitsMap& cmmHitsMap) const
{
  cmmHitsMap.clear();
  if (hitCollection) {
    CmmHitsCollection::const_iterator pos  = hitCollection->begin();
    CmmHitsCollection::const_iterator pose = hitCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CMMJetHits* const hits = *pos;
      const int crate = hits->crate() - m_crateOffsetSw;
      const int key   = crate * 100 + hits->dataID();
      cmmHitsMap.insert(std::make_pair(key, hits));
    }
  }
}

// Set up CMM energy sums map

void JepByteStreamV1Tool::setupCmmEtMap(const CmmSumsCollection*
                                        const etCollection,
                                        ConstCmmSumsMap& cmmEtMap) const
{
  cmmEtMap.clear();
  if (etCollection) {
    CmmSumsCollection::const_iterator pos  = etCollection->begin();
    CmmSumsCollection::const_iterator pose = etCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CMMEtSums* const sums = *pos;
      const int crate = sums->crate() - m_crateOffsetSw;
      const int key   = crate * 100 + sums->dataID();
      cmmEtMap.insert(std::make_pair(key, sums));
    }
  }
}

// Get number of slices and triggered slice offset for next slink

bool JepByteStreamV1Tool::slinkSlices(const int crate, const int module,
                                      const int modulesPerSlink,
                                      int& timeslices,
                                      int& trigJem,
                                      const ConstJetElementMap& jeMap,
                                      const ConstJetHitsMap& hitsMap,
                                      const ConstEnergySumsMap& etMap,
                                      const ConstCmmHitsMap& cmmHitsMap,
                                      const ConstCmmSumsMap& cmmEtMap,
                                      LVL1::JetElementKey& elementKey) const
{
  int slices = -1;
  int trigJ  = m_dfltSlices / 2;
  for (int mod = module; mod < module + modulesPerSlink; ++mod) {
    for (int chan = 0; chan < m_channels; ++chan) {
      double eta = 0.;
      double phi = 0.;
      int layer = 0;
      if ( !m_jemMaps->mapping(crate, mod, chan, eta, phi, layer)) continue;
      const LVL1::JetElement* const je = findJetElement(eta, phi, jeMap, elementKey);
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
    const LVL1::JEMHits* const hits = findJetHits(crate, mod, hitsMap);
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
    const LVL1::JEMEtSums* const et = findEnergySums(crate, mod, etMap);
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
      if (dataID < maxDataID1) hits = findCmmHits(crate, dataID, cmmHitsMap);
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
      if (dataID < maxDataID2) et = findCmmSums(crate, dataID, cmmEtMap);
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
