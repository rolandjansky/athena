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
    m_subDetectorID(0),
    m_requestedType(),
    m_ppmIsRetMuon(false),
    m_ppmIsRetSpare(false),
    m_srcIdMap(nullptr),
    m_rodSourceId(0),
    m_robSourceId(0),
    m_rodRunNumber(0),
    m_rodVer(0),
    m_verCode(0),
    m_ppPointer(0),
    m_ppMaxBit(0),
    m_maxSizeSeen(0),
    m_triggerTowers(nullptr)
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

  ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", name());
  CHECK(incidentSvc.retrieve());
  incidentSvc->addListener(this, IncidentType::EndEvent);

  return StatusCode::SUCCESS;
}
// ===========================================================================
// Finalize

StatusCode PpmByteStreamReadV1V2Tool::finalize() {
  delete m_srcIdMap;

  return StatusCode::SUCCESS;
}

void PpmByteStreamReadV1V2Tool::handle( const Incident& inc )
{
  if ( inc.type() == IncidentType::EndEvent) {

  }
}
// Conversion bytestream to trigger towers
StatusCode PpmByteStreamReadV1V2Tool::convert(
    const IROBDataProviderSvc::VROBFRAG& robFrags,
    xAOD::TriggerTowerContainer* const ttCollection) {

  m_triggerTowers = ttCollection;

  if (m_maxSizeSeen > m_triggerTowers->capacity()){
    m_triggerTowers->reserve (m_maxSizeSeen);
  }

  m_subDetectorID = eformat::TDAQ_CALO_PREPROC;
  m_requestedType = RequestType::PPM;

  ROBIterator rob = robFrags.begin();
  ROBIterator robEnd = robFrags.end();

  int robCounter = 1;
  for (; rob != robEnd; ++rob, ++robCounter) {

    StatusCode sc = processRobFragment_(rob, RequestType::PPM);
    if (!sc.isSuccess()) {

    }
  }

  m_maxSizeSeen = std::max (m_maxSizeSeen, m_triggerTowers->size());
  m_triggerTowers = nullptr;
  return StatusCode::SUCCESS;
}


StatusCode PpmByteStreamReadV1V2Tool::convert(
    xAOD::TriggerTowerContainer* const ttCollection) {
  return convert(LVL1::TrigT1CaloDefs::xAODTriggerTowerLocation, ttCollection);
}

StatusCode PpmByteStreamReadV1V2Tool::convert(const std::string& sgKey,
    xAOD::TriggerTowerContainer* const ttCollection) {

  m_triggerTowers = ttCollection;

  const std::vector<uint32_t>& vID(ppmSourceIDs(sgKey));
  // // get ROB fragments
  IROBDataProviderSvc::VROBFRAG robFrags;
  m_robDataProvider->getROBData(vID, robFrags, "PpmByteStreamxAODReadTool");
  ATH_MSG_DEBUG("Number of ROB fragments:" << robFrags.size());

  CHECK(convert(robFrags, ttCollection));
  return StatusCode::SUCCESS;
}

// ===========================================================================
StatusCode PpmByteStreamReadV1V2Tool::processRobFragment_(
    const ROBIterator& robIter, const RequestType& /*requestedType*/) {

  auto rob = **robIter;

  ATH_MSG_DEBUG(
      "Treating ROB fragment source id #" << MSG::hex << rob.rob_source_id());


  m_rodSourceId = rob.rod_source_id();
  m_robSourceId = rob.source_id();
  const auto sourceID = (m_rodSourceId >> 16) & 0xff;
  const auto rodCrate = m_srcIdMap->crate(m_rodSourceId);
  const auto rodSlink = m_srcIdMap->slink(m_rodSourceId);
  // -------------------------------------------------------------------------
  // Check Rob status
  if (rob.nstatus() > 0) {
    ROBPointer robData;
    rob.status(robData);
    if (*robData != 0) {
      ATH_MSG_WARNING("ROB status error - skipping fragment");
      m_errorTool->robError(m_rodSourceId, *robData);
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


  m_rodVer = rob.rod_version() & 0xffff;
  m_verCode = ((m_rodVer & 0xfff) << 4) | 1;
  m_rodRunNumber = rob.rod_run_no() & 0xffffff;


  if (sourceID != m_subDetectorID) {
    ATH_MSG_ERROR("Wrong subdetector source id for requested objects: " << m_rodSourceId);
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Treating crate " << rodCrate << " slink " << rodSlink);
  m_caloUserHeader = CaloUserHeader(*payload);
  if (!m_caloUserHeader.isValid()) {
    ATH_MSG_ERROR("Invalid or missing user header");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG(
      "Run number: " << MSG::dec << m_rodRunNumber << endmsg
          << "Version code: 0x" << MSG::hex << int(m_verCode) << MSG::dec
          << endmsg << "LUT triggered slice offset:  "
          << int(m_caloUserHeader.lut()) << endmsg
          << "FADC triggered slice offset: " << int(m_caloUserHeader.ppFadc())
          << endmsg << "FADC baseline lower bound:   "
          << int(m_caloUserHeader.ppLowerBound()));

  int indata = 0;
  uint8_t blockType = 0;
  int subBlock = 0;

  for (; payload != payloadEnd; ++payload) {
    if (CaloUserHeader::isValid(*payload) && (subBlock == 0)) {

    } else if (SubBlockHeader::isSubBlockHeader(*payload)) {
      indata = 0;
      CHECK(processPpmBlock_());

      m_ppLuts.clear();
      m_ppFadcs.clear();
      m_ppBlock.clear();

      blockType = (*payload >> 28) & 0xf;

      if ((blockType & 0xd) == 0xc) {
        m_subBlockHeader = SubBlockHeader(*payload);
        ATH_MSG_VERBOSE(
            "SubBlock version #" << int(m_subBlockHeader.version())
             << " format #" << int(m_subBlockHeader.format())
             << " seqNum (compVer) #" << int(m_subBlockHeader.seqNum())
             << " nslice1 #" << int(m_subBlockHeader.nSlice1())
             << " nslice2 #" << int(m_subBlockHeader.nSlice2())
        );
        subBlock = blockType & 0xe;
      } else if (blockType == (subBlock | 1)) {
        processSubBlockStatus_(m_subBlockHeader.crate(), m_subBlockHeader.module(), *payload);
        subBlock = 0;
      }
    } else {
      switch(m_subDetectorID){
      case eformat::TDAQ_CALO_PREPROC:
          CHECK(processPpmWord_(*payload, indata));
          break;
      default:
        break;
      }
      indata++;
    }
  }
  CHECK(processPpmBlock_());
  return StatusCode::SUCCESS;
}

StatusCode PpmByteStreamReadV1V2Tool::processPpmWord_(uint32_t word,
    int indata) {
  if ( (m_subBlockHeader.format() == 0)
      || (m_subBlockHeader.format() >= 2)
      || (m_verCode >= 0x41)) {
    m_ppBlock.push_back(word);
  } else if ((m_verCode == 0x21) || (m_verCode == 0x31)) {
    return processPpmStandardR3V1_(word, indata);
  } else {
    ATH_MSG_ERROR("Unsupported PPM version:format ("
      << m_verCode << ":" << m_subBlockHeader.format()
      <<") combination");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


StatusCode PpmByteStreamReadV1V2Tool::processPpmBlock_() {
  if (m_ppBlock.size() > 0) {
    m_ppPointer = 0;
    if (m_subBlockHeader.format() == 0) {
      StatusCode sc = processPpmNeutral_();
      m_ppBlock.clear();
      CHECK(sc);
      return sc;
    }

    if (m_verCode == 0x31) {
      StatusCode sc = processPpmCompressedR3V1_();
      m_ppBlock.clear();
      CHECK(sc);
      return sc;
    }

    if (m_verCode == 0x41 || m_verCode == 0x42) {
      StatusCode sc = processPpmBlockR4V1_();
      m_ppBlock.clear();
      CHECK(sc);
      return sc;
    }
  }

  if (m_ppLuts.size() > 0) {
    if (m_verCode == 0x21 || m_verCode == 0x31) {
      StatusCode sc = processPpmBlockR3V1_();
      m_ppLuts.clear();
      m_ppFadcs.clear();
      CHECK(sc);
      return sc;
    }
    ATH_MSG_ERROR("Unknown PPM subheader format '"
      << int(m_subBlockHeader.format())
      << "' for rob version '"
      << MSG::hex << int(m_verCode)
      << MSG::dec << "'" );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode PpmByteStreamReadV1V2Tool::processPpmNeutral_() {
  uint8_t numLut = m_subBlockHeader.nSlice1();
  uint8_t numFadc = m_subBlockHeader.nSlice2();
  uint8_t totSlice = 3 * numLut + numFadc;

  uint8_t channel = 0;
  for ( int asic = 0 ; asic < 4 ; ++asic ) {
    for ( int mcm = 0 ; mcm < 16 ; ++mcm ) {
      // ----------------------------------------------------------------------
      std::vector<uint32_t> rotated(totSlice);

      for ( uint8_t slice = 0 ; slice < totSlice ; ++slice ) {
        for ( uint8_t bit = 0 ; bit < 11 ; ++bit ) {
          if ( m_ppBlock[slice * 11 + asic * (11 * totSlice) + bit + 1] & (1 << mcm))
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
        m_subBlockHeader.crate(),
        m_subBlockHeader.module(),
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

StatusCode PpmByteStreamReadV1V2Tool::processPpmCompressedR3V1_() {
  uint8_t chan = 0;
  m_ppPointer = 0;
  m_ppMaxBit = 31 * m_ppBlock.size();
  try{
    while (chan < 64) {
      uint8_t present = 1;
      if (m_subBlockHeader.format() == 3) {
        present = getPpmBytestreamField_(1);
      }

      if (present == 1) {
        uint8_t lutVal = 0;
        uint8_t fmt = 6;
        uint8_t lutSat=0;
        uint8_t lutExt=0;
        uint8_t lutPeak=0;

        std::vector<uint16_t> adcVal = {0 , 0, 0, 0, 0};
        std::vector<uint8_t> adcExt = {0 , 0, 0, 0, 0};

        uint8_t minHeader = getPpmBytestreamField_(4);
        uint8_t minIndex = minHeader % 5;
        if (minHeader < 15) { // Formats 0-5
          if (minHeader < 10) { // Formats 0-1
            fmt = minHeader / 5;
          } else { // Formats 2-5
            fmt = 2 + getPpmBytestreamField_(2);
            uint8_t haveLut = getPpmBytestreamField_(1);
            if (fmt == 2) {
              if (haveLut == 1) {
                lutVal = getPpmBytestreamField_(3);
                lutPeak = 1; // Even if LutVal==0 it seems
              }
            } else {
              uint8_t haveExt = getPpmBytestreamField_(1);
              if (haveLut == 1) {
                lutVal = getPpmBytestreamField_(8);
                lutExt = getPpmBytestreamField_(1);
                lutSat = getPpmBytestreamField_(1);
                lutPeak = getPpmBytestreamField_(1);
              }

              if (haveExt == 1){
                for(uint8_t i = 0; i < 5; ++i) {
                  adcExt[i] = getPpmBytestreamField_(1);
                }
              } else {
                adcExt[2] = lutExt;
              }
            }
          }
          adcVal = getPpmAdcSamplesR3_(fmt, minIndex);
        } else {
          uint8_t haveAdc = getPpmBytestreamField_(1);
          if (haveAdc == 1) {
            uint16_t val = getPpmBytestreamField_(10);
            for(uint8_t i = 0; i < 5; ++i) {
                  adcVal[i] = val;
            }
          }
        }
        // Add Trigger Tower
        //std::vector<uint8_t> luts = {lutVal};
        CHECK(addTriggerTowerV1_(
          m_subBlockHeader.crate(),
          m_subBlockHeader.module(),
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
      m_errorTool->rodError(m_rodSourceId, L1CaloSubBlock::UNPACK_EXCESS_DATA);
  }
  return StatusCode::SUCCESS;
}

std::vector<uint16_t> PpmByteStreamReadV1V2Tool::getPpmAdcSamplesR3_(
  uint8_t format, uint8_t minIndex) {

  std::vector<uint16_t> adc = {0, 0, 0, 0, 0};
  uint8_t minAdc = 0;

  for(uint8_t i = 0; i <5; ++i) {
    uint8_t longField = 0;
    uint8_t numBits = 0;
    if (format > 2) {
      longField = getPpmBytestreamField_(1);
      numBits = longField == 0? 4: (format * 2);
    } else {
      numBits = i == 0? 4: (format + 2);
    }

    if (i == 0) {
      minAdc = getPpmBytestreamField_(numBits);
      if (longField == 0) {
        minAdc += m_caloUserHeader.ppLowerBound();
      }
    } else {
        adc[i] = minAdc + getPpmBytestreamField_(numBits);
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



StatusCode PpmByteStreamReadV1V2Tool::processPpmStandardR3V1_(uint32_t word,
    int inData){

  StatusCode sc = StatusCode::SUCCESS;
  if (m_subBlockHeader.seqNum() == 63) { // Error block
    ATH_MSG_DEBUG("Error PPM subblock");
    //TODO: errorTool
  } else {
    const uint8_t numAdc = m_subBlockHeader.nSlice2();
    const uint8_t numLut = m_subBlockHeader.nSlice1();
    const uint8_t nTotal = numAdc + numLut;
    const uint8_t wordsPerBlock = 8; // 16 towers (4 MCMs) / 2 per word
    const uint8_t iBlk =  inData / wordsPerBlock;
    uint8_t iChan =  m_subBlockHeader.seqNum() + 2 * (inData % wordsPerBlock);

    if (iBlk < numLut) { // First all LUT values
      for(uint8_t i = 0; i < 2; ++i) {
        uint16_t subword = (word >> 16 * i) & 0x7ff;
        m_ppLuts[iChan].push_back(subword);
        iChan++;
      }
    } else if (iBlk < nTotal) { // Next all FADC values
      for(uint8_t i = 0; i < 2; ++i) {
        uint16_t subword = (word >> (16 * i)) & 0x7ff;
        m_ppFadcs[iChan].push_back(subword);
        iChan++;
      }

    } else{
      ATH_MSG_WARNING("Error decoding Ppm word (run1)");
      sc = StatusCode::FAILURE;
    }

  }
  return sc;
}

StatusCode PpmByteStreamReadV1V2Tool::processPpmBlockR4V1_() {
  if (m_subBlockHeader.format() == 1) {
    CHECK(processPpmStandardR4V1_());
    return StatusCode::SUCCESS;
  } else if (m_subBlockHeader.format() >= 2) {
    CHECK(processPpmCompressedR4V1_());
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode PpmByteStreamReadV1V2Tool::processPpmCompressedR4V1_() {
  m_ppPointer = 0;
  m_ppMaxBit = 31 * m_ppBlock.size();

  uint8_t numAdc = m_subBlockHeader.nSlice2();
  uint8_t numLut = m_subBlockHeader.nSlice1();
  int16_t pedCorBase = -20;


  // for(size_t i = 0; i < m_ppBlock.size(); ++i) {
  //   std::bitset<32> x(m_ppBlock[i]);
  //   std::cout << i << " " << x << std::endl;
  // }

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

      if (m_subBlockHeader.format() == 3) {
        present = getPpmBytestreamField_(1);
      }
      if (present == 1) {
        interpretPpmHeaderR4V1_(numAdc, encoding, minIndex);
        CHECK((encoding != -1) && (minIndex != -1));
        // First get the LIT related quantities
        if (encoding < 3) {
          // Get the peal finder bits
          for(uint i=0; i < numLut; ++i) {
            lcpPeak[i] = getPpmBytestreamField_(1);
          }
          // Get Sat80 low bits
          if (encoding > 0) {
            for (uint8_t i = 0; i < numLut; ++i) {
              ljeLow[i] = getPpmBytestreamField_(1);
            }
          }
          // Get LutCP and LutJEP values (these are
          // only present if the peak finder is set).
          if (encoding == 2) {
            for (uint8_t i = 0; i < numLut; ++i) {
              if (lcpPeak[i] == 1) {
                lcpVal[i] = getPpmBytestreamField_(4);
              }
            }
            for(uint8_t i = 0; i < numLut; ++i) {
              if (lcpPeak[i] == 1){
                ljeVal[i] = getPpmBytestreamField_(3);
              }
            }
          }
        } else if (encoding < 6) {
          // Get LUT presence flag for each LUT slice.
          for(uint8_t i = 0; i < numLut; ++i){
            haveLut[i] = getPpmBytestreamField_(1);
          }

          // Get external BCID bits (if block is present).
          uint8_t haveExt = getPpmBytestreamField_(1);

          if (haveExt == 1) {
            for (uint8_t i = 0; i < numAdc; ++i) {
              adcExt[i] = getPpmBytestreamField_(1);
            }
          }

          for(uint8_t i = 0; i < numLut; ++i){
            if (haveLut[i] == 1) {
              lcpVal[i] = getPpmBytestreamField_(8);
              lcpExt[i] = getPpmBytestreamField_(1);
              lcpSat[i] = getPpmBytestreamField_(1);
              lcpPeak[i] = getPpmBytestreamField_(1);
            }
          }
          // Get JEP LUT values and corresponding bits.
          for(uint8_t i = 0; i < numLut; ++i){
            if (haveLut[i] == 1) {
              ljeVal[i] = getPpmBytestreamField_(8);
              ljeLow[i] = getPpmBytestreamField_(1);
              ljeHigh[i] = getPpmBytestreamField_(1);
              ljeRes[i] = getPpmBytestreamField_(1);
            }
          }

        }

      }
       // Next get the ADC related quantities (all encodings).
      adcVal = getPpmAdcSamplesR4_(encoding, minIndex);
      // Finally get the pedestal correction.
      if ((encoding < 3) || (encoding == 6)) {
        for (uint8_t i = 0; i < numLut; ++i)
        {
          pedCor[i] = getPpmBytestreamField_(6) + pedCorBase;
          if (m_subBlockHeader.compVer() > 0) {
            pedEn[i] = 1;
          }
        }
      } else {
        // At the moment there is an enabled bit for every LUT slice
        // (even though its really a global flag).
        // The correction values is a twos complement signed value.
        for (uint8_t i = 0; i < numLut; ++i)
        {
          uint16_t val = getPpmBytestreamField_(10);
          pedCor[i] = ::pedCorrection(val);
          pedEn[i] = getPpmBytestreamField_(1);
        }
      }

    for(uint8_t i=0; i < numLut; ++i){
      lcpBcidVec[i] = uint8_t((lcpPeak[i] << 2) | (lcpSat[i] << 1) | lcpExt[i]);
      ljeSat80Vec[i] = uint8_t((ljeRes[i] << 2) | (ljeHigh[i] << 1) | ljeLow[i]);
    }
    CHECK(addTriggerTowerV2_(m_subBlockHeader.crate(), m_subBlockHeader.module(),
                             chan,
                             std::move(lcpVal), std::move(lcpBcidVec),
                             std::move(ljeVal), std::move(ljeSat80Vec),
                             std::move(adcVal), std::move(adcExt),
                             std::move(pedCor), std::move(pedEn)));
    } // for
  } catch (const std::out_of_range& ex) {
      ATH_MSG_WARNING("Excess Data in Sub-block");
      m_errorTool->rodError(m_rodSourceId, L1CaloSubBlock::UNPACK_EXCESS_DATA);
  }
  //Check status workd
  return StatusCode::SUCCESS;

}

void PpmByteStreamReadV1V2Tool::interpretPpmHeaderR4V1_(uint8_t numAdc,
  int8_t& encoding, int8_t& minIndex) {
 uint8_t minHeader = 0;

  if (numAdc == 5) {
    minHeader = getPpmBytestreamField_(4);

    minIndex = minHeader % 5;
    if (minHeader < 15){ // Encodings 0-5
      if (minHeader < 10) {
        encoding = minHeader / 5;
      } else {
        encoding = 2 + getPpmBytestreamField_(2);
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
        minHeader = getPpmBytestreamField_(numBits);
        uint8_t encValue = fieldSize - 1;
        if (minHeader == encValue) { // Encoding 6
          encoding = 6;
          minIndex = 0;
        } else {
          minHeader += getPpmBytestreamField_(2) << numBits;
          minIndex = minHeader % fieldSize;
          encValue = 3 * fieldSize;

          if (minHeader < encValue) { // Encodings 0-2
            encoding = minHeader / fieldSize;
          } else {
            encoding = 3 + getPpmBytestreamField_(2);
          }
        }
      }
  }
}

std::vector<uint16_t> PpmByteStreamReadV1V2Tool::getPpmAdcSamplesR4_(
  uint8_t encoding, uint8_t minIndex) {
  uint8_t numAdc = m_subBlockHeader.nSlice2();

  if (encoding == 6) {
    uint16_t val = getPpmBytestreamField_(6);
    return std::vector<uint16_t>(numAdc, val);
  } else if ( encoding < 3) {
    std::vector<uint16_t> adc(numAdc, 0);
    uint16_t minAdc = getPpmBytestreamField_(5) + m_caloUserHeader.ppLowerBound();
    adc[minIndex] = minAdc;
    for(uint8_t i = 1; i < numAdc; ++i) {
      adc[i == minIndex? 0: i] = getPpmBytestreamField_(encoding + 2) + minAdc;
    }
    return adc;
  } else {
    std::vector<uint16_t> adc(numAdc, 0);
    uint16_t minAdc = getPpmBytestreamField_(1)
                      ? getPpmBytestreamField_(encoding * 2)
                      : (getPpmBytestreamField_(5) +
                          m_caloUserHeader.ppLowerBound());

    adc[minIndex] = minAdc;
    for (uint8_t i = 1; i < numAdc; ++i) {
      adc[minIndex == i? 0: i] = getPpmBytestreamField_(
                              getPpmBytestreamField_(1)? encoding * 2: 4
                            ) + minAdc;
    }
   return adc;
  }
}

StatusCode PpmByteStreamReadV1V2Tool::processPpmBlockR3V1_() {
  if (m_subBlockHeader.format() == 1) {
    CHECK(processPpmStandardR3V1_());
    return StatusCode::SUCCESS;
  } else if (m_subBlockHeader.format() >= 2) {
    // TODO: convert compressed
    return StatusCode::FAILURE;
  }
  return StatusCode::FAILURE;
}

StatusCode PpmByteStreamReadV1V2Tool::processPpmStandardR4V1_() {
  uint8_t numAdc = m_subBlockHeader.nSlice2();
  uint8_t numLut = m_subBlockHeader.nSlice1();
  uint8_t crate = m_subBlockHeader.crate();
  uint8_t module = m_subBlockHeader.module();


  m_ppPointer = 0;
  m_ppMaxBit = 31 * m_ppBlock.size();

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
        lcpVal.push_back(getPpmBytestreamField_(8));
        lcpBcidVec.push_back(getPpmBytestreamField_(3));
      }

      for (int i = 0; i < numLut; ++i) {
        ljeVal.push_back(getPpmBytestreamField_(8));
        ljeSat80Vec.push_back(getPpmBytestreamField_(3));
      }

      for (int i = 0; i < numAdc; ++i) {
        adcVal.push_back(getPpmBytestreamField_(10));
        adcExt.push_back(getPpmBytestreamField_(1));
      }

      for (int i = 0; i < numLut; ++i) {
        uint16_t pc = getPpmBytestreamField_(10);
        pedCor.push_back(pedCorrection(pc));
        pedEn.push_back(getPpmBytestreamField_(1));
      }
    } catch (const std::out_of_range& ex) {
      ATH_MSG_WARNING("Excess Data in Sub-block");
      m_errorTool->rodError(m_rodSourceId, L1CaloSubBlock::UNPACK_EXCESS_DATA);
    }
    CHECK(
        addTriggerTowerV2_(crate, module, chan,
                           std::move(lcpVal), std::move(lcpBcidVec),
                           std::move(ljeVal), std::move(ljeSat80Vec),
                           std::move(adcVal), std::move(adcExt),
                           std::move(pedCor), std::move(pedEn)));
  }

  return StatusCode::SUCCESS;
}

StatusCode PpmByteStreamReadV1V2Tool::processPpmStandardR3V1_() {
    for(auto lut : m_ppLuts) {
      CHECK(addTriggerTowerV1_(
        m_subBlockHeader.crate(),
        m_subBlockHeader.module(),
        lut.first,
        lut.second,
        m_ppFadcs[lut.first]));
    }
    return StatusCode::SUCCESS;
}

void PpmByteStreamReadV1V2Tool::processSubBlockStatus_(uint8_t crate, uint8_t module, uint32_t payload){
  LVL1::DataError errorBits(0);
  errorBits.set(LVL1::DataError::SubStatusWord, payload);

  const uint32_t error = errorBits.error();
  int curr = m_triggerTowers->size() - 1;
  for(int i=0; i < s_channels; ++i){
    if (curr < 0){
      break;
    }
    auto tt = (*m_triggerTowers)[curr--];
    if (tt->coolId() >> 16 & crateModuleMask(crate, module)){
      tt->setErrorWord(error);
    }else{
      break;
    }
  }
}


StatusCode PpmByteStreamReadV1V2Tool::addTriggerTowerV2_(
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
    std::vector<uint8_t>&& pedEn) {

  uint32_t coolId = ::coolId(crate, module, channel);

  int layer = 0;
  double eta = 0.;
  double phi = 0.;

  bool isData = m_ppmMaps->mapping(crate, module, channel, eta, phi, layer);

  if (!isData && !m_ppmIsRetSpare && !m_ppmIsRetMuon){
    return StatusCode::SUCCESS;
  }

  if (!isData) {
    const int pin  = channel % 16;
    const int asic = channel / 16;
    eta = 16 * crate + module;
    phi = 4 * pin + asic;
  }

  xAOD::TriggerTower* tt = new xAOD::TriggerTower();
  m_triggerTowers->push_back(tt);

  tt->initialize(coolId, eta, phi,
                 std::move(lcpVal), std::move(ljeVal),
                 std::move(pedCor), std::move(pedEn),
                 std::move(lcpBcidVec), std::move(adcVal),
                 std::move(adcExt), std::move(ljeSat80Vec),
                 0, m_caloUserHeader.lut(),
      m_caloUserHeader.ppFadc());
  return StatusCode::SUCCESS;
}

StatusCode PpmByteStreamReadV1V2Tool::addTriggerTowerV1_(
    uint8_t crate,
    uint8_t module,
    uint8_t channel,
    std::vector<uint8_t>&& luts,
    std::vector<uint8_t>&& lcpBcidVec,
    std::vector<uint16_t>&& fadc,
    std::vector<uint8_t>&& bcidExt
  ) {

    std::vector<uint8_t> ljeSat80Vec;

    std::vector<int16_t> pedCor;
    std::vector<uint8_t> pedEn;

   CHECK(addTriggerTowerV2_(crate, module, channel,
                            std::move(luts), std::move(lcpBcidVec),
                            std::move(luts) , std::move(ljeSat80Vec),
                            std::move(fadc), std::move(bcidExt),
                            std::move(pedCor), std::move(pedEn))
   );

   return StatusCode::SUCCESS;
}

StatusCode PpmByteStreamReadV1V2Tool::addTriggerTowerV1_(
    uint8_t crate,
    uint8_t module,
    uint8_t channel,
    const std::vector<uint16_t>& luts,
    const std::vector<uint16_t>& fadc
  ) {

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

   CHECK(addTriggerTowerV1_(crate, module, channel,
                            std::move(lcpVal), std::move(lcpBcidVec),
                            std::move(adcVal), std::move(adcExt)));

   return StatusCode::SUCCESS;
}


// Return reference to vector with all possible Source Identifiers

const std::vector<uint32_t>& PpmByteStreamReadV1V2Tool::ppmSourceIDs(
  const std::string& sgKey) {

  const int crates = 8;
  m_ppmIsRetMuon = false;
  m_ppmIsRetSpare =  false;

  if (sgKey.find("Muon") != std::string::npos) {
    m_ppmIsRetMuon = true;
  } else if (sgKey.find("Spare") != std::string::npos) {
    m_ppmIsRetSpare = true;
  }

  if (m_ppmSourceIDs.empty()) {
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

  if (m_ppmIsRetSpare) {
    return m_ppmSourceIDsSpare;
  }

  if (m_ppmIsRetMuon) {
    return m_ppmSourceIDsMuon;
  }

  return m_ppmSourceIDs;

}



uint32_t PpmByteStreamReadV1V2Tool::getPpmBytestreamField_(const uint8_t numBits) {
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
