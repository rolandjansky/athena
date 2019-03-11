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

#include "TrigT1CaloEvent/CMXJetHits.h"
#include "TrigT1CaloEvent/CMXJetTob.h"
#include "TrigT1CaloEvent/CMXEtSums.h"
#include "TrigT1CaloEvent/JEMEtSums.h"
#include "TrigT1CaloEvent/JEPBSCollectionV2.h"
#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1CaloUtils/JetElementKey.h"
#include "TrigT1CaloMappingToolInterfaces/IL1CaloMappingTool.h"

#include "CmxEnergySubBlock.h"
#include "CmxJetSubBlock.h"
#include "CmxSubBlock.h"
#include "JemJetElement.h"
#include "JemSubBlockV2.h"
#include "L1CaloErrorByteStreamTool.h"
#include "L1CaloSrcIdMap.h"
#include "L1CaloSubBlock.h"
#include "L1CaloUserHeader.h"
#include "ModifySlices.h"

#include "JepByteStreamV2Tool.h"

namespace LVL1BS {

// Interface ID

static const InterfaceID IID_IJepByteStreamV2Tool("JepByteStreamV2Tool", 1, 1);

const InterfaceID& JepByteStreamV2Tool::interfaceID()
{
  return IID_IJepByteStreamV2Tool;
}

// Constructor

JepByteStreamV2Tool::JepByteStreamV2Tool(const std::string& type,
                                     const std::string& name,
				     const IInterface*  parent)
  : AthAlgTool(type, name, parent),
    m_robDataProvider("ROBDataProviderSvc", name),
    m_jemMaps("LVL1::JemMappingTool/JemMappingTool"),
    m_errorTool("LVL1BS::L1CaloErrorByteStreamTool/L1CaloErrorByteStreamTool"),
    m_channels(44), m_crates(2), m_modules(16), m_frames(8), m_locations(4),
    m_maxTobs(4), m_coreOverlap(0),
    m_subDetector(eformat::TDAQ_CALO_JET_PROC_DAQ),
    m_srcIdMap(0), m_elementKey(0),
    m_jemSubBlock(0), m_cmxEnergySubBlock(0), m_cmxJetSubBlock(0),
    m_rodStatus(0), m_fea(0)
{
  declareInterface<JepByteStreamV2Tool>(this);

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
  declareProperty("ROBSourceIDs",       m_sourceIDs,
                  "ROB fragment source identifiers");

  // Properties for writing bytestream only
  declareProperty("DataVersion",    m_version     = 2,                      //<<== CHECK
                  "Format version number in sub-block header");
  declareProperty("DataFormat",     m_dataFormat  = 1,
                  "Format identifier (0-1) in sub-block header");
  declareProperty("SimulSlices",    m_dfltSlices  = 1,
                  "The number of slices in the simulation");
  declareProperty("ForceSlices",    m_forceSlices = 0,
                  "If >0, the number of slices in bytestream");
  declareProperty("CrateMin",       m_crateMin = 0,
                  "Minimum crate number, allows partial output");
  declareProperty("CrateMax",       m_crateMax = m_crates-1,
                  "Maximum crate number, allows partial output");

}

// Destructor

JepByteStreamV2Tool::~JepByteStreamV2Tool()
{
}

// Initialize


StatusCode JepByteStreamV2Tool::initialize()
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
  m_jemSubBlock       = new JemSubBlockV2();
  m_cmxEnergySubBlock = new CmxEnergySubBlock();
  m_cmxJetSubBlock    = new CmxJetSubBlock();
  m_rodStatus         = new std::vector<uint32_t>(2);
  m_fea               = new FullEventAssembler<L1CaloSrcIdMap>();
  return StatusCode::SUCCESS;
}

// Finalize

StatusCode JepByteStreamV2Tool::finalize()
{
  delete m_fea;
  delete m_rodStatus;
  delete m_cmxJetSubBlock;
  delete m_cmxEnergySubBlock;
  delete m_jemSubBlock;
  delete m_elementKey;
  delete m_srcIdMap;
  return StatusCode::SUCCESS;
}

// Conversion bytestream to jet elements

StatusCode JepByteStreamV2Tool::convert(
    const std::string& sgKey,
    DataVector<LVL1::JetElement>* collection)
{
 const std::vector<uint32_t>& vID(sourceIDs(sgKey));
  // // get ROB fragments
  IROBDataProviderSvc::VROBFRAG robFrags;
  m_robDataProvider->getROBData(vID, robFrags, "JepByteStreamV2Tool");
  ATH_MSG_DEBUG("Number of ROB fragments:" << robFrags.size());
  return convert(robFrags, collection);
}

StatusCode JepByteStreamV2Tool::convert(
                            const IROBDataProviderSvc::VROBFRAG& robFrags,
                            DataVector<LVL1::JetElement>*  jeCollection)
{
  JetElementData data (jeCollection);
  return convertBs(robFrags, data);
}

// Conversion bytestream to energy sums
StatusCode JepByteStreamV2Tool::convert(
    const std::string& sgKey,
    DataVector<LVL1::JEMEtSums>* collection)
{
 const std::vector<uint32_t>& vID(sourceIDs(sgKey));
  // // get ROB fragments
  IROBDataProviderSvc::VROBFRAG robFrags;
  m_robDataProvider->getROBData(vID, robFrags, "JepByteStreamV2Tool");
  ATH_MSG_DEBUG("Number of ROB fragments:" << robFrags.size());
  return convert(robFrags, collection);
}

StatusCode JepByteStreamV2Tool::convert(
                            const IROBDataProviderSvc::VROBFRAG& robFrags,
                            DataVector<LVL1::JEMEtSums>*  etCollection)
{
  EnergySumsData data (etCollection);
  return convertBs(robFrags, data);
}

// Conversion bytestream to CMX TOBs
StatusCode JepByteStreamV2Tool::convert(
    const std::string& sgKey,
    DataVector<LVL1::CMXJetTob>* collection)
{
 const std::vector<uint32_t>& vID(sourceIDs(sgKey));
  // // get ROB fragments
  IROBDataProviderSvc::VROBFRAG robFrags;
  m_robDataProvider->getROBData(vID, robFrags, "JepByteStreamV2Tool");
  ATH_MSG_DEBUG("Number of ROB fragments:" << robFrags.size());
  return convert(robFrags, collection);
}

StatusCode JepByteStreamV2Tool::convert(
                            const IROBDataProviderSvc::VROBFRAG& robFrags,
                            DataVector<LVL1::CMXJetTob>*  tobCollection)
{
  CmxTobData data (tobCollection);
  return convertBs(robFrags, data);
}

// Conversion bytestream to CMX hits
StatusCode JepByteStreamV2Tool::convert(
    const std::string& sgKey,
    DataVector<LVL1::CMXJetHits>* collection)
{
 const std::vector<uint32_t>& vID(sourceIDs(sgKey));
  // // get ROB fragments
  IROBDataProviderSvc::VROBFRAG robFrags;
  m_robDataProvider->getROBData(vID, robFrags, "JepByteStreamV2Tool");
  ATH_MSG_DEBUG("Number of ROB fragments:" << robFrags.size());
  return convert(robFrags, collection);
}

StatusCode JepByteStreamV2Tool::convert(
                            const IROBDataProviderSvc::VROBFRAG& robFrags,
                            DataVector<LVL1::CMXJetHits>*  hitCollection)
{
  CmxHitsData data (hitCollection);
  return convertBs(robFrags, data);
}

// Conversion bytestream to CMX energy sums

StatusCode JepByteStreamV2Tool::convert(
    const std::string& sgKey,
    DataVector<LVL1::CMXEtSums>* collection)
{
 const std::vector<uint32_t>& vID(sourceIDs(sgKey));
  // // get ROB fragments
  IROBDataProviderSvc::VROBFRAG robFrags;
  m_robDataProvider->getROBData(vID, robFrags, "JepByteStreamV2Tool");
  ATH_MSG_DEBUG("Number of ROB fragments:" << robFrags.size());
  return convert(robFrags, collection);
}

StatusCode JepByteStreamV2Tool::convert(
                            const IROBDataProviderSvc::VROBFRAG& robFrags,
                            DataVector<LVL1::CMXEtSums>*  etCollection)
{
  CmxSumsData data (etCollection);
  return convertBs(robFrags, data);
}

// Conversion of JEP container to bytestream

StatusCode JepByteStreamV2Tool::convert(const LVL1::JEPBSCollectionV2* const jep,
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
  setupEtMap(jep->EnergySums());
  setupCmxTobMap(jep->CmxTobs());
  setupCmxHitsMap(jep->CmxHits());
  setupCmxEtMap(jep->CmxSums());

  // Loop over data

  const bool neutralFormat = m_dataFormat == L1CaloSubBlock::NEUTRAL;
  const int modulesPerSlink = m_modules / m_slinks;
  int timeslices       = 1;
  int trigJem          = 0;
  int timeslicesNew    = 1;
  int trigJemNew       = 0;
  for (int crate = m_crateMin; crate <= m_crateMax; ++crate) {
    const int hwCrate = crate + m_crateOffsetHw;

    for (int module=0; module < m_modules; ++module) {

      // Pack required number of modules per slink

      if (module%modulesPerSlink == 0) {
	const int daqOrRoi = 0;
	const int slink = module/modulesPerSlink;
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
        JemSubBlockV2* const subBlock = new JemSubBlockV2();
	subBlock->setJemHeader(m_version, m_dataFormat, slice,
	                       hwCrate, module, timeslicesNew);
        m_jemBlocks.push_back(subBlock);
	if (neutralFormat) break;
      }

      // Find jet elements corresponding to each eta/phi pair and fill
      // sub-blocks

      for (int chan=0; chan < m_channels; ++chan) {
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
              JemSubBlockV2* const subBlock = m_jemBlocks[index];
	      const JemJetElement jetEle(chan, emData[slice], hadData[slice],
	                  emErrBits.get(LVL1::DataError::Parity),
	                  hadErrBits.get(LVL1::DataError::Parity),
			  emErrBits.get(LVL1::DataError::LinkDown) +
			 (hadErrBits.get(LVL1::DataError::LinkDown) << 1));
              subBlock->fillJetElement(slice, jetEle);
	      if ((emErrBits.error() >> LVL1::DataError::GLinkParity)) {
	        int gLinkParity   = emErrBits.get(LVL1::DataError::GLinkParity);
		int gLinkProtocol = emErrBits.get(LVL1::DataError::GLinkProtocol);
		int bCNMismatch   = emErrBits.get(LVL1::DataError::BCNMismatch);
		int fIFOOverflow  = emErrBits.get(LVL1::DataError::FIFOOverflow);
		int moduleError   = emErrBits.get(LVL1::DataError::ModuleError);
		int gLinkDown     = emErrBits.get(LVL1::DataError::GLinkDown);
		int gLinkTimeout  = emErrBits.get(LVL1::DataError::GLinkTimeout);
		uint32_t failingBCN = emErrBits.get(LVL1::DataError::FailingBCN);
		subBlock->setStatus(failingBCN, gLinkTimeout, gLinkDown,
		                    moduleError, fIFOOverflow, bCNMismatch,
				    gLinkProtocol, gLinkParity);
	      }
	    }
          }
        }
      }

      // Add energy subsums

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
	  JemSubBlockV2* const subBlock = m_jemBlocks[index];
	  subBlock->setEnergySubsums(slice, exVec[slice], eyVec[slice],
	                                                  etVec[slice]);
        }
      }
      
      // Pack and write the sub-blocks

      DataVector<JemSubBlockV2>::iterator pos;
      for (pos = m_jemBlocks.begin(); pos != m_jemBlocks.end(); ++pos) {
        JemSubBlockV2* const subBlock = *pos;
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

    // Append CMXs to last S-Link of the crate

    // Create a sub-block for each slice (except Neutral format)

    m_cmxEnergyBlocks.clear();
    m_cmxJetBlocks.clear();
    const int summing = (crate == m_crates - 1) ? CmxSubBlock::SYSTEM
                                                : CmxSubBlock::CRATE;
    for (int slice = 0; slice < timeslicesNew; ++slice) {
      CmxEnergySubBlock* const enBlock = new CmxEnergySubBlock();
      const int cmxEnergyVersion = 3;                             // <<== CHECK  Make jo property for each sub-block?
      enBlock->setCmxHeader(cmxEnergyVersion, m_dataFormat, slice, hwCrate,
                            summing, CmxSubBlock::CMX_ENERGY,
			    CmxSubBlock::LEFT, timeslicesNew);
      m_cmxEnergyBlocks.push_back(enBlock);
      CmxJetSubBlock* const jetBlock = new CmxJetSubBlock();
      jetBlock->setCmxHeader(m_version, m_dataFormat, slice, hwCrate,
                             summing, CmxSubBlock::CMX_JET,
			     CmxSubBlock::RIGHT, timeslicesNew);
      m_cmxJetBlocks.push_back(jetBlock);
      if (neutralFormat) break;
    }

    // CMX-Energy

    int maxSource = static_cast<int>(LVL1::CMXEtSums::MAX_SOURCE);
    for (int source = 0; source < maxSource; ++source) {
      if (source >= m_modules) {
        if (summing == CmxSubBlock::CRATE && 
	    source != LVL1::CMXEtSums::LOCAL_STANDARD &&
	    source != LVL1::CMXEtSums::LOCAL_RESTRICTED) continue;
      }
      const LVL1::CMXEtSums* const sums = findCmxSums(crate, source);
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
	  int exError = exErrBits.get(LVL1::DataError::Parity) << 1;
	  int eyError = eyErrBits.get(LVL1::DataError::Parity) << 1;
	  int etError = etErrBits.get(LVL1::DataError::Parity) << 1;
	  if (source >= m_modules) {
	    exError += exErrBits.get(LVL1::DataError::Overflow);
	    eyError += eyErrBits.get(LVL1::DataError::Overflow);
	    etError += etErrBits.get(LVL1::DataError::Overflow);
	  }
	  const int index = ( neutralFormat ) ? 0 : slice;
	  CmxEnergySubBlock* const subBlock = m_cmxEnergyBlocks[index];
	  if (source < m_modules) {
	    subBlock->setSubsums(slice, source,
	                         ex[slice], ey[slice], et[slice],
				 exError, eyError, etError);
          } else {
	    CmxEnergySubBlock::SourceType srcType = CmxEnergySubBlock::MAX_SOURCE_TYPE;
	    CmxEnergySubBlock::SumType    sumType = CmxEnergySubBlock::MAX_SUM_TYPE;
	    CmxEnergySubBlock::HitsType   hitType = CmxEnergySubBlock::MAX_HITS_TYPE;
	    energySubBlockTypes(source, srcType, sumType, hitType);
	    if (srcType != CmxEnergySubBlock::MAX_SOURCE_TYPE) {
	      subBlock->setSubsums(slice, srcType, sumType,
	                           ex[slice], ey[slice], et[slice],
				   exError, eyError, etError);
            } else if (hitType != CmxEnergySubBlock::MAX_HITS_TYPE) {
	      subBlock->setEtHits(slice, hitType, sumType, et[slice]);
            }
          }
        }
      }
    }
    DataVector<CmxEnergySubBlock>::iterator pos;
    pos = m_cmxEnergyBlocks.begin();
    for (; pos != m_cmxEnergyBlocks.end(); ++pos) {
      CmxEnergySubBlock* const subBlock = *pos;
      if ( !subBlock->pack()) {
        msg(MSG::ERROR) << "CMX-Energy sub-block packing failed" << endmsg;
	return StatusCode::FAILURE;
      }
      if (debug) {
	msg() << "CMX-Energy sub-block data words: "
	      << subBlock->dataWords() << endmsg;
      }
      subBlock->write(theROD);
    }

    // CMX-Jet TOBs

    for (int jem = 0; jem < m_modules; ++jem) {
      for (int frame = 0; frame < m_frames; ++frame) {
        for (int loc = 0; loc < m_locations; ++loc) {
	  const int key = tobKey(crate, jem, frame, loc);
          const LVL1::CMXJetTob* const ct = findCmxTob(key);
          if ( ct ) {
            std::vector<int> energyLarge;
            std::vector<int> energySmall;
            std::vector<int> error;
            std::vector<unsigned int> presence;
            ModifySlices::data(ct->energyLgVec(),    energyLarge, timeslicesNew);
            ModifySlices::data(ct->energySmVec(),    energySmall, timeslicesNew);
            ModifySlices::data(ct->errorVec(),       error,       timeslicesNew);
            ModifySlices::data(ct->presenceMapVec(), presence,    timeslicesNew);
            for (int slice = 0; slice < timeslicesNew; ++slice) {
              const LVL1::DataError errBits(error[slice]);
	      int err0 = errBits.get(LVL1::DataError::Parity);
              int err1 = errBits.get(LVL1::DataError::ParityPhase0);
              err1 |= (errBits.get(LVL1::DataError::ParityPhase1))<<1;
              err1 |= (errBits.get(LVL1::DataError::ParityPhase2))<<2;
              err1 |= (errBits.get(LVL1::DataError::ParityPhase3))<<3;
              const int index = ( neutralFormat ) ? 0 : slice;
              CmxJetSubBlock* const subBlock = m_cmxJetBlocks[index];
              subBlock->setTob(slice, jem, frame, loc, energyLarge[slice],
                                                  energySmall[slice], err0);
	      subBlock->setParityBits(slice, jem, err1); // for neutral format
              subBlock->setPresenceMap(slice, jem, presence[slice]);
            }
          }
        }
      }
    }

    // CMX-Jet Hits

    maxSource = static_cast<int>(LVL1::CMXJetHits::MAX_SOURCE);
    for (int source = 0; source < maxSource; ++source) {
      if (summing == CmxSubBlock::CRATE && 
          (source == LVL1::CMXJetHits::REMOTE_MAIN    ||
	   source == LVL1::CMXJetHits::TOTAL_MAIN     ||
	   source == LVL1::CMXJetHits::REMOTE_FORWARD ||
	   source == LVL1::CMXJetHits::TOTAL_FORWARD)) continue;
      int sourceId = jetSubBlockSourceId(source);
      if (sourceId == CmxJetSubBlock::MAX_SOURCE_ID) continue;
      const LVL1::CMXJetHits* const ch = findCmxHits(crate, source);
      if ( ch ) {
        std::vector<unsigned int> hits0;
        std::vector<unsigned int> hits1;
        std::vector<int> err0;
        std::vector<int> err1;
	ModifySlices::data(ch->hitsVec0(),  hits0, timeslicesNew);
	ModifySlices::data(ch->hitsVec1(),  hits1, timeslicesNew);
	ModifySlices::data(ch->errorVec0(), err0,  timeslicesNew);
	ModifySlices::data(ch->errorVec1(), err1,  timeslicesNew);
	for (int slice = 0; slice < timeslicesNew; ++slice) {
	  int error = 0;
	  if (source != LVL1::CMXJetHits::TOPO_CHECKSUM &&
	      source != LVL1::CMXJetHits::TOPO_OCCUPANCY_MAP &&
	      source != LVL1::CMXJetHits::TOPO_OCCUPANCY_COUNTS) {
	    const LVL1::DataError errBits0(err0[slice]);
	    const LVL1::DataError errBits1(err1[slice]);
	    error = (errBits0.get(LVL1::DataError::Overflow) |
	             errBits1.get(LVL1::DataError::Overflow)) << 2;
	    if (source == LVL1::CMXJetHits::REMOTE_MAIN ||
	        source == LVL1::CMXJetHits::REMOTE_FORWARD) {
	      error += (errBits0.get(LVL1::DataError::Parity) +
	               (errBits1.get(LVL1::DataError::Parity) << 1));
	    }
	  }
	  const int index = ( neutralFormat ) ? 0 : slice;
	  CmxJetSubBlock* const subBlock = m_cmxJetBlocks[index];
	  subBlock->setHits(slice, sourceId, 0, hits0[slice], error);
	  if (source != LVL1::CMXJetHits::TOPO_CHECKSUM &&
	      source != LVL1::CMXJetHits::TOPO_OCCUPANCY_MAP) {
	    subBlock->setHits(slice, sourceId, 1, hits1[slice], error);
          }
        }
      }
    }
    DataVector<CmxJetSubBlock>::iterator jos;
    jos = m_cmxJetBlocks.begin();
    for (; jos != m_cmxJetBlocks.end(); ++jos) {
      CmxJetSubBlock* const subBlock = *jos;
      if ( !subBlock->pack()) {
        msg(MSG::ERROR) << "CMX-Jet sub-block packing failed" << endmsg;
	return StatusCode::FAILURE;
      }
      if (debug) {
	msg() << "CMX-Jet sub-block data words: "
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

const std::vector<uint32_t>& JepByteStreamV2Tool::sourceIDs(
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

StatusCode JepByteStreamV2Tool::convertBs(
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
    if (debug) {
      msg() << "Minor format version number: " << MSG::hex
            << minorVersion << MSG::dec << endmsg
            << "JEM triggered slice offset: " << trigJem << endmsg;
    }

    // Loop over sub-blocks

    m_rodErr = L1CaloSubBlock::ERROR_NONE;
    while (payload != payloadEnd) {
      
      if (L1CaloSubBlock::wordType(*payload) != L1CaloSubBlock::HEADER) {
        if (debug) msg() << "Unexpected data sequence" << endmsg;
	m_rodErr = L1CaloSubBlock::ERROR_MISSING_HEADER;
	break;
      }
      if (CmxSubBlock::cmxBlock(*payload)) {
        // CMXs
	if (CmxSubBlock::cmxType(*payload) == CmxSubBlock::CMX_JET) {
	  m_cmxJetSubBlock->clear();
          payload = m_cmxJetSubBlock->read(payload, payloadEnd);
	  if (m_cmxJetSubBlock->crate() != rodCrate) {
	    if (debug) msg() << "Inconsistent crate number in ROD source ID"
	                     << endmsg;
	    m_rodErr = L1CaloSubBlock::ERROR_CRATE_NUMBER;
	    break;
          }
	  if (data.m_collection == CMX_HITS || data.m_collection == CMX_TOBS) {
	    decodeCmxJet(m_cmxJetSubBlock, trigJem, data);
	    if (m_rodErr != L1CaloSubBlock::ERROR_NONE) {
	      if (debug) msg() << "decodeCmxJet failed" << endmsg;
	      break;
	    }
          }
        } else if (CmxSubBlock::cmxType(*payload) == CmxSubBlock::CMX_ENERGY) {
	  m_cmxEnergySubBlock->clear();
	  payload = m_cmxEnergySubBlock->read(payload, payloadEnd);
	  if (m_cmxEnergySubBlock->crate() != rodCrate) {
	    if (debug) msg() << "Inconsistent crate number in ROD source ID"
	                     << endmsg;
	    m_rodErr = L1CaloSubBlock::ERROR_CRATE_NUMBER;
	    break;
          }
	  if (data.m_collection == CMX_SUMS) {
	    decodeCmxEnergy(m_cmxEnergySubBlock, trigJem, static_cast<CmxSumsData&>(data));
	    if (m_rodErr != L1CaloSubBlock::ERROR_NONE) {
	      if (debug) msg() << "decodeCmxEnergy failed" << endmsg;
	      break;
	    }
          }
	} else {
	  if (debug) msg() << "Invalid CMX type in module field" << endmsg;
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
	if (data.m_collection == JET_ELEMENTS || data.m_collection == ENERGY_SUMS) {
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

// Unpack CMX-Energy sub-block

void JepByteStreamV2Tool::decodeCmxEnergy(CmxEnergySubBlock* subBlock,
                                          int trigJem,
                                          CmxSumsData& data)
{
  const bool debug = msgLvl(MSG::DEBUG);
  if (debug) msg(MSG::DEBUG);

  const int hwCrate    = subBlock->crate();
  const int module     = subBlock->cmxPosition();
  const int firmware   = subBlock->cmxFirmware();
  const int summing    = subBlock->cmxSumming();
  const int timeslices = subBlock->timeslices();
  const int sliceNum   = subBlock->slice();
  if (debug) {
    msg() << "CMX-Energy: Crate " << hwCrate
          << "  Module "          << module
	  << "  Firmware "        << firmware
	  << "  Summing "         << summing
          << "  Total slices "    << timeslices
          << "  Slice "           << sliceNum    << endmsg;
  }
  if (timeslices <= trigJem) {
    if (debug) msg() << "Triggered CMX slice from header "
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
      msg() << "CMX-Energy sub-block unpacking failed: " << errMsg << endmsg;
    }
    m_rodErr = subBlock->unpackErrorCode();
    return;
  }

  // Retrieve required data

  const bool neutralFormat = subBlock->format() == L1CaloSubBlock::NEUTRAL;
  const int crate     = hwCrate - m_crateOffsetHw;
  const int swCrate   = crate   + m_crateOffsetSw;
  const int maxSource = static_cast<int>(LVL1::CMXEtSums::MAX_SOURCE);
  std::vector<unsigned int>& exVec(m_uintVec0);
  std::vector<unsigned int>& eyVec(m_uintVec1);
  std::vector<unsigned int>& etVec(m_uintVec2);
  std::vector<int>& exErrVec(m_intVec0);
  std::vector<int>& eyErrVec(m_intVec1);
  std::vector<int>& etErrVec(m_intVec2);
  LVL1::DataError derr;
  derr.set(LVL1::DataError::SubStatusWord, subBlock->subStatus());
  const int ssError = derr.error();
  const int sliceBeg = ( neutralFormat ) ? 0          : sliceNum;
  const int sliceEnd = ( neutralFormat ) ? timeslices : sliceNum + 1;
  for (int slice = sliceBeg; slice < sliceEnd; ++slice) {

    // Energy sums

    for (int source = 0; source < maxSource; ++source) {
      if (source >= m_modules && summing == CmxSubBlock::CRATE && 
	  source != LVL1::CMXEtSums::LOCAL_STANDARD &&
	  source != LVL1::CMXEtSums::LOCAL_RESTRICTED) continue;
      unsigned int ex = 0;
      unsigned int ey = 0;
      unsigned int et = 0;
      int exErr = 0;
      int eyErr = 0;
      int etErr = 0;
      LVL1::DataError exErrBits(ssError);
      LVL1::DataError eyErrBits(ssError);
      LVL1::DataError etErrBits(ssError);
      if (source < m_modules) {
        ex = subBlock->energy(slice, source, CmxEnergySubBlock::ENERGY_EX);
        ey = subBlock->energy(slice, source, CmxEnergySubBlock::ENERGY_EY);
        et = subBlock->energy(slice, source, CmxEnergySubBlock::ENERGY_ET);
        exErr = subBlock->error(slice, source, CmxEnergySubBlock::ENERGY_EX);
        eyErr = subBlock->error(slice, source, CmxEnergySubBlock::ENERGY_EY);
        etErr = subBlock->error(slice, source, CmxEnergySubBlock::ENERGY_ET);
	exErrBits.set(LVL1::DataError::Parity, exErr >> 1);
	eyErrBits.set(LVL1::DataError::Parity, eyErr >> 1);
	etErrBits.set(LVL1::DataError::Parity, etErr >> 1);
        exErr = exErrBits.error();
        eyErr = eyErrBits.error();
        etErr = etErrBits.error();
      } else {
	CmxEnergySubBlock::SourceType srcType = CmxEnergySubBlock::MAX_SOURCE_TYPE;
	CmxEnergySubBlock::SumType    sumType = CmxEnergySubBlock::MAX_SUM_TYPE;
	CmxEnergySubBlock::HitsType   hitType = CmxEnergySubBlock::MAX_HITS_TYPE;
	energySubBlockTypes(source, srcType, sumType, hitType);
	if (srcType != CmxEnergySubBlock::MAX_SOURCE_TYPE) {
	  ex = subBlock->energy(slice, srcType, sumType, CmxEnergySubBlock::ENERGY_EX);
	  ey = subBlock->energy(slice, srcType, sumType, CmxEnergySubBlock::ENERGY_EY);
	  et = subBlock->energy(slice, srcType, sumType, CmxEnergySubBlock::ENERGY_ET);
	  exErr = subBlock->error(slice, srcType, sumType, CmxEnergySubBlock::ENERGY_EX);
	  eyErr = subBlock->error(slice, srcType, sumType, CmxEnergySubBlock::ENERGY_EY);
	  etErr = subBlock->error(slice, srcType, sumType, CmxEnergySubBlock::ENERGY_ET);
	  exErrBits.set(LVL1::DataError::Overflow, exErr);
	  eyErrBits.set(LVL1::DataError::Overflow, eyErr);
	  etErrBits.set(LVL1::DataError::Overflow, etErr);
	  if (srcType == CmxEnergySubBlock::REMOTE) {
	    exErrBits.set(LVL1::DataError::Parity, exErr >> 1);
	    eyErrBits.set(LVL1::DataError::Parity, eyErr >> 1);
	    etErrBits.set(LVL1::DataError::Parity, etErr >> 1);
          }
        } else if (hitType != CmxEnergySubBlock::MAX_HITS_TYPE) {
	  ex = subBlock->hits(slice, hitType, sumType);
	  ey = ex;
	  et = ex;
        }
      }
      exErr = exErrBits.error();
      eyErr = eyErrBits.error();
      etErr = etErrBits.error();
      if (ex || ey || et || exErr || eyErr || etErr) {
        LVL1::CMXEtSums* sums = findCmxSums(data, crate, source);
	if ( ! sums ) {   // create new CMX energy sums
	  exVec.assign(timeslices, 0);
	  eyVec.assign(timeslices, 0);
	  etVec.assign(timeslices, 0);
	  exErrVec.assign(timeslices, 0);
	  eyErrVec.assign(timeslices, 0);
	  etErrVec.assign(timeslices, 0);
	  exVec[slice] = ex;
	  eyVec[slice] = ey;
	  etVec[slice] = et;
	  exErrVec[slice] = exErr;
	  eyErrVec[slice] = eyErr;
	  etErrVec[slice] = etErr;
	  auto sumsp =
            std::make_unique<LVL1::CMXEtSums>(swCrate, source, etVec, exVec, eyVec,
                                              etErrVec, exErrVec, eyErrVec, trigJem);
          const int key = crate*100 + source;
	  data.m_cmxEtMap.insert(std::make_pair(key, sumsp.get()));
	  data.m_cmxEtCollection->push_back(std::move(sumsp));
        } else {
	  exVec = sums->ExVec();
	  eyVec = sums->EyVec();
	  etVec = sums->EtVec();
	  exErrVec = sums->ExErrorVec();
	  eyErrVec = sums->EyErrorVec();
	  etErrVec = sums->EtErrorVec();
	  const int nsl = exVec.size();
	  if (timeslices != nsl) {
	    if (debug) msg() << "Inconsistent number of slices in sub-blocks"
	                     << endmsg;
            m_rodErr = L1CaloSubBlock::ERROR_SLICES;
	    return;
          }
	  if (exVec[slice] != 0 || eyVec[slice] != 0 || etVec[slice] != 0 ||
	      exErrVec[slice] != 0 || eyErrVec[slice] != 0 ||
              etErrVec[slice] != 0) {
            if (debug) msg() << "Duplicate data for slice " << slice << endmsg;
	    m_rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
	    return;
          }
	  exVec[slice] = ex;
	  eyVec[slice] = ey;
	  etVec[slice] = et;
	  exErrVec[slice] = exErr;
	  eyErrVec[slice] = eyErr;
	  etErrVec[slice] = etErr;
	  sums->addEx(exVec, exErrVec);
	  sums->addEy(eyVec, eyErrVec);
	  sums->addEt(etVec, etErrVec);
        }
      }
    }
  }
  
  return;
}

// Unpack CMX-Jet sub-block

void JepByteStreamV2Tool::decodeCmxJet(CmxJetSubBlock* subBlock, int trigJem,
                                       JepByteStreamToolData& data)
{
  const bool debug = msgLvl(MSG::DEBUG);
  if (debug) msg(MSG::DEBUG);

  const int hwCrate    = subBlock->crate();
  const int module     = subBlock->cmxPosition();
  const int firmware   = subBlock->cmxFirmware();
  const int summing    = subBlock->cmxSumming();
  const int timeslices = subBlock->timeslices();
  const int sliceNum   = subBlock->slice();
  if (debug) {
    msg() << "CMX-Jet: Crate " << hwCrate
          << "  Module "       << module
	  << "  Firmware "     << firmware
	  << "  Summing "      << summing
          << "  Total slices " << timeslices
          << "  Slice "        << sliceNum    << endmsg;
  }
  if (timeslices <= trigJem) {
    if (debug) msg() << "Triggered CMX slice from header "
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
      msg() << "CMX-Jet sub-block unpacking failed: " << errMsg << endmsg;
    }
    m_rodErr = subBlock->unpackErrorCode();
    return;
  }

  // Retrieve required data

  const bool neutralFormat = subBlock->format() == L1CaloSubBlock::NEUTRAL;
  const int crate     = hwCrate - m_crateOffsetHw;
  const int swCrate   = crate   + m_crateOffsetSw;
  const int maxSource = static_cast<int>(LVL1::CMXJetHits::MAX_SOURCE);
  std::vector<int>& energyLgVec(m_intVec0);
  std::vector<int>& energySmVec(m_intVec1);
  std::vector<int>& errorVec(m_intVec2);
  std::vector<unsigned int>& presenceMapVec(m_uintVec0);
  std::vector<unsigned int>& hit0Vec(m_uintVec0);
  std::vector<unsigned int>& hit1Vec(m_uintVec1);
  std::vector<int>& err0Vec(m_intVec0);
  std::vector<int>& err1Vec(m_intVec1);
  LVL1::DataError derr;
  derr.set(LVL1::DataError::SubStatusWord, subBlock->subStatus());
  const int ssError = derr.error();
  const int sliceBeg = ( neutralFormat ) ? 0          : sliceNum;
  const int sliceEnd = ( neutralFormat ) ? timeslices : sliceNum + 1;
  for (int slice = sliceBeg; slice < sliceEnd; ++slice) {

    // Jet TOBs

    if (data.m_collection == CMX_TOBS) {
      CmxTobData& tdata = static_cast<CmxTobData&> (data);

      for (int jem = 0; jem < m_modules; ++jem) {
        const unsigned int presenceMap = subBlock->presenceMap(slice, jem);
        for (int tob = 0; tob < m_maxTobs; ++tob) {
	  const int energyLarge = subBlock->energyLarge(slice, jem, tob);
	  const int energySmall = subBlock->energySmall(slice, jem, tob);
	  int error = subBlock->tobError(slice, jem, tob);
	  if (energyLarge == 0 && energySmall == 0 && error == 0) break;
	  const int loc = subBlock->localCoord(slice, jem, tob);
	  const int frame = subBlock->frame(slice, jem, tob);
	  LVL1::DataError errBits(ssError);
	  if (error) {
	    errBits.set(LVL1::DataError::Parity, error);
            if (neutralFormat) {
	      const int parity = subBlock->parityBits(slice, jem);
	      errBits.set(LVL1::DataError::ParityPhase0, parity);
	      errBits.set(LVL1::DataError::ParityPhase1, (parity>>1));
	      errBits.set(LVL1::DataError::ParityPhase2, (parity>>2));
	      errBits.set(LVL1::DataError::ParityPhase3, (parity>>3));
	    }
          }
	  error = errBits.error();
	  const int key = tobKey(crate, jem, frame, loc);
	  LVL1::CMXJetTob* tb = findCmxTob(tdata, key);
	  if ( ! tb ) { // create new CMX TOB
	    energyLgVec.assign(timeslices, 0);
	    energySmVec.assign(timeslices, 0);
	    errorVec.assign(timeslices, 0);
	    presenceMapVec.assign(timeslices, 0);
	    energyLgVec[slice] = energyLarge;
	    energySmVec[slice] = energySmall;
	    errorVec[slice]    = error;
	    presenceMapVec[slice] = presenceMap;
	    auto tbp =
              std::make_unique<LVL1::CMXJetTob>(swCrate, jem, frame, loc,
                                                energyLgVec, energySmVec, errorVec,
                                                presenceMapVec, trigJem);
	    tdata.m_cmxTobMap.insert(std::make_pair(key, tbp.get()));
	    tdata.m_cmxTobCollection->push_back(std::move(tbp));
          } else {
	    energyLgVec = tb->energyLgVec();
	    energySmVec = tb->energySmVec();
	    errorVec    = tb->errorVec();
	    presenceMapVec = tb->presenceMapVec();
	    const int nsl = energyLgVec.size();
	    if (timeslices != nsl) {
	      if (debug) msg() << "Inconsistent number of slices in sub-blocks"
	                       << endmsg;
              m_rodErr = L1CaloSubBlock::ERROR_SLICES;
	      return;
            }
	    if (energyLgVec[slice] != 0 || energySmVec[slice] != 0 ||
	        errorVec[slice]  != 0 || presenceMapVec[slice] != 0) {
              if (debug) msg() << "Duplicate data for slice " << slice << endmsg;
	      m_rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
	      return;
            }
	    energyLgVec[slice] = energyLarge;
	    energySmVec[slice] = energySmall;
	    errorVec[slice]    = error;
	    presenceMapVec[slice] = presenceMap;
	    tb->addTob(energyLgVec, energySmVec, errorVec, presenceMapVec);
          }
        }
      }
    }

    // Jet hit counts and topo info

    else if (data.m_collection == CMX_HITS) {
      CmxHitsData& hdata = static_cast<CmxHitsData&> (data);

      for (int source = 0; source < maxSource; ++source) {
        if (summing == CmxSubBlock::CRATE && 
            (source == LVL1::CMXJetHits::REMOTE_MAIN    ||
  	     source == LVL1::CMXJetHits::TOTAL_MAIN     ||
	     source == LVL1::CMXJetHits::REMOTE_FORWARD ||
	     source == LVL1::CMXJetHits::TOTAL_FORWARD)) continue;
        int sourceId = jetSubBlockSourceId(source);
        if (sourceId == CmxJetSubBlock::MAX_SOURCE_ID) continue;
	const unsigned int hit0 = subBlock->hits(slice, sourceId, 0);
	const unsigned int hit1 = subBlock->hits(slice, sourceId, 1);
	int err0 = subBlock->hitsError(slice, sourceId, 0);
	int err1 = subBlock->hitsError(slice, sourceId, 1);
	LVL1::DataError err0Bits(ssError);
	LVL1::DataError err1Bits(ssError);
	err0Bits.set(LVL1::DataError::Parity, err0);
	err1Bits.set(LVL1::DataError::Parity, err1>>1);
	err0Bits.set(LVL1::DataError::Overflow, err0>>2);
	err1Bits.set(LVL1::DataError::Overflow, err1>>2);
	err0 = err0Bits.error();
	err1 = err1Bits.error();
	if (hit0 || hit1 || err0 || err1) {
          LVL1::CMXJetHits* jh = findCmxHits(hdata, crate, source);
	  if ( ! jh ) {   // create new CMX hits
	    hit0Vec.assign(timeslices, 0);
	    hit1Vec.assign(timeslices, 0);
	    err0Vec.assign(timeslices, 0);
	    err1Vec.assign(timeslices, 0);
	    hit0Vec[slice] = hit0;
	    hit1Vec[slice] = hit1;
	    err0Vec[slice] = err0;
	    err1Vec[slice] = err1;
	    auto jhp =
              std::make_unique<LVL1::CMXJetHits>(swCrate, source, hit0Vec, hit1Vec,
                                                 err0Vec, err1Vec, trigJem);
            const int key = crate*100 + source;
	    hdata.m_cmxHitsMap.insert(std::make_pair(key, jhp.get()));
	    hdata.m_cmxHitCollection->push_back(std::move(jhp));
          } else {
	    hit0Vec = jh->hitsVec0();
	    hit1Vec = jh->hitsVec1();
	    err0Vec = jh->errorVec0();
	    err1Vec = jh->errorVec1();
	    const int nsl = hit0Vec.size();
	    if (timeslices != nsl) {
	      if (debug) msg() << "Inconsistent number of slices in sub-blocks"
	                       << endmsg;
              m_rodErr = L1CaloSubBlock::ERROR_SLICES;
	      return;
            }
	    if (hit0Vec[slice] != 0 || hit1Vec[slice] != 0 ||
	        err0Vec[slice] != 0 || err1Vec[slice] != 0) {
	      if (debug) msg() << "Duplicate data for slice " << slice << endmsg;
	      m_rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
	      return;
            }
	    hit0Vec[slice] = hit0;
	    hit1Vec[slice] = hit1;
	    err0Vec[slice] = err0;
	    err1Vec[slice] = err1;
	    jh->addHits(hit0Vec, hit1Vec, err0Vec, err1Vec);
          }
        }
      }
    }
  }
  
  return;
}

// Unpack JEM sub-block

void JepByteStreamV2Tool::decodeJem(JemSubBlockV2* subBlock, int trigJem,
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
  std::vector<unsigned int>& exVec(m_uintVec0);
  std::vector<unsigned int>& eyVec(m_uintVec1);
  std::vector<unsigned int>& etVec(m_uintVec2);
  LVL1::DataError derr;
  derr.set(LVL1::DataError::SubStatusWord, subBlock->subStatus());
  const int ssError = derr.error();
  std::vector<int> dummy(timeslices);
  const int sliceBeg = ( neutralFormat ) ? 0          : sliceNum;
  const int sliceEnd = ( neutralFormat ) ? timeslices : sliceNum + 1;
  for (int slice = sliceBeg; slice < sliceEnd; ++slice) {

    if (data.m_collection == JET_ELEMENTS) {
      JetElementData& jedata = static_cast<JetElementData&> (data);

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
    } else if (data.m_collection == ENERGY_SUMS) {
      EnergySumsData& sumdata = static_cast<EnergySumsData&> (data);

      // Get energy subsums

      const unsigned int ex = subBlock->ex(slice);
      const unsigned int ey = subBlock->ey(slice);
      const unsigned int et = subBlock->et(slice);
      if (ex | ey | et) {
	LVL1::JEMEtSums* sums = findEnergySums(sumdata, crate, module);
	if ( ! sums ) {   // create new energy sums
	  exVec.assign(timeslices, 0);
	  eyVec.assign(timeslices, 0);
	  etVec.assign(timeslices, 0);
	  exVec[slice] = ex;
	  eyVec[slice] = ey;
	  etVec[slice] = et;
	  auto sumsp =
            std::make_unique<LVL1::JEMEtSums>(swCrate, module, etVec, exVec, eyVec,
                                              trigJem);
          sumdata.m_etMap.insert(std::make_pair(crate*m_modules+module, sumsp.get()));
	  sumdata.m_etCollection->push_back(std::move(sumsp));
        } else {
	  exVec = sums->ExVec();
	  eyVec = sums->EyVec();
	  etVec = sums->EtVec();
	  const int nsl = exVec.size();
	  if (timeslices != nsl) {
	    if (debug) {
	      msg() << "Inconsistent number of slices in sub-blocks"
	            << endmsg;
	    }
            m_rodErr = L1CaloSubBlock::ERROR_SLICES;
	    return;
          }
	  if (exVec[slice] != 0 || eyVec[slice] != 0 || etVec[slice] != 0) {
	    if (debug) msg() << "Duplicate data for slice "
	                     << slice << endmsg;
            m_rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
	    return;
          }
	  exVec[slice] = ex;
	  eyVec[slice] = ey;
	  etVec[slice] = et;
	  sums->addEx(exVec);
	  sums->addEy(eyVec);
	  sums->addEt(etVec);
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

// Find TOB map key for given crate, jem, frame, loc

int JepByteStreamV2Tool::tobKey(const int crate, const int jem,
                                const int frame, const int loc)
{
  return ((((((crate<<4)+jem)<<3)+frame)<<2)+loc);
}

// Find a jet element given eta, phi

const
LVL1::JetElement* JepByteStreamV2Tool::findJetElement(const double eta,
                                                      const double phi) const
{
  const unsigned int key = m_elementKey->jeKey(phi, eta);
  ConstJetElementMap::const_iterator mapIter = m_jeMap.find(key);
  if (mapIter != m_jeMap.end()) return mapIter->second;
  return nullptr;
}

LVL1::JetElement* JepByteStreamV2Tool::findJetElement(const JetElementData& data,
                                                      const double eta,
                                                      const double phi) const
{
  const unsigned int key = m_elementKey->jeKey(phi, eta);
  JetElementMap::const_iterator mapIter = data.m_jeMap.find(key);
  if (mapIter != data.m_jeMap.end()) return mapIter->second;
  return nullptr;
}

// Find energy sums for given crate, module

const
LVL1::JEMEtSums* JepByteStreamV2Tool::findEnergySums(const int crate,
                                                     const int module) const
{
  ConstEnergySumsMap::const_iterator mapIter = m_etMap.find(crate*m_modules + module);
  if (mapIter != m_etMap.end()) return mapIter->second;
  return nullptr;
}

LVL1::JEMEtSums* JepByteStreamV2Tool::findEnergySums(const EnergySumsData& data,
                                                     const int crate,
                                                     const int module) const
{
  EnergySumsMap::const_iterator mapIter = data.m_etMap.find(crate*m_modules + module);
  if (mapIter != data.m_etMap.end()) return mapIter->second;
  return nullptr;
}

// Find CMX TOB for given crate, jem, frame, loc

const
LVL1::CMXJetTob* JepByteStreamV2Tool::findCmxTob(const int key) const
{
  ConstCmxTobMap::const_iterator mapIter = m_cmxTobMap.find(key);
  if (mapIter != m_cmxTobMap.end()) return mapIter->second;
  return nullptr;
}

LVL1::CMXJetTob* JepByteStreamV2Tool::findCmxTob(const CmxTobData& data,
                                                 const int key) const
{
  CmxTobMap::const_iterator mapIter = data.m_cmxTobMap.find(key);
  if (mapIter != data.m_cmxTobMap.end()) return mapIter->second;
  return nullptr;
}

// Find CMX hits for given crate, source

const
LVL1::CMXJetHits* JepByteStreamV2Tool::findCmxHits(const int crate,
                                                   const int source) const
{
  ConstCmxHitsMap::const_iterator mapIter = m_cmxHitsMap.find(crate*100 + source);
  if (mapIter != m_cmxHitsMap.end()) return mapIter->second;
  return nullptr;
}

LVL1::CMXJetHits* JepByteStreamV2Tool::findCmxHits(const CmxHitsData& data,
                                                   const int crate,
                                                   const int source) const
{
  CmxHitsMap::const_iterator mapIter = data.m_cmxHitsMap.find(crate*100 + source);
  if (mapIter != data.m_cmxHitsMap.end()) return mapIter->second;
  return nullptr;
}

// Find CMX energy sums for given crate, module, source

const
LVL1::CMXEtSums* JepByteStreamV2Tool::findCmxSums(const int crate,
                                                  const int source) const
{
  ConstCmxSumsMap::const_iterator mapIter = m_cmxEtMap.find(crate*100 + source);
  if (mapIter != m_cmxEtMap.end()) return mapIter->second;
  return nullptr;
}

LVL1::CMXEtSums* JepByteStreamV2Tool::findCmxSums(const CmxSumsData& data,
                                                  const int crate,
                                                  const int source) const
{
  CmxSumsMap::const_iterator mapIter = data.m_cmxEtMap.find(crate*100 + source);
  if (mapIter != data.m_cmxEtMap.end()) return mapIter->second;
  return nullptr;
}

// Set up jet element map

void JepByteStreamV2Tool::setupJeMap(const JetElementCollection*
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

// Set up energy sums map

void JepByteStreamV2Tool::setupEtMap(const EnergySumsCollection*
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

// Set up CMX TOB map

void JepByteStreamV2Tool::setupCmxTobMap(const CmxTobCollection*
                                                         const tobCollection)
{
  m_cmxTobMap.clear();
  if (tobCollection) {
    CmxTobCollection::const_iterator pos  = tobCollection->begin();
    CmxTobCollection::const_iterator pose = tobCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CMXJetTob* const tob = *pos;
      const int crate = tob->crate() - m_crateOffsetSw;
      const int jem   = tob->jem();
      const int frame = tob->frame();
      const int loc   = tob->location();
      const int key   = tobKey(crate, jem, frame, loc);
      m_cmxTobMap.insert(std::make_pair(key, tob));
    }
  }
}

// Set up CMX hits map

void JepByteStreamV2Tool::setupCmxHitsMap(const CmxHitsCollection*
                                                         const hitCollection)
{
  m_cmxHitsMap.clear();
  if (hitCollection) {
    CmxHitsCollection::const_iterator pos  = hitCollection->begin();
    CmxHitsCollection::const_iterator pose = hitCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CMXJetHits* const hits = *pos;
      const int crate = hits->crate() - m_crateOffsetSw;
      const int key   = crate*100 + hits->source();
      m_cmxHitsMap.insert(std::make_pair(key, hits));
    }
  }
}

// Set up CMX energy sums map

void JepByteStreamV2Tool::setupCmxEtMap(const CmxSumsCollection*
                                                         const etCollection)
{
  m_cmxEtMap.clear();
  if (etCollection) {
    CmxSumsCollection::const_iterator pos  = etCollection->begin();
    CmxSumsCollection::const_iterator pose = etCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CMXEtSums* const sums = *pos;
      const int crate = sums->crate() - m_crateOffsetSw;
      const int key   = crate*100 + sums->source();
      m_cmxEtMap.insert(std::make_pair(key, sums));
    }
  }
}

// Get number of slices and triggered slice offset for next slink

bool JepByteStreamV2Tool::slinkSlices(const int crate, const int module,
                  const int modulesPerSlink, int& timeslices, int& trigJem)
{
  int slices = -1;
  int trigJ  = m_dfltSlices/2;
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
  // CMX last slink of crate
  if (module/modulesPerSlink == m_slinks - 1) {
    for (int jem = module; jem < module + modulesPerSlink; ++jem) {
      for (int frame = 0; frame < m_frames; ++frame) {
        for (int loc = 0; loc < m_locations; ++loc) {
	  const int key = tobKey(crate, jem, frame, loc);
	  const LVL1::CMXJetTob* tob = findCmxTob(key);
	  if (tob) {
	    const int numdat = 4;
            std::vector<int> sums(numdat);
            std::vector<int> sizes(numdat);
	    sums[0] = std::accumulate((tob->energyLgVec()).begin(),
	                              (tob->energyLgVec()).end(), 0);
	    sums[1] = std::accumulate((tob->energySmVec()).begin(),
	                              (tob->energySmVec()).end(), 0);
	    sums[2] = std::accumulate((tob->errorVec()).begin(),
	                              (tob->errorVec()).end(), 0);
	    sums[3] = std::accumulate((tob->presenceMapVec()).begin(),
	                              (tob->presenceMapVec()).end(), 0);
            sizes[0] = (tob->energyLgVec()).size();
            sizes[1] = (tob->energySmVec()).size();
            sizes[2] = (tob->errorVec()).size();
            sizes[3] = (tob->presenceMapVec()).size();
	    const int peak = tob->peak();
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
    }
    const int maxDataID1 = LVL1::CMXJetHits::MAX_SOURCE;
    const int maxDataID2 = LVL1::CMXEtSums::MAX_SOURCE;
    const int maxDataID  = (maxDataID1 > maxDataID2) ? maxDataID1 : maxDataID2;
    for (int source = 0; source < maxDataID; ++source) {
      const int numdat = 6;
      std::vector<unsigned int> sums(numdat);
      std::vector<int> sizes(numdat);
      const LVL1::CMXJetHits* hits = 0;
      if (source < maxDataID1) hits = findCmxHits(crate, source);
      if (hits) {
        sums[0] = std::accumulate((hits->hitsVec0()).begin(),
                                             (hits->hitsVec0()).end(), 0);
        sums[1] = std::accumulate((hits->hitsVec1()).begin(),
                                             (hits->hitsVec1()).end(), 0);
        sums[2] = std::accumulate((hits->errorVec0()).begin(),
                                             (hits->errorVec0()).end(), 0);
        sums[3] = std::accumulate((hits->errorVec1()).begin(),
                                             (hits->errorVec1()).end(), 0);
        sizes[0] = (hits->hitsVec0()).size();
        sizes[1] = (hits->hitsVec1()).size();
        sizes[2] = (hits->errorVec0()).size();
        sizes[3] = (hits->errorVec1()).size();
        const int peak = hits->peak();
        for (int i = 0; i < 4; ++i) {
          if (sums[i] == 0) continue;
          if (slices < 0) {
	    slices = sizes[i];
	    trigJ  = peak;
          } else if (slices != sizes[i] || trigJ != peak) return false;
        }
      }
      const LVL1::CMXEtSums* et = 0;
      if (source < maxDataID2) et = findCmxSums(crate, source);
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

// Get energy subBlock types from CMXEtSums source type

void JepByteStreamV2Tool::energySubBlockTypes(const int source,
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

// Get jet hits subBlock source ID from CMXJetHits source type

int JepByteStreamV2Tool::jetSubBlockSourceId(const int source)
{
  int sourceId = CmxJetSubBlock::MAX_SOURCE_ID;
  switch (source) {
    case LVL1::CMXJetHits::REMOTE_MAIN:
      sourceId = CmxJetSubBlock::REMOTE_MAIN;
      break;
    case LVL1::CMXJetHits::LOCAL_MAIN:
      sourceId = CmxJetSubBlock::LOCAL_MAIN;
      break;
    case LVL1::CMXJetHits::TOTAL_MAIN:
      sourceId = CmxJetSubBlock::TOTAL_MAIN;
      break;
    case LVL1::CMXJetHits::REMOTE_FORWARD:
      sourceId = CmxJetSubBlock::REMOTE_FORWARD;
      break;
    case LVL1::CMXJetHits::LOCAL_FORWARD:
      sourceId = CmxJetSubBlock::LOCAL_FORWARD;
      break;
    case LVL1::CMXJetHits::TOTAL_FORWARD:
      sourceId = CmxJetSubBlock::TOTAL_FORWARD;
      break;
    case LVL1::CMXJetHits::TOPO_CHECKSUM:
      sourceId = CmxJetSubBlock::TOPO_CHECKSUM;
      break;
    case LVL1::CMXJetHits::TOPO_OCCUPANCY_MAP:
      sourceId = CmxJetSubBlock::TOPO_OCCUPANCY_MAP;
      break;
    case LVL1::CMXJetHits::TOPO_OCCUPANCY_COUNTS:
      sourceId = CmxJetSubBlock::TOPO_OCCUPANCY_COUNTS;
      break;
    default:
      break;
  }
  return sourceId;
}

} // end namespace
