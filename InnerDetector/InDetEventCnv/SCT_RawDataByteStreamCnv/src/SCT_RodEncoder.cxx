/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_RodEncoder.h" 

#include "SCT_Cabling/ISCT_CablingTool.h"

#include "InDetRawData/SCT_RDORawData.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

namespace { // Anonymous namespace
  int rodLinkFromOnlineID(const SCT_OnlineId onlineID)
  {
    const uint32_t fibre{onlineID.fibre()};
    const int formatter{static_cast<int>((fibre/12) & 0x7)};
    const int linkNum{static_cast<int>((fibre - (formatter*12)) & 0xF)};
    const int rodLink{(formatter << 4) | linkNum};
    return rodLink;
  } 
  bool isOdd(const int someNumber)
  {
    return static_cast<bool>(someNumber & 1);
  }
 
  bool isEven(const int someNumber) 
  {
    return not isOdd(someNumber);
  }
 
  bool swappedCable(const int moduleSide, const int linkNumber) 
  {
    return isOdd(linkNumber) ? (moduleSide==0) : (moduleSide==1);
  }
} // End of anonymous namespace

// Constructor

SCT_RodEncoder::SCT_RodEncoder(const std::string& type, const std::string& name, 
                               const IInterface* parent) : 
  base_class(type, name, parent),
  m_sctID{nullptr},
  m_swapModuleID{} 
{
}

// Initialize

StatusCode SCT_RodEncoder::initialize() 
{
  ATH_MSG_DEBUG("SCT_RodEncoder::initialize()");
  
  // Retrieve cabling tool
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
  for (auto sctDetElement : *sctDetElementColl) {
    if (sctDetElement->swapPhiReadoutDirection()) {
      m_swapModuleID.insert(sctDetElement->identify());
    }
  }

  return StatusCode::SUCCESS;
}

// Finalize

StatusCode SCT_RodEncoder::finalize() 
{
  return StatusCode::SUCCESS;
}

// fillROD method

void SCT_RodEncoder::fillROD(std::vector<uint32_t>& vec32Data, const uint32_t& robID, 
                             const std::vector<const SCT_RDORawData*>& vecRDOs) const 
{
  // Retrieve errors from SCT_ByteStreamErrorsSvc
  const std::set<IdentifierHash>* timeOutErrors{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::TimeOutError)};
  const std::set<IdentifierHash>* lvl1IDErrors{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::LVL1IDError)};
  const std::set<IdentifierHash>* bcIDErrors{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::BCIDError)};
  const std::set<IdentifierHash>* preambleErrors{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::PreambleError)};
  const std::set<IdentifierHash>* formatterErrors{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::FormatterError)};
  const std::set<IdentifierHash>* trailerErrors{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::TrailerError)};
  const std::set<IdentifierHash>* headerTrailerErrors{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::HeaderTrailerLimitError)};
  const std::set<IdentifierHash>* trailerOverflowErrors{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::TrailerOverflowError)};
  const std::set<IdentifierHash>* abcdErrors{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::ABCDError)};
  const std::set<IdentifierHash>* rawErrors{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::RawError)};

  std::vector<uint16_t> vec16Data;
  
  // Loop over errors here - just add headers (w/ errors), trailers (w/errors), and raw and abcd errors
  addHeadersWithErrors(robID, timeOutErrors, TIMEOUT_ERR, vec16Data);
  addHeadersWithErrors(robID, lvl1IDErrors, L1_ERR, vec16Data);
  addHeadersWithErrors(robID, bcIDErrors, BCID_ERR, vec16Data);
  addHeadersWithErrors(robID, preambleErrors, PREAMBLE_ERR, vec16Data);
  addHeadersWithErrors(robID, formatterErrors, FORMATTER_ERR, vec16Data);
  
  addTrailersWithErrors(robID, trailerErrors, TRAILER_ERR, vec16Data);
  addTrailersWithErrors(robID, headerTrailerErrors, HEADER_TRAILER_ERR, vec16Data);
  addTrailersWithErrors(robID, trailerOverflowErrors, TRAILER_OVFLW_ERR, vec16Data);
  
  addSpecificErrors(robID, abcdErrors, ABCD_ERR, vec16Data);
  addSpecificErrors(robID, rawErrors, RAWDATA_ERR, vec16Data);
  
  std::vector<bool> vec_isDuplicated(vecRDOs.size(), false);
  
  // Loop over RDOs to check for duplicates and fill vec_isDuplicated vector
  for (unsigned int iRDO1{0}; iRDO1<vecRDOs.size(); iRDO1++) {
    const SCT_RDORawData* rdo1{vecRDOs.at(iRDO1)};
    if (rdo1 == nullptr) {
      ATH_MSG_ERROR("RDO pointer is NULL. skipping this hit.");
      vec_isDuplicated.at(iRDO1) = true;
      continue;
    }

    // Check if there is another RDO with the same first strip
    for (unsigned int iRDO2{0}; iRDO2<iRDO1; iRDO2++) {
      const SCT_RDORawData* rdo2{vecRDOs.at(iRDO2)};
      if (vec_isDuplicated.at(iRDO2)) continue;

      if (rdo1->identify() == rdo2->identify()) {
        // Keep RDO with larger cluster size. If cluster sizes are the same, keep the first one.
        if (rdo1->getGroupSize() >= rdo2->getGroupSize()) {
          vec_isDuplicated.at(iRDO2) = true;
        } 
        else {
          vec_isDuplicated.at(iRDO1)  = true;
        }
        break;
      }
    }
  }

  std::vector<int> vecTimeBins;
  int strip{0};
  int timeBin{0};
  int groupSize{0};

  uint32_t lastHeader{0};
  bool firstInROD{true};
  uint16_t lastTrailer{0};

  // Loop over RDOs to decode to 16 bit words
  for (unsigned int iRDO{0}; iRDO<vecRDOs.size(); iRDO++) {
    const SCT_RDORawData* rdo{vecRDOs.at(iRDO)};
    if (vec_isDuplicated.at(iRDO)) continue;

    const uint16_t header{this->getHeaderUsingRDO(rdo)};
    if (header != lastHeader) {
      if (not firstInROD) {
        vec16Data.push_back(lastTrailer);
      }
      firstInROD = false;
      vec16Data.push_back(header);
      lastHeader = header;
      lastTrailer = getTrailer(0);
    }
    if (m_condensed.value()) { // Condensed mode
      strip = getStrip(rdo);
      groupSize = rdo->getGroupSize();
      if (groupSize == 1) { // For single hit
        const int constGroupSize{1};
        const int constStrip{strip};
        encodeData(vecTimeBins, vec16Data, rdo, constGroupSize, constStrip);
      }
      // Sim RDO could have groupe size > 1, then they are split 2 by 2 to built the condensed BS data.
      else { // Encoding in condensed BS paired data from groupe size > 1.
        const int chipFirst{strip/128};
        const int chipLast{(strip+groupSize-1)/128};

        for (int chip{chipFirst}; chip<=chipLast; chip++) {
          int tmpGroupSize = 0;
          if (chipFirst == chipLast) tmpGroupSize = groupSize; // In case of one chip
          else if (chip == chipLast) tmpGroupSize = strip+groupSize-chip*128; // In case of last chip
          else if (chip == chipFirst) tmpGroupSize = (chip+1)*128-strip; // In case of first chip
          else tmpGroupSize = 128; // In case of middle chip
          const int tmpStrip1{chip==chipFirst ? strip : 128*chip};

          const int numPairedRDO{tmpGroupSize/2};
          for (int i =0; i<numPairedRDO; i++) {
            const int constGroupSize{2};
            const int constStrip{tmpStrip1+ (2*i)};
            encodeData(vecTimeBins, vec16Data, rdo, constGroupSize, constStrip);
          } 
          if ((tmpGroupSize != 0) and isOdd(tmpGroupSize)) {// The last hit from a cluster with odd group size
            const int constGroupSize{1};
            const int constStrip{tmpStrip1+ (tmpGroupSize - 1)};
            encodeData(vecTimeBins, vec16Data, rdo, constGroupSize, constStrip);
          }  
        }
      }  
      
    } // End of Condensed mode
    
    else { // Expanded mode
      vecTimeBins.clear();
      const SCT_RDORawData* rdo{vecRDOs.at(iRDO)};
      strip = getStrip(rdo);
      timeBin = getTimeBin(rdo);
      groupSize = rdo->getGroupSize();
      
      for (int t{0}; t < groupSize; t++) {
        vecTimeBins.push_back(timeBin);
        strip++;
      }
      const int constStrip{getStrip(rdo)};
      const int constGroupSize{rdo->getGroupSize()};
      encodeData(vecTimeBins, vec16Data, rdo, constGroupSize, constStrip);
      
    }  // End of Expanded mode
  } // End of RDO decode loop

  if ((not firstInROD) and (lastTrailer != 0)) {
    vec16Data.push_back(lastTrailer);
  }
  
  // 16 bits TO 32 bits and pack into 32 bit vectors
  packFragments(vec16Data,vec32Data);
  
  return;
}

// encodeData method

void SCT_RodEncoder::encodeData(const std::vector<int>& vecTimeBins, std::vector<uint16_t>& vec16Words, 
                                const SCT_RDORawData* rdo, const int& groupSize, const int& strip) const 
{
  const int encodedSide{side(rdo) << 14};
  
  const Identifier idColl{offlineID(rdo)};
  int tmpStrip{strip};
  if (m_swapModuleID.find(idColl) != m_swapModuleID.end()) { // Check if the strip has to be swapped (swap phi readout direction)
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
  
  if (m_condensed.value()) { // Condensed mode
    if (groupSize == 1) { // For single hit
      const uint16_t hitCondSingle{static_cast<uint16_t>(0x8000 | encodedSide | chipNum | clustBaseAddr | firstHitErr)};
      vec16Words.push_back(hitCondSingle);
    } 
    else if (groupSize == 2) { //  For paired strip Hits
      const uint16_t hitCondPaired{static_cast<uint16_t>(0x8001 | encodedSide | chipNum | clustBaseAddr | secondHitErr | firstHitErr)};
      vec16Words.push_back(hitCondPaired);
    }    
  } // End of Condensed mode
  
  else { // Expanded mode
    const int numEven{static_cast<int>((vecTimeBins.size() - 1)/2)};

    // First hit
    const uint16_t hitExpFirst{static_cast<uint16_t>(0x8000 | encodedSide | chipNum | clustBaseAddr | timeBin)};
    vec16Words.push_back(hitExpFirst);

    // Even consecutive strips to the first one 1DDD 1st consec strip 1DDD 2nd consec strip
    for (int i=1; i<=numEven; i++) {
      const uint16_t hitExpEven{static_cast<uint16_t>(0x8088 | ((vecTimeBins[(2*i-1)] & 0xF) << 4) | (vecTimeBins[2*i] & 0xF))};
      vec16Words.push_back(hitExpEven);
    }
    // Last bin of the Odd next hits
    if ((not vecTimeBins.empty()) and isEven(vecTimeBins.size())) {
      const uint16_t hitExpLast{static_cast<uint16_t>(0x8008 | (vecTimeBins[vecTimeBins.size()-1] & 0xF))};
      vec16Words.push_back(hitExpLast);
    }  
  } // End of Expanded mode
  
  return;
}

// packFragments function

void SCT_RodEncoder::packFragments(std::vector<uint16_t>& vec16Words, 
                                   std::vector<uint32_t>& vec32Words) const 
{
  int num16Words{static_cast<int>(vec16Words.size())};
  if (isOdd(num16Words)) {
    // Just add an additional 16bit words to make even size v16 to in the 32 bits word 0x40004000
    vec16Words.push_back(0x4000);
    num16Words++;
  }
  // Now merge 2 consecutive 16 bit words in 32 bit words
  const unsigned short int numWords{2};
  const unsigned short int position[numWords]{0, 16};
  unsigned short int arr16Words[numWords]{0, 0};
  for (int i{0}; i<num16Words; i += numWords) {
    arr16Words[i%numWords]     = vec16Words[i+1];
    arr16Words[(i+1)%numWords] = vec16Words[i];
    const uint32_t uint32Word{set32Bits(arr16Words, position, numWords)};
    vec32Words.push_back(uint32Word);
#ifdef SCT_DEBUG
    ATH_MSG_INFO("SCT encoder -> PackFragments: Output rod 0x"<<std::hex<<uint32Word<<std::dec);
#endif
  }
  
  return;
}

// set32Bits function

uint32_t SCT_RodEncoder::set32Bits(const unsigned short int* arr16Words, 
                                   const unsigned short int* position, 
                                   const unsigned short int& numWords) const 
{
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

// Get RDO info functions

int SCT_RodEncoder::getStrip(const SCT_RDORawData* rdo) const 
{
  const Identifier rdoID{rdo->identify()};
  return m_sctID->strip(rdoID);
}

Identifier SCT_RodEncoder::offlineID(const SCT_RDORawData* rdo) const 
{
  const Identifier rdoId{rdo->identify()};
  return m_sctID->wafer_id(rdoId);
}

uint32_t SCT_RodEncoder::onlineID(const SCT_RDORawData* rdo) const 
{
  const Identifier waferID{offlineID(rdo)};
  const IdentifierHash offlineIDHash{m_sctID->wafer_hash(waferID)};
  return static_cast<uint32_t>(m_cabling->getOnlineIdFromHash(offlineIDHash));
}

int SCT_RodEncoder::getRODLink(const SCT_RDORawData* rdo) const 
{
  return rodLinkFromOnlineID(onlineID(rdo));
}

int SCT_RodEncoder::side(const SCT_RDORawData* rdo) const 
{
  const Identifier rdoID{rdo->identify()};
  int sctSide{m_sctID->side(rdoID)};
  // see if we need to swap sides due to cabling weirdness
  const int linkNum{getRODLink(rdo) & 0xF};
  if (swappedCable(sctSide,linkNum)) sctSide = 1-sctSide;
  return sctSide;
}

int SCT_RodEncoder::getTimeBin(const SCT_RDORawData* rdo) const 
{
  int timeBin{0};
  const SCT3_RawData* rdoCosmic{dynamic_cast<const SCT3_RawData*>(rdo)};
  if (rdoCosmic != 0) timeBin = rdoCosmic->getTimeBin();
  return timeBin;
}

// Get headers and trailer functions

uint16_t SCT_RodEncoder::getHeaderUsingRDO(const SCT_RDORawData* rdo) const 
{
  const int rodLink{getRODLink(rdo)};
  const uint16_t linkHeader{static_cast<uint16_t>(0x2000 | (m_condensed.value() << 8) | rodLink)};
  return linkHeader;
}

uint16_t SCT_RodEncoder::getHeaderUsingHash(const IdentifierHash& linkHash, const int& errorWord) const 
{
  const int rodLink{rodLinkFromOnlineID(m_cabling->getOnlineIdFromHash(linkHash))};
  const uint16_t linkHeader{static_cast<uint16_t>(0x2000 | errorWord | (m_condensed.value() << 8) | rodLink)};
  return linkHeader;
}

uint16_t SCT_RodEncoder::getTrailer(const int& errorWord) const 
{
  const uint16_t linkTrailer{static_cast<uint16_t>(0x4000 | errorWord)};
  return linkTrailer;
}

// Add errors functions

void SCT_RodEncoder::addHeadersWithErrors(const uint32_t& robID, const std::set<IdentifierHash>* errors, 
                                          const ErrorWords& errType, std::vector<uint16_t>& vec16Data) const 
{
  for (const IdentifierHash& linkHash: *errors) {
    const uint32_t errROBID{m_cabling->getRobIdFromHash(linkHash)};
    if (errROBID == robID) {
      const uint16_t header{getHeaderUsingHash(linkHash, errType)};
      vec16Data.push_back(header);
      const uint16_t trailer{getTrailer(NULL_TRAILER_ERR)};
      vec16Data.push_back(trailer);
    }
  }
} 

//
void SCT_RodEncoder::addTrailersWithErrors(const uint32_t& robID, const std::set<IdentifierHash>* errors, 
                                           const ErrorWords& errType, std::vector<uint16_t>& vec16Data) const 
{
  for (const IdentifierHash& linkHash: *errors) {
    const uint32_t errROBID{m_cabling->getRobIdFromHash(linkHash)};
    if (errROBID == robID) {
      const uint16_t header{getHeaderUsingHash(linkHash, NULL_HEADER_ERR)};
      const uint16_t trailer{getTrailer(errType)};
      vec16Data.push_back(header);
      vec16Data.push_back(trailer);
    }
  }
}

void SCT_RodEncoder::addSpecificErrors(const uint32_t& robID, const std::set<IdentifierHash>* errors, 
                                       const ErrorWords& errType, std::vector<uint16_t>& vec16Data) const 
{
  for (const IdentifierHash& linkHash: *errors) {
    const uint32_t errROBID{m_cabling->getRobIdFromHash(linkHash)};
    if (errROBID == robID) {
      const uint16_t header{getHeaderUsingHash(linkHash, NULL_HEADER_ERR)};
      const uint16_t trailer{getTrailer(NULL_TRAILER_ERR)};
      vec16Data.push_back(header);
      vec16Data.push_back(errType);
      vec16Data.push_back(trailer);
    }
  }
}
