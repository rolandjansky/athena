/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_RodDecoder.h"
//STL
#include <deque>
#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>
//Gaudi
#include "GaudiKernel/IIncidentSvc.h"
//Athena
#include "ByteStreamData/RawEvent.h"
//Inner detector
#include "SCT_Cabling/ISCT_CablingSvc.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"

union RawWord{
  uint32_t word32;
  uint16_t word16[2];
};

//constructor
SCT_RodDecoder::SCT_RodDecoder
(const std::string& type, const std::string& name,const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_sct_id{nullptr},
  m_indet_mgr{nullptr},
  m_cabling{"SCT_CablingSvc", name},
  m_condensedMode{false},
  m_superCondensedMode{false},
  m_singleCondHitNumber{0},
  m_pairedCondHitNumber{0},
  m_firstExpHitNumber{0},
  m_evenExpHitNumber{0},
  m_lastExpHitNumber{0},
  m_headnumber{0},
  m_trailnumber{0},
  m_head_error_bcid{0},
  m_head_error_lvl1id{0},
  m_head_error_timeout{0},
  m_head_error_formatter{0},
  m_head_error_preamb{0},
  m_trail_error_overflow{0},
  m_trail_error_limit{0},
  m_trail_error_bit{0},
  m_config_data_bit{0},
  m_flag_error_bit{0},
  m_cond_hit1_error{0},
  m_cond_hit2_error{0},
  m_chip_number_error{0},
  m_unknown_data_format{0},
  m_nHits{0},
  m_nRDOs{0},
  m_maskedLinkNumber{0},
  m_maskedRODNumber{0},
  m_RODClockErrorNumber{0},
  m_truncatedRODNumber{0},
  m_numMissingLinkHeader{0},
  m_numUnknownOfflineId{0},
  m_incidentSvc{"IncidentSvc", name}
{
  declareProperty("CablingSvc", m_cabling);
  declareProperty("TriggerMode", m_triggerMode=true);
  declareInterface<ISCT_RodDecoder>(this);
}

//destructor  
SCT_RodDecoder::~SCT_RodDecoder() {
}

StatusCode SCT_RodDecoder::initialize() {
  ATH_CHECK(AlgTool::initialize());

  ATH_CHECK(m_incidentSvc.retrieve());
 
  /** Retrieve cabling service */
  ATH_CHECK(m_cabling.retrieve());
  ATH_MSG_DEBUG("Retrieved service " << m_cabling);

  ATH_CHECK(detStore()->retrieve(m_indet_mgr,"SCT"));
  
  ATH_CHECK(detStore()->retrieve(m_sct_id,"SCT_ID"));
  m_cntx_sct = m_sct_id->wafer_context();

  ATH_CHECK(m_configTool.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode 
SCT_RodDecoder::finalize() {
  
  /** print out summaries of data and errors decoded */
  ATH_MSG_INFO("SCT BytestreamCnv summary: " << m_headnumber  <<" link headers found");
  if (m_condensedMode) {
    ATH_MSG_INFO("SCT decoding bytestream summary: " << m_singleCondHitNumber << " single strips with hit in condensed mode");
    ATH_MSG_INFO("SCT decoding bytestream summary: " << m_pairedCondHitNumber << " paired strips with hit in condensed mode");
  } else {
    ATH_MSG_INFO("SCT decoding bytestream summary: " << m_firstExpHitNumber << " first strips with hit in expanded mode");
    ATH_MSG_INFO("SCT decoding bytestream summary: " << m_evenExpHitNumber << " consecutive paired strips with hit in expanded mode");
    ATH_MSG_INFO("SCT decoding bytestream summary: " << m_lastExpHitNumber << " last consecutive strip with hit in expanded mode");
  }

  ATH_MSG_INFO("SCT BytestreamCnv summary: " << m_trailnumber << " link trailers found");
  if (m_head_error_bcid > 0)      ATH_MSG_INFO("SCT BytestreamCnv summary: header-> "       << m_head_error_lvl1id << " LVL1d errors found");
  if (m_head_error_timeout > 0)   ATH_MSG_INFO("SCT BytestreamCnv summary: header-> "       << m_head_error_timeout << " timeout errors found");
  if (m_head_error_formatter > 0) ATH_MSG_INFO("SCT BytestreamCnv summary: header-> "       << m_head_error_formatter << " formatter errors found");
  if (m_head_error_preamb > 0)    ATH_MSG_INFO("SCT BytestreamCnv summary: header-> "       << m_head_error_preamb << " preamble errors found");
  if (m_maskedLinkNumber > 0)     ATH_MSG_INFO("SCT BytestreamCnv summary: header-> "       << m_maskedLinkNumber << " masked links found");
  if (m_trail_error_overflow > 0) ATH_MSG_INFO("SCT BytestreamCnv summary: trailer-> "      << m_trail_error_overflow << " trailer data overflow errors found");
  if (m_trail_error_limit > 0)    ATH_MSG_INFO("SCT BytestreamCnv summary: trailer-> "      << m_trail_error_limit << " header trailer limit errors found");
  if (m_trail_error_bit > 0)      ATH_MSG_INFO("SCT BytestreamCnv summary: trailer-> "      << m_trail_error_bit << " trailer bit errors found");
  if (m_config_data_bit > 0)      ATH_MSG_INFO("SCT BytestreamCnv summary: raw Data-> "     << m_config_data_bit << " raw data found: Config data mode");
  if (m_flag_error_bit > 0)       ATH_MSG_INFO("SCT BytestreamCnv summary: flag-> "         << m_flag_error_bit << " module link flag bit errors found");
  if (m_cond_hit1_error > 0)      ATH_MSG_INFO("SCT BytestreamCnv summary: hit-> "          << m_cond_hit1_error << " 1st hit error found in condensed mode");
  if (m_cond_hit2_error > 0)      ATH_MSG_INFO("SCT BytestreamCnv summary: hit-> "          << m_cond_hit2_error << " 2nd hit error found in condensed mode");
  if (m_chip_number_error > 0)    ATH_MSG_INFO("SCT BytestreamCnv summary: hit-> "          << m_chip_number_error << " Chip number > 5 error found");
  if (m_unknown_data_format > 0)  ATH_MSG_INFO("SCT BytestreamCnv summary: unknown data-> " << m_unknown_data_format << " Unknown data format found");
  if (m_RODClockErrorNumber > 0)  ATH_MSG_INFO("SCT BytestreamCnv summary: ROD status word-> " << m_RODClockErrorNumber << " ROD clock errors found");
  if (m_maskedRODNumber > 0)      ATH_MSG_INFO("SCT BytestreamCnv summary: ROB status word-> " << m_maskedRODNumber << " masked RODs found");
  if (m_truncatedRODNumber > 0)   ATH_MSG_INFO("SCT BytestreamCnv summary: ROB status word-> " << m_truncatedRODNumber << " truncated ROBFragments");
  ATH_MSG_INFO("Number of SCT hits in ByteStream-> " << m_nHits);
  ATH_MSG_INFO("Number of SCT RDOs created->       " << m_nRDOs);

  if (m_numMissingLinkHeader > 0) ATH_MSG_WARNING("SCT Missing Link Headers found " << m_numMissingLinkHeader);
  if (m_numUnknownOfflineId  > 0) ATH_MSG_WARNING("SCT unknown onlineId found " << m_numUnknownOfflineId);


  ATH_CHECK(AlgTool::finalize());
  ATH_MSG_DEBUG("SCT_RodDecoder::finalize()");

  return StatusCode::SUCCESS;
}


StatusCode
SCT_RodDecoder::fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,
                               ISCT_RDO_Container& rdoIdc,
                               InDetBSErrContainer* errs,
                               SCT_ByteStreamFractionContainer* bsFracCont,
                               std::vector<IdentifierHash>* vecHash)
{
  uint32_t robid{robFrag.rod_source_id()};
  /**determine whether this data was generated using the ROD simulator */
  uint32_t rod_datatype{robFrag.rod_detev_type()};
  const bool rodSimulatedData{static_cast<bool>((rod_datatype >> 20) & 1)};
  if (bsFracCont) bsFracCont->insert(SCT_ByteStreamFractionContainer::SimulatedData, robid, rodSimulatedData);

  /** look for the bit that denotes "Super-condensed" mode.*/
  m_superCondensedMode = ((rod_datatype >> 21) & 1);   
  if (bsFracCont) bsFracCont->insert(SCT_ByteStreamFractionContainer::SuperCondensedMode, robid, m_superCondensedMode);

  int strip{0};
  int oldstrip{-1};
  int oldside{-1};
  int chip{0};
  int side{0};
  int nStripsInWord{0};
  int linkNb{0};
  uint32_t onlineId{0};
  int tbin{0};
  int groupSize{0};

  bool saved[768*2]{false};
  int ABCerror{0};
  int wordcount{-1};
  RawWord robData;
  robData.word32=0;
  int n;
  int ERRORS{0}; /** encodes the errors on the header
                  * bit 4: error in condensed mode 1st hit
                  * bit 5: error in condensed mode 2nd hit
                  */

  /** These are for the trigger */
  CacheHelper cache;
  cache.vecHash = vecHash;
  
  IdentifierHash currentLinkIdHash;

  std::vector<int> errorHit;

  StatusCode sc{StatusCode::SUCCESS};

  /// look at ROB status word ////////////////////////

  if (robFrag.nstatus()!=0) {
    const uint32_t* rob_status;
    robFrag.status(rob_status);
    if ((*rob_status)!=0) {
      ATH_MSG_DEBUG("ROB status word for robid " << std::hex << robid << " is non-zero " << (*rob_status) << std::dec);
      /// first store generic "ROBFragmentError" error type..
      addRODError(robid,SCT_ByteStreamErrors::ROBFragmentError, errs);
      sc = StatusCode::RECOVERABLE;
      /// now look for specific problems, e.g. truncated or masked-off RODs
      if (((*rob_status) >> 27) & 0x1) {
        ATH_MSG_DEBUG("ROB status word for robid " << std::hex << robid << std::dec << " indicates data truncation.");
        addRODError(robid,SCT_ByteStreamErrors::TruncatedROD, errs);
        m_truncatedRODNumber++;
        return sc;
      }
      if ((((*rob_status) >> 29) & 0x1) or (((*rob_status) >> 31) & 0x1)) {
        ATH_MSG_DEBUG("ROB status word for robid " << std::hex << robid << std::dec << " indicates resource was masked off.");
        addRODError(robid,SCT_ByteStreamErrors::MaskedROD, errs);
        m_maskedRODNumber++;
        return sc;
      }
    }
  }
  
  /// look at ROD status words /////////

  OFFLINE_FRAGMENTS_NAMESPACE::PointerType vRodStatus;
  long unsigned int vRodStatusSize{robFrag.rod_nstatus()};

  robFrag.rod_status(vRodStatus);
  for (long unsigned int j{0}; j<vRodStatusSize; j++) {
    uint32_t statusWord{vRodStatus[j]};
    /** check for clock errors in second ROD status word */
    if (j==1) {
      int timClockError{static_cast<int>((statusWord >> 16) & 0x1)};
      int bocClockError{static_cast<int>((statusWord >> 17) & 0x1)};
      if (timClockError or bocClockError) {
        ATH_MSG_DEBUG(" Clock error in ROD status word: " << timClockError << " " << bocClockError);
        addRODError(robid,SCT_ByteStreamErrors::RODClockError, errs);
        m_RODClockErrorNumber++;
        sc=StatusCode::RECOVERABLE;
      }
      /** look at bits 20-23 for DCS HV */
      int hvBits{static_cast<int>((statusWord >> 20) & 0xf)};
      const bool hvOn{hvBits==0xf};
      if (bsFracCont) bsFracCont->insert(SCT_ByteStreamFractionContainer::HVOn, robid, hvOn);
    }
  }
  
  /** now look at the data words */

  bool foundHeader{false};
  
  OFFLINE_FRAGMENTS_NAMESPACE::PointerType vRobData;
  unsigned long int vRobDataSize{robFrag.rod_ndata()};
  robFrag.rod_data(vRobData);
  
  for (unsigned long int i{0}; i<vRobDataSize; i++) {
    wordcount++;
    robData.word32 = vRobData[i];
    /** the data is 16-bits wide packed to a 32-bit word (rob_it1). So we unpack it here. */
    uint16_t d[2];
    d[1] = robData.word16[0];
    d[0] = robData.word16[1];
    
    for (n=0; n<2; n++) {
      ///---------------------------------------------------------------------
      /// hit element
      ///---------------------------------------------------------------------
      if (d[n]&0x8000) {
        if (!foundHeader) {
          ATH_MSG_INFO(" Missing link header in ROD " << std::hex << robid << std::dec);
          addRODError(robid,SCT_ByteStreamErrors::MissingLinkHeaderError, errs);
          m_numMissingLinkHeader++;
          sc = StatusCode::RECOVERABLE;
          continue;
        }

        m_nHits++;
        if (m_superCondensedMode) {

          /** super-condensed mode
           *  chip info : 4 bits  d[n]>>11)0xF
           *  chip number == (d[n]>>11)&0x7 and chip side == (d[n]>>14)&0x1
           *  For example if d[n]>>11)0xF = 0101 => chip5 or chip5 on side0, d[n]>>11)0xF = 1101 => chip13 or chip5 on side1
           */
          chip  = ((d[n]>>11)&0x7); 
          side = ((d[n]>>14)&0x1);
          strip = chip*128 + ((d[n]>>4)&0x7F);
          tbin = 0x2; /** assuming tbin is 010 in super-condensed mode */
          nStripsInWord = (d[n]&0xf)+1;
          if (chip>5) {
            ATH_MSG_DEBUG("    Hit super-condensed : xxx Chip number = " << chip << " > 5 " << " for hit " << std::hex << d[n]);
            m_chip_number_error++;
            addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError, errs);
            sc=StatusCode::RECOVERABLE;
            continue;
          }

          /**------------ Search for redundancy only for the master chip  */
          if ((side == 1) and ((linkNb%2)==0)) {
            if (((strip != oldstrip) or (side!=oldside)) and (groupSize>0)) { /** if it is a new cluster,
                                                                               * make RDO with the previous cluster */
              int rdoMade{this->makeRDO(oldstrip, groupSize, tbin, onlineId, ERRORS, rdoIdc, cache, errorHit)};
              if (rdoMade == -1) {
                sc=StatusCode::RECOVERABLE;
                addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError, errs);
              } else {
                saved[oldside*768+oldstrip] = rdoMade; 
              }
              oldstrip = strip;
              oldside = side;
              groupSize = 0;
            }
            linkNb++;
          }
          if ((side == 0) and ((linkNb%2)!=0)) {
            if (((strip != oldstrip) or (side!=oldside)) and (groupSize>0)) { /** if it is a new cluster,
                                                                               * make RDO with the previous cluster */
              int rdoMade{this->makeRDO(oldstrip, groupSize, tbin, onlineId, ERRORS, rdoIdc, cache, errorHit)};
              if (rdoMade == -1) {
                sc=StatusCode::RECOVERABLE;
                addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError, errs);
              } else {
                saved[oldside*768+oldstrip] = rdoMade; 
              }
              oldstrip = strip;
              oldside = side;
              groupSize = 0;
            }
            linkNb--;
          }
          onlineId = ((robid & 0xFFFFFF)|(linkNb << 24));
     
          if (groupSize == 0)  {
            oldstrip = strip; /** if it's the first super-condensed word */
            oldside = side;
          }
          
          if ((strip != oldstrip) or (side!=oldside)) {
            /** if it is a new cluster,
             * make RDO with the previous cluster */
            int rdoMade{this->makeRDO(oldstrip, groupSize, tbin, onlineId, ERRORS, rdoIdc, cache, errorHit)};
            if (rdoMade == -1) {
              sc=StatusCode::RECOVERABLE;
              addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError, errs);
            } else {
              saved[oldside*768+oldstrip] = rdoMade; 
            }
            oldstrip = strip;
            oldside = side;
            groupSize = 0;
          }
          groupSize+=nStripsInWord; // Split clusters have the same strip number.
     
        } else if (m_condensedMode) {
    
          /** condensed mode
           *  chip info : 4 bits  d[n]>>11)0xF
           *  chip number == (d[n]>>11)&0x7 and chip side == (d[n]>>14)&0x1
           *  For example if d[n]>>11)0xF = 0101 => chip5 or chip5 on side0, d[n]>>11)0xF = 1101 => chip13 or chip5 on side1
           */
          chip  = ((d[n]>>11)&0x7); 
          side = ((d[n]>>14)&0x1);
          strip = chip*128 + ((d[n]>>4)&0x7F);
          tbin = 0x2; /** assuming tbin is 010 in condensed mode */
          if (chip>5) {
            ATH_MSG_DEBUG("    Hit condensed : xxx Chip number = " << chip << " > 5 " << " for hit " << std::hex << d[n]);
            m_chip_number_error++;
            addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError, errs);
            sc=StatusCode::RECOVERABLE;
            continue;
          }

          /**------------ Search for redundancy only for the master chip  */
          if ((side == 1) and ((linkNb%2)==0)) {
            if (((strip != oldstrip) or (side!=oldside)) and (groupSize>0)) { /** if it is a new cluster,
                                                                               * make RDO with the previous cluster */
              int rdoMade{this->makeRDO(oldstrip, groupSize, tbin, onlineId, ERRORS, rdoIdc, cache, errorHit)};
              if (rdoMade == -1) {
                sc=StatusCode::RECOVERABLE;
                addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError, errs);
              } else {
                saved[oldside*768+oldstrip] = rdoMade; 
              }
              oldstrip = strip;
              oldside = side;
              groupSize = 0;
            }
            linkNb++;
          }
          if ((side == 0) and ((linkNb%2)!=0)) {
            if (((strip != oldstrip) or (side!=oldside)) and (groupSize>0)) { /** if it is a new cluster,
                                                                               * make RDO with the previous cluster */
              int rdoMade{this->makeRDO(oldstrip, groupSize, tbin, onlineId, ERRORS, rdoIdc, cache, errorHit)};
              if (rdoMade == -1) {
                sc=StatusCode::RECOVERABLE;
                addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError, errs);
              } else {
                saved[oldside*768+oldstrip] = rdoMade; 
              }
              oldstrip = strip;
              oldside = side;
              groupSize = 0;
            }
            linkNb--;
          }
          onlineId = ((robid & 0xFFFFFF)|(linkNb << 24));
          if (groupSize == 0)  {
            oldstrip = strip; /** if it's the first condensed word */
            oldside = side;
          }
          if (!(d[n]&0x1)) { /** 1-hit */
            m_singleCondHitNumber++;
            if ((strip != oldstrip) or (side!=oldside)) {
              /** if it is a new cluster,
               * make RDO with the previous cluster */
              int rdoMade{this->makeRDO(oldstrip, groupSize, tbin, onlineId, ERRORS, rdoIdc, cache, errorHit)};
              if (rdoMade == -1) {
                sc=StatusCode::RECOVERABLE;
                addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError, errs);
              } else {
                saved[oldside*768+oldstrip] = rdoMade; 
              }
              oldstrip = strip;
              oldside = side;
              groupSize = 0;
            }
      
            if (d[n]&0x4) { /** Error in the hit */
              ATH_MSG_DEBUG("    Hit condensed : xxx ERROR in 1-hit " << std::hex << d[n]);
              errorHit.push_back(groupSize);
              ERRORS = (ERRORS | 0x10);
              m_cond_hit1_error++;
              sc=StatusCode::RECOVERABLE;
            }
            groupSize = (groupSize>=2 ? groupSize : 1);
          } else { /** 2-hits */
            if (strip > 767) {
              addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError, errs);
              sc=StatusCode::RECOVERABLE;

              ATH_MSG_DEBUG("Condensed mode - strip number out of range");

              continue;
            }
            m_pairedCondHitNumber++;
            if ((strip != oldstrip) or (side!=oldside)) { /** if it is a new cluster,
                                                           * make RDO with the previous cluster 
                                                           */
              int rdoMade{this->makeRDO(oldstrip, groupSize, tbin, onlineId, ERRORS, rdoIdc, cache, errorHit)};
              if (rdoMade == -1) {
                sc=StatusCode::RECOVERABLE;
                addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError, errs);
              } else {
                saved[oldside*768+oldstrip] = rdoMade; 
              }
              oldstrip = strip;
              oldside = side;
              groupSize = 0;
            }    
            if (d[n]&0x4) { /** Error in the first hit */
              ATH_MSG_DEBUG("    Hit condensed : xxx ERROR in 1st hit" << std::hex << d[n]);
              errorHit.push_back(groupSize);
              m_cond_hit1_error++;
              ERRORS = (ERRORS | 0x10);
              sc=StatusCode::RECOVERABLE;
            }
            if (d[n]&0x8) { /** Error in the second hit */
              ATH_MSG_DEBUG("    Hit condensed : xxx ERROR in 2nd hit" << std::hex << d[n]);
              errorHit.push_back(groupSize);
              m_cond_hit1_error++;
              ERRORS = (ERRORS | 0x20);
              sc=StatusCode::RECOVERABLE;
            }
            groupSize = 2;
          }
        } else {
          /** Expanded mode
           * chip info from the first word of expanded cluster : 4 bits  d[n]>>11)0xF
           * chip number == (d[n]>>11)&0x7 and chip side == (d[n]>>14)&0x1
           * For example if d[n]>>11)0xF = 0101 => chip5 or chip5 on side0, d[n]>>11)0xF = 1101 => chip13 or chip5 on side1
           */

          if (!(d[n]&0x8)) {  /** 1st hit cluster expanded */
            m_firstExpHitNumber++;
            chip  = ((d[n]>>11)&0x7);  
            side = ((d[n]>>14)&0x1);
            strip = chip*128 + ((d[n]>>4)&0x7F);
            tbin = d[n]&0x7; /** Real way for obtaining tbin info */
      
            if (chip>5) {
              ATH_MSG_DEBUG("Expanded hit: First hit xxx ERROR chip Nb = " << chip << " > 5");   
              m_chip_number_error++;
              addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError, errs);
              continue;
            }
      

            /** -------------- Search for redundancy only for the master chip  */
            if ((side == 1) and ((linkNb%2)==0))  {
              linkNb++;
            }
            if ((side == 0) and ((linkNb%2)!=0)) {
              linkNb--;
            }
            onlineId = ((robid & 0xFFFFFF)|(linkNb << 24)); 
            groupSize =  1;
            int rdoMade{this->makeRDO(strip, groupSize, tbin, onlineId, ERRORS, rdoIdc, cache, errorHit)};
            if (rdoMade == -1) {
              sc=StatusCode::RECOVERABLE;
              addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError, errs);
            } else {
              saved[side*768+strip] = rdoMade; 
            }
            groupSize = 0;
          } else {  /** next hits cluster expanded */
            if (d[n]&0x80) { /** paired hits */
              if (strip > 767) {
                addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError, errs);
                sc=StatusCode::RECOVERABLE;
                ATH_MSG_DEBUG("Expanded mode - strip number out of range");
                continue;
              }
              m_evenExpHitNumber++;
              if (chip>5) {
                ATH_MSG_DEBUG("Expanded Hit: paired hits xxx ERROR chip Nb = " << chip << " > 5");  
                m_chip_number_error++;
                addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError, errs);
                continue;
              }
              /** first hit from the pair  */
              strip++;
              tbin = d[n]&0x7;
              groupSize = 1;
              int rdoMade{this->makeRDO(strip, groupSize, tbin, onlineId, ERRORS, rdoIdc, cache, errorHit)};
              if (rdoMade == -1) {
                sc=StatusCode::RECOVERABLE;
                addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError, errs);
              } else {
                saved[side*768+strip] = rdoMade; 
              }
              /** second hit from the pair */
              strip++;
              tbin = (d[n] >> 4) & 0x7;
              rdoMade = this->makeRDO(strip, groupSize, tbin, onlineId, ERRORS, rdoIdc, cache, errorHit);
              if (rdoMade == -1) {
                sc=StatusCode::RECOVERABLE;
                addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError, errs);
              } else {
                saved[side*768+strip] = rdoMade; 
              }
              groupSize = 0;
            } else {  /** Last hit of the cluster */
              m_lastExpHitNumber++;
              if (chip>5) {
                ATH_MSG_DEBUG("Expanded Hit: last hit xxx ERROR chip Nb = " << chip << " > 5");  
                m_chip_number_error++;
                addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError, errs);
                continue;
              }     
              strip++;
              tbin = d[n]&0x7;
              groupSize = 1;
              int rdoMade{this->makeRDO(strip, groupSize, tbin, onlineId, ERRORS, rdoIdc, cache, errorHit)};
              if (rdoMade == -1) {
                sc=StatusCode::RECOVERABLE;
                addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError, errs);
              } else {
                saved[side*768+strip] = rdoMade; 
              }
              groupSize = 0; 
            }
          }
        } // end expanded mode
      }// end of hit element
      
      
      ///---------------------------------------------------------------------
      /// Header
      ///---------------------------------------------------------------------
      else if (((d[n]>>13)&0x7) == 0x1) {
        foundHeader=true;
  
        m_headnumber++;
        if (saved[side*768+strip]==false and oldstrip>=0) {
          int rdoMade{this->makeRDO(strip, groupSize, tbin, onlineId, ERRORS, rdoIdc, cache, errorHit)};
          if (rdoMade == -1) {
            sc=StatusCode::RECOVERABLE;
            addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError, errs);
          } else {
            saved[side*768+strip] = rdoMade; 
          }
        }
  
        /** Everything is set to default for a new hunt of RDO */
        strip =0;
        oldstrip = -1;
        oldside = -1;
        groupSize = 0;
        //ErrorHeader = false;
        ERRORS = 0;
        memset(saved,0,768*2);
        errorHit.clear();

        /** Link Number (or stream) in the ROD fragment */
        int rodlinkNb{static_cast<int>(d[n] & 0x7F)};

        /** This is the real calculation for the offline  */
        linkNb = (((rodlinkNb >>4)&0x7)*12+(rodlinkNb &0xF));
        onlineId = ((robid & 0xFFFFFF)|(linkNb << 24));     
        if ((onlineId ==0) or (linkNb > 95)) {
          addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError, errs);
          sc=StatusCode::RECOVERABLE;
          ATH_MSG_DEBUG("Header: xxx Link nb out of range (skipping following data)" << std::dec << linkNb);
          break;
        } else {
          currentLinkIdHash = m_cabling->getHashFromOnlineId(onlineId);
        }
        /// look for masked off links - bit 7
        if (d[n] >> 7 & 0x1) {
          ATH_MSG_DEBUG("Masked link " << onlineId << " " << currentLinkIdHash);
          //no counter increment here, is that correct? (sar)
          addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::MaskedLink, errs);
          sc=StatusCode::RECOVERABLE; 
        }
        if (d[n]&0x800) {
          ATH_MSG_DEBUG("    Header: xxx TimeOut Error " << currentLinkIdHash);
          m_head_error_timeout++;
          addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::TimeOutError, errs);
          sc=StatusCode::RECOVERABLE;
        }
  
        if (d[n]&0x1000) {
          ATH_MSG_DEBUG("    Header: xxx Preamble Error " << currentLinkIdHash);
          m_head_error_preamb++;
          addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::PreambleError, errs);
          sc=StatusCode::RECOVERABLE;
        }
  
        if (d[n]&0x400) {
          ATH_MSG_DEBUG("    Header: xxx LVL1 ID Error " << currentLinkIdHash);
          m_head_error_lvl1id++;
          addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::LVL1IDError, errs);
          sc=StatusCode::RECOVERABLE;
        }
  
        if (d[n]&0x200) {
          ATH_MSG_DEBUG("    Header: xxx BCID Error " << currentLinkIdHash);
          m_head_error_bcid++;
          addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::BCIDError, errs);
          sc=StatusCode::RECOVERABLE;
        }
  
        if ((d[n]&0xF) > 11) {
          ATH_MSG_DEBUG("    Header: xxx Error in formatter " << currentLinkIdHash);
          m_head_error_formatter++;
          addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::FormatterError, errs);
          sc=StatusCode::RECOVERABLE;
        }

        m_condensedMode = static_cast<bool>(d[n]&0x100);

        continue;
  
      } // end header
      
      ///---------------------------------------------------------------------
      /// trailer
      ///---------------------------------------------------------------------
      else if (((d[n]>>13)&0x7) == 0x2) {
        foundHeader=false;
  
        m_trailnumber++;
        //ErrorTrailer = false;
  
        if (d[n]&0x1000) {
          //ErrorTrailer = true;
          ATH_MSG_DEBUG("    Trailer: xxx Trailer ERROR " << std::hex << d[n]);
          m_trail_error_bit++;
          addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::TrailerError, errs);
          sc=StatusCode::RECOVERABLE;
        }
  
        if (d[n]&0x800) {
          //ErrorTrailer = true;
          /** no data should appear between header and trailer
              See 1.2.2 Formatter FPGA - Serial Data Decoding and Formatting of
              http://www-eng.lbl.gov/~jmjoseph/Atlas-SiROD/Manuals/usersManual-v164.pdf */
          ATH_MSG_DEBUG("    Trailer: xxx Header-Trailer limit ERROR " << std::hex << d[n]);
          m_trail_error_limit++;
          addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::HeaderTrailerLimitError, errs);
          sc=StatusCode::RECOVERABLE;
        }
  
        if (d[n]&0x400) {
          //ErrorTrailer = true; /** not sure if there are hit elements before (probably yes but in principle they are fine) */
          ATH_MSG_DEBUG("    Trailer: xxx Data Overflow ERROR " << std::hex << d[n]);
          m_trail_error_overflow++;
          addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::TrailerOverflowError, errs);
          sc=StatusCode::RECOVERABLE;
        }
        if (d[n] & 0xF) {
          // fisrt temporarily masked chip information
          // 0 means no masked chip (always has been 0 until April 2017)
          // 
          // If Rx redundacy is not used, 
          // 1 means chips 0-5 are temporarily masked. 
          // 6 means chip 5 is temporarily masked. 
          // 7 means chips 6-11 are temporarily masked. 
          // 12 means chip 11 is temporarily masked. 
          // 
          // If Rx redundacy is used and link-1 is not used,
          // 1 means chips 0-11 are temporarily masked. 
          // 6 means chips 5-11 are temporarily masked. 
          // 7 means chips 6-11 are temporarily masked. 
          // 12 means chip 11 is temporarily masked. 
          // 
          // If Rx redundacy is used and link-0 is not used,
          // 1 means chips 0-5 are temporarily masked. 
          // 6 means chip 5 is temporarily masked. 
          // 7 means chips 6-11, 0-5 are temporarily masked. 
          // 12 means chips 11, 0-5 are temporarily masked. 
          setFirstTempMaskedChip(currentLinkIdHash, (d[n] & 0xF), errs);
        }
        continue; 
      }
      
      ///---------------------------------------------------------------------
      /// FlaggedABCD error
      /// 000xxxxxxFFFFEEE
      /// 000: FlaggedABCD error: xxxxxxx not used, FFFF: chip, EEE: error code
      ///---------------------------------------------------------------------
      else if (((d[n]>>13)&0x7) == 0x0) {
        chip = ((d[n]>>3)&0xF);
        ABCerror = d[n]&0x7; 
        /** no data should appear for that chip but how do we 
         * want to transmit this information ? */
        IdentifierHash flagIdHash{0};
        if (onlineId == 0) {
          addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError, errs);
          continue;
        } else {
          flagIdHash = m_cabling->getHashFromOnlineId(onlineId);
        }
        ATH_MSG_DEBUG(" xxx Flagged ABCD ERROR in chip " << chip << " Error code ABCerror " << ABCerror << " Link Nb (or Stream) "<<linkNb);
        m_flag_error_bit++;
        // Error code of ABCD error should be 1, 2, 4 or 7.
        if (ABCerror!=0x1 and ABCerror!=0x2 and ABCerror!=0x4 and ABCerror!=0x7) {
          ATH_MSG_DEBUG("ABCD error has an invalid error code " << ABCerror <<
                        " the 16-bit word is 0x" << std::hex << d[n] << std::dec << " for hash " << flagIdHash);
          addSingleError(flagIdHash, SCT_ByteStreamErrors::ABCDError_Invalid, errs);
        } else {
          // Chip is 4 bits. The highest bit 3 represents side. Chip 0-5 on side 0 and chip 8-13 on side 1.
          unsigned int side_ABCDError{static_cast<unsigned int>(chip/8)};
          if (flagIdHash.value()%2!=side_ABCDError) {
            // If the sides from the ABCD error and online ID are different,
            // the module is expected to read side 0 via link 1 and side 1 and via link 0.
            // Hash Id is flipped.
            ATH_MSG_DEBUG("ABCD error and online ID have different side information for hash " << flagIdHash << ". " <<
                          side_ABCDError << " from ABCD error and " << flagIdHash.value()%2 << " from online ID");
            flagIdHash = (flagIdHash.value()/2)*2+side_ABCDError;
          }
          // Chip should be 0-5 or 8-13.
          if (chip%8>=6) {
            ATH_MSG_DEBUG("ABCD error has an invalid chip 0x" << std::hex << chip << std::dec <<
                          " the 16-bit word is 0x" << std::hex << d[n] << std::dec <<
                          " for hash " << flagIdHash.value());
            addSingleError(flagIdHash, SCT_ByteStreamErrors::ABCDError_Invalid, errs);
          } else {
            if (     ABCerror==0x1) addSingleError(flagIdHash, SCT_ByteStreamErrors::ABCDError_Error1, errs);
            else if (ABCerror==0x2) addSingleError(flagIdHash, SCT_ByteStreamErrors::ABCDError_Error2, errs);
            else if (ABCerror==0x4) addSingleError(flagIdHash, SCT_ByteStreamErrors::ABCDError_Error4, errs);
            else if (ABCerror==0x7) addSingleError(flagIdHash, SCT_ByteStreamErrors::ABCDError_Error7, errs);
            if (     chip%8==0) addSingleError(flagIdHash, SCT_ByteStreamErrors::ABCDError_Chip0, errs);
            else if (chip%8==1) addSingleError(flagIdHash, SCT_ByteStreamErrors::ABCDError_Chip1, errs);
            else if (chip%8==2) addSingleError(flagIdHash, SCT_ByteStreamErrors::ABCDError_Chip2, errs);
            else if (chip%8==3) addSingleError(flagIdHash, SCT_ByteStreamErrors::ABCDError_Chip3, errs);
            else if (chip%8==4) addSingleError(flagIdHash, SCT_ByteStreamErrors::ABCDError_Chip4, errs);
            else if (chip%8==5) addSingleError(flagIdHash, SCT_ByteStreamErrors::ABCDError_Chip5, errs);
          }
        }
        addSingleError(flagIdHash, SCT_ByteStreamErrors::ABCDError, errs);
        sc=StatusCode::RECOVERABLE;
        continue;
      } else if (((d[n]>>13)&0x7) == 0x3) {
        ///---------------------------------------------------------------------
        /// Raw Data
        ///---------------------------------------------------------------------
        IdentifierHash rawIdHash{0};
        if (onlineId == 0) {
          addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError, errs);
          continue;
        } else {
          rawIdHash = m_cabling->getHashFromOnlineId(onlineId);
        }

        ATH_MSG_DEBUG(" xxx Raw Data Mode " << std::hex << d[n] << std::dec << ": Config Data Mode ");
        /** too many errors in the BS for the ROD to decode the data */
        m_config_data_bit++;
        addSingleError(rawIdHash, SCT_ByteStreamErrors::RawError, errs);
        sc=StatusCode::RECOVERABLE;
        continue;
      } else {
        ATH_MSG_DEBUG("Data word format unknown ");
        m_unknown_data_format++;
        addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError, errs);
        sc=StatusCode::RECOVERABLE;
      }
    } //end of 16-bit word loop
  }   //end of 32-bit word loop

  /** create RDO of the last ink or stream of the event */
  if (saved[side*768+strip]==false and oldstrip>=0) {
    int rdoMade{makeRDO(strip, groupSize, tbin, onlineId, ERRORS, rdoIdc, cache, errorHit)};
    if (rdoMade == -1) {
      sc=StatusCode::RECOVERABLE;
      addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError, errs);
    } else {
      saved[side*768+strip] = rdoMade; 
    }
  }

  if (bsFracCont) bsFracCont->insert(SCT_ByteStreamFractionContainer::CondensedMode, robid, m_condensedMode);

  if (sc.isFailure()) ATH_MSG_DEBUG("One or more ByteStream errors found ");
  return sc;
}
/** makeRDO has 3 possible return values: 
 *          1 if RDO was successfully created,
 *          0 if collection was deliberately skipped (for trigger)
 *         -1 if there was an error in the decoding - will be passed
 *            on as StatusCode::RECOVERABLE by fillCollection().
 */

int SCT_RodDecoder::makeRDO(int strip, int groupSize, int tbin, uint32_t onlineId, int ERRORS, ISCT_RDO_Container& rdoIdc,
                            CacheHelper& cache,
                            const std::vector<int>& errorHit)
{

  // IIncidentSvc* incsvc;

  if (onlineId == 0x0) {
    ATH_MSG_WARNING("No link header found, possibly corrupt ByteStream.  Will not try to make RDO");
    return -1;
  }
  /** get offlineId from the link number and ROB number */
  IdentifierHash idCollHash{m_cabling->getHashFromOnlineId(onlineId)};
  if (not idCollHash.is_valid()) {
    m_numUnknownOfflineId++;
    ATH_MSG_ERROR("Unknown OfflineId for OnlineId -> cannot create RDO");
    ATH_MSG_WARNING("Unknown OfflineId for OnlineId " << std::hex << onlineId << " -> cannot create RDO" << std::dec);

    //fire an incident whenever there is a "unknown offline id..." so they are listened by /InnerDetector/InDetCalibAlgs/SCT_CalibAlgs/src/SCT_CalibEventInfo
    m_incidentSvc->fireIncident(Incident(name(), "UnknownOfflineId"));
    return -1;
  }

  if (((strip & 0x7f) + (groupSize-1) > 127) or (strip<0) or (strip>767)) {
    ATH_MSG_WARNING("Cluster with " << groupSize << " strips, starting at strip " << strip << " in collection " << idCollHash << " out of range. Will not make RDO");
    return -1;
  }

  /** this option is for the trigger, if there is a vecHash* given, test it ! */
  if (cache.vecHash) {
    if (idCollHash == cache.skipHash) {
      ATH_MSG_VERBOSE("Collection for Hash not to be decoded, skip");
      return 0;
    } else if (idCollHash != cache.lastHash) {
      cache.lastHash = idCollHash;
      /** maybe the new hash is not in the list, so test it */
      std::vector<IdentifierHash>::iterator p{find(cache.vecHash->begin(), cache.vecHash->end(), idCollHash)};
      if (p == cache.vecHash->end()) {
        ATH_MSG_VERBOSE("Collection for Hash not to be decoded, skip");
        /** remember this one, so that we do not find(...) forever */
        cache.skipHash = idCollHash;
        return 0;
      }
    }
  }


  if(rdoIdc.hasExternalCache() && rdoIdc.tryFetch(idCollHash)){
    ATH_MSG_DEBUG("Hash already in collection - cache hit " << idCollHash);
    return 0;
  }

  /** get identifier from the hash, this is not nice */
  Identifier idColl{m_sct_id->wafer_id(idCollHash)};


  /** see if strips go from 0 to 767 or vice versa */
  const InDetDD::SiDetectorElement* p_element{m_indet_mgr->getDetectorElement(idCollHash)};
  if (p_element->swapPhiReadoutDirection()) {
    strip = 767 - strip;
    strip = strip-(groupSize-1);
  }

  Identifier iddigit{m_sct_id->strip_id(idColl,strip)};
  if (not m_sct_id->is_sct(iddigit)) {
    ATH_MSG_WARNING("Cluster with invalid Identifier. Will not make RDO");
    return -1;
  }
  unsigned int rawDataWord{static_cast<unsigned int>(groupSize | (strip << 11) | (tbin <<22) | (ERRORS << 25))};

  ATH_MSG_DEBUG("Output Raw Data " << std::hex << " Coll " << idColl.getString() << ":-> " << m_sct_id->print_to_string(iddigit) << std::dec);



  SCT_RDO_Collection* col = nullptr;
  ATH_CHECK(rdoIdc.naughtyRetrieve(idCollHash, col), 0);//Returns null if not present

  if(!col){
    ATH_MSG_DEBUG(" Collection ID = " << idCollHash << " does not exist, create it ");
    /** create new collection */   
    col  = new SCT_RDO_Collection(idCollHash);
    col->setIdentifier(idColl);
    StatusCode sc = rdoIdc.addCollection(col, idCollHash);
    ATH_MSG_DEBUG("Adding " << idCollHash);
    if (sc.isFailure()){
        ATH_MSG_ERROR("failed to add SCT RDO collection to container");
    }
  }
  
  /** Now the Collection is there for sure. Create RDO and push it
   * into Collection. 
   */
  m_nRDOs++;
  col->push_back(std::make_unique<SCT3_RawData>(iddigit,
                                                     rawDataWord,
                                                     &errorHit));
  return 1;
}

void 
SCT_RodDecoder::addRODError(uint32_t rodid, int errorType,
                            InDetBSErrContainer* errs)
{
  std::vector<IdentifierHash> idHashes;
  m_cabling->getHashesForRod(idHashes, rodid);
  std::vector<IdentifierHash>::iterator hashIt{idHashes.begin()};
  std::vector<IdentifierHash>::iterator hashEnd{idHashes.end()};
  for (; hashIt != hashEnd; ++hashIt) {
    addSingleError(*hashIt, errorType, errs);
  }
  return;
}

bool 
SCT_RodDecoder::addSingleError(const IdentifierHash idHash,
                               const int bsErrorType,
                               InDetBSErrContainer* errs)
{
  bool ok{idHash.is_valid() and errs!=nullptr};
  if (ok) {
    errs->push_back(std::make_unique<std::pair<IdentifierHash, int> >(idHash, bsErrorType));
  }
  return ok;
}

void
SCT_RodDecoder::setFirstTempMaskedChip(const IdentifierHash& hashId, const unsigned int firstTempMaskedChip, InDetBSErrContainer* errs) {
  if (not hashId.is_valid()) {
    ATH_MSG_INFO("setFirstTempMaskedChip hashId " << hashId << " is invalid.");
    return;
  }
  if (firstTempMaskedChip==0) {
    ATH_MSG_WARNING("setFirstTempMaskedChip: firstTempMaskedChip should be greater than 0. firstTempMaskedChip is " << firstTempMaskedChip);
    return;
  }

  // wafer hash -> wafer id -> module id -> wafer hash on side-0, wafer hash on side-1
  const Identifier wafId{m_sct_id->wafer_id(hashId)};
  const Identifier moduleId{m_sct_id->module_id(wafId)};

  // Side 0
  IdentifierHash hash_side0;
  m_sct_id->get_hash(moduleId, hash_side0, &m_cntx_sct);
  unsigned int firstTempMaskedChip_side0{0};
  if (hashId==hash_side0) firstTempMaskedChip_side0 = firstTempMaskedChip;

  // Side 1
  IdentifierHash hash_side1;
  m_sct_id->get_other_side(hash_side0, hash_side1);
  unsigned int firstTempMaskedChip_side1{0};
  if (hashId==hash_side1) firstTempMaskedChip_side1 = firstTempMaskedChip;

  int type{0};
  // Check if Rx redundancy is used or not in this module
  std::pair<bool, bool> badLinks{m_configTool->badLinks(moduleId)};
  if (badLinks.first xor badLinks.second) {
    // Rx redundancy is used in this module.
    if (badLinks.first and not badLinks.second) {
      // link-1 is broken
      type = 1;
    } else if (badLinks.second and not badLinks.first) {
      // link-0 is broken
      type = 2;
    } else if (badLinks.first and badLinks.second) {
      // both link-0 and link-1 are working
      ATH_MSG_WARNING("setFirstTempMaskedChip: Both link-0 and link-1 are working. But Rx redundancy is used... Why?");
      return;
    } else {
      // both link-0 and link-1 are broken
      ATH_MSG_WARNING("setFirstTempMaskedChip: Both link-0 and link-1 are broken. But data are coming... Why?");
      return;
    }
  }

  // "Modified" module (using Rx redundancy) case
  // Information of modified modules are found in
  // modified0 and modified1 functions of SCT_ReadoutTool.cxx and
  // Table 3.8 of CERN-THESIS-2008-001 https://cds.cern.ch/record/1078223
  // However, there are two exceptions of the exceptions.
  unsigned long long fullSerialNumber{m_cabling->getSerialNumberFromHash(hashId).to_ulonglong()};
  if (// Readout through link-0
      fullSerialNumber==20220170200183 or // hash=4662 bec=0 layer=2 eta= 6 phi=39
      fullSerialNumber==20220330200606 or // hash=5032 bec=0 layer=3 eta=-2 phi= 7
      fullSerialNumber==20220330200693    // hash=5554 bec=0 layer=3 eta=-5 phi=29
      ) {
    if (type!=1) ATH_MSG_WARNING("Link-0 is broken but modified module readingout link-0, inconsistent");
    type = 3;

    // An exception:
    // fullSerialNumber=20220170200941, hash=3560 bec=0 layer=1 eta=-6 phi=34
    // This module is a modified one. However, it is not using modified configuration.
    // Readout sequence is 0, 1, 2, 3, 4, 5, 7, 8, 9, 10, 11.
  }
  if (// Readout through link-1
      fullSerialNumber==20220170200653 or // hash=2786 bec=0 layer=1 eta= 4 phi= 1
      fullSerialNumber==20220330200117 or // hash=5516 bec=0 layer=3 eta= 1 phi=27
      fullSerialNumber==20220330200209 or // hash=5062 bec=0 layer=3 eta= 2 phi= 8
      fullSerialNumber==20220330200505 or // hash=5260 bec=0 layer=3 eta= 5 phi=16
      fullSerialNumber==20220330200637 or // hash=4184 bec=0 layer=2 eta=-6 phi=20
      fullSerialNumber==20220330200701    // hash=4136 bec=0 layer=2 eta=-6 phi=18
      ) {
    if (type!=2) ATH_MSG_WARNING("Link-1 is broken but modified module readingout link-1, inconsistent");
    type = 4;

    // Another exception:
    // fullSerialNumber=20220170200113, hash=3428 bec=0 layer=1 eta= 1 phi=28
    // This module is a modified one. However, it is not using modified configuration.
    // Readout sequence is 6, 7, 8, 9, 10, 11, 1, 2, 3, 4, 5.
  }

  static const int chipOrder[5][12]{
    // type=0 not prepared for both link-0 and link-1 are working
    {},
    // type=1 link-1 is broken: chip 0 1 2 3 4 5 6 7 8 9 10 11
    {0, 1, 2, 3,  4,  5, 6, 7, 8,  9, 10, 11},
    // type=2 link-0 is broken: chip 6 7 8 9 10 11 0 1 2 3 4 5
    {6, 7, 8, 9, 10, 11, 0, 1, 2,  3,  4,  5},
    // type=3 "modified" modules and link-1 is broken: chip 0 1 2 3 4 5 7 8 9 10 11 6
    {0, 1, 2, 3,  4,  5, 7, 8, 9, 10, 11,  6},
    // type=4 "modified" modules and link-0 is broken: chip 6 7 8 9 10 11 1 2 3 4 5 0
    {6, 7, 8, 9, 10, 11, 1, 2, 3,  4,  5,  0}
  };

  if (type==0) {
    // both link-0 and link-1 are working

    // Chips 0-5 are on side 0 and chips 6-11 are on side 1.
    // Normally, modules send hits on side 0 via link-0 and side 1 via link-1.
    // The first temporally masked chip value is the id of the chip that is
    // first masked in the readout chain "plus one".
    // If the value is in between 1 to  6, it indicates side 0.
    // If the value is in between 7 to 12, it indicates side 1.
    // However, some special modules send hits on side 0 via link-1 and hits on
    // side 1 via link-0. If the first masked chip value on side 1 (0) is
    // between 1 to 6 (7 to 12), it indicates the module is a special one.
    // In that case, information is swapped.
    if ((6<firstTempMaskedChip_side0 and firstTempMaskedChip_side0<=12) or
        (0<firstTempMaskedChip_side1 and firstTempMaskedChip_side1<= 6)) {
      unsigned int swapFirstTempMaskedChip_side0{firstTempMaskedChip_side0};
      firstTempMaskedChip_side0 = firstTempMaskedChip_side1;
      firstTempMaskedChip_side1 = swapFirstTempMaskedChip_side0;
    }

    if (firstTempMaskedChip_side0>0) {
      for (unsigned int iChip{firstTempMaskedChip_side0-1}; iChip<6; iChip++) {
        addSingleError(hash_side0, SCT_ByteStreamErrors::TempMaskedChip0+iChip, errs);
      }
    }
    if (firstTempMaskedChip_side1>6) {
      for (unsigned int iChip{firstTempMaskedChip_side1-1}; iChip<12; iChip++) {
        addSingleError(hash_side1, SCT_ByteStreamErrors::TempMaskedChip0+iChip-6, errs);
      }
    }
  } else {
    // type=1, 2, 3, 4: cases using Rx redundancy
    bool toBeMasked{false};
    for (int iChip{0}; iChip<12; iChip++) {
      int jChip{chipOrder[type][iChip]};
      if (jChip==static_cast<int>(firstTempMaskedChip-1)) toBeMasked = true;
      if (toBeMasked) {
        if (jChip<6) addSingleError(hash_side0, SCT_ByteStreamErrors::TempMaskedChip0+jChip, errs);
        else         addSingleError(hash_side1, SCT_ByteStreamErrors::TempMaskedChip0+jChip-6, errs);
      }
    }
  }

  ATH_MSG_VERBOSE("setFirstTempMaskedChip Hash " << hashId
                  << " SerialNumber " << m_cabling->getSerialNumberFromHash(hashId).str()
                  << " moduleId " << moduleId
                  << " barrel_ec " << m_sct_id->barrel_ec(wafId)
                  << " layer_disk " << m_sct_id->layer_disk(wafId)
                  << " eta_module " << m_sct_id->eta_module(wafId)
                  << " phi_module " << m_sct_id->phi_module(wafId)
                  << " side " << m_sct_id->side(wafId)
                  << " firstTempMaskedChip " << firstTempMaskedChip);
}
