/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ===========================================================================
// Includes
// ===========================================================================
// STD:
// ===========================================================================
#include <stdexcept>
#include <bitset>
#include <iomanip>
// ===========================================================================

#include "eformat/SourceIdentifier.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "xAODTrigL1Calo/TriggerTowerAuxContainer.h" 

// PAS all adapted from L1Calo, since they hide all of their headers in "src"{!!}
#include "ZdcByteStream/ZdcCaloUserHeader.h"
#include "ZdcByteStream/ZdcSubBlockHeader.h"
#include "ZdcByteStream/ZdcSubBlockStatus.h"
#include "ZdcByteStream/ZdcWordDecoder.h"
#include "ZdcByteStream/ZdcToString.h"
#include "ZdcByteStream/ZdcL1CaloSubBlock.h" // Only for error codes
#include "ZdcEvent/ZdcDigitsCollection.h"
#include "ZdcConditions/ZdcCablingService.h"

//#include "TrigT1CaloByteStream/L1CaloSubBlock.h" // Only for error codes
//#include "TrigT1CaloByteStream/L1CaloSrcIdMap.h"
#include "ZdcByteStream/ZdcSrcIdMap.h"
#include "ZdcByteStream/ZdcPpmSubBlock.h"

#include "ZdcByteStream/ZdcByteStreamReadV1V2Tool.h"

//#include "xAODForward/ZdcModuleContainer.h"
//#include "xAODForward/ZdcModuleAuxContainer.h"

// ===========================================================================

const int      slink2ppmChannel[64] = 
  {0,  4,  8,  12,  16,  20,  24,  28,  32,  36,  40,  44,  48,  52,  56,  60,
   3,  7, 11,  15,  19,  23,  27,  31,  35,  39,  43,  47,  51,  55,  59,  63,
   1,  5,  9,  13,  17,  21,  25,  29,  33,  37,  41,  45,  49,  53,  57,  61,
   2,  6, 10,  14,  18,  22,  26,  30,  34,  38,  42,  46,  50,  54,  58,  62 };



namespace {
uint32_t bitFieldSize(uint32_t word, uint8_t offset, uint8_t size) {
  return (word >> offset) & ((1U << size) - 1);
}

uint32_t coolId(uint8_t crate, uint8_t module, uint8_t channel) {
  const uint8_t pin = channel % 16;
  const uint8_t asic = channel / 16;
  return (crate << 24) | (1 << 20) | (module << 16) | (pin << 8) | asic;
} 

int16_t pedCorrection(uint16_t twoBytePedCor) {
  return twoBytePedCor > 511? (twoBytePedCor - 1024): twoBytePedCor;
}

#if 0
std::string noAuxSuffix(const std::string& name) {
  if ((name.size() > 4) && (name.substr(name.size()-4, 4) == "Aux.")) {
    return name.substr(0, name.size() - 4);
  }
  return name;
}
#endif
}
// namespace removed since I was not planning on putting everything into the LVL1BS namespace (yet)
// ===========================================================================
//namespace LVL1BS {
// ===========================================================================
// Constructor
ZdcByteStreamReadV1V2Tool::ZdcByteStreamReadV1V2Tool(const std::string& name /*=
  "ZdcByteStreamxAODReadTool"*/) :
    AsgTool(name),
    m_errorTool("ZdcL1CaloErrorByteStreamTool/ZdcL1CaloErrorByteStreamTool"),
    //m_ppmMaps("LVL1::PpmMappingTool/PpmMappingTool"),
    m_robDataProvider("ROBDataProviderSvc", name) {
  declareInterface<ZdcByteStreamReadV1V2Tool>(this);
  //declareProperty("PpmMappingTool", m_ppmMaps,
  //    "Crate/Module/Channel to Eta/Phi/Layer mapping tool");
  declareProperty("ROBDataProviderSvc", m_robDataProvider,
        "Get ROB source IDs service");
}

// ===========================================================================
// Initialize


StatusCode ZdcByteStreamReadV1V2Tool::initialize() {

  m_srcIdMap = new ZdcSrcIdMap();
  CHECK(m_errorTool.retrieve());
  //CHECK(m_ppmMaps.retrieve());
  CHECK(m_robDataProvider.retrieve());

  const ZdcID* zdcID = nullptr;
  if (detStore()->retrieve( zdcID ).isFailure() ) {
    msg(MSG::ERROR) << "execute: Could not retrieve ZdcID object from the detector store" << endmsg;
    return StatusCode::FAILURE;
  }
  else {
    msg(MSG::DEBUG) << "execute: retrieved ZdcID" << endmsg;
  }
  m_zdcID = zdcID;

  initSourceIDs();
  
  return StatusCode::SUCCESS;
}
// ===========================================================================
// Finalize

StatusCode ZdcByteStreamReadV1V2Tool::finalize() {
  delete m_srcIdMap;

  return StatusCode::SUCCESS;
}

// Conversion bytestream to trigger towers
StatusCode ZdcByteStreamReadV1V2Tool::convert(
    const IROBDataProviderSvc::VROBFRAG& robFrags,
    xAOD::TriggerTowerContainer* const ttCollection) const
{
  State state;
  state.m_subDetectorID = eformat::FORWARD_ZDC;
  state.m_triggerTowers = ttCollection;

  ROBIterator rob = robFrags.begin();
  ROBIterator robEnd = robFrags.end();

  int robCounter = 1;
  for (; rob != robEnd; ++rob, ++robCounter) {
    StatusCode sc = processRobFragment_(state, rob, RequestType::PPM);
    if (!sc.isSuccess()) {

    }
  }
  return StatusCode::SUCCESS;
}

// Conversion bytestream to trigger towers and then to ZDC digits
/*
StatusCode ZdcByteStreamReadV1V2Tool::convert(
    const IROBDataProviderSvc::VROBFRAG& robFrags,
    ZdcDigitsCollection* zdcCollection) 
*/
StatusCode ZdcByteStreamReadV1V2Tool::convert(   
					      const IROBDataProviderSvc::VROBFRAG& robFrags,
					      ZdcDigitsCollection* zdcCollection) const
{

  xAOD::TriggerTowerContainer* ttCollection = new xAOD::TriggerTowerContainer;
  xAOD::TriggerTowerAuxContainer* aux = new xAOD::TriggerTowerAuxContainer();
  ttCollection->setStore(aux);

  ATH_MSG_DEBUG("Getting TT collection!");

  StatusCode sc = convert(robFrags,ttCollection);

  ATH_MSG_DEBUG("Got it!");

  if (sc != StatusCode::SUCCESS)
    {
      ATH_MSG_DEBUG("ZDC TT Conversion failed");
      return sc;
    }
  
  ATH_MSG_DEBUG("convertTT2ZD");
  ZdcDigitsCollection* zc = convertTT2ZD(ttCollection);  
      
  ATH_MSG_DEBUG("convertedTT2ZD!  Now copying");
  (*zdcCollection) = (*zc); // will copy work?;
  ATH_MSG_DEBUG("Copied!");

  return StatusCode::SUCCESS;
}

StatusCode ZdcByteStreamReadV1V2Tool::convert(
    xAOD::TriggerTowerContainer* const ttCollection) const {
  //return convert(LVL1::TrigT1CaloDefs::xAODTriggerTowerLocation, ttCollection);
  return convert("ZdcTriggerTowerContainer", ttCollection);
}

StatusCode ZdcByteStreamReadV1V2Tool::convert(const std::string& sgKey,
    xAOD::TriggerTowerContainer* const ttCollection) const {

  //std::cout << "convert " << sgKey << std::endl;

  const std::vector<uint32_t>& vID(ppmSourceIDs(sgKey));

  // // get ROB fragments
  IROBDataProviderSvc::VROBFRAG robFrags;
  m_robDataProvider->getROBData(vID, robFrags, "ZdcByteStreamxAODReadTool");
  ATH_MSG_DEBUG("Number of ROB fragments:" << robFrags.size());

  CHECK(convert(robFrags, ttCollection));
  return StatusCode::SUCCESS;
}

// ===========================================================================
StatusCode ZdcByteStreamReadV1V2Tool::processRobFragment_(
    State& state,
    const ROBIterator& robIter, const RequestType& /*requestedType*/) const {

  auto rob = **robIter;

  ATH_MSG_DEBUG(
      "Treating ROB fragment source id #" << MSG::hex << rob.rob_source_id());


  state.m_rodSourceId = rob.rod_source_id();
  state.m_robSourceId = rob.source_id();
  const auto sourceID = (state.m_rodSourceId >> 16) & 0xff;
  const auto rodCrate = ZdcSrcIdMap::crate(state.m_rodSourceId);
  const auto rodSlink = ZdcSrcIdMap::slink(state.m_rodSourceId);
  // -------------------------------------------------------------------------
  // Check Rob status
  if (rob.nstatus() > 0) {
    ROBPointer robData;
    rob.status(robData);
    if (*robData != 0) {
      ATH_MSG_WARNING("ROB status error - skipping fragment");
      m_errorTool->robError(state.m_rodSourceId, *robData);
      return StatusCode::FAILURE;
    }
  }
  // -------------------------------------------------------------------------
  RODPointer payloadBeg;
  RODPointer payloadEnd;
  RODPointer payload;

  rob.rod_data(payloadBeg);
  payloadEnd = payloadBeg + rob.rod_ndata();
  payload = payloadBeg;
  // -------------------------------------------------------------------------
  if (payload == payloadEnd) {
    ATH_MSG_DEBUG("ROB fragment empty");
    return StatusCode::FAILURE;
  }
  // -------------------------------------------------------------------------


  uint16_t rodVer = rob.rod_version() & 0xffff;
  state.m_verCode = ((rodVer & 0xfff) << 4) | 1;
  uint32_t rodRunNumber = rob.rod_run_no() & 0xffffff;


  if (sourceID != state.m_subDetectorID) {
    ATH_MSG_ERROR("Wrong subdetector source id for requested objects: " << state.m_rodSourceId);
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Treating crate " << rodCrate << " slink " << rodSlink);

  state.m_caloUserHeader = ZdcCaloUserHeader(*payload);
  if (!state.m_caloUserHeader.isValid()) {
    ATH_MSG_ERROR("Invalid or missing user header");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG(
                "Run number: " << MSG::dec << rodRunNumber << endmsg
                << "Version code: 0x" << MSG::hex << int(state.m_verCode) << MSG::dec
                << endmsg << "LUT triggered slice offset:  "
                << int(state.m_caloUserHeader.lut()) << endmsg
                << "FADC triggered slice offset: " << int(state.m_caloUserHeader.ppFadc())
                << endmsg << "FADC baseline lower bound:   "
                << int(state.m_caloUserHeader.ppLowerBound()));

  int indata = 0;
  uint8_t blockType = 0;
  int subBlock = 0;

  for (; payload != payloadEnd; ++payload) {
    if (ZdcCaloUserHeader::isValid(*payload) && (subBlock == 0)) {

    } else if (ZdcSubBlockHeader::isSubBlockHeader(*payload)) {
      indata = 0;
      CHECK(processPpmBlock_(state));
      
      state.m_ppLuts.clear();
      state.m_ppFadcs.clear();
      state.m_ppBlock.clear();

      blockType = (*payload >> 28) & 0xf;

      if ((blockType & 0xd) == 0xc) {
        state.m_subBlockHeader = ZdcSubBlockHeader(*payload);
        ATH_MSG_VERBOSE(
            "SubBlock version #" << int(state.m_subBlockHeader.version())
             << " format #" << int(state.m_subBlockHeader.format())
             << " seqNum (compVer) #" << int(state.m_subBlockHeader.seqNum())
             << " nslice1 #" << int(state.m_subBlockHeader.nSlice1())
             << " nslice2 #" << int(state.m_subBlockHeader.nSlice2())
        );
        subBlock = blockType & 0xe;
      } else if (blockType == (subBlock | 1)) {
        //m_subBlockStatus = ZdcSubBlockStatus(*payload);
        subBlock = 0;
      }
    } else {
      switch(state.m_subDetectorID){
      case eformat::TDAQ_CALO_PREPROC:
          CHECK(processPpmWord_(state, *payload, indata));
          break;
      case eformat::FORWARD_ZDC:
          CHECK(processPpmWord_(state, *payload, indata));
          break;
      default:
        break;
      }
      indata++;
    }
  }
  CHECK(processPpmBlock_(state));
  return StatusCode::SUCCESS;
}

StatusCode ZdcByteStreamReadV1V2Tool::processPpmWord_(State& state,
                                                      uint32_t word,
    int indata) const {
  if ( (state.m_subBlockHeader.format() == 0) 
      || (state.m_subBlockHeader.format() >= 2) 
      || (state.m_verCode >= 0x41)) {
    state.m_ppBlock.push_back(word);
  } else if ((state.m_verCode == 0x21) || (state.m_verCode == 0x31)) {
    return processPpmStandardR3V1_(state, word, indata);
  } else {
    ATH_MSG_ERROR("Unsupported PPM version:format (" 
      << state.m_verCode << ":" << state.m_subBlockHeader.format() 
      <<") combination");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


StatusCode ZdcByteStreamReadV1V2Tool::processPpmBlock_(State& state) const {
  if (!state.m_ppBlock.empty()) {
    if (state.m_subBlockHeader.format() == 0) {
      StatusCode sc = processPpmNeutral_(state);
      state.m_ppBlock.clear();
      CHECK(sc);
      return sc;
    }

    if (state.m_verCode == 0x31) {
      StatusCode sc = processPpmCompressedR3V1_(state);
      state.m_ppBlock.clear();
      CHECK(sc);
      return sc;
    }

    if (state.m_verCode == 0x41 || state.m_verCode == 0x42) {
      StatusCode sc = processPpmBlockR4V1_(state);
      state.m_ppBlock.clear();
      CHECK(sc);
      return sc;
    }
  }

  if (!state.m_ppLuts.empty()) {
    if (state.m_verCode == 0x21 || state.m_verCode == 0x31) {
      StatusCode sc = processPpmBlockR3V1_(state);
      state.m_ppLuts.clear();
      state.m_ppFadcs.clear();
      CHECK(sc);
      return sc;
    }
    ATH_MSG_ERROR("Unknown PPM subheader format '" 
      << int(state.m_subBlockHeader.format()) 
      << "' for rob version '"
      << MSG::hex << int(state.m_verCode) 
      << MSG::dec << "'" );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode ZdcByteStreamReadV1V2Tool::processPpmNeutral_(State& state) const {
  uint8_t numLut = state.m_subBlockHeader.nSlice1();
  uint8_t numFadc = state.m_subBlockHeader.nSlice2();
  uint8_t totSlice = 3 * numLut + numFadc;

  uint8_t channel = 0;
  for ( int asic = 0 ; asic < 4 ; ++asic ) {
    for ( int mcm = 0 ; mcm < 16 ; ++mcm ) {
      // ----------------------------------------------------------------------
      std::vector<uint32_t> rotated(totSlice);

      for ( uint8_t slice = 0 ; slice < totSlice ; ++slice ) {
        for ( uint8_t bit = 0 ; bit < 11 ; ++bit ) {
          if ( state.m_ppBlock[slice * 11 + asic * (11 * totSlice) + bit + 1] & (1 << mcm))
              rotated[slice] |= (1 << bit);
          }
      }

      bool nonZeroData = false;
      for (uint8_t slice = 0; slice < numLut; ++slice) {
        if (rotated[slice] 
            || rotated[slice + numLut] 
            || rotated[slice + 2 * numLut + numFadc]) { // CP, JET
          nonZeroData = true;
          break;
        }
      }

      std::vector<uint8_t> lcpVal;
      std::vector<uint8_t> lcpBcidVec;
      std::vector<uint8_t> ljeVal;
      std::vector<uint8_t> ljeSat80Vec;
      std::vector<int16_t> pedCor;
      std::vector<uint8_t> pedEn;

      std::vector<uint16_t> adcVal;
      std::vector<uint8_t> adcExt;

      if (nonZeroData) {
        for (uint8_t slice = 0; slice < numLut; ++slice) {
          lcpVal.push_back(rotated[slice] & 0xff);
          ljeVal.push_back(rotated[slice + numLut] & 0xff);
          pedCor.push_back(::pedCorrection(rotated[slice + 2 * numLut + numFadc] & 0x3ff));
          
          lcpBcidVec.push_back((rotated[slice] >> 8) & 0x7);
          ljeSat80Vec.push_back((rotated[slice + numLut] >> 8) & 0x7);
          pedEn.push_back((rotated[slice + 2 * numLut + numFadc] >> 10) & 0x1);
        }
      }

      for (uint8_t slice = 0; slice < numFadc; ++slice) {
        if (rotated[slice + numLut]) { // CP, JET
          nonZeroData = true;
          break;
        }
      }

      if (nonZeroData) {
        for (uint8_t slice = 0; slice < numFadc; ++ slice) {
          adcVal.push_back(rotated[slice + 2 * numLut] & 0x3ff);
          adcExt.push_back((rotated[slice + 2 * numLut] >> 10 & 0x1) & 0x3ff);
        }
      }

      CHECK(addTriggerTowerV2_(
        state,
        state.m_subBlockHeader.crate(),
        state.m_subBlockHeader.module(),
        channel,
        lcpVal,
        lcpBcidVec,
        ljeVal,
        ljeSat80Vec, adcVal,
        adcExt,
        pedCor,
        pedEn));
      // ---------------------------------------------------------------------
      channel++;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode ZdcByteStreamReadV1V2Tool::processPpmCompressedR3V1_(State& state) const {
  uint8_t chan = 0;
  BitReader br (state.m_ppBlock);
  try{
    while (chan < 64) {
      uint8_t present = 1;
      if (state.m_subBlockHeader.format() == 3) {
        present = br.getField (1); 
      } 

      if (present == 1) {
        uint8_t lutVal = 0;
        uint8_t fmt = 6;
        uint8_t lutSat=0;
        uint8_t lutExt=0;
        uint8_t lutPeak=0;

        std::vector<uint16_t> adcVal = {0 , 0, 0, 0, 0};
        std::vector<uint8_t> adcExt = {0 , 0, 0, 0, 0};

        uint8_t minHeader = br.getField (4);
        uint8_t minIndex = minHeader % 5;
        if (minHeader < 15) { // Formats 0-5
          if (minHeader < 10) { // Formats 0-1
            fmt = minHeader / 5;
          } else { // Formats 2-5
            fmt = 2 + br.getField (2);
            uint8_t haveLut = br.getField (1);
            if (fmt == 2) {
              if (haveLut == 1) {
                lutVal = br.getField (3);
                lutPeak = 1; // Even if LutVal==0 it seems
              }
            } else {
              uint8_t haveExt = br.getField (1);
              if (haveLut == 1) {
                lutVal = br.getField (8);
                lutExt = br.getField (1);
                lutSat = br.getField (1);
                lutPeak = br.getField (1);
              }

              if (haveExt == 1){
                for(uint8_t i = 0; i < 5; ++i) {
                  adcExt[i] = br.getField (1);
                }
              } else {
                adcExt[2] = lutExt;
              }
            }
          }
          adcVal = getPpmAdcSamplesR3_(state, br, fmt, minIndex);
        } else {
          uint8_t haveAdc = br.getField (1);
          if (haveAdc == 1) {
            uint16_t val = br.getField (10);
            for(uint8_t i = 0; i < 5; ++i) {
                  adcVal[i] = val;
            }
          }
        }
        // Add Trigger Tower
        //std::vector<uint8_t> luts = {lutVal};
        CHECK(addTriggerTowerV1_(
          state,
          state.m_subBlockHeader.crate(),
          state.m_subBlockHeader.module(),
          chan,
          std::vector<uint8_t> {lutVal},
          std::vector<uint8_t> {uint8_t(lutExt | (lutSat << 1) | (lutPeak << 2))},
          adcVal,
          adcExt
        ));
      }
      chan++;
    }
  }catch (const std::out_of_range& ex) {
      ATH_MSG_WARNING("Excess Data in Sub-block");
      m_errorTool->rodError(state.m_rodSourceId, ZdcL1CaloSubBlock::UNPACK_EXCESS_DATA);
  } 
  return StatusCode::SUCCESS;
}

std::vector<uint16_t> ZdcByteStreamReadV1V2Tool::getPpmAdcSamplesR3_(
  State& state,
  BitReader& br, uint8_t format, uint8_t minIndex) const {

  std::vector<uint16_t> adc = {0, 0, 0, 0, 0};
  uint8_t minAdc = 0;

  for(uint8_t i = 0; i <5; ++i) {
    uint8_t longField = 0;
    uint8_t numBits = 0;
    if (format > 2) {
      longField = br.getField (1);
      numBits = longField == 0? 4: (format * 2);
    } else {
      numBits = i == 0? 4: (format + 2);
    }

    if (i == 0) {
      minAdc = br.getField (numBits);
      if (longField == 0) {
        minAdc += state.m_caloUserHeader.ppLowerBound();
      }
    } else {
        adc[i] = minAdc + br.getField (numBits);
    }
  }

  if (minIndex == 0) {
    adc[0] = minAdc;
  } else {
    adc[0] = adc[minIndex];
    adc[minIndex] = minAdc;
  }
  return adc;
}



StatusCode ZdcByteStreamReadV1V2Tool::processPpmStandardR3V1_(State& state,
                                                              uint32_t word,
    int inData) const {
  StatusCode sc;
  if (state.m_subBlockHeader.seqNum() == 63) { // Error block
    ATH_MSG_DEBUG("Error PPM subblock");
    //TODO: errorTool
  } else {
    const uint8_t numAdc = state.m_subBlockHeader.nSlice2();
    const uint8_t numLut = state.m_subBlockHeader.nSlice1();
    const uint8_t nTotal = numAdc + numLut;
    const uint8_t wordsPerBlock = 8; // 16 towers (4 MCMs) / 2 per word
    const uint8_t iBlk =  inData / wordsPerBlock;
    uint8_t iChan =  state.m_subBlockHeader.seqNum() + 2 * (inData % wordsPerBlock);
    
    if (iBlk < numLut) { // First all LUT values
      for(uint8_t i = 0; i < 2; ++i) {
        uint16_t subword = (word >> 16 * i) & 0x7ff;
        state.m_ppLuts[iChan].push_back(subword);
        iChan++;
      }
    } else if (iBlk < nTotal) { // Next all FADC values
      for(uint8_t i = 0; i < 2; ++i) {
        uint16_t subword = (word >> (16 * i)) & 0x7ff;
        state.m_ppFadcs[iChan].push_back(subword);
        iChan++;
      }
    
    } else{
      ATH_MSG_WARNING("Error decoding Ppm word (run1)");
      sc = StatusCode::FAILURE;
    }
 
  }
  return sc;
}

StatusCode ZdcByteStreamReadV1V2Tool::processPpmBlockR4V1_(State& state) const {
  if (state.m_subBlockHeader.format() == 1) {
    CHECK(processPpmStandardR4V1_(state));
    return StatusCode::SUCCESS;
  } else if (state.m_subBlockHeader.format() >= 2) {
    // TODO: convert compressed
    CHECK(processPpmCompressedR4V1_(state));
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode ZdcByteStreamReadV1V2Tool::processPpmCompressedR4V1_(State& state) const {
  BitReader br (state.m_ppBlock);

  uint8_t numAdc = state.m_subBlockHeader.nSlice2();
  uint8_t numLut = state.m_subBlockHeader.nSlice1();
  int16_t pedCorBase = -20;


  // for(size_t i = 0; i < state.m_ppBlock.size(); ++i) {
  //   std::bitset<32> x(state.m_ppBlock[i]);
  //   std::cout << i << " " << x << std::endl;
  // }

  try{
    for(uint8_t chan = 0; chan < 64; ++chan) {
      uint8_t present = 1;

      std::vector<uint8_t> haveLut(numLut, 0);
      std::vector<uint8_t> lcpVal(numLut, 0);
      
      std::vector<uint8_t> lcpExt(numLut, 0);
      std::vector<uint8_t> lcpSat(numLut, 0);
      std::vector<uint8_t> lcpPeak(numLut, 0);
      std::vector<uint8_t> lcpBcidVec(numLut, 0);
      
      std::vector<uint8_t> ljeVal(numLut, 0);
      
      std::vector<uint8_t> ljeLow(numLut, 0);
      std::vector<uint8_t> ljeHigh(numLut, 0);
      std::vector<uint8_t> ljeRes(numLut, 0);
      std::vector<uint8_t> ljeSat80Vec(numLut, 0);

      std::vector<uint16_t> adcVal(numAdc, 0);
      std::vector<uint8_t> adcExt(numAdc, 0);
      std::vector<int16_t> pedCor(numLut, 0);
      std::vector<uint8_t> pedEn(numLut, 0);
  
      int8_t encoding = -1;
      int8_t minIndex = -1;

      if (state.m_subBlockHeader.format() == 3) {
        present = br.getField (1);
      }
      if (present == 1) {
        interpretPpmHeaderR4V1_(br, numAdc, encoding, minIndex);
        CHECK((encoding != -1) && (minIndex != -1));
        // First get the LIT related quantities
        if (encoding < 3) {
          // Get the peal finder bits
          for(uint i=0; i < numLut; ++i) {
            lcpPeak[i] = br.getField (1);
          }
          // Get Sat80 low bits
          if (encoding > 0) {
            for (uint8_t i = 0; i < numLut; ++i) {
              ljeLow[i] = br.getField (1);
            }
          }
          // Get LutCP and LutJEP values (these are
          // only present if the peak finder is set).
          if (encoding == 2) {
            for (uint8_t i = 0; i < numLut; ++i) {
              if (lcpPeak[i] == 1) {
                lcpVal[i] = br.getField (4);
              }
            }
            for(uint8_t i = 0; i < numLut; ++i) {
              if (lcpPeak[i] == 1){
                ljeVal[i] = br.getField (3);
              }
            }
          }            
        } else if (encoding < 6) {
          // Get LUT presence flag for each LUT slice. 
          for(uint8_t i = 0; i < numLut; ++i){
            haveLut[i] = br.getField (1);
          }

          // Get external BCID bits (if block is present).
          uint8_t haveExt = br.getField (1);

          if (haveExt == 1) {
            for (uint8_t i = 0; i < numAdc; ++i) {
              adcExt[i] = br.getField (1);
            }
          }
          
          for(uint8_t i = 0; i < numLut; ++i){
            if (haveLut[i] == 1) {
              lcpVal[i] = br.getField (8);
              lcpExt[i] = br.getField (1);
              lcpSat[i] = br.getField (1);
              lcpPeak[i] = br.getField (1);
            }
          }
          // Get JEP LUT values and corresponding bits.         
          for(uint8_t i = 0; i < numLut; ++i){
            if (haveLut[i] == 1) {
              ljeVal[i] = br.getField (8);
              ljeLow[i] = br.getField (1);
              ljeHigh[i] = br.getField (1);
              ljeRes[i] = br.getField (1);
            }
          }
  
        }

      }

      if (minIndex < 0)
        return StatusCode::FAILURE;

       // Next get the ADC related quantities (all encodings).
      adcVal = getPpmAdcSamplesR4_(state, br, encoding, minIndex);
      // Finally get the pedestal correction.
      if ((encoding < 3) || (encoding == 6)) {
        for (uint8_t i = 0; i < numLut; ++i)
        {
          pedCor[i] = br.getField (6) + pedCorBase;
          if (state.m_subBlockHeader.compVer() > 0) {
            pedEn[i] = 1;
          }
        }
      } else {
        // At the moment there is an enabled bit for every LUT slice
        // (even though its really a global flag).
        // The correction values is a twos complement signed value.
        for (uint8_t i = 0; i < numLut; ++i)
        {
          uint16_t val = br.getField (10);
          pedCor[i] = ::pedCorrection(val);
          pedEn[i] = br.getField (1);
        }
      }

    for(uint8_t i=0; i < numLut; ++i){
      lcpBcidVec[i] = uint8_t((lcpPeak[i] << 2) | (lcpSat[i] << 1) | lcpExt[i]);
      ljeSat80Vec[i] = uint8_t((ljeRes[i] << 2) | (ljeHigh[i] << 1) | ljeLow[i]); 
    }
    CHECK(addTriggerTowerV2_(state, state.m_subBlockHeader.crate(), state.m_subBlockHeader.module(),
      chan, lcpVal, lcpBcidVec, ljeVal, ljeSat80Vec, adcVal, adcExt, pedCor,
      pedEn));
    }
  } catch (const std::out_of_range& ex) {
      ATH_MSG_WARNING("Excess Data in Sub-block");
      m_errorTool->rodError(state.m_rodSourceId, ZdcL1CaloSubBlock::UNPACK_EXCESS_DATA);
  }
  return StatusCode::SUCCESS;

}

void ZdcByteStreamReadV1V2Tool::interpretPpmHeaderR4V1_(BitReader& br,
  uint8_t numAdc,
  int8_t& encoding, int8_t& minIndex) const {
 uint8_t minHeader = 0;

  if (numAdc == 5) {
    minHeader = br.getField (4);
    //ATH_MSG_DEBUG("SASHA: minHeader=" << int(minHeader));
    minIndex = minHeader % 5;
    if (minHeader < 15){ // Encodings 0-5
      if (minHeader < 10) {
        encoding = minHeader / 5;
      } else {
        encoding = 2 + br.getField (2);
      }
    } else {
      encoding = 6;
    }
  } else {
      uint8_t numBits = 0;
      if (numAdc ==3 ) {
        numBits = 2;
      } else if (numAdc == 7) {
        numBits = 3;
      } else if (numAdc < 16) {
        numBits = 4;
      }

      if (numBits > 0) {
        uint8_t fieldSize = 1 << numBits;
        minHeader = br.getField (numBits);
        uint8_t encValue = fieldSize - 1;
        if (minHeader == encValue) { // Encoding 6
          encoding = 6;
          minIndex = 0; 
        } else {
          minHeader += br.getField (2) << numBits;
          minIndex = minHeader % fieldSize;
          encValue = 3 * fieldSize;

          if (minHeader < encValue) { // Encodings 0-2
            encoding = minHeader / fieldSize;
          } else {
            encoding = 3 + br.getField (2);
          }
        }
      }
  }
}

std::vector<uint16_t> ZdcByteStreamReadV1V2Tool::getPpmAdcSamplesR4_(
  State& state,
  BitReader& br, uint8_t encoding, uint8_t minIndex) const {
  uint8_t numAdc = state.m_subBlockHeader.nSlice2();

  if (encoding == 6) {
    uint16_t val = br.getField (6);
    return std::vector<uint16_t>(numAdc, val);
  } else if ( encoding < 3) {
    std::vector<uint16_t> adc(numAdc, 0);
    uint16_t minAdc = br.getField (5) + state.m_caloUserHeader.ppLowerBound();
    adc[minIndex] = minAdc;
    for(uint8_t i = 1; i < numAdc; ++i) {
      adc[i == minIndex? 0: i] = br.getField (encoding + 2) + minAdc;
    } 
    return adc;   
  } else {
    std::vector<uint16_t> adc(numAdc, 0);
    uint16_t minAdc = br.getField (1)
                      ? br.getField (encoding * 2)
                      : (br.getField (5) + 
                          state.m_caloUserHeader.ppLowerBound());
 
    adc[minIndex] = minAdc;
    for (uint8_t i = 1; i < numAdc; ++i) {
      adc[minIndex == i? 0: i] = br.getField (
                              br.getField (1)? encoding * 2: 4
                            ) + minAdc;
    }
   return adc;
  }
}

StatusCode ZdcByteStreamReadV1V2Tool::processPpmBlockR3V1_(State& state) const {
  if (state.m_subBlockHeader.format() == 1) {
    CHECK(processPpmStandardR3V1_(state));
    return StatusCode::SUCCESS;
  } else if (state.m_subBlockHeader.format() >= 2) {
    // TODO: convert compressed
    return StatusCode::FAILURE;
  }
  return StatusCode::FAILURE;
}

StatusCode ZdcByteStreamReadV1V2Tool::processPpmStandardR4V1_(State& state) const {

  //std::cout << "Here I am in  processPpmStandardR4V1_" << std::endl;
  uint8_t numAdc = state.m_subBlockHeader.nSlice2();
  uint8_t numLut = state.m_subBlockHeader.nSlice1();
  uint8_t crate = state.m_subBlockHeader.crate();
  uint8_t module = state.m_subBlockHeader.module();


  BitReader br (state.m_ppBlock);

  for (uint8_t chan = 0; chan < 64; ++chan) {
    //for (uint8_t k = 0; k < 4; ++k) {
    std::vector<uint8_t> lcpVal;
    std::vector<uint8_t> lcpBcidVec;

    std::vector<uint8_t> ljeVal;
    std::vector<uint8_t> ljeSat80Vec;



    std::vector<uint16_t> adcVal;
    std::vector<uint8_t> adcExt;
    std::vector<int16_t> pedCor;
    std::vector<uint8_t> pedEn;
    try {
      for (int i = 0; i < numLut; ++i) {
        lcpVal.push_back(br.getField (8));
        lcpBcidVec.push_back(br.getField (3));
      }

      for (int i = 0; i < numLut; ++i) {
        ljeVal.push_back(br.getField (8));
        ljeSat80Vec.push_back(br.getField (3));
      }

      for (int i = 0; i < numAdc; ++i) {
        adcVal.push_back(br.getField (10));
        adcExt.push_back(br.getField (1));
      }

      for (int i = 0; i < numLut; ++i) {
        uint16_t pc = br.getField (10);
        pedCor.push_back(pedCorrection(pc));
        pedEn.push_back(br.getField (1));
      }
    } catch (const std::out_of_range& ex) {
      ATH_MSG_WARNING("Excess Data in Sub-block");
      m_errorTool->rodError(state.m_rodSourceId, ZdcL1CaloSubBlock::UNPACK_EXCESS_DATA);
    }
    CHECK(
        addTriggerTowerV2_(state, crate, module, chan, lcpVal, lcpBcidVec,
            ljeVal, ljeSat80Vec, adcVal, adcExt, pedCor, pedEn));
  }

  return StatusCode::SUCCESS;
}

StatusCode ZdcByteStreamReadV1V2Tool::processPpmStandardR3V1_(State& state) const {
    for(const auto& lut : state.m_ppLuts) {
      CHECK(addTriggerTowerV1_(
        state,
        state.m_subBlockHeader.crate(), 
        state.m_subBlockHeader.module(),
        lut.first,
        lut.second,
        state.m_ppFadcs[lut.first]));
    }
    return StatusCode::SUCCESS;
}

StatusCode ZdcByteStreamReadV1V2Tool::addTriggerTowerV2_(
    State& state,
    uint8_t crate,
    uint8_t module,
    uint8_t channel,
    const std::vector<uint8_t>& lcpVal,
    const std::vector<uint8_t>& lcpBcidVec,
    const std::vector<uint8_t>& ljeVal,
    const std::vector<uint8_t>& ljeSat80Vec,
    const std::vector<uint16_t>& adcVal,
    const std::vector<uint8_t>& adcExt,
    const std::vector<int16_t>& pedCor,
    const std::vector<uint8_t>& pedEn) const {

  //int layer = 0;
  int error = 0;
  double eta = 0.;
  double phi = 0.;

  bool isNotSpare = false; // PAS - just to minimize changing code
  // PAS - not using L1Calo PPMMap
  /*  
  bool isNotSpare = m_ppmMaps->mapping(crate, module, channel, eta, phi, layer);
  if (!isNotSpare && !m_ppmIsRetSpare && !m_ppmIsRetMuon){
    return StatusCode::SUCCESS;
  }
  */

  if (!isNotSpare) {
    const int pin  = channel % 16;
    const int asic = channel / 16;
    eta = 16 * crate + module;
    phi = 4 * pin + asic;
  }

  uint32_t coolId = ::coolId(crate, module, channel);
  CHECK(state.m_coolIds.count(coolId) == 0);
  state.m_coolIds.insert(coolId);

  xAOD::TriggerTower* tt = new xAOD::TriggerTower();
  //std::cout << ZdcToString(*tt) << std::endl;

  state.m_triggerTowers->push_back(tt);
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
  //         const std::vector<uint_least8_t>& sat80, 
  //         const uint_least16_t& error,
  //         const uint_least8_t& peak,
  //         const uint_least8_t& adcPeak
  // );
  tt->initialize(coolId, eta, phi, lcpVal, ljeVal, pedCor, pedEn,
      lcpBcidVec, adcVal, adcExt, ljeSat80Vec, error, state.m_caloUserHeader.lut(),
      state.m_caloUserHeader.ppFadc());
  return StatusCode::SUCCESS;
}

StatusCode ZdcByteStreamReadV1V2Tool::addTriggerTowerV1_(
    State& state,
    uint8_t crate,
    uint8_t module,
    uint8_t channel,
    const std::vector<uint8_t>& luts,
    const std::vector<uint8_t>& lcpBcidVec,
    const std::vector<uint16_t>& fadc,
    const std::vector<uint8_t>& bcidExt
  ) const {

    std::vector<uint8_t> ljeSat80Vec;

    std::vector<int16_t> pedCor;
    std::vector<uint8_t> pedEn;

   CHECK(addTriggerTowerV2_(state, crate, module, channel, luts, lcpBcidVec,
            luts , ljeSat80Vec, fadc, bcidExt, pedCor, pedEn)
   );

   return StatusCode::SUCCESS;
}

StatusCode ZdcByteStreamReadV1V2Tool::addTriggerTowerV1_(
    State& state,
    uint8_t crate,
    uint8_t module,
    uint8_t channel,
    const std::vector<uint16_t>& luts,
    const std::vector<uint16_t>& fadc
  ) const {

    std::vector<uint8_t> lcpVal;
    std::vector<uint8_t> lcpBcidVec;

    std::vector<uint16_t> adcVal;
    std::vector<uint8_t> adcExt;

    for(auto lut: luts) {
      lcpVal.push_back(BitField::get<uint8_t>(lut, 0, 8));
      lcpBcidVec.push_back(BitField::get<uint8_t>(lut, 8, 3));
    }

    for(auto f: fadc) {
      adcExt.push_back(BitField::get<uint8_t>(f, 0, 1));
      adcVal.push_back(BitField::get<uint16_t>(f, 1, 10));
    }

   CHECK(addTriggerTowerV1_(state, crate, module, channel, lcpVal, lcpBcidVec,
            adcVal, adcExt));

   return StatusCode::SUCCESS;
}

void ZdcByteStreamReadV1V2Tool::initSourceIDs()
{
  const int crates = 8;
  m_ppmSourceIDs.clear();
  m_ppmSourceIDsSpare.clear();
  m_ppmSourceIDsMuon.clear();

  for (int crate = 0; crate < crates; ++crate) {
    for (int slink = 0; slink < m_srcIdMap->maxSlinks(); ++slink) {
      //const uint32_t rodId = m_srcIdMap->getRodID(crate, slink, 0,
      //    eformat::TDAQ_CALO_PREPROC);
      const uint32_t rodId = m_srcIdMap->getRodID(crate, slink, 0,
                                                  eformat::FORWARD_ZDC);
      const uint32_t robId = m_srcIdMap->getRobID(rodId);
      m_ppmSourceIDs.push_back(robId);
      //std::cout << "robId=" << std::hex << robId << std::dec << std::endl;
      if (crate > 1 && crate < 6) {
        m_ppmSourceIDsSpare.push_back(robId);
        if (crate < 4 && slink == 0) {
          m_ppmSourceIDsMuon.push_back(robId);
        }
      }
    }
  }
}

// Return reference to vector with all possible Source Identifiers

const std::vector<uint32_t>& ZdcByteStreamReadV1V2Tool::ppmSourceIDs(
  const std::string& sgKey) const
{
  if (sgKey.find("Muon") != std::string::npos) {
    return m_ppmSourceIDsMuon;
  }
  else if (sgKey.find("Spare") != std::string::npos) {
    return m_ppmSourceIDsSpare;
  }

  return m_ppmSourceIDs;
}



uint32_t ZdcByteStreamReadV1V2Tool::BitReader::getField
  (const uint8_t numBits)
{
  if ((m_ppPointer + numBits) <= m_ppMaxBit) {
    uint32_t iWord = m_ppPointer / 31;
    uint8_t iBit = m_ppPointer % 31;
    m_ppPointer += numBits;

    uint32_t result;
    if ((iBit + numBits) <= 31) {
      result = ::bitFieldSize(m_ppBlock[iWord], iBit, numBits);
    } else {
      uint8_t nb1 = 31 - iBit;
      uint8_t nb2 = numBits - nb1;
      uint32_t field1 = ::bitFieldSize(m_ppBlock[iWord], iBit, nb1);
      uint32_t field2 = ::bitFieldSize(m_ppBlock[iWord + 1], 0, nb2);
      result = field1 | (field2 << nb1);
    }

    return result;
  }

  throw std::out_of_range("Requested too much bits from ppm block");
}

ZdcDigitsCollection* ZdcByteStreamReadV1V2Tool::convertTT2ZD(xAOD::TriggerTowerContainer* const ttCollection) const
{
  typedef std::map<uint32_t,ZdcDigits*> hashmapType;
  hashmapType digits_map;
  Identifier chan_id;

  for (const xAOD::TriggerTower* tt : *ttCollection)
    {
      //std::cout << "dumping TT" << std::endl;
      //std::cout << ZdcToString(*tt ) << std::endl;

      //uint32_t coolId = tt->coolId();
      //uint32_t coolId = tt->auxdata<uint32_t>("coolId");
      uint32_t coolId = tt->coolId();
      uint32_t pin = (coolId>>8) & 0xf;
      uint32_t asic = coolId & 0xf;
      uint32_t slinkChannel = asic*16 + pin;
      uint32_t ppmChannel = slink2ppmChannel[slinkChannel];

      uint32_t module = (coolId>>16) & 0xf;

      ATH_MSG_DEBUG( "--> ZCS: " << ZdcCablingService::getInstance() << " mod=" << module << " slinkC=" << slinkChannel << " ppmC=" << ppmChannel );
      chan_id = ZdcCablingService::getInstance()->h2s_channel_id(module, ppmChannel);
      
      const uint32_t chan_hash = chan_id.get_identifier32().get_compact();
      int gain  = ZdcCablingService::getInstance()->hwid2gain(module,ppmChannel);
      int delay = ZdcCablingService::getInstance()->hwid2delay(module,ppmChannel);
      
      ATH_MSG_DEBUG("Trying to find chan_hash " << chan_hash << " g" << gain << "d" << delay);

      hashmapType::iterator iter = digits_map.find(chan_hash);
      if (iter == digits_map.end())
	{
	  ATH_MSG_DEBUG("new channel for " << chan_id);
	  digits_map.insert(std::pair<uint32_t,ZdcDigits*>(chan_hash,new ZdcDigits(chan_id)));
	  iter = digits_map.find(chan_hash);
	}
      if (iter != digits_map.end())
	{
	  ATH_MSG_DEBUG("new auxdata for " << chan_id);
	  if (gain==0&&delay==0) (*iter).second->set_digits_gain0_delay0( tt->adc() );
	  if (gain==1&&delay==0) (*iter).second->set_digits_gain1_delay0( tt->adc() );
	  if (gain==0&&delay==1) (*iter).second->set_digits_gain0_delay1( tt->adc() );
	  if (gain==1&&delay==1) (*iter).second->set_digits_gain1_delay1( tt->adc() );
	}
      
    }

  ZdcDigitsCollection* zdcCollection = new ZdcDigitsCollection();

  hashmapType::iterator iter = digits_map.begin();
  hashmapType::iterator iter_end = digits_map.end();

  while (iter != iter_end)
    {
      /*
      Identifier id = ((*iter).second)->identify();
	std::cout  
	<< "side=" << m_zdcID->side(id)
	<< " mod=" << m_zdcID->module(id)
	<< " type=" << m_zdcID->type(id)
	<< " chan=" << m_zdcID->channel(id) 
	<< std::endl;
	((*iter).second)->print();
      */ 
      zdcCollection->push_back((*iter).second);
      ++iter;
    }

  return zdcCollection;
}

/*
xAOD::ZdcModuleContainer* ZdcByteStreamReadV1V2Tool::convertTT2ZM(xAOD::TriggerTowerContainer* const ttCollection)
{
  typedef std::map<uint32_t,xAOD::ZdcModule*> hashmapType;
  hashmapType digits_map;
  Identifier chan_id;
  
  xAOD::TriggerTowerContainer::iterator tt_itr = ttCollection->begin();
  xAOD::TriggerTowerContainer::iterator tt_end = ttCollection->end();
  
  for (;tt_itr != tt_end;tt_itr++)
    {
      std::cout << "dumping TT" << std::endl;
      std::cout << ZdcToString(*(*tt_itr) ) << std::endl;

      //uint32_t coolId = (*tt_itr).coolId();
      //uint32_t coolId = (*tt_itr)->auxdata<uint32_t>("coolId");
      uint32_t coolId = (*tt_itr)->coolId();
      uint32_t pin = (coolId>>8) & 0xf;
      uint32_t asic = coolId & 0xf;
      uint32_t slinkChannel = asic*16 + pin;
      uint32_t ppmChannel = slink2ppmChannel[slinkChannel];

      uint32_t module = (coolId>>16) & 0xf;

      ATH_MSG_DEBUG( "--> ZCS: " << ZdcCablingService::getInstance() << " mod=" << module << " slinkC=" << slinkChannel << " ppmC=" << ppmChannel );
      chan_id = ZdcCablingService::getInstance()->h2s_channel_id(module, ppmChannel);
      
      const uint32_t chan_hash = chan_id.get_identifier32().get_compact();
      int gain  = ZdcCablingService::getInstance()->hwid2gain(module,ppmChannel);
      int delay = ZdcCablingService::getInstance()->hwid2delay(module,ppmChannel);
      
      ATH_MSG_DEBUG("Trying to find chan_hash " << chan_hash << " g" << gain << "d" << delay);

      hashmapType::iterator iter = digits_map.find(chan_hash);
      if (iter == digits_map.end())
	{
	  ATH_MSG_DEBUG("new channel for " << chan_id);
	  digits_map.insert(std::pair<uint32_t,xAOD::ZdcModule*>(chan_hash,new xAOD::ZdcModule()));
	  iter = digits_map.find(chan_hash);
	}
      if (iter != digits_map.end())
	{
	  ATH_MSG_DEBUG("new elementlinks for " << chan_id);
	  (*iter).second->setId(chan_hash);
	  (*iter).second->setSide(m_zdcID->side(chan_id));
	  (*iter).second->setModule(m_zdcID->module(chan_id));
	  (*iter).second->setType(m_zdcID->type(chan_id));
	  (*iter).second->setChannel(m_zdcID->channel(chan_id));
	  if (gain==0&&delay==0) (*iter).second->setTTg0d0Link( ElementLink<xAOD::TriggerTowerContainer_v2> ((*tt_itr),*ttCollection ) );
	  if (gain==0&&delay==1) (*iter).second->setTTg0d1Link( ElementLink<xAOD::TriggerTowerContainer_v2> ((*tt_itr),*ttCollection ) );
	  if (gain==1&&delay==0) (*iter).second->setTTg1d0Link( ElementLink<xAOD::TriggerTowerContainer_v2> ((*tt_itr),*ttCollection ) );
	  if (gain==1&&delay==1) (*iter).second->setTTg1d1Link( ElementLink<xAOD::TriggerTowerContainer_v2> ((*tt_itr),*ttCollection ) );
	}
      
    }

  xAOD::ZdcModuleContainer* zmc = new xAOD::ZdcModuleContainer;
  xAOD::ZdcModuleAuxContainer* zmac = new xAOD::ZdcModuleAuxContainer;
  zmc->setStore(zmac);

  hashmapType::iterator iter = digits_map.begin();
  hashmapType::iterator iter_end = digits_map.end();

  while (iter != iter_end)
    {
      std::cout  
	<< " side=" <<  ((*iter).second)->side()
	<< " mod="  <<  ((*iter).second)->module()
	<< " type=" <<  ((*iter).second)->type()
	<< " chan=" <<  ((*iter).second)->channel()
	<< std::endl;

      zmc->push_back((*iter).second);
      iter++;
    }

  return zmc;
}
*/

// ===========================================================================
//} // end namespace
// ===========================================================================
