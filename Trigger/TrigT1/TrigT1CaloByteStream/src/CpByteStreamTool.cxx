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

#include "TrigT1CaloEvent/CMMCPHits.h"
#include "TrigT1CaloEvent/CPMHits.h"
#include "TrigT1CaloEvent/CPMTower.h"
#include "TrigT1CaloEvent/CPBSCollection.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1CaloUtils/TriggerTowerKey.h"
#include "TrigT1CaloMappingToolInterfaces/IL1CaloMappingTool.h"

#include "CmmCpSubBlock.h"
#include "CmmSubBlock.h"
#include "CpmSubBlock.h"
#include "L1CaloErrorByteStreamTool.h"
#include "L1CaloSrcIdMap.h"
#include "L1CaloSubBlock.h"
#include "L1CaloUserHeader.h"
#include "ModifySlices.h"

#include "CpByteStreamTool.h"

namespace LVL1BS {

// Interface ID

static const InterfaceID IID_ICpByteStreamTool("CpByteStreamTool", 1, 1);

const InterfaceID& CpByteStreamTool::interfaceID()
{
  return IID_ICpByteStreamTool;
}

// Constructor

CpByteStreamTool::CpByteStreamTool(const std::string& type,
                                   const std::string& name,
				   const IInterface*  parent)
   : AthAlgTool(type, name, parent),
     m_cpmMaps("LVL1::CpmMappingTool/CpmMappingTool"),
     m_errorTool("LVL1BS::L1CaloErrorByteStreamTool/L1CaloErrorByteStreamTool"),
     m_channels(80), m_crates(4), m_modules(14),
     m_coreOverlap(0), m_subDetector(eformat::TDAQ_CALO_CLUSTER_PROC_DAQ),
     m_srcIdMap(0), m_towerKey(0), m_cpmSubBlock(0), m_cmmCpSubBlock(0),
     m_rodStatus(0), m_fea(0)
{
  declareInterface<CpByteStreamTool>(this);

  declareProperty("CpmMappingTool", m_cpmMaps,
                  "Crate/Module/Channel to Eta/Phi/Layer mapping tool");

  declareProperty("CrateOffsetHw",  m_crateOffsetHw  = 8,
                  "Offset of CP crate numbers in bytestream");
  declareProperty("CrateOffsetSw",  m_crateOffsetSw  = 0,
                  "Offset of CP crate numbers in RDOs");

  // Properties for reading bytestream only
  declareProperty("ROBSourceIDs",       m_sourceIDs,
                  "ROB fragment source identifiers");

  // Properties for writing bytestream only
  declareProperty("DataVersion",    m_version     = 1,
                  "Format version number in sub-block header");
  declareProperty("DataFormat",     m_dataFormat  = 1,
                  "Format identifier (0-1) in sub-block header");
  declareProperty("SlinksPerCrate", m_slinks      = 2,
                  "The number of S-Links per crate");
  declareProperty("SimulSlices",    m_dfltSlices  = 1,
                  "The number of slices in the simulation");
  declareProperty("ForceSlices",    m_forceSlices = 0,
                  "If >0, the number of slices in bytestream");

}

// Destructor

CpByteStreamTool::~CpByteStreamTool()
{
}

// Initialize


StatusCode CpByteStreamTool::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endmsg;

  StatusCode sc = m_cpmMaps.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to retrieve tool " << m_cpmMaps << endmsg;
    return sc;
  } else msg(MSG::INFO) << "Retrieved tool " << m_cpmMaps << endmsg;

  sc = m_errorTool.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to retrieve tool " << m_errorTool << endmsg;
    return sc;
  } else msg(MSG::INFO) << "Retrieved tool " << m_errorTool << endmsg;

  m_srcIdMap      = new L1CaloSrcIdMap();
  m_towerKey      = new LVL1::TriggerTowerKey();
  m_cpmSubBlock   = new CpmSubBlock();
  m_cmmCpSubBlock = new CmmCpSubBlock();
  m_rodStatus     = new std::vector<uint32_t>(2);
  m_fea           = new FullEventAssembler<L1CaloSrcIdMap>();
  return StatusCode::SUCCESS;
}

// Finalize

StatusCode CpByteStreamTool::finalize()
{
  delete m_fea;
  delete m_rodStatus;
  delete m_cmmCpSubBlock;
  delete m_cpmSubBlock;
  delete m_towerKey;
  delete m_srcIdMap;
  return StatusCode::SUCCESS;
}

// Conversion bytestream to CPM towers

StatusCode CpByteStreamTool::convert(
                            const IROBDataProviderSvc::VROBFRAG& robFrags,
                            DataVector<LVL1::CPMTower>* const ttCollection)
{
  CpmTowerData data (ttCollection);
  return convertBs(robFrags, data);
}

// Conversion bytestream to CPM hits

StatusCode CpByteStreamTool::convert(
                            const IROBDataProviderSvc::VROBFRAG& robFrags,
                            DataVector<LVL1::CPMHits>* const hitCollection)
{
  CpmHitsData data (hitCollection);
  return convertBs(robFrags, data);
}

// Conversion bytestream to CMM-CP hits

StatusCode CpByteStreamTool::convert(
                            const IROBDataProviderSvc::VROBFRAG& robFrags,
                            DataVector<LVL1::CMMCPHits>* const hitCollection)
{
  CmmHitsData data (hitCollection);
  return convertBs(robFrags, data);
}

// Conversion of CP container to bytestream

StatusCode CpByteStreamTool::convert(const LVL1::CPBSCollection* const cp,
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

  setupCpmTowerMap(cp->towers());
  setupCpmHitsMap(cp->hits());
  setupCmmCpHitsMap(cp->cmmHits());

  // Loop over data

  const bool neutralFormat   = m_dataFormat == L1CaloSubBlock::NEUTRAL;
  const int  modulesPerSlink = m_modules / m_slinks;
  int timeslices    = 1;
  int trigCpm       = 0;
  int timeslicesNew = 1;
  int trigCpmNew    = 0;
  for (int crate=0; crate < m_crates; ++crate) {
    const int hwCrate = crate + m_crateOffsetHw;

    // CPM modules are numbered 1 to m_modules
    for (int module=1; module <= m_modules; ++module) {
      const int mod = module - 1;

      // Pack required number of modules per slink

      if (mod%modulesPerSlink == 0) {
	const int daqOrRoi = 0;
	const int slink = (m_slinks == 2) ? 2*(mod/modulesPerSlink)
	                                  : mod/modulesPerSlink;
        if (debug) {
          msg() << "Treating crate " << hwCrate
                << " slink " << slink << endmsg;
        }
	// Get number of CPM slices and triggered slice offset
	// for this slink
	if ( ! slinkSlices(crate, module, modulesPerSlink,
	                                  timeslices, trigCpm)) {
	  msg(MSG::ERROR) << "Inconsistent number of slices or "
	                  << "triggered slice offsets in data for crate "
	                  << hwCrate << " slink " << slink << endmsg;
	  return StatusCode::FAILURE;
        }
	timeslicesNew = (m_forceSlices) ? m_forceSlices : timeslices;
	trigCpmNew    = ModifySlices::peak(trigCpm, timeslices, timeslicesNew);
        if (debug) {
	  msg() << "Data Version/Format: " << m_version
	        << " " << m_dataFormat << endmsg
                << "Slices/offset: " << timeslices << " " << trigCpm;
	  if (timeslices != timeslicesNew) {
	    msg() << " modified to " << timeslicesNew << " " << trigCpmNew;
          }
	  msg() << endmsg;
        }
        L1CaloUserHeader userHeader;
        userHeader.setCpm(trigCpmNew);
	const uint32_t rodIdCpm = m_srcIdMap->getRodID(hwCrate, slink, daqOrRoi,
	                                                        m_subDetector);
	theROD = m_fea->getRodData(rodIdCpm);
	theROD->push_back(userHeader.header());
	m_rodStatusMap.insert(make_pair(rodIdCpm, m_rodStatus));
      }
      if (debug) msg() << "Module " << module << endmsg;

      // Create a sub-block for each slice (except Neutral format)

      m_cpmBlocks.clear();
      for (int slice = 0; slice < timeslicesNew; ++slice) {
        CpmSubBlock* const subBlock = new CpmSubBlock();
	subBlock->setCpmHeader(m_version, m_dataFormat, slice,
	                       hwCrate, module, timeslicesNew);
        m_cpmBlocks.push_back(subBlock);
	if (neutralFormat) break;
      }

      // Find CPM towers corresponding to each eta/phi pair and fill
      // sub-blocks

      for (int chan=0; chan < m_channels; ++chan) {
	double eta = 0.;
	double phi = 0.;
	int layer = 0;
	if (m_cpmMaps->mapping(crate, module, chan, eta, phi, layer)) {
          const unsigned int key = m_towerKey->ttKey(phi, eta);
          const LVL1::CPMTower* const tt = findCpmTower(key);
	  if (tt ) {
	    std::vector<int> emData;
	    std::vector<int> hadData;
	    std::vector<int> emError;
	    std::vector<int> hadError;
	    ModifySlices::data(tt->emEnergyVec(),  emData,   timeslicesNew);
	    ModifySlices::data(tt->hadEnergyVec(), hadData,  timeslicesNew);
	    ModifySlices::data(tt->emErrorVec(),   emError,  timeslicesNew);
	    ModifySlices::data(tt->hadErrorVec(),  hadError, timeslicesNew);
            for (int slice = 0; slice < timeslicesNew; ++slice) {
	      const LVL1::DataError emErrBits(emError[slice]);
	      const LVL1::DataError hadErrBits(hadError[slice]);
	      const int emErr  =
	                      (emErrBits.get(LVL1::DataError::LinkDown) << 1) |
	                       emErrBits.get(LVL1::DataError::Parity);
	      const int hadErr =
	                     (hadErrBits.get(LVL1::DataError::LinkDown) << 1) |
	                      hadErrBits.get(LVL1::DataError::Parity);
	      const int index  = ( neutralFormat ) ? 0 : slice;
              CpmSubBlock* const subBlock = m_cpmBlocks[index];
              subBlock->fillTowerData(slice, chan, emData[slice],
	                              hadData[slice], emErr, hadErr);
	    }
          }
        }
      }

      // Add CPM hits

      const LVL1::CPMHits* const hits = findCpmHits(crate, module);
      if (hits) {
        std::vector<unsigned int> vec0;
        std::vector<unsigned int> vec1;
	ModifySlices::data(hits->HitsVec0(), vec0, timeslicesNew);
	ModifySlices::data(hits->HitsVec1(), vec1, timeslicesNew);
        for (int slice = 0; slice < timeslicesNew; ++slice) {
	  const int index = ( neutralFormat ) ? 0 : slice;
	  CpmSubBlock* const subBlock = m_cpmBlocks[index];
	  subBlock->setHits(slice, vec0[slice], vec1[slice]);
        }
      }
      
      // Pack and write the sub-blocks

      DataVector<CpmSubBlock>::iterator pos;
      for (pos = m_cpmBlocks.begin(); pos != m_cpmBlocks.end(); ++pos) {
        CpmSubBlock* const subBlock = *pos;
	if ( !subBlock->pack()) {
	  msg(MSG::ERROR) << "CPM sub-block packing failed" << endmsg;
	  return StatusCode::FAILURE;
	}
        if (debug) {
          msg() << "CPM sub-block data words: "
	        << subBlock->dataWords() << endmsg;
        }
	subBlock->write(theROD);
      }
    }

    // Append CMMs to last S-Link of the crate

    // Create a sub-block for each slice (except Neutral format)

    m_cmmHit0Blocks.clear();
    m_cmmHit1Blocks.clear();
    const int summing = (crate == m_crates - 1) ? CmmSubBlock::SYSTEM
                                                : CmmSubBlock::CRATE;
    for (int slice = 0; slice < timeslicesNew; ++slice) {
      CmmCpSubBlock* const h0Block = new CmmCpSubBlock();
      h0Block->setCmmHeader(m_version, m_dataFormat, slice, hwCrate,
                            summing, CmmSubBlock::CMM_CP,
			    CmmSubBlock::RIGHT, timeslicesNew);
      m_cmmHit0Blocks.push_back(h0Block);
      CmmCpSubBlock* const h1Block = new CmmCpSubBlock();
      h1Block->setCmmHeader(m_version, m_dataFormat, slice, hwCrate,
                            summing, CmmSubBlock::CMM_CP,
			    CmmSubBlock::LEFT, timeslicesNew);
      m_cmmHit1Blocks.push_back(h1Block);
      if (neutralFormat) break;
    }

    // CMM-CP

    const int maxDataID = LVL1::CMMCPHits::MAXID;
    for (int dataID = 1; dataID < maxDataID; ++dataID) {
      int source = dataID;
      if (dataID > m_modules) {
        if (summing == CmmSubBlock::CRATE && 
	    dataID != LVL1::CMMCPHits::LOCAL) continue;
	switch (dataID) {
	  case LVL1::CMMCPHits::LOCAL:
	    source = CmmCpSubBlock::LOCAL;
	    break;
	  case LVL1::CMMCPHits::REMOTE_0:
	    source = CmmCpSubBlock::REMOTE_0;
	    break;
	  case LVL1::CMMCPHits::REMOTE_1:
	    source = CmmCpSubBlock::REMOTE_1;
	    break;
	  case LVL1::CMMCPHits::REMOTE_2:
	    source = CmmCpSubBlock::REMOTE_2;
	    break;
	  case LVL1::CMMCPHits::TOTAL:
	    source = CmmCpSubBlock::TOTAL;
	    break;
          default:
	    continue;
        }
      }
      const LVL1::CMMCPHits* const ch = findCmmCpHits(crate, dataID);
      if ( ch ) {
        std::vector<unsigned int> hits0;
        std::vector<unsigned int> hits1;
        std::vector<int> err0;
        std::vector<int> err1;
	ModifySlices::data(ch->HitsVec0(),  hits0, timeslicesNew);
	ModifySlices::data(ch->HitsVec1(),  hits1, timeslicesNew);
	ModifySlices::data(ch->ErrorVec0(), err0,  timeslicesNew);
	ModifySlices::data(ch->ErrorVec1(), err1,  timeslicesNew);
	for (int slice = 0; slice < timeslicesNew; ++slice) {
	  const LVL1::DataError err0Bits(err0[slice]);
	  const LVL1::DataError err1Bits(err1[slice]);
	  const int index = ( neutralFormat ) ? 0 : slice;
	  CmmCpSubBlock* subBlock = m_cmmHit0Blocks[index];
	  subBlock->setHits(slice, source, hits0[slice],
	                           err0Bits.get(LVL1::DataError::Parity));
	  subBlock = m_cmmHit1Blocks[index];
	  subBlock->setHits(slice, source, hits1[slice],
	                           err1Bits.get(LVL1::DataError::Parity));
        }
      }
    }
    DataVector<CmmCpSubBlock>::iterator cos = m_cmmHit0Blocks.begin();
    for (; cos != m_cmmHit0Blocks.end(); ++cos) {
      CmmCpSubBlock* const subBlock = *cos;
      if ( !subBlock->pack()) {
        msg(MSG::ERROR) << "CMM-Cp sub-block packing failed" << endmsg;
	return StatusCode::FAILURE;
      }
      if (debug) {
        msg() << "CMM-Cp sub-block data words: "
	      << subBlock->dataWords() << endmsg;
      }
      subBlock->write(theROD);
    }
    cos = m_cmmHit1Blocks.begin();
    for (; cos != m_cmmHit1Blocks.end(); ++cos) {
      CmmCpSubBlock* const subBlock = *cos;
      if ( !subBlock->pack()) {
        msg(MSG::ERROR) << "CMM-Cp sub-block packing failed" << endmsg;
	return StatusCode::FAILURE;
      }
      if (debug) {
        msg() << "CMM-Cp sub-block data words: "
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

const std::vector<uint32_t>& CpByteStreamTool::sourceIDs(
                                               const std::string& sgKey)
{
  // Check if overlap tower channels wanted
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

StatusCode CpByteStreamTool::convertBs(
                            const IROBDataProviderSvc::VROBFRAG& robFrags,
                            CpByteStreamToolData& data)
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
       (m_srcIdMap->slink(sourceID) != 0 && m_srcIdMap->slink(sourceID) != 2) ||
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
    int trigCpm = userHeader.cpm();
    int trigCmm = userHeader.cpCmm();
    if (debug) {
      msg() << "Minor format version number: " << MSG::hex
            << minorVersion << MSG::dec << endmsg
            << "CPM triggered slice offset: "  << trigCpm << endmsg
            << "CMM triggered slice offset: "  << trigCmm << endmsg;
    }
    if (trigCpm != trigCmm) {
      const int newTrig = (trigCpm > trigCmm) ? trigCpm : trigCmm;
      trigCpm = newTrig;
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
        // CMM
	if (CmmSubBlock::cmmType(*payload) == CmmSubBlock::CMM_CP) {
	  m_cmmCpSubBlock->clear();
          payload = m_cmmCpSubBlock->read(payload, payloadEnd);
	  if (m_cmmCpSubBlock->crate() != rodCrate) {
	    if (debug) msg() << "Inconsistent crate number in ROD source ID"
	                     << endmsg;
	    m_rodErr = L1CaloSubBlock::ERROR_CRATE_NUMBER;
	    break;
          }
	  if (data.m_collection == CMM_CP_HITS) {
	    decodeCmmCp(m_cmmCpSubBlock, trigCmm, static_cast<CmmHitsData&>(data));
	    if (m_rodErr != L1CaloSubBlock::ERROR_NONE) {
	      if (debug) msg() << "decodeCmmCp failed" << endmsg;
	      break;
	    }
          }
        } else {
          if (debug) msg() << "Invalid CMM type in module field" << endmsg;
          m_rodErr = L1CaloSubBlock::ERROR_MODULE_NUMBER;
	  break;
        }
      } else {
        // CPM
        CpmSubBlock subBlock;
	m_cpmSubBlock->clear();
        payload = m_cpmSubBlock->read(payload, payloadEnd);
	if (m_cpmSubBlock->crate() != rodCrate) {
	  if (debug) msg() << "Inconsistent crate number in ROD source ID"
	                   << endmsg;
	  m_rodErr = L1CaloSubBlock::ERROR_CRATE_NUMBER;
	  break;
        }
	if (data.m_collection == CPM_TOWERS || data.m_collection == CPM_HITS) {
	  decodeCpm(m_cpmSubBlock, trigCpm, data);
	  if (m_rodErr != L1CaloSubBlock::ERROR_NONE) {
	    if (debug) msg() << "decodeCpm failed" << endmsg;
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

// Unpack CMM-CP sub-block

void CpByteStreamTool::decodeCmmCp(CmmCpSubBlock* subBlock, int trigCmm,
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
    msg() << "CMM-CP: Crate "  << hwCrate
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
      msg() << "CMM-CP sub-block unpacking failed: " << errMsg << endmsg;
    }
    m_rodErr = subBlock->unpackErrorCode();
    return;
  }

  // Retrieve required data

  const bool neutralFormat = subBlock->format() == L1CaloSubBlock::NEUTRAL;
  const uint32_t subStatus = subBlock->subStatus();
  const int crate    = hwCrate - m_crateOffsetHw;
  const int swCrate  = crate   + m_crateOffsetSw;
  const int maxSid   = CmmCpSubBlock::MAX_SOURCE_ID;
  const int sliceBeg = ( neutralFormat ) ? 0          : sliceNum;
  const int sliceEnd = ( neutralFormat ) ? timeslices : sliceNum + 1;
  for (int slice = sliceBeg; slice < sliceEnd; ++slice) {

    // Jet hit counts

    for (int source = 1; source < maxSid; ++source) {
      int dataID = source;
      if (source > m_modules) {
        if (summing == CmmSubBlock::CRATE &&
	    source != CmmCpSubBlock::LOCAL) continue;
	switch (source) {
	  case CmmCpSubBlock::LOCAL:
	    dataID = LVL1::CMMCPHits::LOCAL;
	    break;
	  case CmmCpSubBlock::REMOTE_0:
	    dataID = LVL1::CMMCPHits::REMOTE_0;
	    break;
	  case CmmCpSubBlock::REMOTE_1:
	    dataID = LVL1::CMMCPHits::REMOTE_1;
	    break;
	  case CmmCpSubBlock::REMOTE_2:
	    dataID = LVL1::CMMCPHits::REMOTE_2;
	    break;
	  case CmmCpSubBlock::TOTAL:
	    dataID = LVL1::CMMCPHits::TOTAL;
	    break;
          default:
	    continue;
        }
      }
      const unsigned int hits = subBlock->hits(slice, source);
      int err = subBlock->hitsError(slice, source);
      LVL1::DataError errorBits;
      errorBits.set(LVL1::DataError::Parity, err & 0x1);
      errorBits.set(LVL1::DataError::SubStatusWord, subStatus);
      err = errorBits.error();
      if (hits || err) {
        LVL1::CMMCPHits* ch = findCmmCpHits(data, crate, dataID);
	if ( ! ch ) {   // create new CMM hits
	  m_hitsVec0.assign(timeslices, 0);
	  m_hitsVec1.assign(timeslices, 0);
	  m_errVec0.assign(timeslices, 0);
	  m_errVec1.assign(timeslices, 0);
	  if (module == CmmSubBlock::RIGHT) {
	    m_hitsVec0[slice] = hits;
	    m_errVec0[slice]  = err;
	  } else {
	    m_hitsVec1[slice] = hits;
	    m_errVec1[slice]  = err;
	  }
          auto chp =
            std::make_unique<LVL1::CMMCPHits>(swCrate, dataID, m_hitsVec0, m_hitsVec1,
                                              m_errVec0, m_errVec1, trigCmm);
          const int key = crate*100 + dataID;
	  data.m_cmmHitsMap.insert(std::make_pair(key, chp.get()));
	  data.m_cmmHitCollection->push_back(std::move(chp));
        } else {
	  m_hitsVec0 = ch->HitsVec0();
	  m_hitsVec1 = ch->HitsVec1();
	  m_errVec0  = ch->ErrorVec0();
	  m_errVec1  = ch->ErrorVec1();
	  const int nsl = m_hitsVec0.size();
	  if (timeslices != nsl) {
	    if (debug) msg() << "Inconsistent number of slices in sub-blocks"
	                     << endmsg;
            m_rodErr = L1CaloSubBlock::ERROR_SLICES;
	    return;
          }
	  if ((module == CmmSubBlock::RIGHT && (m_hitsVec0[slice] != 0 ||
	       m_errVec0[slice] != 0)) || (module == CmmSubBlock::LEFT &&
	       (m_hitsVec1[slice] != 0 || m_errVec1[slice]  != 0))) {
            if (debug) msg() << "Duplicate data for slice " << slice << endmsg;
	    m_rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
	    return;
          }
	  if (module == CmmSubBlock::RIGHT) {
	    m_hitsVec0[slice] = hits;
	    m_errVec0[slice]  = err;
	  } else {
	    m_hitsVec1[slice] = hits;
	    m_errVec1[slice]  = err;
	  }
	  ch->addHits(m_hitsVec0, m_hitsVec1, m_errVec0, m_errVec1);
        }
      }
    }
  }
  
  return;
}

// Unpack CPM sub-block

void CpByteStreamTool::decodeCpm(CpmSubBlock* subBlock,
                                 int trigCpm, CpByteStreamToolData& data)
{
  const bool debug   = msgLvl(MSG::DEBUG);
  const bool verbose = msgLvl(MSG::VERBOSE);
  if (debug) msg(MSG::DEBUG);

  const int hwCrate    = subBlock->crate();
  const int module     = subBlock->module();
  const int timeslices = subBlock->timeslices();
  const int sliceNum   = subBlock->slice();
  if (debug) {
    msg() << "CPM: Crate "     << hwCrate
          << "  Module "       << module
          << "  Total slices " << timeslices
          << "  Slice "        << sliceNum    << endmsg;
  }
  if (module < 1 || module > m_modules) {
    if (debug) msg() << "Unexpected module number: " << module << endmsg;
    m_rodErr = L1CaloSubBlock::ERROR_MODULE_NUMBER;
    return;
  }
  if (timeslices <= trigCpm) {
    if (debug) msg() << "Triggered CPM slice from header "
                     << "inconsistent with number of slices: "
                     << trigCpm << ", " << timeslices << endmsg;
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
      msg() << "CPM sub-block unpacking failed: " << errMsg << endmsg;
    }
    m_rodErr = subBlock->unpackErrorCode();
    return;
  }

  // Retrieve required data

  const bool neutralFormat = subBlock->format() == L1CaloSubBlock::NEUTRAL;
  LVL1::DataError dErr;
  dErr.set(LVL1::DataError::SubStatusWord, subBlock->subStatus());
  const int ssError  = dErr.error();
  const int crate    = hwCrate - m_crateOffsetHw;
  const int swCrate  = crate   + m_crateOffsetSw;
  const int sliceBeg = ( neutralFormat ) ? 0          : sliceNum;
  const int sliceEnd = ( neutralFormat ) ? timeslices : sliceNum + 1;
  for (int slice = sliceBeg; slice < sliceEnd; ++slice) {

    if (data.m_collection == CPM_TOWERS) {
      CpmTowerData& tdata = static_cast<CpmTowerData&> (data);

      // Loop over tower channels and fill CPM towers

      for (int chan = 0; chan < m_channels; ++chan) {
        if (!ssError && !subBlock->anyTowerData(chan)) continue;
	const int em     = subBlock->emData(slice, chan);
	const int had    = subBlock->hadData(slice, chan);
	const int emErr  = subBlock->emError(slice, chan);
	const int hadErr = subBlock->hadError(slice, chan);
	int emErr1 = ssError;
	if (emErr) {
	  LVL1::DataError emErrBits(emErr1);
	  emErrBits.set(LVL1::DataError::Parity, emErr & 0x1);
	  emErrBits.set(LVL1::DataError::LinkDown, (emErr >> 1) & 0x1);
	  emErr1 = emErrBits.error();
	}
	int hadErr1 = ssError;
	if (hadErr) {
	  LVL1::DataError hadErrBits(hadErr1);
	  hadErrBits.set(LVL1::DataError::Parity, hadErr & 0x1);
	  hadErrBits.set(LVL1::DataError::LinkDown, (hadErr >> 1) & 0x1);
	  hadErr1 = hadErrBits.error();
	}
        if (em || had || emErr1 || hadErr1) {
	  double eta = 0.;
	  double phi = 0.;
	  int layer = 0;
	  if (m_cpmMaps->mapping(crate, module, chan, eta, phi, layer)) {
	    if (layer == m_coreOverlap) {
	      const unsigned int key = m_towerKey->ttKey(phi, eta);
	      LVL1::CPMTower* tt = findCpmTower(tdata, key);
	      if ( ! tt ) {   // create new CPM tower
	        m_emVec.assign(timeslices, 0);
	        m_hadVec.assign(timeslices, 0);
	        m_emErrVec.assign(timeslices, 0);
	        m_hadErrVec.assign(timeslices, 0);
	        m_emVec[slice]     = em;
	        m_hadVec[slice]    = had;
	        m_emErrVec[slice]  = emErr1;
	        m_hadErrVec[slice] = hadErr1;
                auto ttp = 
                  std::make_unique<LVL1::CPMTower>(phi, eta, m_emVec, m_emErrVec,
                                                   m_hadVec, m_hadErrVec, trigCpm);
	        tdata.m_ttMap.insert(std::make_pair(key, ttp.get()));
	        tdata.m_ttCollection->push_back(std::move(ttp));
              } else {
	        m_emVec     = tt->emEnergyVec();
	        m_hadVec    = tt->hadEnergyVec();
	        m_emErrVec  = tt->emErrorVec();
	        m_hadErrVec = tt->hadErrorVec();
		const int nsl = m_emVec.size();
	        if (timeslices != nsl) {
	          if (debug) {
		    msg() << "Inconsistent number of slices in sub-blocks"
	                  << endmsg;
	          }
                  m_rodErr = L1CaloSubBlock::ERROR_SLICES;
	          return;
                }
		if (m_emVec[slice]    != 0 || m_hadVec[slice]    != 0 ||
		    m_emErrVec[slice] != 0 || m_hadErrVec[slice] != 0) {
                  if (debug) msg() << "Duplicate data for slice "
		                   << slice << endmsg;
	          m_rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
	          return;
                }
	        m_emVec[slice]     = em;
	        m_hadVec[slice]    = had;
	        m_emErrVec[slice]  = emErr1;
	        m_hadErrVec[slice] = hadErr1;
	        tt->fill(m_emVec, m_emErrVec, m_hadVec, m_hadErrVec, trigCpm);
	      }
	    }
          } else if (verbose && (em || had || emErr || hadErr)) {
	    msg(MSG::VERBOSE) << "Non-zero data but no channel mapping for channel "
	                      << chan << endmsg;
	    msg(MSG::DEBUG);
          }
        } else if (verbose) {
	  msg(MSG::VERBOSE) << "No CPM tower data for channel "
	                    << chan << " slice " << slice << endmsg;
	  msg(MSG::DEBUG);
        }
      }
    } else if (data.m_collection == CPM_HITS) {
      CpmHitsData& hdata = static_cast<CpmHitsData&> (data);

      // Get CPM hits

      const unsigned int hits0 = subBlock->hits0(slice);
      const unsigned int hits1 = subBlock->hits1(slice);
      if (hits0 || hits1) {
        LVL1::CPMHits* ch = findCpmHits(hdata, crate, module);
	if ( ! ch ) {   // create new CPM hits
	  m_hitsVec0.assign(timeslices, 0);
	  m_hitsVec1.assign(timeslices, 0);
	  m_hitsVec0[slice] = hits0;
	  m_hitsVec1[slice] = hits1;
          auto chp = 
            std::make_unique<LVL1::CPMHits>(swCrate, module, m_hitsVec0, m_hitsVec1, trigCpm);
	  hdata.m_hitsMap.insert(std::make_pair(crate*m_modules+module-1, chp.get()));
	  hdata.m_hitCollection->push_back(std::move(chp));
        } else {
	  m_hitsVec0 = ch->HitsVec0();
	  m_hitsVec1 = ch->HitsVec1();
	  const int nsl = m_hitsVec0.size();
	  if (timeslices != nsl) {
	    if (debug) msg() << "Inconsistent number of slices in sub-blocks"
	                     << endmsg;
            m_rodErr = L1CaloSubBlock::ERROR_SLICES;
	    return;
          }
	  if (m_hitsVec0[slice] != 0 || m_hitsVec1[slice] != 0) {
            if (debug) msg() << "Duplicate data for slice " << slice << endmsg;
	    m_rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
	    return;
          }
	  m_hitsVec0[slice] = hits0;
	  m_hitsVec1[slice] = hits1;
	  ch->addHits(m_hitsVec0, m_hitsVec1);
        }
      } else if (verbose) {
        msg(MSG::VERBOSE) << "No CPM hits data for crate/module/slice "
                          << hwCrate << "/" << module << "/" << slice
   			  << endmsg;
        msg(MSG::DEBUG);
      }
    }
  }
  return;
}

// Find a CPM tower for given key

const
LVL1::CPMTower* CpByteStreamTool::findCpmTower(const unsigned int key) const
{
  ConstCpmTowerMap::const_iterator mapIter = m_ttMap.find(key);
  if (mapIter != m_ttMap.end()) return mapIter->second;
  return nullptr;
}

LVL1::CPMTower* CpByteStreamTool::findCpmTower(const CpmTowerData& data,
                                               const unsigned int key) const
{
  CpmTowerMap::const_iterator mapIter = data.m_ttMap.find(key);
  if (mapIter != data.m_ttMap.end()) return mapIter->second;
  return nullptr;
}

// Find CPM hits for given crate, module

const
LVL1::CPMHits* CpByteStreamTool::findCpmHits(const int crate, const int module) const
{
  ConstCpmHitsMap::const_iterator mapIter =
    m_hitsMap.find(crate*m_modules + module - 1);
  if (mapIter != m_hitsMap.end()) return mapIter->second;
  return nullptr;
}

LVL1::CPMHits* CpByteStreamTool::findCpmHits(const CpmHitsData& data,
                                             const int crate, const int module) const
{
  CpmHitsMap::const_iterator mapIter =
    data.m_hitsMap.find(crate*m_modules + module - 1);
  if (mapIter != data.m_hitsMap.end()) return mapIter->second;
  return nullptr;
}

// Find CMM-CP hits for given crate, dataID

const
LVL1::CMMCPHits* CpByteStreamTool::findCmmCpHits(const int crate,
                                                 const int dataID) const
{
  ConstCmmCpHitsMap::const_iterator mapIter = m_cmmHitsMap.find(crate*100 + dataID);
  if (mapIter != m_cmmHitsMap.end()) return mapIter->second;
  return nullptr;
}

LVL1::CMMCPHits* CpByteStreamTool::findCmmCpHits(const CmmHitsData& data,
                                                 const int crate,
                                                 const int dataID) const
{
  CmmCpHitsMap::const_iterator mapIter = data.m_cmmHitsMap.find(crate*100 + dataID);
  if (mapIter != data.m_cmmHitsMap.end()) return mapIter->second;
  return nullptr;
}

// Set up CPM tower map

void CpByteStreamTool::setupCpmTowerMap(const CpmTowerCollection*
                                                          const ttCollection)
{
  m_ttMap.clear();
  if (ttCollection) {
    CpmTowerCollection::const_iterator pos  = ttCollection->begin();
    CpmTowerCollection::const_iterator pose = ttCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CPMTower* const tt = *pos;
      const unsigned int key = m_towerKey->ttKey(tt->phi(), tt->eta());
      m_ttMap.insert(std::make_pair(key, tt));
    }
  }
}

// Set up CPM hits map

void CpByteStreamTool::setupCpmHitsMap(const CpmHitsCollection*
                                                        const hitCollection)
{
  m_hitsMap.clear();
  if (hitCollection) {
    CpmHitsCollection::const_iterator pos  = hitCollection->begin();
    CpmHitsCollection::const_iterator pose = hitCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CPMHits* const hits = *pos;
      const int crate = hits->crate() - m_crateOffsetSw;
      const int key   = m_modules * crate + hits->module() - 1;
      m_hitsMap.insert(std::make_pair(key, hits));
    }
  }
}

// Set up CMM-CP hits map

void CpByteStreamTool::setupCmmCpHitsMap(const CmmCpHitsCollection*
                                                          const hitCollection)
{
  m_cmmHitsMap.clear();
  if (hitCollection) {
    CmmCpHitsCollection::const_iterator pos  = hitCollection->begin();
    CmmCpHitsCollection::const_iterator pose = hitCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CMMCPHits* const hits = *pos;
      const int crate = hits->crate() - m_crateOffsetSw;
      const int key   = crate*100 + hits->dataID();
      m_cmmHitsMap.insert(std::make_pair(key, hits));
    }
  }
}

// Get number of slices and triggered slice offset for next slink

bool CpByteStreamTool::slinkSlices(const int crate, const int module,
                  const int modulesPerSlink, int& timeslices, int& trigCpm)
{
  int slices = -1;
  int trigC  = m_dfltSlices/2;
  for (int mod = module; mod < module + modulesPerSlink; ++mod) {
    for (int chan = 0; chan < m_channels; ++chan) {
      double eta = 0.;
      double phi = 0.;
      int layer = 0;
      if ( !m_cpmMaps->mapping(crate, mod, chan, eta, phi, layer)) continue;
      const unsigned int key = m_towerKey->ttKey(phi, eta);
      const LVL1::CPMTower* const tt = findCpmTower(key);
      if ( !tt ) continue;
      const int numdat = 4;
      std::vector<int> sums(numdat);
      std::vector<int> sizes(numdat);
      sums[0] = std::accumulate((tt->emEnergyVec()).begin(),
                                (tt->emEnergyVec()).end(), 0);
      sums[1] = std::accumulate((tt->hadEnergyVec()).begin(),
                                (tt->hadEnergyVec()).end(), 0);
      sums[2] = std::accumulate((tt->emErrorVec()).begin(),
                                (tt->emErrorVec()).end(), 0);
      sums[3] = std::accumulate((tt->hadErrorVec()).begin(),
                                (tt->hadErrorVec()).end(), 0);
      sizes[0] = (tt->emEnergyVec()).size();
      sizes[1] = (tt->hadEnergyVec()).size();
      sizes[2] = (tt->emErrorVec()).size();
      sizes[3] = (tt->hadErrorVec()).size();
      const int peak = tt->peak();
      for (int i = 0; i < numdat; ++i) {
        if (sums[i] == 0) continue;
        if (slices < 0) {
	  slices = sizes[i];
	  trigC  = peak;
	} else if (slices != sizes[i] || trigC != peak) return false;
      }
    }
    const LVL1::CPMHits* const hits = findCpmHits(crate, mod);
    if (hits) {
      const int numdat = 2;
      std::vector<unsigned int> sums(numdat);
      std::vector<int> sizes(numdat);
      sums[0] = std::accumulate((hits->HitsVec0()).begin(),
                                         (hits->HitsVec0()).end(), 0);
      sums[1] = std::accumulate((hits->HitsVec1()).begin(),
                                         (hits->HitsVec1()).end(), 0);
      sizes[0] = (hits->HitsVec0()).size();
      sizes[1] = (hits->HitsVec1()).size();
      const int peak = hits->peak();
      for (int i = 0; i < numdat; ++i) {
        if (sums[i] == 0) continue;
        if (slices < 0) {
	  slices = sizes[i];
	  trigC  = peak;
        } else if (slices != sizes[i] || trigC != peak) return false;
      }
    }
  }
  // CMM last slink of crate
  if (module/modulesPerSlink == m_slinks - 1) {
    const int maxDataID = LVL1::CMMCPHits::MAXID;
    for (int dataID = 0; dataID < maxDataID; ++dataID) {
      const int numdat = 4;
      std::vector<unsigned int> sums(numdat);
      std::vector<int> sizes(numdat);
      const LVL1::CMMCPHits* const hits = findCmmCpHits(crate, dataID);
      if (hits) {
        sums[0] = std::accumulate((hits->HitsVec0()).begin(),
                                             (hits->HitsVec0()).end(), 0);
        sums[1] = std::accumulate((hits->HitsVec1()).begin(),
                                             (hits->HitsVec1()).end(), 0);
        sums[2] = std::accumulate((hits->ErrorVec0()).begin(),
                                             (hits->ErrorVec0()).end(), 0);
        sums[3] = std::accumulate((hits->ErrorVec1()).begin(),
                                             (hits->ErrorVec1()).end(), 0);
        sizes[0] = (hits->HitsVec0()).size();
        sizes[1] = (hits->HitsVec1()).size();
        sizes[2] = (hits->ErrorVec0()).size();
        sizes[3] = (hits->ErrorVec1()).size();
        const int peak = hits->peak();
        for (int i = 0; i < numdat; ++i) {
          if (sums[i] == 0) continue;
          if (slices < 0) {
	    slices = sizes[i];
	    trigC  = peak;
          } else if (slices != sizes[i] || trigC != peak) return false;
        }
      }
    }
  }
  if (slices < 0) slices = m_dfltSlices;
  timeslices = slices;
  trigCpm    = trigC;
  return true;
}

} // end namespace
