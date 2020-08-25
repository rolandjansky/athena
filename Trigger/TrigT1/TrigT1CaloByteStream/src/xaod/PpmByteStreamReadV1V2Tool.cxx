/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ===========================================================================
// Includes
// ===========================================================================
// STD:
// ===========================================================================
#include <stdexcept>
#include <bitset>
// ===========================================================================
#include "eformat/SourceIdentifier.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloUtils/DataError.h"
#include "xAODTrigL1Calo/TriggerTowerAuxContainer.h"

#include "CaloUserHeader.h"
#include "SubBlockHeader.h"
#include "SubBlockStatus.h"
#include "WordDecoder.h"

#include "../L1CaloSubBlock.h" // Only for error codes
#include "../L1CaloSrcIdMap.h"

#include "PpmByteStreamReadV1V2Tool.h"

#include "CxxUtils/atomic_fetch_minmax.h"
// ===========================================================================

namespace {
uint32_t bitFieldSize(uint32_t word, uint8_t offset, uint8_t size) {
  return (word >> offset) & ((1U << size) - 1);
}

uint32_t crateModuleMask(uint8_t crate, uint8_t module) {
  return (crate << 8) | (1 << 4) | module;
}

uint32_t coolId(uint8_t crate, uint8_t module, uint8_t channel) {
  const uint8_t pin = channel % 16;
  const uint8_t asic = channel / 16;
  return (crateModuleMask(crate, module) << 16) | (pin << 8) | asic;
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
// ===========================================================================
namespace LVL1BS {
// ===========================================================================
// Constructor
PpmByteStreamReadV1V2Tool::PpmByteStreamReadV1V2Tool(const std::string& name /*=
  "PpmByteStreamxAODReadTool"*/) :
    AsgTool(name),
    m_errorTool("LVL1BS::L1CaloErrorByteStreamTool/L1CaloErrorByteStreamTool"),
    m_ppmMaps("LVL1::PpmMappingTool/PpmMappingTool"),
    m_robDataProvider("ROBDataProviderSvc", name),
    m_srcIdMap(nullptr),
    m_maxSizeSeen(0)
{
  declareInterface<PpmByteStreamReadV1V2Tool>(this);
  declareProperty("PpmMappingTool", m_ppmMaps,
      "Crate/Module/Channel to Eta/Phi/Layer mapping tool");
  declareProperty("ROBDataProviderSvc", m_robDataProvider,
        "Get ROB source IDs service");
}

// ===========================================================================
// Initialize


StatusCode PpmByteStreamReadV1V2Tool::initialize() {
  ATH_MSG_INFO(
      "Initializing " << name() << " - package version " << PACKAGE_VERSION);

  m_srcIdMap = new L1CaloSrcIdMap();
  CHECK(m_errorTool.retrieve());
  CHECK(m_ppmMaps.retrieve());
  CHECK(m_robDataProvider.retrieve());

  initSourceIDs();

  return StatusCode::SUCCESS;
}
// ===========================================================================
// Finalize

StatusCode PpmByteStreamReadV1V2Tool::finalize() {
  delete m_srcIdMap;

  return StatusCode::SUCCESS;
}

// Conversion bytestream to trigger towers
StatusCode PpmByteStreamReadV1V2Tool::convert(
    State& state,
    const IROBDataProviderSvc::VROBFRAG& robFrags) const
{
  if (m_maxSizeSeen > state.m_triggerTowers->capacity()){
    state.m_triggerTowers->reserve (m_maxSizeSeen);
  }

  state.m_subDetectorID = eformat::TDAQ_CALO_PREPROC;

  ROBIterator rob = robFrags.begin();
  ROBIterator robEnd = robFrags.end();

  int robCounter = 1;
  for (; rob != robEnd; ++rob, ++robCounter) {

    StatusCode sc = processRobFragment_(state, rob, RequestType::PPM);
    if (!sc.isSuccess()) {

    }
  }

  CxxUtils::atomic_fetch_max (&m_maxSizeSeen, state.m_triggerTowers->size());
  return StatusCode::SUCCESS;
}


StatusCode PpmByteStreamReadV1V2Tool::convert(
    xAOD::TriggerTowerContainer* const ttCollection) const
{
  return convert(LVL1::TrigT1CaloDefs::xAODTriggerTowerLocation, ttCollection);
}

StatusCode PpmByteStreamReadV1V2Tool::convert(const std::string& sgKey,
    xAOD::TriggerTowerContainer* const ttCollection) const
{
  State state;
  state.m_triggerTowers = ttCollection;
  const std::vector<uint32_t>& vID(ppmSourceIDs(state, sgKey));
  // // get ROB fragments
  IROBDataProviderSvc::VROBFRAG robFrags;
  m_robDataProvider->getROBData(vID, robFrags, "PpmByteStreamxAODReadTool");
  ATH_MSG_DEBUG("Number of ROB fragments:" << robFrags.size());

  CHECK(convert(state, robFrags));
  return StatusCode::SUCCESS;
}

// ===========================================================================
StatusCode PpmByteStreamReadV1V2Tool::processRobFragment_(
    State& state,
    const ROBIterator& robIter, const RequestType& /*requestedType*/) const
{
  auto rob = **robIter;

  ATH_MSG_DEBUG(
      "Treating ROB fragment source id #" << MSG::hex << rob.rob_source_id());


  state.m_rodSourceId = rob.rod_source_id();
  state.m_robSourceId = rob.source_id();
  const auto sourceID = (state.m_rodSourceId >> 16) & 0xff;
  const auto rodCrate = m_srcIdMap->crate(state.m_rodSourceId);
  const auto rodSlink = m_srcIdMap->slink(state.m_rodSourceId);
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
  uint32_t rodRunNumber = rob.rod_run_no() & 0xffffff;
  state.m_verCode = ((rodVer & 0xfff) << 4) | 1;


  if (sourceID != state.m_subDetectorID) {
    ATH_MSG_ERROR("Wrong subdetector source id for requested objects: " << state.m_rodSourceId);
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Treating crate " << rodCrate << " slink " << rodSlink);
  state.m_caloUserHeader = CaloUserHeader(*payload);
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
    if (CaloUserHeader::isValid(*payload) && (subBlock == 0)) {

    } else if (SubBlockHeader::isSubBlockHeader(*payload)) {
      indata = 0;
      CHECK(processPpmBlock_(state));

      state.m_ppLuts.clear();
      state.m_ppFadcs.clear();
      state.m_ppBlock.clear();

      blockType = (*payload >> 28) & 0xf;

      if ((blockType & 0xd) == 0xc) {
        state.m_subBlockHeader = SubBlockHeader(*payload);
        ATH_MSG_VERBOSE(
            "SubBlock version #" << int(state.m_subBlockHeader.version())
             << " format #" << int(state.m_subBlockHeader.format())
             << " seqNum (compVer) #" << int(state.m_subBlockHeader.seqNum())
             << " nslice1 #" << int(state.m_subBlockHeader.nSlice1())
             << " nslice2 #" << int(state.m_subBlockHeader.nSlice2())
        );
        subBlock = blockType & 0xe;
      } else if (blockType == (subBlock | 1)) {
        processSubBlockStatus_(state, state.m_subBlockHeader.crate(), state.m_subBlockHeader.module(), *payload);
        subBlock = 0;
      }
    } else {
      switch(state.m_subDetectorID){
      case eformat::TDAQ_CALO_PREPROC:
          CHECK(processPpmWord_(state, *payload, indata));
          break;
      default:
        break;
      }
      indata++;
    }
  }
  CHECK(processPpmBlock_(state));
  state.m_ppLuts.clear();
  state.m_ppFadcs.clear();
  state.m_ppBlock.clear();
  return StatusCode::SUCCESS;
}

StatusCode PpmByteStreamReadV1V2Tool::processPpmWord_(State& state,
                                                      uint32_t word,
                                                      int indata) const
{
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


StatusCode PpmByteStreamReadV1V2Tool::processPpmBlock_(State& state) const
{
  if (state.m_ppBlock.size() > 0) {
    if (state.m_subBlockHeader.format() == 0) {
      StatusCode sc = processPpmNeutral_(state);
      CHECK(sc);
      return sc;
    }

    if (state.m_verCode == 0x31) {
      StatusCode sc = processPpmCompressedR3V1_(state);
      CHECK(sc);
      return sc;
    }

    if (state.m_verCode == 0x41 || state.m_verCode == 0x42) {
      StatusCode sc = processPpmBlockR4V1_(state);
      CHECK(sc);
      return sc;
    }
  }

  if (state.m_ppLuts.size() > 0) {
    if (state.m_verCode == 0x21 || state.m_verCode == 0x31) {
      StatusCode sc = processPpmBlockR3V1_(state);
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

StatusCode PpmByteStreamReadV1V2Tool::processPpmNeutral_(State& state) const
{
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
        std::move(lcpVal),
        std::move(lcpBcidVec),
        std::move(ljeVal),
        std::move(ljeSat80Vec),
        std::move(adcVal),
        std::move(adcExt),
        std::move(pedCor),
        std::move(pedEn)));
      // ---------------------------------------------------------------------
      channel++;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode PpmByteStreamReadV1V2Tool::processPpmCompressedR3V1_(State& state) const
{
  BitReader br (state.m_ppBlock);
  uint8_t chan = 0;
  try{
    while (chan < 64) {
      uint8_t present = 1;
      if (state.m_subBlockHeader.format() == 3) {
        present = br.getField(1);
      }

      if (present == 1) {
        uint8_t lutVal = 0;
        uint8_t fmt = 6;
        uint8_t lutSat=0;
        uint8_t lutExt=0;
        uint8_t lutPeak=0;

        std::vector<uint16_t> adcVal = {0 , 0, 0, 0, 0};
        std::vector<uint8_t> adcExt = {0 , 0, 0, 0, 0};

        uint8_t minHeader = br.getField(4);
        uint8_t minIndex = minHeader % 5;
        if (minHeader < 15) { // Formats 0-5
          if (minHeader < 10) { // Formats 0-1
            fmt = minHeader / 5;
          } else { // Formats 2-5
            fmt = 2 + br.getField( 2);
            uint8_t haveLut = br.getField(1);
            if (fmt == 2) {
              if (haveLut == 1) {
                lutVal = br.getField(3);
                lutPeak = 1; // Even if LutVal==0 it seems
              }
            } else {
              uint8_t haveExt = br.getField(1);
              if (haveLut == 1) {
                lutVal = br.getField(8);
                lutExt = br.getField(1);
                lutSat = br.getField(1);
                lutPeak = br.getField(1);
              }

              if (haveExt == 1){
                for(uint8_t i = 0; i < 5; ++i) {
                  adcExt[i] = br.getField(1);
                }
              } else {
                adcExt[2] = lutExt;
              }
            }
          }
          adcVal = getPpmAdcSamplesR3_(state, br, fmt, minIndex);
        } else {
          uint8_t haveAdc = br.getField(1);
          if (haveAdc == 1) {
            uint16_t val = br.getField(10);
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
          std::move(adcVal),
          std::move(adcExt)
        ));
      }
      chan++;
    }
  }catch (const std::out_of_range& ex) {
      ATH_MSG_WARNING("Excess Data in Sub-block");
      m_errorTool->rodError(state.m_rodSourceId, L1CaloSubBlock::UNPACK_EXCESS_DATA);
  }
  return StatusCode::SUCCESS;
}

std::vector<uint16_t> PpmByteStreamReadV1V2Tool::getPpmAdcSamplesR3_(
  State& state,
  BitReader& br, uint8_t format, uint8_t minIndex) const
{
  std::vector<uint16_t> adc = {0, 0, 0, 0, 0};
  uint8_t minAdc = 0;

  for(uint8_t i = 0; i <5; ++i) {
    uint8_t longField = 0;
    uint8_t numBits = 0;
    if (format > 2) {
      longField = br.getField(1);
      numBits = longField == 0? 4: (format * 2);
    } else {
      numBits = i == 0? 4: (format + 2);
    }

    if (i == 0) {
      minAdc = br.getField(numBits);
      if (longField == 0) {
        minAdc += state.m_caloUserHeader.ppLowerBound();
      }
    } else {
        adc[i] = minAdc + br.getField(numBits);
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



StatusCode PpmByteStreamReadV1V2Tool::processPpmStandardR3V1_(State& state,
                                                              uint32_t word,
                                                              int inData) const
{

  StatusCode sc = StatusCode::SUCCESS;
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

StatusCode PpmByteStreamReadV1V2Tool::processPpmBlockR4V1_(State& state) const
{
  if (state.m_subBlockHeader.format() == 1) {
    CHECK(processPpmStandardR4V1_(state));
    return StatusCode::SUCCESS;
  } else if (state.m_subBlockHeader.format() >= 2) {
    CHECK(processPpmCompressedR4V1_(state));
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode PpmByteStreamReadV1V2Tool::processPpmCompressedR4V1_(State& state) const
{
  BitReader br (state.m_ppBlock);
  uint8_t numAdc = state.m_subBlockHeader.nSlice2();
  uint8_t numLut = state.m_subBlockHeader.nSlice1();
  int16_t pedCorBase = -20;


  try{
    for(uint8_t chan = 0; chan < s_channels; ++chan) {
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
        present = br.getField(1);
      }
      if (present == 1) {
        interpretPpmHeaderR4V1_(br, numAdc, encoding, minIndex);
        CHECK((encoding != -1) && (minIndex != -1));
        // First get the LIT related quantities
        if (encoding < 3) {
          // Get the peal finder bits
          for(uint i=0; i < numLut; ++i) {
            lcpPeak[i] = br.getField(1);
          }
          // Get Sat80 low bits
          if (encoding > 0) {
            for (uint8_t i = 0; i < numLut; ++i) {
              ljeLow[i] = br.getField(1);
            }
          }
          // Get LutCP and LutJEP values (these are
          // only present if the peak finder is set).
          if (encoding == 2) {
            for (uint8_t i = 0; i < numLut; ++i) {
              if (lcpPeak[i] == 1) {
                lcpVal[i] = br.getField(4);
              }
            }
            for(uint8_t i = 0; i < numLut; ++i) {
              if (lcpPeak[i] == 1){
                ljeVal[i] = br.getField(3);
              }
            }
          }
        } else if (encoding < 6) {
          // Get LUT presence flag for each LUT slice.
          for(uint8_t i = 0; i < numLut; ++i){
            haveLut[i] = br.getField(1);
          }

          // Get external BCID bits (if block is present).
          uint8_t haveExt = br.getField(1);

          if (haveExt == 1) {
            for (uint8_t i = 0; i < numAdc; ++i) {
              adcExt[i] = br.getField(1);
            }
          }

          for(uint8_t i = 0; i < numLut; ++i){
            if (haveLut[i] == 1) {
              lcpVal[i] = br.getField(8);
              lcpExt[i] = br.getField(1);
              lcpSat[i] = br.getField(1);
              lcpPeak[i] = br.getField(1);
            }
          }
          // Get JEP LUT values and corresponding bits.
          for(uint8_t i = 0; i < numLut; ++i){
            if (haveLut[i] == 1) {
              ljeVal[i] = br.getField(8);
              ljeLow[i] = br.getField(1);
              ljeHigh[i] = br.getField(1);
              ljeRes[i] = br.getField(1);
            }
          }

        }

      }
       // Next get the ADC related quantities (all encodings).
      adcVal = getPpmAdcSamplesR4_(state, br, encoding, minIndex);
      // Finally get the pedestal correction.
      if ((encoding < 3) || (encoding == 6)) {
        for (uint8_t i = 0; i < numLut; ++i)
        {
          pedCor[i] = br.getField(6) + pedCorBase;
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
          uint16_t val = br.getField(10);
          pedCor[i] = ::pedCorrection(val);
          pedEn[i] = br.getField(1);
        }
      }

    for(uint8_t i=0; i < numLut; ++i){
      lcpBcidVec[i] = uint8_t((lcpPeak[i] << 2) | (lcpSat[i] << 1) | lcpExt[i]);
      ljeSat80Vec[i] = uint8_t((ljeRes[i] << 2) | (ljeHigh[i] << 1) | ljeLow[i]);
    }
    CHECK(addTriggerTowerV2_(state,
                             state.m_subBlockHeader.crate(), state.m_subBlockHeader.module(),
                             chan,
                             std::move(lcpVal), std::move(lcpBcidVec),
                             std::move(ljeVal), std::move(ljeSat80Vec),
                             std::move(adcVal), std::move(adcExt),
                             std::move(pedCor), std::move(pedEn)));
    } // for
  } catch (const std::out_of_range& ex) {
      ATH_MSG_WARNING("Excess Data in Sub-block");
      m_errorTool->rodError(state.m_rodSourceId, L1CaloSubBlock::UNPACK_EXCESS_DATA);
  }
  //Check status workd
  return StatusCode::SUCCESS;

}

void PpmByteStreamReadV1V2Tool::interpretPpmHeaderR4V1_(BitReader& br,
                                                        uint8_t numAdc,
  int8_t& encoding, int8_t& minIndex) const
{
 uint8_t minHeader = 0;

  if (numAdc == 5) {
    minHeader = br.getField(4);

    minIndex = minHeader % 5;
    if (minHeader < 15){ // Encodings 0-5
      if (minHeader < 10) {
        encoding = minHeader / 5;
      } else {
        encoding = 2 + br.getField(2);
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
        minHeader = br.getField(numBits);
        uint8_t encValue = fieldSize - 1;
        if (minHeader == encValue) { // Encoding 6
          encoding = 6;
          minIndex = 0;
        } else {
          minHeader += br.getField(2) << numBits;
          minIndex = minHeader % fieldSize;
          encValue = 3 * fieldSize;

          if (minHeader < encValue) { // Encodings 0-2
            encoding = minHeader / fieldSize;
          } else {
            encoding = 3 + br.getField(2);
          }
        }
      }
  }
}

std::vector<uint16_t> PpmByteStreamReadV1V2Tool::getPpmAdcSamplesR4_(
  State& state,
  BitReader& br,
  uint8_t encoding, uint8_t minIndex) const
{
  uint8_t numAdc = state.m_subBlockHeader.nSlice2();

  if (encoding == 6) {
    uint16_t val = br.getField(6);
    return std::vector<uint16_t>(numAdc, val);
  } else if ( encoding < 3) {
    std::vector<uint16_t> adc(numAdc, 0);
    uint16_t minAdc = br.getField(5) + state.m_caloUserHeader.ppLowerBound();
    adc[minIndex] = minAdc;
    for(uint8_t i = 1; i < numAdc; ++i) {
      adc[i == minIndex? 0: i] = br.getField(encoding + 2) + minAdc;
    }
    return adc;
  } else {
    std::vector<uint16_t> adc(numAdc, 0);
    uint16_t minAdc = br.getField(1)
                      ? br.getField(encoding * 2)
                      : (br.getField(5) +
                          state.m_caloUserHeader.ppLowerBound());

    adc[minIndex] = minAdc;
    for (uint8_t i = 1; i < numAdc; ++i) {
      adc[minIndex == i? 0: i] = br.getField( 
                              br.getField(1)? encoding * 2: 4
                            ) + minAdc;
    }
   return adc;
  }
}

StatusCode PpmByteStreamReadV1V2Tool::processPpmBlockR3V1_(State& state) const
{
  if (state.m_subBlockHeader.format() == 1) {
    CHECK(processPpmStandardR3V1_(state));
    return StatusCode::SUCCESS;
  } else if (state.m_subBlockHeader.format() >= 2) {
    // TODO: convert compressed
    return StatusCode::FAILURE;
  }
  return StatusCode::FAILURE;
}

StatusCode PpmByteStreamReadV1V2Tool::processPpmStandardR4V1_(State& state) const
{
  BitReader br (state.m_ppBlock);
  uint8_t numAdc = state.m_subBlockHeader.nSlice2();
  uint8_t numLut = state.m_subBlockHeader.nSlice1();
  uint8_t crate = state.m_subBlockHeader.crate();
  uint8_t module = state.m_subBlockHeader.module();

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
        lcpVal.push_back(br.getField(8));
        lcpBcidVec.push_back(br.getField(3));
      }

      for (int i = 0; i < numLut; ++i) {
        ljeVal.push_back(br.getField(8));
        ljeSat80Vec.push_back(br.getField(3));
      }

      for (int i = 0; i < numAdc; ++i) {
        adcVal.push_back(br.getField(10));
        adcExt.push_back(br.getField(1));
      }

      for (int i = 0; i < numLut; ++i) {
        uint16_t pc = br.getField(10);
        pedCor.push_back(pedCorrection(pc));
        pedEn.push_back(br.getField(1));
      }
    } catch (const std::out_of_range& ex) {
      ATH_MSG_WARNING("Excess Data in Sub-block");
      m_errorTool->rodError(state.m_rodSourceId, L1CaloSubBlock::UNPACK_EXCESS_DATA);
    }
    CHECK(
        addTriggerTowerV2_(state,
                           crate, module, chan,
                           std::move(lcpVal), std::move(lcpBcidVec),
                           std::move(ljeVal), std::move(ljeSat80Vec),
                           std::move(adcVal), std::move(adcExt),
                           std::move(pedCor), std::move(pedEn)));
  }

  return StatusCode::SUCCESS;
}

StatusCode PpmByteStreamReadV1V2Tool::processPpmStandardR3V1_(State& state) const
{
    for(auto lut : state.m_ppLuts) {
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

void PpmByteStreamReadV1V2Tool::processSubBlockStatus_(State& state,
                                                       uint8_t crate, uint8_t module, uint32_t payload) const
{
  LVL1::DataError errorBits(0);
  errorBits.set(LVL1::DataError::SubStatusWord, payload);

  const uint32_t error = errorBits.error();
  int curr = state.m_triggerTowers->size() - 1;
  for(int i=0; i < s_channels; ++i){
    if (curr < 0){
      break;
    }
    auto tt = (*state.m_triggerTowers)[curr--];
    if (tt->coolId() >> 16 & crateModuleMask(crate, module)){
      tt->setErrorWord(error);
    }else{
      break;
    }
  }
}


StatusCode PpmByteStreamReadV1V2Tool::addTriggerTowerV2_(
    State& state,
    uint8_t crate,
    uint8_t module,
    uint8_t channel,
    std::vector<uint8_t>&& lcpVal,
    std::vector<uint8_t>&& lcpBcidVec,
    std::vector<uint8_t>&& ljeVal,
    std::vector<uint8_t>&& ljeSat80Vec,
    std::vector<uint16_t>&& adcVal,
    std::vector<uint8_t>&& adcExt,
    std::vector<int16_t>&& pedCor,
    std::vector<uint8_t>&& pedEn) const
{
  uint32_t coolId = ::coolId(crate, module, channel);

  int layer = 0;
  double eta = 0.;
  double phi = 0.;

  bool isData = m_ppmMaps->mapping(crate, module, channel, eta, phi, layer);

  if (!isData && !state.m_ppmIsRetSpare && !state.m_ppmIsRetMuon){
    return StatusCode::SUCCESS;
  }

  if (!isData) {
    const int pin  = channel % 16;
    const int asic = channel / 16;
    eta = 16 * crate + module;
    phi = 4 * pin + asic;
  }

  xAOD::TriggerTower* tt = new xAOD::TriggerTower();
  state.m_triggerTowers->push_back(tt);

  tt->initialize(coolId, eta, phi,
                 std::move(lcpVal), std::move(ljeVal),
                 std::move(pedCor), std::move(pedEn),
                 std::move(lcpBcidVec), std::move(adcVal),
                 std::move(adcExt), std::move(ljeSat80Vec),
                 0, state.m_caloUserHeader.lut(),
      state.m_caloUserHeader.ppFadc());
  return StatusCode::SUCCESS;
}

StatusCode PpmByteStreamReadV1V2Tool::addTriggerTowerV1_(
    State& state,
    uint8_t crate,
    uint8_t module,
    uint8_t channel,
    std::vector<uint8_t>&& luts,
    std::vector<uint8_t>&& lcpBcidVec,
    std::vector<uint16_t>&& fadc,
    std::vector<uint8_t>&& bcidExt
  ) const
{
    std::vector<uint8_t> ljeSat80Vec;

    std::vector<int16_t> pedCor;
    std::vector<uint8_t> pedEn;

   CHECK(addTriggerTowerV2_(state,
                            crate, module, channel,
                            std::move(luts), std::move(lcpBcidVec),
                            std::move(luts) , std::move(ljeSat80Vec),
                            std::move(fadc), std::move(bcidExt),
                            std::move(pedCor), std::move(pedEn))
   );

   return StatusCode::SUCCESS;
}

StatusCode PpmByteStreamReadV1V2Tool::addTriggerTowerV1_(
    State& state,
    uint8_t crate,
    uint8_t module,
    uint8_t channel,
    const std::vector<uint16_t>& luts,
    const std::vector<uint16_t>& fadc
  ) const
{
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

   CHECK(addTriggerTowerV1_(state,
                            crate, module, channel,
                            std::move(lcpVal), std::move(lcpBcidVec),
                            std::move(adcVal), std::move(adcExt)));

   return StatusCode::SUCCESS;
}


// Return reference to vector with all possible Source Identifiers

void PpmByteStreamReadV1V2Tool::initSourceIDs()
{
  const int crates = 8;

  m_ppmSourceIDs.clear();
  m_ppmSourceIDsSpare.clear();
  m_ppmSourceIDsMuon.clear();

  for (int crate = 0; crate < crates; ++crate) {
    for (int slink = 0; slink < m_srcIdMap->maxSlinks(); ++slink) {
      const uint32_t rodId = m_srcIdMap->getRodID(crate, slink, 0,
                                                  eformat::TDAQ_CALO_PREPROC);
      const uint32_t robId = m_srcIdMap->getRobID(rodId);
      m_ppmSourceIDs.push_back(robId);
      if (crate > 1 && crate < 6) {
        m_ppmSourceIDsSpare.push_back(robId);
        if (crate < 4 && slink == 0) {
          m_ppmSourceIDsMuon.push_back(robId);
        }
      }
    }
  }
}



const std::vector<uint32_t>& PpmByteStreamReadV1V2Tool::ppmSourceIDs(
  State& state,
  const std::string& sgKey) const
{
  state.m_ppmIsRetMuon = false;
  state.m_ppmIsRetSpare =  false;

  if (sgKey.find("Muon") != std::string::npos) {
    state.m_ppmIsRetMuon = true;
  } else if (sgKey.find("Spare") != std::string::npos) {
    state.m_ppmIsRetSpare = true;
  }

  if (state.m_ppmIsRetSpare) {
    return m_ppmSourceIDsSpare;
  }

  if (state.m_ppmIsRetMuon) {
    return m_ppmSourceIDsMuon;
  }

  return m_ppmSourceIDs;
}



uint32_t PpmByteStreamReadV1V2Tool::BitReader::getField
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
// ===========================================================================
} // end namespace
// ===========================================================================
