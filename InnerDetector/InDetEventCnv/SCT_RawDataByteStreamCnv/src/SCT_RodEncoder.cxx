/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/** Implementation of SCT_RodEncoder class */
#include "SCT_RodEncoder.h" 

///InDet
#include "InDetIdentifier/SCT_ID.h"

///Athena
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetRawData/SCT_RDORawData.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

namespace {
  int rodLinkFromOnlineId(const uint32_t id) {
    const SCT_OnlineId online{id};
    const uint32_t fibre{online.fibre()};
    const int formatter{static_cast<int>((fibre/12) & 0x7)};
    const int linknum{static_cast<int>((fibre - (formatter*12)) & 0xF)};
    const int rodLink{(formatter << 4) | linknum};
    return rodLink;
  } 
  bool isOdd(const int someNumber){
    return static_cast<bool>(someNumber & 1);
  }
 
  bool isEven(const int someNumber) {
    return not isOdd(someNumber);
  }
 
  bool swappedCable(const int moduleSide, const int linkNumber) {
    return isOdd(linkNumber) ? (moduleSide==0) : (moduleSide==1);
  }
}//end of anon namespace


SCT_RodEncoder::SCT_RodEncoder
(const std::string& type, const std::string& name, const IInterface* parent) : 
  base_class(type, name, parent),
  m_sctID{nullptr},
  m_swapModuleId{} {
  }

StatusCode SCT_RodEncoder::initialize() {
  //prob. dont need this next line now:
  //ATH_CHECK( AlgTool::initialize());
  ATH_MSG_DEBUG("SCT_RodEncoder::initialize()");
  
  /** Retrieve cabling tool */
  ATH_CHECK(m_cabling.retrieve());
  ATH_MSG_DEBUG("Retrieved tool " << m_cabling);

  ATH_CHECK(detStore()->retrieve(m_sctID, "SCT_ID"));
  ATH_CHECK(m_bsErrTool.retrieve());

  // See if strip numbers go from 0 to 767 or vice versa for all the wafers.
  // swapPhiReadoutDirection will not change during a run.
  // Since this is access to SiDetectorElement during initialization,
  // condition object of SiDetectorElementCollection is not accessible.
  // SCT_DetectorManager has to be used.
  const InDetDD::SCT_DetectorManager* sctDetManager{nullptr};
  ATH_CHECK(detStore()->retrieve(sctDetManager, "SCT"));

  const InDetDD::SiDetectorElementCollection* sctDetElementColl{sctDetManager->getDetectorElementCollection()};
  for (const InDetDD::SiDetectorElement* sctDetElement: *sctDetElementColl) {
    if (sctDetElement->swapPhiReadoutDirection()) {
      m_swapModuleId.insert(sctDetElement->identify());
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode SCT_RodEncoder::finalize() {
  return StatusCode::SUCCESS;
}

///=========================================================================
///  fillROD convert SCT RDO to a vector of 32bit words
///=========================================================================

void SCT_RodEncoder::fillROD(std::vector<uint32_t>& vec32data, const uint32_t& robid, 
                             vRDOs_t& vecRDOs) const {
  
  /** retrieve errors from SCT_ByteStreamErrorsSvc */

  const std::set<IdentifierHash>* timeOutErrors{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::TimeOutError)};
  const std::set<IdentifierHash>* l1idErrors{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::LVL1IDError)};
  const std::set<IdentifierHash>* bcidErrors{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::BCIDError)};
  const std::set<IdentifierHash>* preambleErrors{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::PreambleError)};
  const std::set<IdentifierHash>* formatterErrors{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::FormatterError)};
  const std::set<IdentifierHash>* trailerErrors{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::TrailerError)};
  const std::set<IdentifierHash>* headertrailerErrors{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::HeaderTrailerLimitError)};
  const std::set<IdentifierHash>* traileroverflowErrors{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::TrailerOverflowError)};
  const std::set<IdentifierHash>* abcdErrors{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::ABCDError)};
  const std::set<IdentifierHash>* rawErrors{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::RawError)};

  std::vector<int> vecTimeBins;
  std::vector<uint16_t> vec16data;
  int strip{0};
  int timeBin{0};
  int groupsize{0};
  
  /** loop over errors here - just add headers (w/ errors), trailers (w/errors), 
   * and raw and abcd errors */

  addHeadersWithErrors(robid, timeOutErrors, TIMEOUT_ERR, vec16data);
  addHeadersWithErrors(robid, l1idErrors, L1_ERR, vec16data);
  addHeadersWithErrors(robid, bcidErrors, BCID_ERR, vec16data);
  addHeadersWithErrors(robid, preambleErrors, PREAMBLE_ERR, vec16data);
  addHeadersWithErrors(robid, formatterErrors, FORMATTER_ERR, vec16data);
  //
  addTrailersWithErrors(robid, trailerErrors, TRAILER_ERR, vec16data);
  addTrailersWithErrors(robid, headertrailerErrors, HEADER_TRAILER_ERR, vec16data);
  addTrailersWithErrors(robid, traileroverflowErrors, TRAILER_OVFLW_ERR, vec16data);
  //
  addSpecificErrors(robid, abcdErrors, ABCD_ERR, vec16data);
  addSpecificErrors(robid, rawErrors, RAWDATA_ERR, vec16data);
  
  std::vector<bool> vec_isDuplicated(vecRDOs.size(), false);
  for (unsigned int iRdo1{0}; iRdo1<vecRDOs.size(); iRdo1++) {
    const RDO* rawdata1{vecRDOs.at(iRdo1)};
    if (rawdata1==nullptr) {
      ATH_MSG_ERROR("RDO pointer is NULL. skipping this hit.");
      vec_isDuplicated.at(iRdo1) = true;
      continue;
    }

    // Check if there is another RDO with the same first strip
    for (unsigned int iRdo2{0}; iRdo2<iRdo1; iRdo2++) {
      const RDO* rawdata2{vecRDOs.at(iRdo2)};
      if (vec_isDuplicated.at(iRdo2)) continue;

      if (rawdata1->identify()==rawdata2->identify()) {
        // Keep RDO with larger cluster size. If cluster sizes are the same, keep the first one.
        if (rawdata1->getGroupSize()>=rawdata2->getGroupSize()) {
          vec_isDuplicated.at(iRdo2) = true;
        } else {
          vec_isDuplicated.at(iRdo1)  = true;
        }
        break;
      }
    }
  }

  uint32_t lastHeader{0};
  bool firstInRod{true};
  uint16_t lastTrailer{0};
  for (unsigned int iRdo{0}; iRdo<vecRDOs.size(); iRdo++) {
    const RDO* rawdata{vecRDOs.at(iRdo)};
    if (vec_isDuplicated.at(iRdo)) continue;

    const uint16_t header{this->getHeaderUsingRDO(rawdata)};
    if (header != lastHeader) {
      if (! firstInRod) {
        vec16data.push_back(lastTrailer);
      }
      firstInRod = false;
      vec16data.push_back(header);
      lastHeader = header;
      lastTrailer = getTrailer(0);
    }
    if (m_condensed.value()) { /** Condensed mode */
      strip = getStrip(rawdata);
      groupsize = rawdata->getGroupSize();
      if (groupsize == 1) { /** For single hit */
        const int constGroupSize{1};
        const int constStrip{strip};
        encodeData(vecTimeBins, vec16data, rawdata, constGroupSize, constStrip);
      }
      /** Sim rdo could have groupe size > 1 then I need to split 
       * them 2 by 2 to built the condensed BS data */
      else { /** Encoding in condensed BS paired data from groupe size > 1 */
        const int chipFirst{strip/128};
        const int chipLast{(strip+groupsize-1)/128};

        for (int chip{chipFirst}; chip<=chipLast; chip++) {
          int tmpGroupsize = 0;
          if (chipFirst==chipLast) tmpGroupsize = groupsize; // In case of one chip
          else if (chip==chipLast) tmpGroupsize = strip+groupsize-chip*128; // In case of last chip
          else if (chip==chipFirst) tmpGroupsize = (chip+1)*128-strip; // In case of first chip
          else tmpGroupsize = 128; // In case of middle chip
          const int tmpStrip1{chip==chipFirst ? strip : 128*chip};

          const int n_pairedRdo{tmpGroupsize/2};
          for (int i =0; i<n_pairedRdo; i++) {
            const int constGroupSize{2};
            const int constStrip{tmpStrip1+ (2*i)};
            encodeData(vecTimeBins, vec16data, rawdata, constGroupSize, constStrip);
          } 
          if ((tmpGroupsize != 0) and isOdd(tmpGroupsize)) {/** The last hit from a cluster with odd group size */
            const int constGroupSize{1};
            const int constStrip{tmpStrip1+ (tmpGroupsize - 1)};
            encodeData(vecTimeBins, vec16data, rawdata, constGroupSize, constStrip);
          }  
        }
      }  
      
    } /** end of condensed Mode */
    
    else {/** Expanded mode */
      
      vecTimeBins.clear();
      const RDO* rawdata{vecRDOs.at(iRdo)};
      strip = getStrip(rawdata);
      timeBin = getTimeBin(rawdata);
      groupsize = rawdata->getGroupSize();
      
      for (int t{0}; t < groupsize; t++) {
        vecTimeBins.push_back(timeBin);
        strip++;
      }
      const int constStrip{getStrip(rawdata)};
      const int constGroupSize{rawdata->getGroupSize()};
      encodeData(vecTimeBins, vec16data, rawdata, constGroupSize, constStrip);
      
    }  // End of (else) Expanded
  } //end of RDO loop
  if (!firstInRod and lastTrailer!=0) {
    vec16data.push_back(lastTrailer);
  }
  /** 16 bits TO 32 bits and pack into 32 bit vectors */
  packFragments(vec16data,vec32data);
  
  return;
  
} // end of fillROD(...)

void SCT_RodEncoder::addHeadersWithErrors(const uint32_t& robid, const std::set<IdentifierHash>* errors, const ErrorWords& errType, std::vector<uint16_t>& vec16data) const {
  for (const IdentifierHash& linkHash: *errors) {
    const uint32_t errRobId{m_cabling->getRobIdFromHash(linkHash)};
    if (errRobId == robid) {
      const uint16_t header{getHeaderUsingHash(linkHash, errType)};
      vec16data.push_back(header);
      const uint16_t trailer{getTrailer(NULL_TRAILER_ERR)};
      vec16data.push_back(trailer);
    }
  }
} 

//
void SCT_RodEncoder::addTrailersWithErrors(const uint32_t& robid, const std::set<IdentifierHash>* errors, const ErrorWords& errType, std::vector<uint16_t>& vec16data) const {
  for (const IdentifierHash& linkHash: *errors) {
    const uint32_t errRobId{m_cabling->getRobIdFromHash(linkHash)};
    if (errRobId == robid) {
      const uint16_t header{getHeaderUsingHash(linkHash, NULL_HEADER_ERR)};
      const uint16_t trailer{getTrailer(errType)};
      vec16data.push_back(header);
      vec16data.push_back(trailer);
    }
  }
}

void SCT_RodEncoder::addSpecificErrors(const uint32_t& robid, const std::set<IdentifierHash>* errors, const ErrorWords& errType, std::vector<uint16_t>& vec16data) const {
  for (const IdentifierHash& linkHash: *errors) {
    const uint32_t errRobId{m_cabling->getRobIdFromHash(linkHash)};
    if (errRobId == robid) {
      const uint16_t header{getHeaderUsingHash(linkHash, NULL_HEADER_ERR)};
      const uint16_t trailer{getTrailer(NULL_TRAILER_ERR)};
      vec16data.push_back(header);
      vec16data.push_back(errType);
      vec16data.push_back(trailer);
    }
  }
}

///=========================================================================
/// Encode Data function
///========================================================================= 

void SCT_RodEncoder::encodeData(std::vector<int>& vecTimeBins, std::vector<uint16_t>& vec16Words, const RDO *rdo, const int& groupSize, const int& strip) const {
  
  ///-------------------------------------------------------------------------------------
  /// Check if the strip has to be swapped (swap phi readout direction)
  ///-------------------------------------------------------------------------------------
  
  const int encodedSide{side(rdo) << 14};
  
  const Identifier idColl{offlineId(rdo)};
  int tmpStrip{strip};
  if (m_swapModuleId.find(idColl)!= m_swapModuleId.end()) {
    tmpStrip= 767 - tmpStrip;
    tmpStrip= tmpStrip-(groupSize-1);
  }
  
  const int chipNum{((tmpStrip/128) & 0x7) << 11};
  const int clustBaseAddr{((tmpStrip-(chipNum*128)) & 0x7F) << 4};
  int timeBin{0};
  int firstHitErr{0 << 2};
  int secondHitErr{0 << 3};
  
  const SCT3_RawData* rdoCosmic{dynamic_cast<const SCT3_RawData*>(rdo)};
  if (rdoCosmic != nullptr) {
    timeBin = getTimeBin(rdoCosmic);
    firstHitErr = ((rdoCosmic)->FirstHitError()) << 2;
    secondHitErr = ((rdoCosmic)->SecondHitError()) << 3;
  }
  
  ///-------------------------------------------------------------------------------------
  /// Condensed
  ///-------------------------------------------------------------------------------------
  if (m_condensed.value()) {/** single Hit on condensed Mode */
    if (groupSize == 1) {/** Group size = 1 */
      const uint16_t HitCondSingle{static_cast<uint16_t>(0x8000 | encodedSide | chipNum | clustBaseAddr | firstHitErr)};
      vec16Words.push_back(HitCondSingle);
    } else if (groupSize == 2) {/** paired strip Hits on condensed Mode */
      const uint16_t HitCondPaired{static_cast<uint16_t>(0x8001 | encodedSide | chipNum | clustBaseAddr | secondHitErr | firstHitErr)};
      vec16Words.push_back(HitCondPaired);
    }    
  }  /// end of condensed
  ///-------------------------------------------------------------------------------------
  /// Expanded
  ///-------------------------------------------------------------------------------------
  else {
    const int numEven{static_cast<int>((vecTimeBins.size() - 1)/2)};
    /** First hit */
    const uint16_t HitExpFirst{static_cast<uint16_t>(0x8000 | encodedSide | chipNum | clustBaseAddr | timeBin)};
    vec16Words.push_back(HitExpFirst);
    /** Even consecutive strips to the first one 1DDD 1st consec strip 1DDD 2nd consec strip */
    for (int i=1; i<=numEven; i++) {
      const uint16_t HitExpEven{static_cast<uint16_t>(0x8088 | ((vecTimeBins[(2*i-1)] & 0xF) << 4) | (vecTimeBins[2*i] & 0xF))};
      vec16Words.push_back(HitExpEven);
    }
    /** Last bin of the Odd next hits */
    if ((not vecTimeBins.empty() ) and isEven(vecTimeBins.size())) {
      const uint16_t HitExpLast{static_cast<uint16_t>(0x8008 | (vecTimeBins[vecTimeBins.size()-1] & 0xF))};
      vec16Words.push_back(HitExpLast);
    }  
  } /** end of expanded */
  
  return;
} /** end of encodeData */

///=========================================================================
/// Converting the 16 bit vector v16 to 32 bit words v32
///=========================================================================
void SCT_RodEncoder::packFragments(std::vector<uint16_t>& vec16Words, std::vector<uint32_t>& vec32Words) const {
  int num16words{static_cast<int>(vec16Words.size())};
  if (isOdd(num16words)) {
    /** just add an additional 16bit words to make even size v16 to in the 32 bits word 0x40004000 */
    vec16Words.push_back(0x4000);
    num16words++;
  }
  /** now merge 2 consecutive 16 bit words in 32 bit words */
  const unsigned short int numWords{2};
  const unsigned short int position[numWords]{0, 16};
  unsigned short int arr16words[numWords]{0, 0};
  for (int i{0}; i<num16words; i += numWords) {
    arr16words[i%numWords]     = vec16Words[i+1];
    arr16words[(i+1)%numWords] = vec16Words[i];
    const uint32_t uint32word{set32bits(arr16words, position, numWords)};
    vec32Words.push_back(uint32word);
#ifdef SCT_DEBUG
    ATH_MSG_INFO("SCT encoder -> PackFragments: Output rod 0x"<<std::hex<<uint32word<<std::dec);
#endif
  }
  
  return;
}

///=========================================================================
/// set the 16 bit word v16 to 32 bit words v32
///=========================================================================

uint32_t SCT_RodEncoder::set32bits(const unsigned short int* arr16Words, const unsigned short int* position, const unsigned short int& numWords) const {
  uint32_t uint32Word{0};
  uint32_t pos{0};
  uint32_t uint16Word{0};
  for (uint16_t i{0}; i<numWords; i++) {
    uint16Word = static_cast<uint32_t>(*(arr16Words+i));
    pos = static_cast<uint32_t>(*(position+i));
    uint32Word |= (uint16Word<<pos);
  } 
  return uint32Word;
}

///=========================================================================
/// Link and Side Numbers 
///=========================================================================
/** Strip number */ 
int SCT_RodEncoder::getStrip(const RDO* rdo) const {
  const Identifier rdoId{rdo->identify()};
  return m_sctID->strip(rdoId);
}

/** RDO ID */
Identifier SCT_RodEncoder::offlineId(const RDO* rdo) const {
  const Identifier rdoId{rdo->identify()};
  return m_sctID->wafer_id(rdoId);
}

/** ROD online ID */
uint32_t SCT_RodEncoder::onlineId(const RDO* rdo) const {
  const Identifier waferId{offlineId(rdo)};
  const IdentifierHash offlineIdHash{m_sctID->wafer_hash(waferId)};
  return (const uint32_t) m_cabling->getOnlineIdFromHash(offlineIdHash);
}

/** ROD Link Number In the ROD header data */
int SCT_RodEncoder::getRODLink(const RDO* rdo) const {
  return rodLinkFromOnlineId(onlineId(rdo));
}

/** Side Info */
int SCT_RodEncoder::side(const RDO* rdo) const {
  const Identifier rdoId{rdo->identify()};
  int sctSide{m_sctID->side(rdoId)};
  /** see if we need to swap sides due to cabling weirdness */
  const int linkNum{getRODLink(rdo) & 0xF};
  if (swappedCable(sctSide,linkNum)) sctSide = 1-sctSide;
  return sctSide;
}

/** Time Bin Info */
int SCT_RodEncoder::getTimeBin(const RDO* rdo) const {
  int timeBin{0};
  const SCT3_RawData* rdoCosmic{dynamic_cast<const SCT3_RawData*>(rdo)};
  if (rdoCosmic != 0) timeBin = rdoCosmic->getTimeBin();
  return timeBin;
}

///-------------------------------------------------------------------------------------
/// Link header
///-------------------------------------------------------------------------------------

uint16_t 
SCT_RodEncoder::getHeaderUsingRDO(const RDO* rdo) const {
  const int rodLink{getRODLink(rdo)};
  const uint16_t LinkHeader{static_cast<uint16_t>(0x2000 | (m_condensed.value() << 8) | rodLink)};
  return LinkHeader;
}

uint16_t 
SCT_RodEncoder::getHeaderUsingHash(const IdentifierHash& linkHash, const int& errorWord) const {
  const int rodLink{rodLinkFromOnlineId(m_cabling->getOnlineIdFromHash(linkHash))};
  const uint16_t LinkHeader{static_cast<uint16_t>(0x2000 | errorWord | (m_condensed.value() << 8) | rodLink)};
  return LinkHeader;
}

///-------------------------------------------------------------------------------------
/// Link trailer
///-------------------------------------------------------------------------------------
uint16_t 
SCT_RodEncoder::getTrailer(const int& errorword) const {
  const uint16_t LinkTrailer{static_cast<uint16_t>(0x4000 | errorword)};
  return LinkTrailer;
}
