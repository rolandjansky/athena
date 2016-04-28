/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_RodDecoder.h"
//STL
#include <deque>
#include <vector>
#include <utility>
#include <algorithm>
//Gaudi
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IssueSeverity.h"
#include "GaudiKernel/IIncidentSvc.h"

//Athena
#include "eformat/Version.h"
#include "ByteStreamData/RawEvent.h"

//Inner detector
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "SCT_Cabling/ISCT_CablingSvc.h"
#include "SCT_ConditionsServices/ISCT_ByteStreamErrorsSvc.h"
#include <iostream>



union RawWord{
  uint32_t word32;
  uint16_t word16[2];
};

//constructor
SCT_RodDecoder::SCT_RodDecoder
( const std::string& type, const std::string& name,const IInterface* parent )
  :  AthAlgTool(type,name,parent),
     m_sct_id(0),
     m_indet_mgr(0),
     m_cabling("SCT_CablingSvc",name),
     m_byteStreamErrSvc("SCT_ByteStreamErrorsSvc",name),
     m_condensedMode(false),
     m_superCondensedMode(false),
     m_incidentSvc("IncidentSvc", name)
{
  declareProperty("CablingSvc",m_cabling);
  declareProperty("ErrorsSvc",m_byteStreamErrSvc);
  declareProperty("ByteStreamErrContainer",m_bsErrContainerName="SCT_ByteStreamErrs");
  declareProperty("TriggerMode",m_triggerMode=true);
  declareInterface< ISCT_RodDecoder  >( this );
}

//destructor  
SCT_RodDecoder::~SCT_RodDecoder() {
}

StatusCode SCT_RodDecoder::initialize() {
  StatusCode sc = AlgTool::initialize(); 

  if (sc.isFailure()) return sc;
 
  /** Retrieve cabling service */
  if (m_cabling.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve service " << m_cabling << endreq;
    return StatusCode::FAILURE;
  } else 
    msg(MSG::DEBUG) << "Retrieved service " << m_cabling << endreq;

  sc = detStore()->retrieve(m_indet_mgr,"SCT") ;
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Cannot retrieve SCT_DetectorManager!"  << endreq;
    return StatusCode::FAILURE;
  } 
  sc = detStore()->retrieve(m_sct_id,"SCT_ID");
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Cannot retrieve ID helper"  << endreq;
    return StatusCode::FAILURE;
  }

  if (m_byteStreamErrSvc.retrieve().isFailure()) 
    msg(MSG::FATAL) <<"Failed to get ByteStreamErrorSvc"<<endreq;

  IIncidentSvc* incsvc;
  sc = service("IncidentSvc", incsvc);
  int priority = 100;
  if( sc.isSuccess() ) {
    incsvc->addListener( this, "BeginEvent", priority);
    incsvc->addListener( this, "EndEvent", priority);
  }

  m_singleCondHitNumber  = 0 ;
  m_pairedCondHitNumber  = 0 ;
  m_firstExpHitNumber    = 0 ;
  m_evenExpHitNumber     = 0 ;
  m_lastExpHitNumber     = 0 ;
  m_headnumber           = 0 ;
  m_trailnumber          = 0 ;
  m_head_error_bcid      = 0 ;
  m_head_error_lvl1id    = 0 ;
  m_head_error_timeout   = 0 ;
  m_head_error_formatter = 0 ;
  m_head_error_preamb    = 0 ;
  m_trail_error_overflow = 0 ;
  m_trail_error_limit    = 0 ;
  m_trail_error_bit      = 0 ;
  m_config_data_bit      = 0 ;
  m_flag_error_bit       = 0 ;
  m_cond_hit1_error      = 0 ;
  m_cond_hit2_error      = 0 ;
  m_chip_number_error    = 0 ;
  m_unknown_data_format  = 0 ;
  m_nHits                = 0 ;
  m_nRDOs                = 0 ;
  m_maskedLinkNumber     = 0 ;
  m_maskedRODNumber      = 0 ;
  m_RODClockErrorNumber  = 0 ;
  m_truncatedRODNumber   = 0 ;
  m_numMissingLinkHeader = 0 ;
  m_numUnknownOfflineId  = 0 ;

  m_errorHit = new std::vector<int>;
  return StatusCode::SUCCESS ;
}
/** at the start of every event, create the InDetBSErrContainer and record it to SG */
void
SCT_RodDecoder::handle(const Incident& inc) {
  if ((inc.type() == "BeginEvent") && (! m_triggerMode) ){
    m_bsErrCont = new InDetBSErrContainer();
    StatusCode sc = evtStore()->record(m_bsErrCont,m_bsErrContainerName);
    if (sc.isFailure() ) msg(MSG::ERROR) << "Failed to record BSErrors to SG"<<endreq;
  }
  m_byteStreamErrSvc->setCondensedReadout(m_condensedMode);
  return;
}

StatusCode 
SCT_RodDecoder::finalize() {
  
  /** print out summaries of data and errors decoded */
  msg(MSG::INFO)<<"SCT BytestreamCnv summary: "<<m_headnumber  <<" link headers found"  <<endreq;
  if(m_condensedMode) {
    msg(MSG::INFO)<<"SCT decoding bytestream summary: "<<m_singleCondHitNumber <<" single strips with hit in condensed mode"  <<endreq;
    msg(MSG::INFO)<<"SCT decoding bytestream summary: "<<m_pairedCondHitNumber <<" paired strips with hit in condensed mode"  <<endreq;
  } else {
    msg(MSG::INFO)<<"SCT decoding bytestream summary: "<<m_firstExpHitNumber <<" first strips with hit in expanded mode"  <<endreq;
    msg(MSG::INFO)<<"SCT decoding bytestream summary: "<<m_evenExpHitNumber <<" consecutive paired strips with hit in expanded mode"  <<endreq;
    msg(MSG::INFO)<<"SCT decoding bytestream summary: "<<m_lastExpHitNumber <<" last consecutive strip with hit in expanded mode"  <<endreq;
  }

  msg(MSG::INFO)<<"SCT BytestreamCnv summary: "<<m_trailnumber <<" link trailers found" <<endreq;
  if (m_head_error_bcid > 0)      msg(MSG::INFO)<<"SCT BytestreamCnv summary: header-> "       <<m_head_error_bcid<<" BCID errors found"<<endreq;
  if (m_head_error_lvl1id > 0)    msg(MSG::INFO)<<"SCT BytestreamCnv summary: header-> "       <<m_head_error_lvl1id<<" LVL1d errors found"<<endreq;
  if (m_head_error_timeout > 0)   msg(MSG::INFO)<<"SCT BytestreamCnv summary: header-> "       <<m_head_error_timeout   <<" timeout errors found"<<endreq;
  if (m_head_error_formatter > 0) msg(MSG::INFO)<<"SCT BytestreamCnv summary: header-> "       <<m_head_error_formatter <<" formatter errors found"<<endreq;
  if (m_head_error_preamb > 0)    msg(MSG::INFO)<<"SCT BytestreamCnv summary: header-> "       <<m_head_error_preamb<<" preamble errors found"<<endreq;
  if (m_maskedLinkNumber > 0)     msg(MSG::INFO)<<"SCT BytestreamCnv summary: header-> " <<m_maskedLinkNumber<<" masked links found"<<endreq;
  if (m_trail_error_overflow > 0) msg(MSG::INFO)<<"SCT BytestreamCnv summary: trailer-> "      <<m_trail_error_overflow <<" trailer data overflow errors found"<<endreq;
  if (m_trail_error_limit > 0)    msg(MSG::INFO)<<"SCT BytestreamCnv summary: trailer-> "      <<m_trail_error_limit<<" header trailer limit errors found"<<endreq; 
  if (m_trail_error_bit > 0)      msg(MSG::INFO)<<"SCT BytestreamCnv summary: trailer-> "      <<m_trail_error_bit<<" trailer bit errors found"<<endreq; 
  if (m_config_data_bit > 0)      msg(MSG::INFO)<<"SCT BytestreamCnv summary: raw Data-> "     <<m_config_data_bit <<" raw data found: Config data mode"<<endreq;
  if (m_flag_error_bit > 0)       msg(MSG::INFO)<<"SCT BytestreamCnv summary: flag-> "         <<m_flag_error_bit<<" module link flag bit errors found"<<endreq;
  if (m_cond_hit1_error > 0)      msg(MSG::INFO)<<"SCT BytestreamCnv summary: hit-> "          <<m_cond_hit1_error<<" 1st hit error found in condensed mode" <<endreq;
  if (m_cond_hit2_error > 0)      msg(MSG::INFO)<<"SCT BytestreamCnv summary: hit-> "          <<m_cond_hit2_error<<" 2nd hit error found in condensed mode" <<endreq;
  if (m_chip_number_error > 0)    msg(MSG::INFO)<<"SCT BytestreamCnv summary: hit-> "          <<m_chip_number_error<<" Chip number > 5 error found"<<endreq;
  if (m_unknown_data_format > 0)  msg(MSG::INFO)<<"SCT BytestreamCnv summary: unknown data-> " <<m_unknown_data_format<<" Unknown data format found"<<endreq;
  if (m_RODClockErrorNumber > 0)  msg(MSG::INFO)<<"SCT BytestreamCnv summary: ROD status word-> " <<m_RODClockErrorNumber<<" ROD clock errors found"<<endreq;
  if (m_maskedRODNumber > 0)      msg(MSG::INFO)<<"SCT BytestreamCnv summary: ROB status word-> " <<m_maskedRODNumber<<" masked RODs found"<<endreq;
  if (m_truncatedRODNumber > 0)   msg(MSG::INFO)<<"SCT BytestreamCnv summary: ROB status word-> " <<m_truncatedRODNumber<<" truncated ROBFragments"<<endreq;
  msg(MSG::INFO)<<"Number of SCT hits in ByteStream-> "<<m_nHits<<endreq;
  msg(MSG::INFO)<<"Number of SCT RDOs created->       "<<m_nRDOs<<endreq;

  if (m_numMissingLinkHeader > 0) msg(MSG::WARNING)<<"SCT Missing Link Headers found "<<m_numMissingLinkHeader<<endreq;
  if (m_numUnknownOfflineId  > 0) ATH_MSG_WARNING("SCT unknown onlineId found "<<m_numUnknownOfflineId);


  StatusCode sc = AlgTool::finalize();
  if (sc.isFailure()) return sc ;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"SCT_RodDecoder::finalize()"<<endreq;
  delete m_errorHit;
  return sc;
}


StatusCode
SCT_RodDecoder::fillCollection( const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment* robFrag,SCT_RDO_Container* rdoIdc,std::vector<IdentifierHash>* vecHash ){
  uint32_t robid = robFrag->rod_source_id();
  /**determine whether this data was generated using the ROD simulator */
  uint32_t rod_datatype = robFrag->rod_detev_type();
  if ((rod_datatype >> 20 ) & 1)   m_byteStreamErrSvc->setRODSimulatedData();

  /** look for the bit that denotes "Super-condensed" mode.*/
  m_superCondensedMode =  ((rod_datatype >> 21 ) & 1);   


  int strip = 0;
  int oldstrip = -1;
  int chip = 0;
  int side = 0;
  int nStripsInWord=0;
  int linkNb = 0 ;
  uint32_t onlineId = 0;
  int tbin = 0;
  int groupSize = 0;
  //bool ErrorHeader = false;
  //bool ErrorTrailer = false;

  bool saved[768];
  int ABCerror = 0;
  int wordcount = -1;
  RawWord robData;
  robData.word32=0;
  int n;
  int ERRORS = 0; /** encodes the errors on the header
                  * bit 4: error in condensed mode 1st hit
                  * bit 5: error in condensed mode 2nd hit
      */

  /** These are for the trigger */
  IdentifierHash skipHash=0xffffffff, lastHash=0xffffffff;

  IdentifierHash currentLinkIdHash = 0xffffffff;

  memset(saved,0,768);
  m_errorHit->clear();

  StatusCode sc = StatusCode::SUCCESS;

  /// look at ROB status word ////////////////////////

  if (robFrag->nstatus() !=0) {  
    const uint32_t* rob_status;
    robFrag->status(rob_status);
    if ((*rob_status) != 0) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"ROB status word for robid "<<std::hex<<robid<<" is non-zero "<<(*rob_status)<<std::dec<<endreq;
      /// first store generic "ROBFragmentError" error type..
      addRODError(robid,SCT_ByteStreamErrors::ROBFragmentError);
      sc = StatusCode::RECOVERABLE;
      /// now look for specific problems, e.g. truncated or masked-off RODs
      if (((*rob_status) >> 27) & 0x1) {
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"ROB status word for robid "<<std::hex<<robid<<std::dec<<" indicates data truncation."<<endreq;
        addRODError(robid,SCT_ByteStreamErrors::TruncatedROD);
        m_truncatedRODNumber++;
        return sc;
      }
      if ((((*rob_status) >> 29) & 0x1) ||(((*rob_status) >> 31) & 0x1)) {
        if (msgLvl(MSG::DEBUG))  msg(MSG::DEBUG)<<"ROB status word for robid "<<std::hex<<robid<<std::dec<<" indicates resource was masked off."<<endreq;
        addRODError(robid,SCT_ByteStreamErrors::MaskedLink);
        m_maskedRODNumber++;
        return sc;
      }
    }
  }
  
  /// look at ROD status words /////////

  OFFLINE_FRAGMENTS_NAMESPACE::PointerType vRodStatus;
  int vRodStatusSize = robFrag->rod_nstatus();

  robFrag->rod_status(vRodStatus);
  for (int j=0; j< vRodStatusSize; j++) {
    uint32_t statusWord = vRodStatus[j];
    /** check for clock errors in second ROD status word */
    if (j ==1 ) {
      int timClockError = (statusWord >> 16) & 0x1;
      int bocClockError = (statusWord >> 17) & 0x1;
      if (timClockError || bocClockError) {
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<" Clock error in ROD status word: "<<timClockError<<" "<<bocClockError<<endreq;
        addRODError(robid,SCT_ByteStreamErrors::RODClockError);
        m_RODClockErrorNumber++;
        sc=StatusCode::RECOVERABLE;
      }
      /** look at bits 20-23 for DCS HV */
      int hvBits = (statusWord >> 20)& 0xf;
      if (hvBits==0xf) m_byteStreamErrSvc->addRODHVCounter(true); 
      else m_byteStreamErrSvc->addRODHVCounter(false); 
    }
  }
  
  /** now look at the data words */

  bool foundHeader = false;
  
  OFFLINE_FRAGMENTS_NAMESPACE::PointerType vRobData;
  int vRobDataSize = robFrag->rod_ndata();
  robFrag->rod_data( vRobData ); 
  
  for (int i=0; i<vRobDataSize; i++ ) {
    wordcount++;
    robData.word32 = vRobData[i] ;
    /** the data is 16-bits wide packed to a 32-bit word (rob_it1). So we unpack it here. */
    uint16_t d[2];
    d[1] = robData.word16[0];
    d[0] = robData.word16[1];
    
    for( n=0;n<2;n++){
      ///---------------------------------------------------------------------
      /// hit element
      ///---------------------------------------------------------------------
      if (d[n]&0x8000) {
	if (!foundHeader) {
	  msg(MSG::INFO)<<" Missing link header in ROD "<<std::hex<<robid<<std::dec<<endreq;
	  addRODError(robid,SCT_ByteStreamErrors::MissingLinkHeaderError);
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
           chip  = ((d[n]>>11)&0x7) ; 
           side = ((d[n]>>14)&0x1) ;
           strip = chip*128 + ((d[n]>>4)&0x7F) ;
           tbin = 0x2 ; /** assuming tbin is 010 in super-condensed mode */
           nStripsInWord = (d[n]&0xf)+1;
           if(chip>5){
             if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"    Hit super-condensed : xxx Chip number = "<<chip<<" > 5 "<<" for hit "<<std::hex<<d[n]<<endreq ;
             m_chip_number_error++ ;
             addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError);
             sc=StatusCode::RECOVERABLE;
             continue ;
           }

           /**------------ Search for redundancy only for the master chip  */
           if ((side == 1) && ((linkNb%2)==0)) { 
             if ((strip != oldstrip) && (groupSize>0)) { /** if it is a new cluster,  
                * make RDO with the previous cluster */
               int rdoMade = this->makeRDO(oldstrip,groupSize,tbin,onlineId,ERRORS,rdoIdc,vecHash,skipHash,lastHash);
         if ( rdoMade == -1 ) {
           sc=StatusCode::RECOVERABLE;
           addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError);
         } else saved[oldstrip] = rdoMade; 
         oldstrip = strip;
         groupSize = 0;
       }
       linkNb++ ;
     }
     if ((side == 0) && ((linkNb%2)!=0)) {
       if ((strip != oldstrip) && (groupSize>0)) { /** if it is a new cluster,  
                * make RDO with the previous cluster */
         int rdoMade = this->makeRDO(oldstrip,groupSize,tbin,onlineId,ERRORS,rdoIdc,vecHash,skipHash,lastHash);
         if ( rdoMade == -1 ) {
           sc=StatusCode::RECOVERABLE;
           addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError);
         } else saved[oldstrip] = rdoMade; 
         oldstrip = strip;
         groupSize = 0;
       }
       linkNb-- ;
     }
     onlineId = ((robid & 0xFFFFFF)|(linkNb << 24)) ;
     
     if (groupSize == 0)  oldstrip = strip; /** if it's the first super-condensed word */
     
     if (strip != oldstrip){ 
       /** if it is a new cluster,  
        * make RDO with the previous cluster */
       int rdoMade = this->makeRDO(oldstrip,groupSize,tbin,onlineId,ERRORS,rdoIdc,vecHash,skipHash,lastHash);
       if ( rdoMade == -1 ) {
         sc=StatusCode::RECOVERABLE;
         addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError);
       } else saved[oldstrip] = rdoMade; 
       oldstrip = strip;
       groupSize = 0;
     }
     
     groupSize+=nStripsInWord ;
     
  } else if (m_condensedMode) {
    
          /** condensed mode
           *  chip info : 4 bits  d[n]>>11)0xF
           *  chip number == (d[n]>>11)&0x7 and chip side == (d[n]>>14)&0x1
           *  For example if d[n]>>11)0xF = 0101 => chip5 or chip5 on side0, d[n]>>11)0xF = 1101 => chip13 or chip5 on side1
           */
           chip  = ((d[n]>>11)&0x7) ; 
           side = ((d[n]>>14)&0x1) ;
           strip = chip*128 + ((d[n]>>4)&0x7F) ;
           tbin = 0x2 ; /** assuming tbin is 010 in condensed mode */
           if(chip>5){
             if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"    Hit condensed : xxx Chip number = "<<chip<<" > 5 "<<" for hit "<<std::hex<<d[n]<<endreq ;
             m_chip_number_error++ ;
             addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError);
             sc=StatusCode::RECOVERABLE;
             continue ;
           }

           /**------------ Search for redundancy only for the master chip  */
           if ((side == 1) && ((linkNb%2)==0)) { 
             if ((strip != oldstrip) && (groupSize>0)) { /** if it is a new cluster,  
               * make RDO with the previous cluster */
               int rdoMade = this->makeRDO(oldstrip,groupSize,tbin,onlineId,ERRORS,rdoIdc,vecHash,skipHash,lastHash);
             if ( rdoMade == -1 ) {
               sc=StatusCode::RECOVERABLE;
                 addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError);
            } else saved[oldstrip] = rdoMade; 
            oldstrip = strip;
            groupSize = 0;
          }
          linkNb++ ;
        }
        if ((side == 0) && ((linkNb%2)!=0)) {
          if ((strip != oldstrip) && (groupSize>0)) { /** if it is a new cluster,  
               * make RDO with the previous cluster */
            int rdoMade = this->makeRDO(oldstrip,groupSize,tbin,onlineId,ERRORS,rdoIdc,vecHash,skipHash,lastHash);
            if ( rdoMade == -1 ) {
              sc=StatusCode::RECOVERABLE;
              addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError);
            } else saved[oldstrip] = rdoMade; 
            oldstrip = strip;
            groupSize = 0;
          }
          linkNb-- ;
        }
        onlineId = ((robid & 0xFFFFFF)|(linkNb << 24)) ;
        if (groupSize == 0)  oldstrip = strip; /** if it's the first condensed word */
        if (!(d[n]&0x1)){ /** 1-hit */
          m_singleCondHitNumber++ ;
          if (strip != oldstrip){ 
          /** if it is a new cluster,  
           * make RDO with the previous cluster */
            int rdoMade = this->makeRDO(oldstrip,groupSize,tbin,onlineId,ERRORS,rdoIdc,vecHash,skipHash,lastHash);
          if ( rdoMade == -1 ) {
            sc=StatusCode::RECOVERABLE;
            addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError);
        } else saved[oldstrip] = rdoMade; 
        oldstrip = strip;
        groupSize = 0;
      }
      
      if (d[n]&0x4){ /** Error in the hit */
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "    Hit condensed : xxx ERROR in 1-hit "<<std::hex<<d[n]<< endreq ;
        m_errorHit->push_back(groupSize);
        ERRORS = (ERRORS | 0x10) ;
        m_cond_hit1_error++ ;
        sc=StatusCode::RECOVERABLE;
      }
      groupSize++ ;
    }
    else { /** 2-hits */
      if (strip > 767) {
          addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError);
        sc=StatusCode::RECOVERABLE;

        if (msgLvl(MSG::DEBUG)) 
    msg(MSG::DEBUG) <<"Condensed mode - strip number out of range"<<endreq;

    continue;
      }
      m_pairedCondHitNumber++ ;
      if (strip != oldstrip) { /** if it is a new cluster, 
              * make RDO with the previous cluster 
              */
        int rdoMade = this->makeRDO(oldstrip,groupSize,tbin,onlineId,ERRORS,rdoIdc,vecHash,skipHash,lastHash);
        if ( rdoMade == -1 ) {
        sc=StatusCode::RECOVERABLE;
        addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError);
        } else 
    saved[oldstrip] = rdoMade; 
        oldstrip = strip;
        groupSize = 0;
      }    
      if (d[n]&0x4) { /** Error in the first hit */
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "    Hit condensed : xxx ERROR in 1st hit"<<std::hex<<d[n]<< endreq ;
        m_errorHit->push_back(groupSize);
        m_cond_hit1_error++ ;
        ERRORS = (ERRORS | 0x10) ;
        sc=StatusCode::RECOVERABLE;
      }
      groupSize++ ;    
      if (d[n]&0x8) { /** Error in the second hit */
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "    Hit condensed : xxx ERROR in 2nd hit"<<std::hex<<d[n]<< endreq ;
        m_errorHit->push_back(groupSize);
        m_cond_hit1_error++ ;
        ERRORS = (ERRORS | 0x20) ;
        sc=StatusCode::RECOVERABLE;
      }
      groupSize++ ;
    }
  }  else { 
    /** Expanded mode
     * chip info from the first word of expanded cluster : 4 bits  d[n]>>11)0xF
     * chip number == (d[n]>>11)&0x7 and chip side == (d[n]>>14)&0x1
     * For example if d[n]>>11)0xF = 0101 => chip5 or chip5 on side0, d[n]>>11)0xF = 1101 => chip13 or chip5 on side1
     */

          if (!(d[n]&0x8)) {  /** 1st hit cluster expanded */
      m_firstExpHitNumber++ ;
      chip  = ((d[n]>>11)&0x7) ;  
      side = ((d[n]>>14)&0x1) ;
      strip = chip*128 + ((d[n]>>4)&0x7F) ;
      tbin = d[n]&0x7 ; /** Real way for obtaining tbin info */
      
      if(chip>5){
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Expanded hit: First hit xxx ERROR chip Nb = " <<chip<< " > 5" <<endreq ;   
        m_chip_number_error++ ;
        addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError);
        continue ;
      }
      

      /** -------------- Search for redundancy only for the master chip  */
      if ((side == 1) && ((linkNb%2)==0))  {
        linkNb++ ;
      }
      if ((side == 0) && ((linkNb%2)!=0)) {
        linkNb-- ;
      }
      onlineId = ((robid & 0xFFFFFF)|(linkNb << 24)) ; 
      groupSize =  1 ;
      int rdoMade = this->makeRDO(strip,groupSize,tbin,onlineId,ERRORS,rdoIdc,vecHash,skipHash,lastHash) ;
      if ( rdoMade == -1 ) {
        sc=StatusCode::RECOVERABLE;
        addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError);
      } else 
        saved[strip] = rdoMade; 
            groupSize = 0 ;
    } else {  /** next hits cluster expanded */
      if (d[n]&0x80) { /** paired hits */
        if (strip > 767) {
          addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError);
          sc=StatusCode::RECOVERABLE;
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"Expanded mode - strip number out of range"<<endreq;
          continue;
        }
        m_evenExpHitNumber++ ;
        if(chip>5){
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Expanded Hit: paired hits xxx ERROR chip Nb = "<<chip<<" > 5"<<endreq ;  
          m_chip_number_error++ ;
          addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError);
          continue ;
        }
        /** first hit from the pair  */
        strip++;
        tbin = d[n]&0x7;
        groupSize = 1;
        int rdoMade = this->makeRDO(strip,groupSize,tbin,onlineId,ERRORS,rdoIdc,vecHash,skipHash,lastHash);
        if ( rdoMade == -1 ) {
          sc=StatusCode::RECOVERABLE;
          addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError);
        } else  saved[strip] = rdoMade; 
        /** second hit from the pair */
        strip++;
        tbin = (d[n] >> 4) & 0x7 ;
        rdoMade = this->makeRDO(strip,groupSize,tbin,onlineId,ERRORS,rdoIdc,vecHash,skipHash,lastHash);
        if ( rdoMade == -1 ) {
          sc=StatusCode::RECOVERABLE;
          addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError);
        } else saved[strip] = rdoMade; 
        groupSize = 0;
      }else {  /** Last hit of the cluster */
        m_lastExpHitNumber++ ;
        if(chip>5){
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"Expanded Hit: last hit xxx ERROR chip Nb = "<<chip<<" > 5"<<endreq ;  
          m_chip_number_error++ ;
          addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError);
          continue ;
        }     
        strip++;
        tbin = d[n]&0x7;
        groupSize = 1;
        int rdoMade = this->makeRDO(strip,groupSize,tbin,onlineId,ERRORS,rdoIdc,vecHash,skipHash,lastHash);
        if ( rdoMade == -1 ) {
          sc=StatusCode::RECOVERABLE;
          addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError);
        } else saved[strip] = rdoMade; 
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
	
        m_headnumber++ ;
        if (saved[strip]==false && oldstrip>=0) {
    int rdoMade = this->makeRDO(strip, groupSize++, tbin, onlineId, ERRORS,rdoIdc,vecHash,skipHash,lastHash); 
    if ( rdoMade == -1 ) {
      sc=StatusCode::RECOVERABLE;
      addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError);
    } else saved[strip] = rdoMade; 
  }
  
   /** Everything is set to default for a new hunt of RDO */
  strip =0 ;
  oldstrip = -1 ;
  groupSize = 0 ;
  //ErrorHeader = false ;
  ERRORS = 0 ;
  memset(saved,0,768);
  m_errorHit->clear();

  /** Link Number (or stream) in the ROD fragment */
  int rodlinkNb = d[n] & 0x7F ;  

  /** This is the real calculation for the offline  */
  linkNb  = ((( rodlinkNb >>4)&0x7)*12+(rodlinkNb &0xF)) ;
  onlineId = ((robid & 0xFFFFFF)|(linkNb << 24)) ;     
  if ((onlineId ==0) || (linkNb > 95)) {
    addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError);
    sc=StatusCode::RECOVERABLE;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"Header: xxx Link nb out of range (skipping following data)"<<std::dec<<linkNb<<endreq ;
    break;
  } else {
    currentLinkIdHash = m_cabling->getHashFromOnlineId(onlineId) ;
  }
  /// look for masked off links - bit 7
  if (d[n] >> 7 & 0x1) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"Masked link "<<onlineId<<" "<<currentLinkIdHash<<endreq;
    //no counter increment here , is that correct? (sar)
    addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::MaskedLink);
    sc=StatusCode::RECOVERABLE; 
  }
  if (d[n]&0x800){
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "    Header: xxx TimeOut Error " <<currentLinkIdHash<<  endreq ;
    m_head_error_timeout++;
    addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::TimeOutError);
    sc=StatusCode::RECOVERABLE;
  }
  
  if (d[n]&0x1000){
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "    Header: xxx Preamble Error " <<currentLinkIdHash<<  endreq ;
    m_head_error_preamb++ ;
    addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::PreambleError);
    sc=StatusCode::RECOVERABLE;
  }
  
  if (d[n]&0x400){ 
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "    Header: xxx LVL1 ID Error " <<currentLinkIdHash<< endreq ;
    m_head_error_lvl1id++ ;
    addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::LVL1IDError);
    sc=StatusCode::RECOVERABLE;
  }
  
  if (d[n]&0x200){
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "    Header: xxx BCID Error " << currentLinkIdHash<< endreq ;
    m_head_error_bcid++ ;
    addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::BCIDError);
    sc=StatusCode::RECOVERABLE;
  }
  
  if ((d[n]&0xF) > 11) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "    Header: xxx Error in formatter " << currentLinkIdHash<< endreq ;
    m_head_error_formatter++ ;
    addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::FormatterError);
    sc=StatusCode::RECOVERABLE;
  }
  
  if (d[n]&0x100)  m_condensedMode = true ;
  else             m_condensedMode = false ;
  continue;
  
} // end header
      
      ///---------------------------------------------------------------------
      /// trailer
      ///---------------------------------------------------------------------
      else if (((d[n]>>13)&0x7) == 0x2) { 
	foundHeader=false;
	
  m_trailnumber++ ;
  //ErrorTrailer = false;
  
  if (d[n]&0x1000){
    //ErrorTrailer = true;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "    Trailer: xxx Trailer ERROR " << std::hex << d[n] << endreq ;
    m_trail_error_bit++ ;
    addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::TrailerError);
    sc=StatusCode::RECOVERABLE;
  }
  
  if (d[n]&0x800){
    //ErrorTrailer = true;/** no data should appear between header and trailer */
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "    Trailer: xxx Header-Trailer limit ERROR " << std::hex << d[n] << endreq ;
    m_trail_error_limit++ ;
    addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::HeaderTrailerLimitError);
    sc=StatusCode::RECOVERABLE;
  }
  
  if (d[n]&0x400){
    //ErrorTrailer = true; /** not sure if there are hit elements before (probably yes but in principle they are fine) */
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "    Trailer: xxx Data Overflow ERROR " << std::hex << d[n] << endreq ;
    m_trail_error_overflow++ ;
    addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::TrailerOverflowError);
    sc=StatusCode::RECOVERABLE;
  }
  continue; 
      }
      
      ///---------------------------------------------------------------------
      /// FlaggedABCD error
      ///---------------------------------------------------------------------
      else if (((d[n]>>13)&0x7) == 0x0){
        chip = ((d[n]>>3)&0x7) ;
        ABCerror = d[n]&0x7; 
        /** no data should appear for that chip but how do we 
         * want to transmit this information ? */
        IdentifierHash flagIdHash(0);
      if (onlineId == 0) {
        addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError);
        continue ;
      } else {
        flagIdHash = m_cabling->getHashFromOnlineId(onlineId) ;
      }
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<" xxx Flagged ABCD ERROR in chip "<<chip<<" Error code ABCerror "<<ABCerror<<" Link Nb (or Stream) "<<linkNb<<endreq ;
      m_flag_error_bit++ ;
      addSingleError(flagIdHash, SCT_ByteStreamErrors::ABCDError);
      sc=StatusCode::RECOVERABLE;
      continue;
    } else if (((d[n]>>13)&0x7) == 0x3){
        ///---------------------------------------------------------------------
        /// Raw Data
        ///---------------------------------------------------------------------
        IdentifierHash rawIdHash(0);
        if (onlineId == 0) {
          addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError);
          continue ;
        } else {
          rawIdHash = m_cabling->getHashFromOnlineId(onlineId) ;
        }
  
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " xxx Raw Data Mode "<<std::hex<<d[n]<<std::dec<<": Config Data Mode "<<endreq ;
        /** too many errors in the BS for the ROD to decode the data */
        m_config_data_bit++ ;
        addSingleError(rawIdHash, SCT_ByteStreamErrors::RawError);
        sc=StatusCode::RECOVERABLE;
        continue;
      }else {
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Data word format unknown " << endreq;
        m_unknown_data_format++ ;
        addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError);
        sc=StatusCode::RECOVERABLE;
      }
    } //end of 16-bit word loop
  }   //end of 32-bit word loop

  /** create RDO of the last ink or stream of the event */
  if (saved[strip]==false && oldstrip>=0){
    int rdoMade = makeRDO(strip, groupSize++, tbin, onlineId, ERRORS,rdoIdc,vecHash,skipHash,lastHash);
    if ( rdoMade == -1 ) {
      sc=StatusCode::RECOVERABLE;
      addSingleError(currentLinkIdHash, SCT_ByteStreamErrors::ByteStreamParseError);
    } else saved[strip] = rdoMade; 
  }

  // Set this ROD as decoded in SCT_ByteStreamErrorSvc
  m_byteStreamErrSvc->setDecodedROD(robid);

  if (sc.isFailure() and msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "One or more ByteStream errors found " << endreq;
  return sc;
}
/** makeRDO has 3 possible return values: 
 *          1 if RDO was successfully created,
 *          0 if collection was deliberately skipped (for trigger)
 *         -1 if there was an error in the decoding - will be passed
 *            on as StatusCode::RECOVERABLE by fillCollection().
 */

int SCT_RodDecoder::makeRDO(int strip, int groupSize,int tbin, uint32_t onlineId, int ERRORS, SCT_RDO_Container* rdoIdc, std::vector<IdentifierHash>* vecHash,IdentifierHash& skipHash, IdentifierHash& lastHash){

  // IIncidentSvc* incsvc;

  if (onlineId == 0x0) {
    msg(MSG::WARNING)<<"No link header found, possibly corrupt ByteStream.  Will not try to make RDO"<<endreq;
    return -1;
  }
  /** get offlineId from the link number and ROB number */
  IdentifierHash idCollHash =  m_cabling->getHashFromOnlineId(onlineId) ;
  if (idCollHash==0xffffffff) {
    m_numUnknownOfflineId++;
    ATH_MSG_ERROR("Unknown OfflineId for OnlineId -> cannot create RDO");
    ATH_MSG_WARNING("Unknown OfflineId for OnlineId "<<std::hex<<onlineId <<" -> cannot create RDO"<<std::dec);

    //fire an incident whenever there is a "unknown offline id..." so they are listened by /InnerDetector/InDetCalibAlgs/SCT_CalibAlgs/src/SCT_CalibEventInfo
    m_incidentSvc->fireIncident(Incident(name(), "UnknownOfflineId"));
    return -1;
  }

  if (strip + (groupSize-1) > 767 ) {
    msg(MSG::WARNING) <<"Cluster with "<<groupSize<<" strips, starting at strip "<<strip<<" in collection "<<idCollHash<<" out of range. Will not make RDO"<<endreq;
    return -1;
  }

  /** this option is for the trigger, if there is a vecHash* given, test it ! */
  if (vecHash) {
    if (idCollHash == skipHash) {
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Collection for Hash not to be decoded, skip" << endreq;
      return 0;
    }  else if (idCollHash != lastHash) {
      lastHash = idCollHash;
      /** maybe the new hash is not in the list, so test it */
      std::vector<IdentifierHash>::iterator p = find(vecHash->begin(),vecHash->end(),idCollHash);
      if (p == vecHash->end()) {
        if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Collection for Hash not to be decoded, skip" << endreq;
        /** remember this one, so that we do not find(...) forever */
        skipHash = idCollHash;
        return 0;
      }
    }
  }

  /** get identifier from the hash, this is not nice */
  Identifier idColl= m_sct_id->wafer_id(idCollHash);

  SCT_RDORawData *RDO = 0 ;      
  SCT_RDO_Collection* theColl = 0;
  SCT_RDO_Container::const_iterator itColl;

  /** see if strips go from 0 to 767 or vice versa */
  const InDetDD::SiDetectorElement * p_element = (m_indet_mgr->getDetectorElement(idCollHash));
  if (p_element->swapPhiReadoutDirection()) {
    strip = 767 - strip ;
    strip = strip-(groupSize-1);
  }

  Identifier iddigit = m_sct_id->strip_id(idColl,strip) ;
  unsigned int rawDataWord = (groupSize | (strip << 11) | (tbin <<22) | (ERRORS << 25)) ;

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"Output Raw Data "<<std::hex<<" Coll "<<idColl.getString()<<":-> "<<m_sct_id->print_to_string(iddigit)<<std::dec<<endreq ;
  
  itColl = rdoIdc->indexFind( idCollHash );
  
  /** Check if the Collection is already created.*/
  if ( itColl != rdoIdc->end() ) theColl = const_cast<SCT_RDO_Collection*>(&**itColl); //very nasty!!
  else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Collection ID = " << idCollHash<< " does not exist, create it " << endreq;
    /** create new collection */
    theColl = new SCT_RDO_Collection ( idCollHash );
    /** set the Identifier to be nice to downstream clients */
    theColl->setIdentifier(idColl);
    /** add collection into IDC */
    StatusCode sc = rdoIdc->addCollection(theColl, idCollHash);
    if (sc.isFailure()) msg(MSG::ERROR) <<"Failed to add SCT RDO collection to container"<<endreq;
  }
  
  /** Now the Collection is there for sure. Create RDO and push it
   * into Collection. 
   */
  RDO = new SCT3_RawData( iddigit, rawDataWord, m_errorHit );
  m_nRDOs++;
  theColl->push_back( RDO );
  return 1;
}

void 
SCT_RodDecoder::addRODError(uint32_t rodid, int errorType) {
  std::vector<IdentifierHash> idHashes;
  m_cabling->getHashesForRod(idHashes,rodid);
  std::vector<IdentifierHash>::iterator hashIt = idHashes.begin();
  std::vector<IdentifierHash>::iterator hashEnd = idHashes.end();
  for (; hashIt != hashEnd; ++hashIt) {
      addSingleError(*hashIt, errorType);
  }
  return;
}

bool 
SCT_RodDecoder::addSingleError(const IdentifierHash & idHash, const int bsErrorType){
  bool ok(true);
  if (m_triggerMode) {
    m_byteStreamErrSvc->addErrorCount(bsErrorType);
  } else {
    ok=idHash.is_valid();
    if (ok){
      std::pair<IdentifierHash, int>* err = new std::pair<IdentifierHash, int>(std::make_pair(idHash, bsErrorType)); 
      m_bsErrCont->push_back(err);
    }
  }
  return ok;
}
