/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ===========================================================================
// Includes
// ===========================================================================
// STD:
// ===========================================================================
#include <numeric>
#include <set>
#include <utility>
// ===========================================================================
// Athena
// ===========================================================================
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/SegMemSvc.h"
// ===========================================================================
// TrigT1
// ===========================================================================
#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/TriggerTowerAuxContainer.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

#include "TrigT1CaloUtils/DataError.h"

#include "L1CaloSrcIdMap.h"
#include "TrigT1CaloMappingToolInterfaces/IL1CaloMappingTool.h"
#include "L1CaloErrorByteStreamTool.h"
#include "PpmSubBlockV2.h"
#include "CmmSubBlock.h"
#include "L1CaloUserHeader.h"
// ===========================================================================
#include "PpmByteStreamV2Tool.h"
// ===========================================================================
namespace LVL1BS {
// ===========================================================================

// Interface ID
static const InterfaceID IID_IPpmByteStreamV2Tool("PpmByteStreamV2Tool", 1, 1);

const InterfaceID& PpmByteStreamV2Tool::interfaceID() {
  return IID_IPpmByteStreamV2Tool;
}
// ===========================================================================
// Constructor
PpmByteStreamV2Tool::PpmByteStreamV2Tool(const std::string& type,
    const std::string& name, const IInterface* parent) :
    AthAlgTool(type, name, parent),
        m_sms("SegMemSvc/SegMemSvc", name),
        m_srcIdMap(0),
        m_ppmMaps("LVL1::PpmMappingTool/PpmMappingTool"),
        m_errorTool("LVL1BS::L1CaloErrorByteStreamTool/L1CaloErrorByteStreamTool"),
        m_subDetector(eformat::TDAQ_CALO_PREPROC),
        m_channelsType(ChannelsType::Data),
		m_fea(0){
  
  declareInterface<PpmByteStreamV2Tool>(this);

  declareProperty("PpmMappingTool", m_ppmMaps,
                  "Crate/Module/Channel to Eta/Phi/Layer mapping tool");

  declareProperty("NCrates", m_crates = 8, "Number of crates");
  declareProperty("NModules", m_modules = 16, "Number of modules");
  declareProperty("NChannels", m_channels = 64, "Number of channels");
  declareProperty("DataSize", m_dataSize = 3584, "Data size");
  declareProperty("MaxSlinks", m_maxSlinks = 4, "Max slinks");

  declareProperty("PrintCompStats",     m_printCompStats  = 0,
                  "Print compressed format statistics");
  declareProperty("FADCBaseline", m_fadcBaseline = 0,
                  "FADC baseline lower bound for compressed formats");

  // Properties for writing bytestream only
  declareProperty("DataFormat", m_dataFormat = 1,
                    "Format identifier (0-3) in sub-block header");
  declareProperty("SubHeaderVersion", m_subheaderVersion = 1,
                      "Version identifier (1-2) in sub-block header");

}
// ===========================================================================
// Destructor
PpmByteStreamV2Tool::~PpmByteStreamV2Tool() {
}
// ===========================================================================
// Initialize

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode PpmByteStreamV2Tool::initialize() {
  ATH_MSG_INFO("Initializing " << name() << " - package version "
      << PACKAGE_VERSION);

  StatusCode sc = m_sms.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve service " << m_sms);
    return sc;
  } else {
    ATH_MSG_INFO("Retrieved service " << m_sms);
  }

  m_srcIdMap = new L1CaloSrcIdMap { };
  m_fea = new FullEventAssembler<L1CaloSrcIdMap> { };

  return StatusCode::SUCCESS;
}
// ===========================================================================
// Finalize

StatusCode PpmByteStreamV2Tool::finalize() {
  delete m_srcIdMap;
  return StatusCode::SUCCESS;
}
// ===========================================================================
void PpmByteStreamV2Tool::reserveMemory() {
  const int maxChannels = m_crates * m_modules * m_channels;
  const int chanBitVecSize = maxChannels/32;
  const int modBitVecSize = (m_crates * m_modules)/32;
  
  // TriggerTowerMap ttMap;
  // TriggerTowerMap::iterator itt;

  int dataCount = 0;

  if (m_ttData.empty()) {
     m_ttData.reserve(m_dataSize);
     
     m_dataChan.resize(chanBitVecSize);
     m_chanLayer.resize(chanBitVecSize);
     m_dataMod.resize(modBitVecSize);
     m_ttPos.resize(maxChannels);

     for (int crate = 0; crate < m_crates; ++crate) {
        for (int module = 0; module < m_modules; ++module) {
          const int index2 = (crate << 4) + module;
          const int word2  = index2 / 32;
          const int bit2   = index2 % 32;
          for (int channel = 0; channel < m_channels; ++channel) {
            const int index = (crate << 10) + (module << 6) + channel;
            const int word  = index / 32;
            const int bit   = index % 32;
            double eta = 0.;
            double phi = 0.;
            int layer = 0;
            // unsigned int key = 0;
            if (m_ppmMaps->mapping(crate, module, channel, eta, phi, layer)) {
              xAOD::TriggerTowerAuxContainer* aux = new xAOD::TriggerTowerAuxContainer();
              xAOD::TriggerTower* tt =  new (m_sms->allocate<xAOD::TriggerTower>(SegMemSvc::JOB))xAOD::TriggerTower();
              tt->setStore(aux);


              // tt->initialize(
              //         const uint_least32_t& coolId,
              //         const uint_least8_t& layer,
              //         const float& eta,
              //         const float& phi,
              //         const std::vector<uint_least8_t>& lut_cp,
              //         const std::vector<uint_least8_t>& lut_jep,
              //         const std::vector<int_least16_t>& correction,
              //         const std::vector<uint_least8_t>& correctionEnabled,
              //         const std::vector<uint_least8_t>& bcidVec,
              //         const std::vector<uint_least16_t>& adc,
              //         const std::vector<uint_least8_t>& bcidExt,
              //         const uint_least16_t& error,
              //         const uint_least8_t& peak,
              //         const uint_least8_t& adcPeak
              // );
              const std::vector<uint_least32_t> dummy_vector32 {0};
              const std::vector<int_least16_t> dummy_svector16 {0};
              const std::vector<uint_least16_t> dummy_vector16 {0};
              const std::vector<uint_least8_t> dummy_vector8 {0};
              tt->initialize(
                0,
                0,
                eta,
                phi,
                dummy_vector8,
                dummy_vector8,
                dummy_svector16,
                dummy_vector8,
                dummy_vector8,
                dummy_vector16,
                dummy_vector8,
                0,
                0,
                0
              );


              m_ttData.push_back(tt);
              m_ttPos[index] = dataCount++;
              // ttMap.insert(std::make_pair(key,count));
              m_chanLayer[word] |= (layer << bit);
              m_dataChan[word]  |= (1 << bit);
              m_dataMod[word2]  |= (1 << bit2);
            }
          }
        }
      }
  } 
}

void PpmByteStreamV2Tool::collectTriggerTowers(
  const IROBDataProviderSvc::VROBFRAG& robFrags
) 
{
  // const bool debug   = msgLvl(MSG::DEBUG);
  // const bool verbose = msgLvl(MSG::VERBOSE);
  // if (debug) msg(MSG::DEBUG);

  TriggerTowerVector& ttColRef = m_ttData;
  ChannelBitVector& colChan = m_dataChan;
  ChannelBitVector& colMod = m_dataMod;

  const int maxChannels = m_crates * m_modules * m_channels;
  const int chanBitVecSize = maxChannels/32;
  m_foundChan.assign(chanBitVecSize, 0);

  int ttCount = 0;
  
  // Vectors to unpack into
  std::vector<uint_least8_t> lutCp;
  std::vector<uint_least8_t> lutJep;
  std::vector<uint_least16_t> fadc;
  
  std::vector<uint_least8_t> bcidLutCp;
  std::vector<uint_least8_t> satLutJep;
  std::vector<uint_least8_t> bcidFadc;
  std::vector<int_least16_t> correction;
  std::vector<uint_least8_t> correctionEnabled;
  
  // =========================================================================
  // Loop over ROB fragments
  // =========================================================================
  int robCount = 0;
  std::set<uint32_t> dupCheck;
  ROBIterator rob    = robFrags.begin();
  ROBIterator robEnd = robFrags.end();  

  for (; rob != robEnd; ++rob) {
    ATH_MSG_DEBUG("Treating ROB fragment " << robCount);
    ++robCount;
    
    // Skip fragments with ROB status errors
    uint32_t robid = (*rob)->source_id();

    if ((*rob)->nstatus() > 0) {
      ROBPointer robData;
      (*rob)->status(robData);
      if (*robData != 0) {
        m_errorTool->robError(robid, *robData);
        ATH_MSG_WARNING("ROB status error - skipping fragment");
        continue;
      }
    }
    // -----------------------------------------------------------------------
    // Unpack ROD data (slinks)
    // -----------------------------------------------------------------------
    RODPointer payloadBeg;
    RODPointer payload;
    RODPointer payloadEnd;
    (*rob)->rod_data(payloadBeg);
    payloadEnd = payloadBeg + (*rob)->rod_ndata();
    payload = payloadBeg;
    
    if (payload == payloadEnd) {
      ATH_MSG_DEBUG("ROB fragment empty");
      continue;
    } 
    // -----------------------------------------------------------------------    
    const uint32_t sourceID = (*rob)->rod_source_id();
    const int rodCrate = m_srcIdMap->crate(sourceID);
    const int rodSlink = m_srcIdMap->slink(sourceID);
    // -----------------------------------------------------------------------
    // Check identifier
    // Sasha: why we need this check: ROD == ROB
    // -----------------------------------------------------------------------
    // if (
    //     m_srcIdMap->getRobID(sourceID) != robid         ||
    //     m_srcIdMap->subDet(sourceID)   != m_subDetector ||
    //     m_srcIdMap->daqOrRoi(sourceID) != 0             ||
    //     rodSlink    >= m_maxSlinks   ||
    //     rodCrate    >= m_crates) {
    //       m_errorTool->rodError(robid, L1CaloSubBlock::ERROR_ROD_ID);
    //     if (debug) {
    //       msg(MSG::DEBUG) << "Wrong source identifier in data: ROD "
    //       << MSG::hex << sourceID << "  ROB " << robid
    //       << MSG::dec << endreq;
    //     }
    //     continue;
    // }
    // Skip duplicate fragments
    if (!dupCheck.insert(robid).second) {
      m_errorTool->rodError(robid, L1CaloSubBlock::ERROR_DUPLICATE_ROB);
      ATH_MSG_DEBUG("Skipping duplicate ROB fragment");
      continue;
    }
    // -----------------------------------------------------------------------
    // Check minor version
    // -----------------------------------------------------------------------

    const int majorVersion = (*rob)->rod_version() >> 16;
    const int minorVersion = (*rob)->rod_version() & 0xffff;


    // std::cout << "++> ROD version " << std::hex << (*rob)->rod_version() << std::dec << std::endl;
    // if (minorVersion > m_srcIdMap->minorVersionPreLS1()) {
    //   if (debug) msg() << "Skipping post-LS1 data" << endreq;
    //   continue;
    // }
    // -----------------------------------------------------------------------

    ATH_MSG_DEBUG("Treating crate " << rodCrate << " slink " << rodSlink);

    // First word should be User Header
    if ( !L1CaloUserHeader::isValid(*payload) ) {
      m_errorTool->rodError(robid, L1CaloSubBlock::ERROR_USER_HEADER);
      ATH_MSG_DEBUG("Invalid or missing user header");
      continue;
    }

    L1CaloUserHeader userHeader(*payload);
    userHeader.setVersion(minorVersion);

    ++payload; // Skip word

    // triggered slice offsets
    const uint_least8_t trigLut  = userHeader.ppmLut();
    const uint_least8_t trigFadc = userHeader.ppmFadc();
    // FADC baseline lower bound
    m_fadcBaseline = userHeader.lowerBound();

    ATH_MSG_DEBUG( 
      "Major format version number: "
      << MSG::hex  << majorVersion << MSG::dec << endreq
      << "Minor format version number: "
      << MSG::hex << minorVersion << MSG::dec << endreq
      << "LUT triggered slice offset:  " << int(trigLut)        << endreq
      << "FADC triggered slice offset: " << int(trigFadc)       << endreq
      << "FADC baseline lower bound:   " << m_fadcBaseline);

    const int runNumber = (*rob)->rod_run_no() & 0xffffff;

    // --------------------------------------------------------------------
    int chanPerSubBlock = 0;
    bool firstBlock = false; // Sasha: not used. delete?
    uint32_t firstWord = 0;  // Sasha: not used. delete?
    RODPointer payloadFirst;
    if (payload != payloadEnd) {
      // --------------------------------------------------------------------
      if (
            L1CaloSubBlock::wordType(*payload) != L1CaloSubBlock::HEADER ||
            CmmSubBlock::cmmBlock(*payload)
         ) {
              m_errorTool->rodError(robid, L1CaloSubBlock::ERROR_MISSING_HEADER);
              ATH_MSG_DEBUG("Missing Sub-block header");
              continue;
      }
      // --------------------------------------------------------------------
      firstBlock = true;
      firstWord = *payload;
      // --------------------------------------------------------------------
      // Sasha: Check first sublock for errors?
      // --------------------------------------------------------------------
      if (m_ppmBlocks.empty()) {
        m_ppmBlocks.push_back(new PpmSubBlockV2());
      }
      // --------------------------------------------------------------------
      PpmSubBlockV2* const subBlock = m_ppmBlocks[0];
      subBlock->clear();
      payloadFirst = subBlock->read(payload, payloadEnd);

      chanPerSubBlock = subBlock->channelsPerSubBlock();
      if (chanPerSubBlock == 0) {
        m_errorTool->rodError(robid, subBlock->unpackErrorCode());
        ATH_MSG_DEBUG("Unsupported version/data format: "
                     << subBlock->version() << "/"
                     << subBlock->format());
        continue;
      }
      
      ATH_MSG_DEBUG("Channels per sub-block: " << chanPerSubBlock);
    } else {
      ATH_MSG_DEBUG("ROB fragment contains user header only");
      continue;
    }

    const int numSubBlocks = m_channels/chanPerSubBlock;
    const int size = m_ppmBlocks.size();
    if (numSubBlocks > size) {
      // Sasha: Insert empty subblocks?
      for (int i = size; i < numSubBlocks; ++i) {
        m_ppmBlocks.push_back(new PpmSubBlockV2());
      }
    }
    // -----------------------------------------------------------------------
    // Loop over PPMs
    // -----------------------------------------------------------------------
    payload = payloadBeg;
    ++payload;
    unsigned int rodErr = L1CaloSubBlock::ERROR_NONE;
    // -----------------------------------------------------------------------
    // Sasha: Why we separetly check first block? Rewrite!
    // -----------------------------------------------------------------------
    while (payload != payloadEnd) {

      // Get all sub-blocks for one PPM (first already read in above)
      int crate = 0;
      int module = 0;
      int nPpmBlocks = 0;

      for (int block = 0; block < numSubBlocks; ++block) {
        const uint32_t word = (firstBlock) ? firstWord : *payload;
        if ( L1CaloSubBlock::wordType(word) != L1CaloSubBlock::HEADER ||
             CmmSubBlock::cmmBlock(word) ||
             PpmSubBlockV2::errorBlock(word)) {
          ATH_MSG_DEBUG("Unexpected data sequence");
          rodErr = L1CaloSubBlock::ERROR_MISSING_HEADER;
          break;
        }
        
        if ( chanPerSubBlock != m_channels && 
             L1CaloSubBlock::seqno(word) != block * chanPerSubBlock) {
          ATH_MSG_DEBUG("Unexpected channel sequence number: "
            << L1CaloSubBlock::seqno(word) << " expected " 
            << block * chanPerSubBlock);
          rodErr = L1CaloSubBlock::ERROR_MISSING_SUBBLOCK;
          break;
        }
        
        PpmSubBlockV2* const subBlock = m_ppmBlocks[block];
        nPpmBlocks++;
        
        if (firstBlock) {
          payload = payloadFirst;
          firstBlock = false;
        } else {
          subBlock->clear();
          payload = subBlock->read(payload, payloadEnd);
        }
        
        if (block == 0) {
          crate = subBlock->crate();
          module = subBlock->module();
          ATH_MSG_DEBUG("Crate " << crate << "  Module " << module);
          if (crate != rodCrate) {
            ATH_MSG_DEBUG("Inconsistent crate number in ROD source ID");
            rodErr = L1CaloSubBlock::ERROR_CRATE_NUMBER;
            break;
          }
        } else {
            if (subBlock->crate() != crate) {
              ATH_MSG_DEBUG("Inconsistent crate number in sub-blocks");
              rodErr = L1CaloSubBlock::ERROR_CRATE_NUMBER;
              break;
            }
            if (subBlock->module() != module) {
              ATH_MSG_DEBUG("Inconsistent module number in sub-blocks");
              rodErr = L1CaloSubBlock::ERROR_MODULE_NUMBER;
              break;
            }
        }
        if (payload == payloadEnd && block != numSubBlocks - 1) {
          ATH_MSG_DEBUG("Premature end of data");
          rodErr = L1CaloSubBlock::ERROR_MISSING_SUBBLOCK;
          break;
        }

      } // for

      if (rodErr != L1CaloSubBlock::ERROR_NONE) break;
      // Is there an error block?
      bool isErrBlock = false;
      if (payload != payloadEnd) {
        if (
            L1CaloSubBlock::wordType(*payload) == L1CaloSubBlock::HEADER && 
            !CmmSubBlock::cmmBlock(*payload) && 
            PpmSubBlockV2::errorBlock(*payload))  {
          ATH_MSG_DEBUG("Error block found");
          
          if (PpmSubBlockV2::isRun2(minorVersion)) {
            ATH_MSG_WARNING("TODO: Handle error blocks in RUN2 (alaexander.mazurov@cern.ch)");
            ++payload;
            break;
          }

          if (!m_errorBlock) {
            m_errorBlock = new PpmSubBlockV2();
          } else {
            m_errorBlock->clear();
          }
          isErrBlock = true;
          payload = m_errorBlock->read(payload, payloadEnd);
          if (m_errorBlock->crate() != crate) {
            ATH_MSG_DEBUG("Inconsistent crate number in error block");
            rodErr = L1CaloSubBlock::ERROR_CRATE_NUMBER;
            break;
          }
          if (m_errorBlock->module() != module) {
            ATH_MSG_DEBUG("Inconsistent module number in error block");
            rodErr = L1CaloSubBlock::ERROR_MODULE_NUMBER;
            break;
          }
          if (m_errorBlock->dataWords() && !m_errorBlock->unpack()) {
            std::string errMsg(m_errorBlock->unpackErrorMsg());
            ATH_MSG_DEBUG("Unpacking error block failed: " << errMsg);
            rodErr = m_errorBlock->unpackErrorCode();
            break;
          }
        }
      }

     

      const int index2 = (crate << 4) + module;
      const int word2  = index2 / 32;
      const int bit2   = index2 % 32;
      if (!((colMod[word2] >> bit2) & 1)) continue;

      // Loop over sub-blocks and fill trigger towers

      for (int block = 0; block < nPpmBlocks; ++block) {
        PpmSubBlockV2* const subBlock = m_ppmBlocks[block];
        
        // Don't bother unpacking modules that aren't used for required collection
        if (!isErrBlock && !subBlock->dataWords()){
          ATH_MSG_DEBUG("Don't handle error blocks in RUN2. Need to be fixed (alexander.mazurov@cern.ch)");
          break;
        }   
        
        subBlock->setLutOffset(trigLut);
        subBlock->setFadcOffset(trigFadc);
        subBlock->setPedestal(m_pedestal);
        subBlock->setFadcBaseline(m_fadcBaseline);
        subBlock->setRunNumber(runNumber);
        subBlock->setRodVersion((*rob)->rod_version());
        
        msg(MSG::DEBUG) << "Unpacking sub-block version/format/seqno: "
          << subBlock->version() << "/" << subBlock->format() << "/"
          << subBlock->seqno() << endreq;
        
        if (subBlock->dataWords() && !subBlock->unpack()) {
          std::string errMsg(subBlock->unpackErrorMsg());
          ATH_MSG_DEBUG("Unpacking PPM sub-block failed: " << errMsg);
          rodErr = subBlock->unpackErrorCode();
          break;
        }
        if (m_printCompStats) {
          addCompStats(subBlock->compStats());
        }
        
        for (int chan = 0; chan < chanPerSubBlock; ++chan) {
          const int channel = block*chanPerSubBlock + chan;
          const int index = (crate << 10) + (module << 6) + channel;
          const int word  = index / 32;
          const int bit   = index %   32;
        
          if ( !((colChan[word] >> bit) & 1)) continue; // skip unwanted channels
          if (((m_foundChan[word] >> bit) & 1)) {
            ATH_MSG_DEBUG("Duplicate data for crate/module/channel: "
                         << crate << "/" << module << "/" << channel);
            rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
            break;
          }
         // Get data
          subBlock->ppmData(channel, lutCp, lutJep, fadc, bcidLutCp, satLutJep,
            bcidFadc, correction, correctionEnabled);
          
          if (lutCp.size() < size_t(trigLut + 1)) {
            ATH_MSG_DEBUG("Triggered LUT slice from header "
                    << "inconsistent with number of slices: "
              << trigLut << ", " << lutCp.size());

            rodErr = L1CaloSubBlock::ERROR_SLICES;
            break;
          }
      
          if (fadc.size() < size_t(trigFadc + 1)) {
            ATH_MSG_DEBUG("Triggered FADC slice from header "
                    << "inconsistent with number of slices: "
              << trigFadc << ", " << fadc.size());
            rodErr = L1CaloSubBlock::ERROR_SLICES;
            break;
          }
          
          LVL1::DataError errorBits(0);
          if (isErrBlock) {
            errorBits.set(LVL1::DataError::PPMErrorWord,
                                     m_errorBlock->ppmError(channel));
            errorBits.set(LVL1::DataError::SubStatusWord,
                                     m_errorBlock->subStatus());
          } else {
              errorBits.set(LVL1::DataError::PPMErrorWord,
                                       subBlock->ppmError(channel));
              const PpmSubBlockV2* const lastBlock =
                                                m_ppmBlocks[nPpmBlocks - 1];
              errorBits.set(LVL1::DataError::SubStatusWord,
                                       lastBlock->subStatus());
          }
          // Wrong bit set for compressed formats 1.01 to 1.03
          if (subBlock->format() > 1 && subBlock->seqno() < 4) {
            errorBits.set(LVL1::DataError::ModuleError,
                 (errorBits.error() >> (LVL1::DataError::ModuleError+1)) & 0x1);
          }
          
          const int error = errorBits.error();

        // Save to TriggerTower
          const int layer = ((m_chanLayer[word] >> bit) & 1);
          const int coolid = coolId(crate, module, channel);

          ATH_MSG_VERBOSE(
            "|coolid:" << coolid << "(0x" << MSG::hex << coolid << MSG::dec << ")/" << std::endl
            << "crate:" << crate << "/" << std::endl
            << "module:" << module << "/" << std::endl
            << "channel:" << channel << "/" << std::endl
            << "lut_cp:" << vectorToString(lutCp) << std::endl
            << "lut_jep:" << vectorToString(lutJep) << std::endl
            << "fadc:" << vectorToString(fadc) << std::endl
            << "bcid_lutcp:" << vectorToString(bcidLutCp) << std::endl
            << "sat_lutjep:" << vectorToString(satLutJep) << std::endl
            << "bcid_fadc:" << vectorToString(bcidFadc) << std::endl
            << "correction:" << vectorToString(correction) << std::endl
            << "correction_enabled:" << vectorToString(correctionEnabled) << std::endl
            << "error:" << MSG::hex << error << MSG::dec << "|");
          
          m_foundChan[word] |= (1 << bit);
          ++ttCount;
          xAOD::TriggerTower* tt = ttColRef[m_ttPos[index]];
          
          // =================================================================
          // Update Trigger Towers objects
          // =================================================================

          tt->setCoolId(coolid);
          tt->setLayer(layer);
          tt->setLut_cp(lutCp);
          tt->setLut_jep(lutJep);
          tt->setAdc(fadc);
          
          tt->setBcidVec(bcidLutCp);
          tt->setBcidExt(bcidFadc);

          tt->setCorrection(correction);
          tt->setCorrectionEnabled(correctionEnabled);
          
          tt->setPeak(trigLut);
          tt->setAdcPeak(trigFadc);
          // =================================================================

        } // for chan
        if (rodErr != L1CaloSubBlock::ERROR_NONE) break;
      } // for block
      
      if (rodErr != L1CaloSubBlock::ERROR_NONE) break;
    } //while
    
    if (rodErr != L1CaloSubBlock::ERROR_NONE) {
      m_errorTool->rodError(robid, rodErr);
    }

    // TODO: (sasha) Reset any missing channels (should be rare)
    // -----------------------------------------------------------------------
    // Reset any missing channels (should be rare)
    // -----------------------------------------------------------------------
    // if (ttCount != colSize) {
    //   if (debug) {
    //     msg(MSG::DEBUG) << "Found " << ttCount << " channels, expected " << colSize << endreq;
    //   }
      
    //   std::vector<int> dummy(1);
    //   for (int word = 0; word < chanBitVecSize; ++word) {
    //     if (m_foundChan[word] != colChan[word]) {
    //       for (int bit = 0; bit < 32; ++bit) {
    //         if (((m_foundChan[word]>>bit)&1) != ((colChan[word]>>bit)&1)) {
    //           const int index = word*32 + bit;
    //           xAOD::TriggerTower* tt = ttColRef[m_ttPos[index]];
    //           const int layer = ((m_chanLayer[word] >> bit) & 1);
    //           tt->setLayer(layer);
    //           tt->setAdc(fadc);
    //           tt->setPeak(trigLut);
    //           tt->setAdcPeak(trigFadc);

    //         } // if
    //       } // for
    //     } // if
    //   }  // for
    // } // if   

    // ---------------------------------------------------------------------
  } // for

}

uint_least32_t PpmByteStreamV2Tool::coolId(int crate, int module, 
  int channel) const {
  const int pin  = channel % 16;
  const int asic = channel / 16;
  return (crate << 24) | (1 << 20) | (module << 16) | (pin << 8) | asic;
}

// Conversion bytestream to trigger towers
StatusCode PpmByteStreamV2Tool::convert(
    const IROBDataProviderSvc::VROBFRAG& robFrags,
    xAOD::TriggerTowerContainer* const ttCollection) {

  reserveMemory();
  collectTriggerTowers(robFrags);

  for (auto* tt: m_ttData) {
    if (tt->coolId()) {
      ttCollection->push_back(tt);
    }
  }



  return StatusCode::SUCCESS;
}
// ===========================================================================
// Conversion of trigger towers to bytestream

StatusCode PpmByteStreamV2Tool::convert(
    const xAOD::TriggerTowerContainer* const ttCollection,
    RawEventWrite* const /*re*/) {

  // Clear the event assembler

  m_fea->clear();

  const uint16_t minorVersion = m_srcIdMap->minorVersionPreLS1();
  m_fea->setRodMinorVersion(minorVersion);
  m_rodStatusMap.clear();

  // Pointer to ROD data vector

  //FullEventAssembler<L1CaloSrcIdMap>::RODDATA* theROD = 0;

  // Set up trigger tower maps

  setupSourceTowers(ttCollection);
  // Create the sub-blocks to do the packing

  PpmSubBlockV2 subBlock;
  const int chanPerSubBlock = subBlock.channelsPerSubBlock(m_subheaderVersion,
															     m_dataFormat);
  if (chanPerSubBlock == 0) {
	  ATH_MSG_ERROR("Unsupported version/data format: "
					<< m_subheaderVersion << "/" << m_dataFormat);
	  return StatusCode::FAILURE;
  }

  //TODO: Sasha implement
  ATH_MSG_ERROR("Convert xaod trigger towers to the new uncompressed ROD format"\
		  " is not ready. Write to alexander.mazurov@cern.ch that you need it");
  return StatusCode::FAILURE;
}
// ===========================================================================

// Return reference to vector with all possible Source Identifiers

const std::vector<uint32_t>& PpmByteStreamV2Tool::sourceIDs(
    const std::string& sgKey) {
  // -------------------------------------------------------------------------
  const std::string SPARE_FLAG("Spare");
  const std::string MUON_FLAG("Muon");

  // Check if spare channels wanted
  std::string::size_type pos = sgKey.find(SPARE_FLAG);
  if (pos != std::string::npos
      && pos == (sgKey.length() - SPARE_FLAG.length())) {
    m_channelsType = ChannelsType::Spare;
  } else {
    // Check if muon channels wanted
    pos = sgKey.find(MUON_FLAG);
    if (pos != std::string::npos
        && pos == (sgKey.length() - MUON_FLAG.length())) {
      m_channelsType = ChannelsType::Muon;
    } else {
      // Default - all channels
      m_channelsType = ChannelsType::Data;
    }
  }
  // -------------------------------------------------------------------------
  if (m_sourceIDs.empty()) {
    for (int crate = 0; crate < m_crates; ++crate) {
      for (int slink = 0; slink < m_srcIdMap->maxSlinks(); ++slink) {
        //       const int daqOrRoi = 0;
        const uint32_t rodId = m_srcIdMap->getRodID(crate, slink, 0,
            m_subDetector);
        // In principle rodId === robId
        const uint32_t robId = m_srcIdMap->getRobID(rodId);

        // In m_sourceIDs we store all ROB Ids from L1Calo
        m_sourceIDs.push_back(robId);

        // Spare IDs in crates 2,3,4,5
        if (m_channelsType != ChannelsType::Data) {
          const int BEGIN_SPARE_CRATE { 2 };
          const int END_SPARE_CRATE { 5 };

          if (crate >= BEGIN_SPARE_CRATE && crate < END_SPARE_CRATE) {
            if (m_channelsType == ChannelsType::Spare) {
              m_sourceIDsSpare.push_back(robId);
            }
            // Muon ids in create 2,3 and slink=0
            const int END_MUON_CRATE { 3 };
            const int SLINK_MUON { 0 };

            if (m_channelsType == ChannelsType::Muon && crate <= END_MUON_CRATE
                && slink == SLINK_MUON) {
              m_sourceIDsMuon.push_back(robId);
            }
          }
        }
      }
    }
  }
  // -------------------------------------------------------------------------
  switch (m_channelsType) {
  case ChannelsType::Spare:
    return m_sourceIDsSpare;
  case ChannelsType::Muon:
    return m_sourceIDsMuon;
  default:
    return m_sourceIDs;
  }
}

// Add compression stats to totals
void PpmByteStreamV2Tool::addCompStats(const std::vector<uint32_t>& stats)
{
  if (stats.empty()) return;
  const int n = stats.size();
  if (m_compStats.empty()) m_compStats.resize(n);
  for (int i = 0; i < n; ++i) m_compStats[i] += stats[i];
}

// Print compression stats
void PpmByteStreamV2Tool::printCompStats() const
{
  msg() << "Compression stats format/count: ";
  const int n = m_compStats.size();
  for (int i = 0; i < n; ++i) {
    msg() << " " << i << "/" << m_compStats[i];
  }
  msg() << endreq;
}


void PpmByteStreamV2Tool::setupSourceTowers(
		const xAOD::TriggerTowerContainer* ttCollection) {
	using std::accumulate;
	for(auto* tt: *ttCollection) {
		if (
			  accumulate(tt->lut_cp().begin(), tt->lut_cp().end(), 0)
			  ||
			  accumulate(tt->lut_jep().begin(), tt->lut_jep().end(), 0)
		) {
			// Collect only non empty towers
			m_source_towers.push_back(tt);
		}

	}
}


// ===========================================================================
}// end namespace
