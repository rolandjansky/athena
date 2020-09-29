/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "TrigT1CaloToolInterfaces/IL1CaloMappingTool.h"

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
     m_subDetector(eformat::TDAQ_CALO_CLUSTER_PROC_DAQ)
{
  declareInterface<CpByteStreamTool>(this);

  declareProperty("CpmMappingTool", m_cpmMaps,
                  "Crate/Module/Channel to Eta/Phi/Layer mapping tool");

  declareProperty("CrateOffsetHw",  m_crateOffsetHw  = 8,
                  "Offset of CP crate numbers in bytestream");
  declareProperty("CrateOffsetSw",  m_crateOffsetSw  = 0,
                  "Offset of CP crate numbers in RDOs");

  // Properties for reading bytestream only
  declareProperty("ROBSourceIDs",       m_sourceIDsProp,
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
  ATH_MSG_INFO ("Initializing " << name() << " - package version "
                << PACKAGE_VERSION);

  ATH_CHECK( m_cpmMaps.retrieve() );
  ATH_CHECK( m_errorTool.retrieve() );

  return StatusCode::SUCCESS;
}

// Finalize

StatusCode CpByteStreamTool::finalize()
{
  return StatusCode::SUCCESS;
}

// Conversion bytestream to CPM towers

StatusCode CpByteStreamTool::convert(
                            const std::string& sgKey,
                            const IROBDataProviderSvc::VROBFRAG& robFrags,
                            DataVector<LVL1::CPMTower>* const ttCollection) const
{
  CpmTowerData data (ttCollection);
  return convertBs(sgKey, robFrags, data);
}

// Conversion bytestream to CPM hits

StatusCode CpByteStreamTool::convert(
                            const std::string& sgKey,
                            const IROBDataProviderSvc::VROBFRAG& robFrags,
                            DataVector<LVL1::CPMHits>* const hitCollection) const
{
  CpmHitsData data (hitCollection);
  return convertBs(sgKey, robFrags, data);
}

// Conversion bytestream to CMM-CP hits

StatusCode CpByteStreamTool::convert(
                            const std::string& sgKey,
                            const IROBDataProviderSvc::VROBFRAG& robFrags,
                            DataVector<LVL1::CMMCPHits>* const hitCollection) const
{
  CmmHitsData data (hitCollection);
  return convertBs(sgKey, robFrags, data);
}

// Conversion of CP container to bytestream

StatusCode CpByteStreamTool::convert(const LVL1::CPBSCollection* const cp,
                                           RawEventWrite* const re) const
{
  const bool debug = msgLvl(MSG::DEBUG);
  if (debug) msg(MSG::DEBUG);

  // Clear the event assembler
  FullEventAssembler<L1CaloSrcIdMap> fea;
  fea.clear();
  const uint16_t minorVersion = m_srcIdMap.minorVersion();
  fea.setRodMinorVersion(minorVersion);

  // Pointer to ROD data vector

  FullEventAssembler<L1CaloSrcIdMap>::RODDATA* theROD = 0;

  // Set up the container maps

  LVL1::TriggerTowerKey towerKey;

  // CPM tower map
  ConstCpmTowerMap ttMap;
  setupCpmTowerMap(cp->towers(), ttMap, towerKey);

  // CPM hits map
  ConstCpmHitsMap hitsMap;
  setupCpmHitsMap(cp->hits(), hitsMap);

  // CMM-CP hits map
  ConstCmmCpHitsMap cmmHitsMap;
  setupCmmCpHitsMap(cp->cmmHits(), cmmHitsMap);

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
                           timeslices, trigCpm,
                           ttMap,
                           hitsMap,
                           cmmHitsMap,
                           towerKey))
        {
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
	const uint32_t rodIdCpm = m_srcIdMap.getRodID(hwCrate, slink, daqOrRoi,
	                                                        m_subDetector);
	theROD = fea.getRodData(rodIdCpm);
	theROD->push_back(userHeader.header());
      }
      if (debug) msg() << "Module " << module << endmsg;

      // Create a sub-block for each slice (except Neutral format)

      // Vector for current CPM sub-blocks
      DataVector<CpmSubBlock> cpmBlocks;

      for (int slice = 0; slice < timeslicesNew; ++slice) {
        CpmSubBlock* const subBlock = new CpmSubBlock();
	subBlock->setCpmHeader(m_version, m_dataFormat, slice,
	                       hwCrate, module, timeslicesNew);
        cpmBlocks.push_back(subBlock);
	if (neutralFormat) break;
      }

      // Find CPM towers corresponding to each eta/phi pair and fill
      // sub-blocks

      for (int chan=0; chan < m_channels; ++chan) {
	double eta = 0.;
	double phi = 0.;
	int layer = 0;
	if (m_cpmMaps->mapping(crate, module, chan, eta, phi, layer)) {
          const unsigned int key = towerKey.ttKey(phi, eta);
          const LVL1::CPMTower* const tt = findCpmTower(key, ttMap);
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
              CpmSubBlock* const subBlock = cpmBlocks[index];
              subBlock->fillTowerData(slice, chan, emData[slice],
	                              hadData[slice], emErr, hadErr);
	    }
          }
        }
      }

      // Add CPM hits

      const LVL1::CPMHits* const hits = findCpmHits(crate, module, hitsMap);
      if (hits) {
        std::vector<unsigned int> vec0;
        std::vector<unsigned int> vec1;
	ModifySlices::data(hits->HitsVec0(), vec0, timeslicesNew);
	ModifySlices::data(hits->HitsVec1(), vec1, timeslicesNew);
        for (int slice = 0; slice < timeslicesNew; ++slice) {
	  const int index = ( neutralFormat ) ? 0 : slice;
	  CpmSubBlock* const subBlock = cpmBlocks[index];
	  subBlock->setHits(slice, vec0[slice], vec1[slice]);
        }
      }
      
      // Pack and write the sub-blocks

      DataVector<CpmSubBlock>::iterator pos;
      for (pos = cpmBlocks.begin(); pos != cpmBlocks.end(); ++pos) {
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

    /// Vector for current CMM-CP hit0 sub-blocks
    DataVector<CmmCpSubBlock> cmmHit0Blocks;
    /// Vector for current CMM-CP hit1 sub-blocks
    DataVector<CmmCpSubBlock> cmmHit1Blocks;

    const int summing = (crate == m_crates - 1) ? CmmSubBlock::SYSTEM
                                                : CmmSubBlock::CRATE;
    for (int slice = 0; slice < timeslicesNew; ++slice) {
      CmmCpSubBlock* const h0Block = new CmmCpSubBlock();
      h0Block->setCmmHeader(m_version, m_dataFormat, slice, hwCrate,
                            summing, CmmSubBlock::CMM_CP,
			    CmmSubBlock::RIGHT, timeslicesNew);
      cmmHit0Blocks.push_back(h0Block);
      CmmCpSubBlock* const h1Block = new CmmCpSubBlock();
      h1Block->setCmmHeader(m_version, m_dataFormat, slice, hwCrate,
                            summing, CmmSubBlock::CMM_CP,
			    CmmSubBlock::LEFT, timeslicesNew);
      cmmHit1Blocks.push_back(h1Block);
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
      const LVL1::CMMCPHits* const ch = findCmmCpHits(crate, dataID, cmmHitsMap);
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
	  CmmCpSubBlock* subBlock = cmmHit0Blocks[index];
	  subBlock->setHits(slice, source, hits0[slice],
	                           err0Bits.get(LVL1::DataError::Parity));
	  subBlock = cmmHit1Blocks[index];
	  subBlock->setHits(slice, source, hits1[slice],
	                           err1Bits.get(LVL1::DataError::Parity));
        }
      }
    }
    DataVector<CmmCpSubBlock>::iterator cos = cmmHit0Blocks.begin();
    for (; cos != cmmHit0Blocks.end(); ++cos) {
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
    cos = cmmHit1Blocks.begin();
    for (; cos != cmmHit1Blocks.end(); ++cos) {
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

  fea.fill(re, msg());

  return StatusCode::SUCCESS;
}

// Return reference to vector with all possible Source Identifiers

std::vector<uint32_t> CpByteStreamTool::makeSourceIDs() const
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

const std::vector<uint32_t>& CpByteStreamTool::sourceIDs() const
{
  static const std::vector<uint32_t> sourceIDs = makeSourceIDs();
  return sourceIDs;
}

// Convert bytestream to given container type

StatusCode CpByteStreamTool::convertBs(
                            const std::string& sgKey,
                            const IROBDataProviderSvc::VROBFRAG& robFrags,
                            CpByteStreamToolData& data) const
{
  LocalData ld;

  // Check if overlap tower channels wanted
  const std::string flag("Overlap");
  const std::string::size_type pos = sgKey.find(flag);
  ld.coreOverlap =
   (pos == std::string::npos || pos != sgKey.length() - flag.length()) ? 0 : 1;

  const bool debug = msgLvl(MSG::DEBUG);
  if (debug) msg(MSG::DEBUG);

  // Loop over ROB fragments

  CmmCpSubBlock cmmCpSubBlock;
  CpmSubBlock cpmSubBlock;

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
       (m_srcIdMap.slink(sourceID) != 0 && m_srcIdMap.slink(sourceID) != 2) ||
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

    ld.rodErr = L1CaloSubBlock::ERROR_NONE;
    while (payload != payloadEnd) {
      
      if (L1CaloSubBlock::wordType(*payload) != L1CaloSubBlock::HEADER) {
        if (debug) msg() << "Unexpected data sequence" << endmsg;
	ld.rodErr = L1CaloSubBlock::ERROR_MISSING_HEADER;
	break;
      }
      if (CmmSubBlock::cmmBlock(*payload)) {
        // CMM
	if (CmmSubBlock::cmmType(*payload) == CmmSubBlock::CMM_CP) {
	  cmmCpSubBlock.clear();
          payload = cmmCpSubBlock.read(payload, payloadEnd);
	  if (cmmCpSubBlock.crate() != rodCrate) {
	    if (debug) msg() << "Inconsistent crate number in ROD source ID"
	                     << endmsg;
	    ld.rodErr = L1CaloSubBlock::ERROR_CRATE_NUMBER;
	    break;
          }
	  if (data.m_collection == CMM_CP_HITS) {
	    decodeCmmCp(&cmmCpSubBlock, trigCmm, static_cast<CmmHitsData&>(data),
                        ld);
	    if (ld.rodErr != L1CaloSubBlock::ERROR_NONE) {
	      if (debug) msg() << "decodeCmmCp failed" << endmsg;
	      break;
	    }
          }
        } else {
          if (debug) msg() << "Invalid CMM type in module field" << endmsg;
          ld.rodErr = L1CaloSubBlock::ERROR_MODULE_NUMBER;
	  break;
        }
      } else {
        // CPM
        CpmSubBlock subBlock;
	cpmSubBlock.clear();
        payload = cpmSubBlock.read(payload, payloadEnd);
	if (cpmSubBlock.crate() != rodCrate) {
	  if (debug) msg() << "Inconsistent crate number in ROD source ID"
	                   << endmsg;
	  ld.rodErr = L1CaloSubBlock::ERROR_CRATE_NUMBER;
	  break;
        }
	if (data.m_collection == CPM_TOWERS || data.m_collection == CPM_HITS) {
	  decodeCpm(&cpmSubBlock, trigCpm, data, ld);
	  if (ld.rodErr != L1CaloSubBlock::ERROR_NONE) {
	    if (debug) msg() << "decodeCpm failed" << endmsg;
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

// Unpack CMM-CP sub-block

void CpByteStreamTool::decodeCmmCp(CmmCpSubBlock* subBlock, int trigCmm,
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
      msg() << "CMM-CP sub-block unpacking failed: " << errMsg << endmsg;
    }
    ld.rodErr = subBlock->unpackErrorCode();
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
	  ld.hitsVec0.assign(timeslices, 0);
	  ld.hitsVec1.assign(timeslices, 0);
	  ld.errVec0.assign(timeslices, 0);
	  ld.errVec1.assign(timeslices, 0);
	  if (module == CmmSubBlock::RIGHT) {
	    ld.hitsVec0[slice] = hits;
	    ld.errVec0[slice]  = err;
	  } else {
	    ld.hitsVec1[slice] = hits;
	    ld.errVec1[slice]  = err;
	  }
          auto chp =
            std::make_unique<LVL1::CMMCPHits>(swCrate, dataID, ld.hitsVec0, ld.hitsVec1,
                                              ld.errVec0, ld.errVec1, trigCmm);
          const int key = crate*100 + dataID;
	  data.m_cmmHitsMap.insert(std::make_pair(key, chp.get()));
	  data.m_cmmHitCollection->push_back(std::move(chp));
        } else {
	  ld.hitsVec0 = ch->HitsVec0();
	  ld.hitsVec1 = ch->HitsVec1();
	  ld.errVec0  = ch->ErrorVec0();
	  ld.errVec1  = ch->ErrorVec1();
	  const int nsl = ld.hitsVec0.size();
	  if (timeslices != nsl) {
	    if (debug) msg() << "Inconsistent number of slices in sub-blocks"
	                     << endmsg;
            ld.rodErr = L1CaloSubBlock::ERROR_SLICES;
	    return;
          }
	  if ((module == CmmSubBlock::RIGHT && (ld.hitsVec0[slice] != 0 ||
	       ld.errVec0[slice] != 0)) || (module == CmmSubBlock::LEFT &&
	       (ld.hitsVec1[slice] != 0 || ld.errVec1[slice]  != 0))) {
            if (debug) msg() << "Duplicate data for slice " << slice << endmsg;
	    ld.rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
	    return;
          }
	  if (module == CmmSubBlock::RIGHT) {
	    ld.hitsVec0[slice] = hits;
	    ld.errVec0[slice]  = err;
	  } else {
	    ld.hitsVec1[slice] = hits;
	    ld.errVec1[slice]  = err;
	  }
	  ch->addHits(ld.hitsVec0, ld.hitsVec1, ld.errVec0, ld.errVec1);
        }
      }
    }
  }
  
  return;
}

// Unpack CPM sub-block

void CpByteStreamTool::decodeCpm(CpmSubBlock* subBlock,
                                 int trigCpm, CpByteStreamToolData& data,
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
    msg() << "CPM: Crate "     << hwCrate
          << "  Module "       << module
          << "  Total slices " << timeslices
          << "  Slice "        << sliceNum    << endmsg;
  }
  if (module < 1 || module > m_modules) {
    if (debug) msg() << "Unexpected module number: " << module << endmsg;
    ld.rodErr = L1CaloSubBlock::ERROR_MODULE_NUMBER;
    return;
  }
  if (timeslices <= trigCpm) {
    if (debug) msg() << "Triggered CPM slice from header "
                     << "inconsistent with number of slices: "
                     << trigCpm << ", " << timeslices << endmsg;
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
      msg() << "CPM sub-block unpacking failed: " << errMsg << endmsg;
    }
    ld.rodErr = subBlock->unpackErrorCode();
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
	    if (layer == ld.coreOverlap) {
	      const unsigned int key = ld.towerKey->ttKey(phi, eta);
	      LVL1::CPMTower* tt = findCpmTower(tdata, key);
	      if ( ! tt ) {   // create new CPM tower
	        ld.emVec.assign(timeslices, 0);
	        ld.hadVec.assign(timeslices, 0);
	        ld.emErrVec.assign(timeslices, 0);
	        ld.hadErrVec.assign(timeslices, 0);
	        ld.emVec[slice]     = em;
	        ld.hadVec[slice]    = had;
	        ld.emErrVec[slice]  = emErr1;
	        ld.hadErrVec[slice] = hadErr1;
                auto ttp = 
                  std::make_unique<LVL1::CPMTower>(phi, eta, ld.emVec, ld.emErrVec,
                                                   ld.hadVec, ld.hadErrVec, trigCpm);
	        tdata.m_ttMap.insert(std::make_pair(key, ttp.get()));
	        tdata.m_ttCollection->push_back(std::move(ttp));
              } else {
	        ld.emVec     = tt->emEnergyVec();
	        ld.hadVec    = tt->hadEnergyVec();
	        ld.emErrVec  = tt->emErrorVec();
	        ld.hadErrVec = tt->hadErrorVec();
		const int nsl = ld.emVec.size();
	        if (timeslices != nsl) {
	          if (debug) {
		    msg() << "Inconsistent number of slices in sub-blocks"
	                  << endmsg;
	          }
                  ld.rodErr = L1CaloSubBlock::ERROR_SLICES;
	          return;
                }
		if (ld.emVec[slice]    != 0 || ld.hadVec[slice]    != 0 ||
		    ld.emErrVec[slice] != 0 || ld.hadErrVec[slice] != 0) {
                  if (debug) msg() << "Duplicate data for slice "
		                   << slice << endmsg;
	          ld.rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
	          return;
                }
	        ld.emVec[slice]     = em;
	        ld.hadVec[slice]    = had;
	        ld.emErrVec[slice]  = emErr1;
	        ld.hadErrVec[slice] = hadErr1;
	        tt->fill(ld.emVec, ld.emErrVec, ld.hadVec, ld.hadErrVec, trigCpm);
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
	  ld.hitsVec0.assign(timeslices, 0);
	  ld.hitsVec1.assign(timeslices, 0);
	  ld.hitsVec0[slice] = hits0;
	  ld.hitsVec1[slice] = hits1;
          auto chp = 
            std::make_unique<LVL1::CPMHits>(swCrate, module, ld.hitsVec0, ld.hitsVec1, trigCpm);
	  hdata.m_hitsMap.insert(std::make_pair(crate*m_modules+module-1, chp.get()));
	  hdata.m_hitCollection->push_back(std::move(chp));
        } else {
	  ld.hitsVec0 = ch->HitsVec0();
	  ld.hitsVec1 = ch->HitsVec1();
	  const int nsl = ld.hitsVec0.size();
	  if (timeslices != nsl) {
	    if (debug) msg() << "Inconsistent number of slices in sub-blocks"
	                     << endmsg;
            ld.rodErr = L1CaloSubBlock::ERROR_SLICES;
	    return;
          }
	  if (ld.hitsVec0[slice] != 0 || ld.hitsVec1[slice] != 0) {
            if (debug) msg() << "Duplicate data for slice " << slice << endmsg;
	    ld.rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
	    return;
          }
	  ld.hitsVec0[slice] = hits0;
	  ld.hitsVec1[slice] = hits1;
	  ch->addHits(ld.hitsVec0, ld.hitsVec1);
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
LVL1::CPMTower* CpByteStreamTool::findCpmTower(const unsigned int key,
                                               const ConstCpmTowerMap& ttMap) const
{
  ConstCpmTowerMap::const_iterator mapIter = ttMap.find(key);
  if (mapIter != ttMap.end()) return mapIter->second;
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
LVL1::CPMHits* CpByteStreamTool::findCpmHits(const int crate,
                                             const int module,
                                             const ConstCpmHitsMap& hitsMap) const
{
  ConstCpmHitsMap::const_iterator mapIter =
    hitsMap.find(crate*m_modules + module - 1);
  if (mapIter != hitsMap.end()) return mapIter->second;
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
                                                 const int dataID,
                                                 const ConstCmmCpHitsMap& cmmHitsMap) const
{
  ConstCmmCpHitsMap::const_iterator mapIter = cmmHitsMap.find(crate*100 + dataID);
  if (mapIter != cmmHitsMap.end()) return mapIter->second;
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
                                        const ttCollection,
                                        ConstCpmTowerMap& ttMap,
                                        LVL1::TriggerTowerKey& towerKey) const
{
  ttMap.clear();
  if (ttCollection) {
    CpmTowerCollection::const_iterator pos  = ttCollection->begin();
    CpmTowerCollection::const_iterator pose = ttCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CPMTower* const tt = *pos;
      const unsigned int key = towerKey.ttKey(tt->phi(), tt->eta());
      ttMap.insert(std::make_pair(key, tt));
    }
  }
}

// Set up CPM hits map

void CpByteStreamTool::setupCpmHitsMap(const CpmHitsCollection*
                                       const hitCollection,
                                       ConstCpmHitsMap& hitsMap) const
{
  hitsMap.clear();
  if (hitCollection) {
    CpmHitsCollection::const_iterator pos  = hitCollection->begin();
    CpmHitsCollection::const_iterator pose = hitCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CPMHits* const hits = *pos;
      const int crate = hits->crate() - m_crateOffsetSw;
      const int key   = m_modules * crate + hits->module() - 1;
      hitsMap.insert(std::make_pair(key, hits));
    }
  }
}

// Set up CMM-CP hits map

void CpByteStreamTool::setupCmmCpHitsMap(const CmmCpHitsCollection*
                                         const hitCollection,
                                         ConstCmmCpHitsMap& cmmHitsMap) const
{
  cmmHitsMap.clear();
  if (hitCollection) {
    CmmCpHitsCollection::const_iterator pos  = hitCollection->begin();
    CmmCpHitsCollection::const_iterator pose = hitCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CMMCPHits* const hits = *pos;
      const int crate = hits->crate() - m_crateOffsetSw;
      const int key   = crate*100 + hits->dataID();
      cmmHitsMap.insert(std::make_pair(key, hits));
    }
  }
}

// Get number of slices and triggered slice offset for next slink

bool CpByteStreamTool::slinkSlices(const int crate, const int module,
                                   const int modulesPerSlink,
                                   int& timeslices,
                                   int& trigCpm,
                                   const ConstCpmTowerMap& ttMap,
                                   const ConstCpmHitsMap& hitsMap,
                                   const ConstCmmCpHitsMap& cmmHitsMap,
                                   LVL1::TriggerTowerKey& towerKey) const
{
  int slices = -1;
  int trigC  = m_dfltSlices/2;
  for (int mod = module; mod < module + modulesPerSlink; ++mod) {
    for (int chan = 0; chan < m_channels; ++chan) {
      double eta = 0.;
      double phi = 0.;
      int layer = 0;
      if ( !m_cpmMaps->mapping(crate, mod, chan, eta, phi, layer)) continue;
      const unsigned int key = towerKey.ttKey(phi, eta);
      const LVL1::CPMTower* const tt = findCpmTower(key, ttMap);
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
    const LVL1::CPMHits* const hits = findCpmHits(crate, mod, hitsMap);
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
      const LVL1::CMMCPHits* const hits = findCmmCpHits(crate, dataID, cmmHitsMap);
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
