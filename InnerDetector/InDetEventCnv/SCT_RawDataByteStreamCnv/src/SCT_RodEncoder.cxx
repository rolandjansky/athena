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
    const uint32_t f{online.fibre()};
    const int formatter{static_cast<int>((f/12) & 0x7)};
    const int linknb{static_cast<int>((f - (formatter*12)) & 0xF)};
    const int rodlink{(formatter << 4) | linknb};
    return rodlink;
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
  m_sct_id{nullptr},
  m_swapModuleId{} {
  }

StatusCode SCT_RodEncoder::initialize() {
  //prob. dont need this next line now:
  //ATH_CHECK( AlgTool::initialize());
  ATH_MSG_DEBUG("SCT_RodEncoder::initialize()");
  
  /** Retrieve cabling tool */
  ATH_CHECK(m_cabling.retrieve());
  ATH_MSG_DEBUG("Retrieved tool " << m_cabling);

  ATH_CHECK(detStore()->retrieve(m_sct_id, "SCT_ID"));
  ATH_CHECK(m_bsErrs.retrieve());

  // See if strip numbers go from 0 to 767 or vice versa for all the wafers.
  // swapPhiReadoutDirection will not change during a run.
  // Since this is access to SiDetectorElement during initialization,
  // condition object of SiDetectorElementCollection is not accessible.
  // SCT_DetectorManager has to be used.
  const InDetDD::SCT_DetectorManager* detManager{nullptr};
  ATH_CHECK(detStore()->retrieve(detManager, "SCT"));
  const InDetDD::SiDetectorElementCollection* elements{detManager->getDetectorElementCollection()};
  for (const InDetDD::SiDetectorElement* element: *elements) {
    if (element->swapPhiReadoutDirection()) {
      m_swapModuleId.insert(element->identify());
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

void SCT_RodEncoder::fillROD(std::vector<uint32_t>& v32rod, const uint32_t& robid, 
                             vRDOs_t& rdoVec) const {
  
  /** retrieve errors from SCT_ByteStreamErrorsSvc */

  const std::set<IdentifierHash>* timeOutErrors{m_bsErrs->getErrorSet(SCT_ByteStreamErrors::TimeOutError)};
  const std::set<IdentifierHash>* l1idErrors{m_bsErrs->getErrorSet(SCT_ByteStreamErrors::LVL1IDError)};
  const std::set<IdentifierHash>* bcidErrors{m_bsErrs->getErrorSet(SCT_ByteStreamErrors::BCIDError)};
  const std::set<IdentifierHash>* preambleErrors{m_bsErrs->getErrorSet(SCT_ByteStreamErrors::PreambleError)};
  const std::set<IdentifierHash>* formatterErrors{m_bsErrs->getErrorSet(SCT_ByteStreamErrors::FormatterError)};
  const std::set<IdentifierHash>* trailerErrors{m_bsErrs->getErrorSet(SCT_ByteStreamErrors::TrailerError)};
  const std::set<IdentifierHash>* headertrailerErrors{m_bsErrs->getErrorSet(SCT_ByteStreamErrors::HeaderTrailerLimitError)};
  const std::set<IdentifierHash>* traileroverflowErrors{m_bsErrs->getErrorSet(SCT_ByteStreamErrors::TrailerOverflowError)};
  const std::set<IdentifierHash>* rawErrors{m_bsErrs->getErrorSet(SCT_ByteStreamErrors::RawError)};
  const std::set<IdentifierHash>* abcdErrors{m_bsErrs->getErrorSet(SCT_ByteStreamErrors::ABCDError)};

  std::vector<int> vtbin;
  std::vector<uint16_t> v16data;
  int strip1{0};
  int theTimeBin{0};
  int groupsize{0};
  
  /** loop over errors here - just add headers (w/ errors), trailers (w/errors), 
   * and raw and abcd errors */

  addHeadersWithErrors(robid, timeOutErrors, TIMEOUT_ERR, v16data);
  addHeadersWithErrors(robid, l1idErrors, L1_ERR, v16data);
  addHeadersWithErrors(robid, bcidErrors, BCID_ERR, v16data);
  addHeadersWithErrors(robid, preambleErrors, PREAMBLE_ERR, v16data);
  addHeadersWithErrors(robid, formatterErrors, FORMATTER_ERR, v16data);
  //
  addTrailersWithErrors(robid, trailerErrors, TRAILER_ERR, v16data);
  addTrailersWithErrors(robid, headertrailerErrors, HEADER_TRAILER_ERR, v16data);
  addTrailersWithErrors(robid, traileroverflowErrors, TRAILER_OVFLW_ERR, v16data);
  //
  addSpecificErrors(robid, abcdErrors, ABCD_ERR, v16data);
  addSpecificErrors(robid, rawErrors, RAWDATA_ERR, v16data);
  
  std::vector<bool> v_isDuplicated(rdoVec.size(), false);
  for (unsigned int iRdo{0}; iRdo<rdoVec.size(); iRdo++) {
    const RDO* rawdata{rdoVec.at(iRdo)};
    if (rawdata==nullptr) {
      ATH_MSG_ERROR("RDO pointer is NULL. skipping this hit.");
      v_isDuplicated.at(iRdo) = true;
      continue;
    }

    // Check if there is another RDO with the same first strip
    for (unsigned int iRdo2{0}; iRdo2<iRdo; iRdo2++) {
      const RDO* rawdata2{rdoVec.at(iRdo2)};
      if (v_isDuplicated.at(iRdo2)) continue;

      if (rawdata->identify()==rawdata2->identify()) {
        // Keep RDO with larger cluster size. If cluster sizes are the same, keep the first one.
        if (rawdata->getGroupSize()>=rawdata2->getGroupSize()) {
          v_isDuplicated.at(iRdo2) = true;
        } else {
          v_isDuplicated.at(iRdo)  = true;
        }
        break;
      }
    }
  }

  uint32_t lastHeader{0};
  bool firstInRod{true};
  uint16_t lastTrailer{0};
  for (unsigned int iRdo{0}; iRdo<rdoVec.size(); iRdo++) {
    const RDO* rawdata{rdoVec.at(iRdo)};
    if (v_isDuplicated.at(iRdo)) continue;

    const uint16_t header{this->getHeaderUsingRDO(rawdata)};
    if (header != lastHeader) {
      if (! firstInRod) {
        v16data.push_back(lastTrailer);
      }
      firstInRod = false;
      v16data.push_back(header);
      lastHeader = header;
      lastTrailer = getTrailer(0);
    }
    if (m_condensed.value()) { /** Condensed mode */
      strip1 = strip(rawdata);
      groupsize = groupSize(rawdata);
      if (groupsize == 1) { /** For single hit */
        const int gSize{1};
        const int strip2{strip1};
        encodeData(vtbin, v16data, rawdata, gSize, strip2);
      }
      /** Sim rdo could have groupe size > 1 then I need to split 
       * them 2 by 2 to built the condensed BS data */
      else { /** Encoding in condensed BS paired data from groupe size > 1 */
        const int chipFirst{strip1/128};
        const int chipLast{(strip1+groupsize-1)/128};

        for (int chip{chipFirst}; chip<=chipLast; chip++) {
          int tmpGroupsize = 0;
          if (chipFirst==chipLast) tmpGroupsize = groupsize; // In case of one chip
          else if (chip==chipLast) tmpGroupsize = strip1+groupsize-chip*128; // In case of last chip
          else if (chip==chipFirst) tmpGroupsize = (chip+1)*128-strip1; // In case of first chip
          else tmpGroupsize = 128; // In case of middle chip
          const int tmpStrip1{chip==chipFirst ? strip1 : 128*chip};

          const int n_pairedRdo{tmpGroupsize/2};
          for (int i =0; i<n_pairedRdo; i++) {
            const int gSize{2};
            const int strip2{tmpStrip1+ (2*i)};
            encodeData(vtbin, v16data, rawdata, gSize, strip2);
          } 
          if ((tmpGroupsize != 0) and isOdd(tmpGroupsize)) {/** The last hit from a cluster with odd group size */
            const int gSize{1};
            const int strip2{tmpStrip1+ (tmpGroupsize - 1)};
            encodeData(vtbin, v16data, rawdata, gSize, strip2);
          }  
        }

      }  
      
    } /** end of condensed Mode */
    
    else {/** Expanded mode */
      
      vtbin.clear();
      const RDO* rawdata{rdoVec.at(iRdo)};
      strip1 = strip(rawdata);
      theTimeBin = tbin(rawdata);
      groupsize = groupSize(rawdata);
      
      for (int t{0}; t < groupsize; t++) {
        vtbin.push_back(theTimeBin);
        strip1++;
      }
      const int strip2{strip(rawdata)};
      const int gSize{groupSize(rawdata)};
      encodeData(vtbin, v16data, rawdata, gSize, strip2);
      
    }  // End of (else) Expanded
  } //end of RDO loop
  if (!firstInRod and lastTrailer!=0) {
    v16data.push_back(lastTrailer);
  }
  /** 16 bits TO 32 bits and pack into 32 bit vectors */
  packFragments(v16data,v32rod);
  
  return;
  
} // end of fillROD(...)

void SCT_RodEncoder::addHeadersWithErrors(const uint32_t& robid, const std::set<IdentifierHash>* errors, const ErrorWords& errType, std::vector<uint16_t>& v16data) const {
  for (const IdentifierHash& linkHash: *errors) {
    const uint32_t errRobId{m_cabling->getRobIdFromHash(linkHash)};
    if (errRobId == robid) {
      const uint16_t header{getHeaderUsingHash(linkHash, errType)};
      v16data.push_back(header);
      const uint16_t trailer{getTrailer(NULL_TRAILER_ERR)};
      v16data.push_back(trailer);
    }
  }
} 

//
void SCT_RodEncoder::addTrailersWithErrors(const uint32_t& robid, const std::set<IdentifierHash>* errors, const ErrorWords& errType, std::vector<uint16_t>& v16data) const {
  for (const IdentifierHash& linkHash: *errors) {
    const uint32_t errRobId{m_cabling->getRobIdFromHash(linkHash)};
    if (errRobId == robid) {
      const uint16_t header{getHeaderUsingHash(linkHash, NULL_HEADER_ERR)};
      const uint16_t trailer{getTrailer(errType)};
      v16data.push_back(header);
      v16data.push_back(trailer);
    }
  }
}

void SCT_RodEncoder::addSpecificErrors(const uint32_t& robid, const std::set<IdentifierHash>* errors, const ErrorWords& errType, std::vector<uint16_t>& v16data) const {
  for (const IdentifierHash& linkHash: *errors) {
    const uint32_t errRobId{m_cabling->getRobIdFromHash(linkHash)};
    if (errRobId == robid) {
      const uint16_t header{getHeaderUsingHash(linkHash, NULL_HEADER_ERR)};
      const uint16_t trailer{getTrailer(NULL_TRAILER_ERR)};
      v16data.push_back(header);
      v16data.push_back(errType);
      v16data.push_back(trailer);
    }
  }
}

///=========================================================================
/// Encode Data function
///========================================================================= 

void SCT_RodEncoder::encodeData(std::vector<int>& vtbin, std::vector<uint16_t>& v16, const RDO *rdo, const int& gSize, const int& strip2) const {
  
  ///-------------------------------------------------------------------------------------
  /// Check if the strip has to be swapped (swap phi readout direction)
  ///-------------------------------------------------------------------------------------
  
  const int encodedSide{side(rdo) << 14};
  
  const Identifier idColl{offlineId(rdo)};
  int strip1{strip2};
  if (m_swapModuleId.find(idColl)!= m_swapModuleId.end()) {
    strip1= 767 - strip1;
    strip1= strip1-(gSize-1);
  }
  
  const int chipNb{((strip1/128) & 0x7) << 11};
  const int clustBaseAddr{((strip1-(chipNb*128)) & 0x7F) << 4};
  int theTimeBin{0};
  int firstHitErr{0 << 2};
  int secondHitErr{0 << 3};
  
  const SCT3_RawData* rdoCosmic{dynamic_cast<const SCT3_RawData*>(rdo)};
  if (rdoCosmic != nullptr) {
    theTimeBin = tbin(rdoCosmic);
    firstHitErr = ((rdoCosmic)->FirstHitError()) << 2;
    secondHitErr = ((rdoCosmic)->SecondHitError()) << 3;
  }
  
  ///-------------------------------------------------------------------------------------
  /// Condensed
  ///-------------------------------------------------------------------------------------
  if (m_condensed.value()) {/** single Hit on condensed Mode */
    if (gSize == 1) {/** Group size = 1 */
      const uint16_t HitCondSingle{static_cast<uint16_t>(0x8000 | encodedSide | chipNb | clustBaseAddr | firstHitErr)};
      v16.push_back(HitCondSingle);
    } else if (gSize == 2) {/** paired strip Hits on condensed Mode */
      const uint16_t HitCondPaired{static_cast<uint16_t>(0x8001 | encodedSide | chipNb | clustBaseAddr | secondHitErr | firstHitErr)};
      v16.push_back(HitCondPaired);
    }    
  }  /// end of condensed
  ///-------------------------------------------------------------------------------------
  /// Expanded
  ///-------------------------------------------------------------------------------------
  else {
    const int nEven{static_cast<int>((vtbin.size() - 1)/2)};
    /** First hit */
    const uint16_t HitExpFirst{static_cast<uint16_t>(0x8000 | encodedSide | chipNb | clustBaseAddr | theTimeBin)};
    v16.push_back(HitExpFirst);
    /** Even consecutive strips to the first one 1DDD 1st consec strip 1DDD 2nd consec strip */
    for (int i=1; i<=nEven; i++) {
      const uint16_t HitExpEven{static_cast<uint16_t>(0x8088 | ((vtbin[(2*i-1)] & 0xF) << 4) | (vtbin[2*i] & 0xF))};
      v16.push_back(HitExpEven);
    }
    /** Last bin of the Odd next hits */
    if ((not vtbin.empty() ) and isEven(vtbin.size())) {
      const uint16_t HitExpLast{static_cast<uint16_t>(0x8008 | (vtbin[vtbin.size()-1] & 0xF))};
      v16.push_back(HitExpLast);
    }  
  } /** end of expanded */
  
  return;
} /** end of encodeData */

///=========================================================================
/// Converting the 16 bit vector v16 to 32 bit words v32
///=========================================================================
void SCT_RodEncoder::packFragments(std::vector<uint16_t>& v16, std::vector<uint32_t>& v32) const {
  int n16words{static_cast<int>(v16.size())};
  if (isOdd(n16words)) {
    /** just add an additional 16bit words to make even size v16 to in the 32 bits word 0x40004000 */
    v16.push_back(0x4000);
    n16words++;
  }
  /** now merge 2 consecutive 16 bit words in 32 bit words */
  const unsigned short int nWords{2};
  const unsigned short int position[nWords]{0, 16};
  unsigned short int v16words[nWords]{0, 0};
  for (int i{0}; i<n16words; i += nWords) {
    v16words[i%nWords]     = v16[i+1];
    v16words[(i+1)%nWords] = v16[i];
    const uint32_t v32word{set32bits(v16words, position, nWords)};
    v32.push_back(v32word);
#ifdef SCT_DEBUG
    ATH_MSG_INFO("SCT encoder -> PackFragments: Output rod 0x"<<std::hex<<v32word<<std::dec);
#endif
  }
  
  return;
}

///=========================================================================
/// set the 16 bit word v16 to 32 bit words v32
///=========================================================================

uint32_t SCT_RodEncoder::set32bits(const unsigned short int* v16, const unsigned short int* pos, const unsigned short int& n) const {
  uint32_t v32{0};
  uint32_t p{0};
  uint32_t v{0};
  for (uint16_t i{0}; i<n; i++) {
    v = static_cast<uint32_t>(*(v16+i));
    p = static_cast<uint32_t>(*(pos+i));
    v32 |= (v<<p);
  } 
  return v32;
}

///=========================================================================
/// Link and Side Numbers 
///=========================================================================
/** Strip number */ 
int SCT_RodEncoder::strip(const RDO* rdo) const {
  const Identifier rdoId{rdo->identify()};
  return m_sct_id->strip(rdoId);
}

/** RDO ID */
Identifier SCT_RodEncoder::offlineId(const RDO* rdo) const {
  const Identifier rdoId{rdo->identify()};
  return m_sct_id->wafer_id(rdoId);
}

/** ROD online ID */
uint32_t SCT_RodEncoder::onlineId(const RDO* rdo) const {
  const Identifier waferId{offlineId(rdo)};
  const IdentifierHash offlineIdHash{m_sct_id->wafer_hash(waferId)};
  const uint32_t thisOnlineId{m_cabling->getOnlineIdFromHash(offlineIdHash)};
  return thisOnlineId;
}

/** ROD Link Number In the ROD header data */
int SCT_RodEncoder::rodLink(const RDO* rdo) const {
  return rodLinkFromOnlineId(onlineId(rdo));
}

/** Side Info */
int SCT_RodEncoder::side(const RDO* rdo) const {
  const Identifier rdoId{rdo->identify()};
  int s{m_sct_id->side(rdoId)};
  /** see if we need to swap sides due to cabling weirdness */
  const int linknb{rodLink(rdo) & 0xF};
  if (swappedCable(s,linknb)) s = 1-s;
  return s;
}

/** Time Bin Info */
int SCT_RodEncoder::tbin(const RDO* rdo) const {
  int theTimeBin{0};
  const SCT3_RawData* rdoCosmic{dynamic_cast<const SCT3_RawData*>(rdo)};
  if (rdoCosmic != 0) theTimeBin = rdoCosmic->getTimeBin();
  return theTimeBin;
}

///-------------------------------------------------------------------------------------
/// Link header
///-------------------------------------------------------------------------------------

uint16_t 
SCT_RodEncoder::getHeaderUsingRDO(const RDO* rdo) const {
  const int rodlink{rodLink(rdo)};
  const uint16_t LinkHeader{static_cast<uint16_t>(0x2000 | (m_condensed.value() << 8) | rodlink)};
  return LinkHeader;
}

uint16_t 
SCT_RodEncoder::getHeaderUsingHash(const IdentifierHash& linkHash, const int& errorWord) const {
  const int rodlink{rodLinkFromOnlineId(m_cabling->getOnlineIdFromHash(linkHash))};
  const uint16_t LinkHeader{static_cast<uint16_t>(0x2000 | errorWord | (m_condensed.value() << 8) | rodlink)};
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
