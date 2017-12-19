/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** Implementation of SCT_RodEncoder class */
#include "SCT_RodEncoder.h" 

///SCT
#include "SCT_ConditionsServices/ISCT_ByteStreamErrorsSvc.h"

///InDet
#include "InDetIdentifier/SCT_ID.h"

///Athena
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetRawData/SCT_RDORawData.h"

///STL
#include <set>

namespace {
  int rodLinkFromOnlineId(const uint32_t id) {
    SCT_OnlineId online{id};
    uint32_t f{online.fibre()};
    int formatter{static_cast<int>((f/12) & 0x7)};
    int linknb{static_cast<int>((f - (formatter*12)) & 0xF)};
    int rodlink{(formatter << 4) | linknb};
    return rodlink;
  } 
  bool isOdd(const int someNumber){
    return bool(someNumber & 1); 
  }
 
  bool isEven(const int someNumber){
    return !isOdd(someNumber);
  }
 
  bool swappedCable(const int moduleSide, const int linkNumber){
    return isOdd(linkNumber) ? (moduleSide==0) : (moduleSide==1);
  }
}//end of anon namespace


SCT_RodEncoder::SCT_RodEncoder
(const std::string& type, const std::string& name,const IInterface* parent) : 
  AthAlgTool(type, name, parent),
  m_cabling{"SCT_CablingSvc", name},
  m_bsErrs{"SCT_ByteStreamErrorsSvc", name},
  m_sct_id{nullptr},
  m_condensed{false},
  m_swapModuleId{},
  m_singleCondHitNumber{0},
  m_pairedCondHitNumber{0},
  m_firstExpHitNumber{0},
  m_evenExpHitNumber{0},
  m_lastExpHitNumber{0},
  m_headerNumber{0},
  m_trailerNumber{0} {
    declareInterface<ISCT_RodEncoder>(this);
    declareProperty("CondensedMode", m_condensed=true);
  }


/** destructor  */
SCT_RodEncoder::~SCT_RodEncoder() {
  //nop
}


StatusCode SCT_RodEncoder::initialize() {
  //prob. dont need this next line now:
  //ATH_CHECK( AlgTool::initialize()); 
  ATH_MSG_DEBUG("SCT_RodEncoder::initialize()");
  
  /** Retrieve cabling service */
  ATH_CHECK(m_cabling.retrieve());
  ATH_MSG_DEBUG("Retrieved service " << m_cabling);
  ATH_CHECK(detStore()->retrieve(m_sct_id, "SCT_ID"));
  ATH_CHECK(m_bsErrs.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode SCT_RodEncoder::finalize() {
  return StatusCode::SUCCESS;
}

///=========================================================================
///  fillROD convert SCT RDO to a vector of 32bit words
///=========================================================================  

void SCT_RodEncoder::fillROD(std::vector<uint32_t>& v32rod, uint32_t robid, 
                             vRDOs_t& rdoVec) {
  
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
    if (rawdata==0) {
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

    uint16_t header{this->getHeaderUsingRDO(rawdata)};
    if (header != lastHeader) {
      if (! firstInRod) {
        v16data.push_back(lastTrailer);
      }
      firstInRod = false;
      v16data.push_back(header);
      lastHeader = header;
      lastTrailer = getTrailer(0);
    }
    if (m_condensed) { /** Condensed mode  */
      strip1 = strip(rawdata);
      groupsize = groupSize(rawdata);
      if (groupsize == 1) { /** For single hit */
        int gSize{1};
        int strip2{strip1};
        encodeData(vtbin, v16data, rawdata, gSize, strip2);
      }
      /** Sim rdo could have groupe size > 1 then I need to split 
       * them 2 by 2 to built the condensed BS data */
      else { /** Encoding in condensed BS paired data from groupe size > 1 */
        int chipFirst{strip1/128};
        int chipLast{(strip1+groupsize-1)/128};

        for (int chip{chipFirst}; chip<=chipLast; chip++) {
          int tmpGroupsize = 0;
          if (chipFirst==chipLast) tmpGroupsize = groupsize; // In case of one chip
          else if (chip==chipLast) tmpGroupsize = strip1+groupsize-chip*128; // In case of last chip
          else if (chip==chipFirst) tmpGroupsize = (chip+1)*128-strip1; // In case of first chip
          else tmpGroupsize = 128; // In case of middle chip
          int tmpStrip1{chip==chipFirst ? strip1 : 128*chip};

          int n_pairedRdo{tmpGroupsize/2};
          for (int i =0; i<n_pairedRdo; i++) {
            int gSize{2};
            int strip2{tmpStrip1+ (2*i)};
            encodeData(vtbin, v16data, rawdata, gSize, strip2);
          } 
          if ((tmpGroupsize != 0) and isOdd(tmpGroupsize)) {/** The last hit from a cluster with odd group size */
            int gSize{1};
            int strip2{tmpStrip1+ (tmpGroupsize - 1)};
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
      int strip2{strip(rawdata)};
      int gSize{groupSize(rawdata)};
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

void SCT_RodEncoder::addHeadersWithErrors(const uint32_t robid, const std::set<IdentifierHash>* errors, ErrorWords errType, std::vector<uint16_t>& v16data) {
  std::set<IdentifierHash>::iterator step{errors->begin()};
  const std::set<IdentifierHash>::iterator end{errors->end()};
  for (;step != end; ++step) {
    IdentifierHash linkHash{*step};
    uint32_t errRobId{m_cabling->getRobIdFromHash(linkHash)};
    if (errRobId == robid) {
      uint16_t header{getHeaderUsingHash(linkHash, errType)};
      v16data.push_back(header);
      uint16_t trailer{getTrailer(NULL_TRAILER_ERR)};
      v16data.push_back(trailer);
    }
  }
} 

//
void SCT_RodEncoder::addTrailersWithErrors(const uint32_t robid, const std::set<IdentifierHash>* errors, ErrorWords errType, std::vector<uint16_t>& v16data) {
  std::set<IdentifierHash>::iterator step{errors->begin()};
  const std::set<IdentifierHash>::iterator end{errors->end()};
  for (;step != end; ++step) {
    IdentifierHash linkHash{*step};
    uint32_t errRobId{m_cabling->getRobIdFromHash(linkHash)};
    if (errRobId == robid) {
      uint16_t header{getHeaderUsingHash(linkHash, NULL_HEADER_ERR)};
      uint16_t trailer{getTrailer(errType)};
      v16data.push_back(header);
      v16data.push_back(trailer);
    }
  }
}

void SCT_RodEncoder::addSpecificErrors(const uint32_t robid, const std::set<IdentifierHash>* errors, ErrorWords errType, std::vector<uint16_t>& v16data) {
  std::set<IdentifierHash>::iterator step{errors->begin()};
  const std::set<IdentifierHash>::iterator end{errors->end()};
  for (;step != end; ++step) {
    IdentifierHash linkHash(*step);
    uint32_t errRobId{m_cabling->getRobIdFromHash(linkHash)};
    if (errRobId == robid) {
      uint16_t header{getHeaderUsingHash(linkHash, NULL_HEADER_ERR)};
      uint16_t trailer{getTrailer(NULL_TRAILER_ERR)};
      v16data.push_back(header);
      v16data.push_back(errType);
      v16data.push_back(trailer);
    }
  }
}

///=========================================================================
///  Encode Data function
///========================================================================= 

void SCT_RodEncoder::encodeData(std::vector<int>& vtbin, std::vector<uint16_t>& v16, const RDO *rdo, int gSize, int strip2) {
  
  ///-------------------------------------------------------------------------------------
  /// Check if the strip has to be swapped (swap phi readout direction)
  ///-------------------------------------------------------------------------------------
  
  int encodedSide{side(rdo) << 14};
  int strip1{strip2};
  
  Identifier idColl{offlineId(rdo)};
  if (m_swapModuleId.find(idColl)!= m_swapModuleId.end()) {
    strip1= 767 - strip1;
    strip1= strip1-(gSize-1);
  }
  
  int chipNb{((strip1/128) & 0x7) << 11};
  int clustBaseAddr{((strip1-(chipNb*128)) & 0x7F) << 4};
  int theTimeBin{0};
  int firstHitErr{0 << 2};
  int secondHitErr{0 << 3};
  
  const SCT3_RawData* rdoCosmic{dynamic_cast<const SCT3_RawData*>(rdo)};
  if (rdoCosmic != 0) {       
    theTimeBin = tbin(rdoCosmic);
    firstHitErr = ((rdoCosmic)->FirstHitError()) << 2;
    secondHitErr = ((rdoCosmic)->SecondHitError()) << 3;
  }
  
  ///-------------------------------------------------------------------------------------
  ///   Condensed
  ///-------------------------------------------------------------------------------------
  if (m_condensed) {/** single Hit on condensed Mode */
    
    if (gSize == 1) {/** Group size = 1 */
      uint16_t HitCondSingle{static_cast<uint16_t>(0x8000 | encodedSide | chipNb | clustBaseAddr | firstHitErr)};
      v16.push_back(HitCondSingle);
      m_singleCondHitNumber++;
    } else if (gSize == 2) {/** paired strip Hits on condensed Mode  */
      uint16_t HitCondPaired{static_cast<uint16_t>(0x8001 | encodedSide | chipNb | clustBaseAddr | secondHitErr | firstHitErr)};
      v16.push_back(HitCondPaired);
      m_pairedCondHitNumber++;
    }    
  }  /// end of condensed
  
  ///-------------------------------------------------------------------------------------
  ///   Expanded
  ///-------------------------------------------------------------------------------------
  else{  
    int nEven{static_cast<int>((vtbin.size() - 1)/2)};
    /** First hit */
    uint16_t HitExpFirst{static_cast<uint16_t>(0x8000 | encodedSide | chipNb | clustBaseAddr | theTimeBin)};
    v16.push_back(HitExpFirst);
    m_firstExpHitNumber++;
    /** Even consecutive strips to the first one 1DDD 1st consec strip 1DDD 2nd consec strip */
    for (int i=1; i<=nEven; i++) {
      uint16_t HitExpEven{static_cast<uint16_t>(0x8088 | ((vtbin[(2*i-1)] & 0xF) << 4) | (vtbin[2*i] & 0xF))};
      v16.push_back(HitExpEven); 
      m_evenExpHitNumber++;
    }
    /** Last bin of the Odd next hits    */
    if ((not vtbin.empty() ) and isEven(vtbin.size())) {
      uint16_t HitExpLast{static_cast<uint16_t>(0x8008 | (vtbin[vtbin.size()-1] & 0xF))};
      v16.push_back(HitExpLast);
      m_lastExpHitNumber++;
    }  

  } /** end of  expanded */
  
  return;
}  /** end of encodeData */

///=========================================================================
///  Converting the 16 bit vector v16 to 32 bit words v32
///=========================================================================  
void SCT_RodEncoder::packFragments(std::vector<uint16_t>& v16, std::vector<uint32_t>& v32) const {
  int n16words{static_cast<int>(v16.size())};
  if (isOdd(n16words)) {
    /** just add an additional 16bit words to make even size v16 to in the 32 bits word 0x40004000  */
    v16.push_back(0x4000); 
    n16words++;
  }
  /** now merge 2 consecutive 16 bit words in 32 bit words */
  const unsigned short int nWords{2};
  unsigned short int position[nWords]{0, 16};
  unsigned short int v16words[nWords]{0,  0};
  for (int i=0; i<n16words; ) {
    v16words[i%nWords]     = v16[i+1];
    v16words[(i+1)%nWords] = v16[i];
    uint32_t v32word{set32bits(v16words, position, nWords)};
    v32.push_back(v32word);
    i += nWords;
#ifdef SCT_DEBUG
    ATH_MSG_INFO("SCT encoder -> PackFragments: Output rod 0x"<<std::hex<<v32word<<std::dec);
#endif
  }
  
  return;
}

///=========================================================================
///  set the 16 bit word v16 to 32 bit words v32
///=========================================================================  

uint32_t SCT_RodEncoder::set32bits(const unsigned short int* v16, const unsigned short int* pos, const unsigned short int n) const { 
  uint32_t v32{0};
  uint32_t p{0};
  uint32_t v{0};
  for (uint16_t i{0}; i<n; i++) {
    v   = (uint32_t) (*(v16+i));
    p   = (uint32_t) (*(pos+i));
    v32 = v32 | ( v<<p );
  } 
  return v32;
}

///=========================================================================
///  Link and Side Numbers 
///=========================================================================
/** Strip number  */ 
int SCT_RodEncoder::strip(const RDO* rdo) {
  Identifier rdoId{rdo->identify()};
  return m_sct_id->strip(rdoId);
}

/** RDO ID  */
Identifier SCT_RodEncoder::offlineId(const RDO* rdo) {
  Identifier rdoId{rdo->identify()};
  return m_sct_id->wafer_id(rdoId);
}

/** ROD online ID  */
uint32_t  SCT_RodEncoder::onlineId(const RDO* rdo) {
  Identifier rdoId{rdo->identify()}; 
  Identifier thisId{m_sct_id->wafer_id(rdoId)};
  IdentifierHash offlineIdHash{m_sct_id->wafer_hash(thisId)};
  uint32_t thisOnlineId{m_cabling->getOnlineIdFromHash(offlineIdHash)};
  return thisOnlineId;
}

/** ROD Link Number In the ROD header data  */
int  SCT_RodEncoder::rodLink(const RDO* rdo) {
  return rodLinkFromOnlineId(onlineId(rdo));
}

/** Side Info */
int  SCT_RodEncoder::side(const RDO* rdo) {
  Identifier rdoId{rdo->identify()};
  int s{m_sct_id->side(rdoId)};
  /** see if we need to swap sides due to cabling weirdness */
  int linknb{rodLink(rdo) & 0xF};
  if (swappedCable(s,linknb)) s = 1-s;
  return s;
}

/** Time Bin Info */
int  SCT_RodEncoder::tbin(const RDO* rdo) {
  int theTimeBin{0};
  const SCT3_RawData* rdoCosmic{dynamic_cast<const SCT3_RawData*>(rdo)};
  if (rdoCosmic != 0) theTimeBin = rdoCosmic->getTimeBin();
  return theTimeBin;
}

///-------------------------------------------------------------------------------------
///   Link header
///-------------------------------------------------------------------------------------

uint16_t 
SCT_RodEncoder::getHeaderUsingRDO(const RDO* rdo) {
  int rodlink{rodLink(rdo)};
  uint16_t LinkHeader{static_cast<uint16_t>(0x2000 | (m_condensed << 8) | rodlink)};
  m_headerNumber++;
  return LinkHeader;
}

///-------------------------------------------------------------------------------------
///   Link trailer
///-------------------------------------------------------------------------------------
uint16_t 
SCT_RodEncoder::getTrailer(int errorword) {
  uint16_t LinkTrailer{static_cast<uint16_t>(0x4000 | errorword)};
  m_trailerNumber++;
  return LinkTrailer;
}

uint16_t 
SCT_RodEncoder::getHeaderUsingHash(IdentifierHash linkHash, int errorWord) {
  int rodlink{rodLinkFromOnlineId(m_cabling->getOnlineIdFromHash(linkHash))};
  uint16_t LinkHeader{static_cast<uint16_t>(0x2000 | errorWord | (m_condensed << 8) | rodlink)};
  m_headerNumber++;
  return LinkHeader;
}
