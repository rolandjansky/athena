/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TgcRODReadOut.h"
#include "TgcSlbData.h"
#include "TgcSlbDataHelper.h"

#include "TGCcablingInterface/ITGCcablingServerSvc.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

// constructor
Muon::TgcRODReadOut::TgcRODReadOut()
  : m_tgcRdo(0), m_cabling(0)
{ 
  m_tgcSlbDataHelper = new TgcSlbDataHelper;

  m_failedSetRdo = 0;
  for(unsigned int rodId=0; rodId<NROD+1; rodId++) {
    m_failedDecodeRodToRdo[rodId] = 0;
    m_failedHeaderSizeRawData[rodId] = 0;
    m_failedSetSbLoc[rodId] = 0;
    m_failedSetType[rodId] = 0;
    m_failedGetSLBIDfromRxID[rodId] = 0;
    m_failedGetReadoutIDfromSLBID[rodId] = 0;
  }
}

// destructor
Muon::TgcRODReadOut::~TgcRODReadOut()
{ 
    MsgStream log(Athena::getMessageSvc(),"Muon::TgcRODReadOut");
    if(m_failedSetRdo) {
      log << MSG::WARNING << "setRdo failed " << m_failedSetRdo << " times" << endmsg;
    } 
    for(unsigned int rodId=0; rodId<NROD+1; rodId++) {
      if(m_failedDecodeRodToRdo[rodId]) {
	log << MSG::WARNING << "rodId=" << rodId << " : decodeRodToRdo faied " 
	     << m_failedDecodeRodToRdo[rodId] << " times" << endmsg;
      }
      if(m_failedDecodeRodToRdo[rodId]) {
	log << MSG::WARNING << "rodId=" << rodId << " : decodeRodToRdo faied " 
	     << m_failedDecodeRodToRdo[rodId] << " times" << endmsg;
      }
      if(m_failedHeaderSizeRawData[rodId]) {
	log << MSG::WARNING << "rodId=" << rodId << " : Header or SizeRawData was strange " 
	     << m_failedHeaderSizeRawData[rodId] << " times" << endmsg;
      }
      if(m_failedSetSbLoc[rodId]) {
	log << MSG::WARNING << "rodId=" << rodId << " : setSbLoc failed " 
	     << m_failedSetSbLoc[rodId] << " times" << endmsg;
      }
      if(m_failedSetType[rodId]) {
	log << MSG::WARNING << "rodId=" << rodId << " : setType failed "
             << m_failedSetType[rodId] << " times" << endmsg;
      }
      if(m_failedGetSLBIDfromRxID[rodId]) {
	log << MSG::WARNING << "rodId=" << rodId << " : getSLBIDfromRxID failed "
             << m_failedGetSLBIDfromRxID[rodId] << " times" << endmsg;
      }
      if(m_failedGetReadoutIDfromSLBID[rodId]) {
	log << MSG::WARNING << "rodId=" << rodId << " : getReadoutIDfromSLBID failed " 
	     << m_failedGetReadoutIDfromSLBID[rodId] << " times" << endmsg;
      }
    }
  delete m_tgcSlbDataHelper; m_tgcSlbDataHelper=0;
}

const TgcRdo* Muon::TgcRODReadOut::getRdo(void) const 
{
  return m_tgcRdo;
}

StatusCode Muon::TgcRODReadOut::setRdo(TgcRdo * v_tgcRdo) 
{
  m_tgcRdo = v_tgcRdo; 
  return StatusCode::SUCCESS;
}

StatusCode Muon::TgcRODReadOut::byteStream2Rdo(const ByteStream& bs, 
					       TgcRdo& tgcRdo, 
					       uint32_t source_id)
{
  MsgStream log(Athena::getMessageSvc(),"Muon::TgcRODReadOut");
  bool t_debug = (log.level()<=MSG::DEBUG);
  
  // set collection
  if(!setRdo(&tgcRdo)){
    if(t_debug || !m_failedSetRdo) {
      log << (!m_failedSetRdo ? MSG::WARNING : MSG::DEBUG)
	       << " Can't set tgcRdo pointer correctly: Skip decoding of remaining hits of this event..." 
	       << endmsg;
    } 
    m_failedSetRdo++;

    return StatusCode::SUCCESS;
  }

  // set ROD attribute
  uint16_t subDetectorId = (source_id & 0x00FF0000) >> 16;
  uint16_t rodId         = (source_id & 0x000000FF);

  uint32_t l1Id          = tgcRdo.l1Id();
  uint16_t bcId          = tgcRdo.bcId();

  m_tgcRdo->setOnlineId(subDetectorId, rodId);

  // decode
  if(!(decodeRodToRdo(bs,subDetectorId, rodId,l1Id, bcId).isSuccess())){
    unsigned int tmpRodId = rodId + (subDetectorId==CSIDE ? NROD/2 : 0);
    if(tmpRodId>NROD) tmpRodId = NROD;
    
    if(t_debug || !m_failedDecodeRodToRdo[tmpRodId]) {
      log << (!m_failedDecodeRodToRdo[tmpRodId] ? MSG::WARNING : MSG::DEBUG)
	       << " Can't convert TGC BS to RDO: "
	       << "subDetectorId = " << subDetectorId
	       << "rodId = " << rodId
	       << "l1Id = " << l1Id
	       << "bcId = " << bcId
	       << endmsg;    
      log << (!m_failedDecodeRodToRdo[tmpRodId] ? MSG::WARNING : MSG::DEBUG)
	       << "Corrupted data, Skip decoding of remaining hits of this event..."
	       << endmsg;
    }
    m_failedDecodeRodToRdo[tmpRodId]++;

    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;
}

StatusCode Muon::TgcRODReadOut::check(const ByteStream& bs, 
				      TgcRdo& tgcRdo, 
				      uint32_t source_id)
{

  // create another TgcTdo
  TgcRdo* newRdo = new TgcRdo();

  MsgStream log(Athena::getMessageSvc(),"Muon::TgcRODReadOut");
 
  // set collection
  if(!newRdo || !setRdo(newRdo)){
    log << MSG::WARNING << " Can't set tgcRdo pointer correctly: Skip decoding of remaining hits of this event..." 
	     << endmsg;
    if(newRdo){
      delete newRdo; newRdo = 0;
    } 
    return StatusCode::SUCCESS;
  }

  // set ROD attribute
  uint16_t subDetectorId = (source_id & 0x00FF0000) >> 16;
  uint16_t rodId         = (source_id & 0x000000FF);

  uint32_t l1Id          = tgcRdo.l1Id();
  uint16_t bcId          = tgcRdo.bcId();

  m_tgcRdo->setOnlineId(subDetectorId, rodId);

  // decode
  if(!(decodeRodToRdo(bs,subDetectorId, rodId,l1Id, bcId).isSuccess())){
    log << MSG::WARNING << " Can't convert TGC BS to RDO: "
	     << "subDetectorId = " << subDetectorId
	     << "rodId = " << rodId
	     << "l1Id = " << l1Id
	     << "bcId = " << bcId
	     << endmsg;
    log << MSG::WARNING << "Corrupted data, Skip decoding of remaining hits of this event..."
	     << endmsg;
    delete newRdo; newRdo = 0;
    return StatusCode::SUCCESS;
  }
  
  // compare 
  if(!compare(&tgcRdo, newRdo)){
    log << MSG::WARNING << "Can't compare TgcRdos: Skip decoding of remaining hits of this event..."<<endmsg;
    delete newRdo; newRdo = 0;
    return StatusCode::SUCCESS;
  }

  delete newRdo; newRdo = 0;

  return StatusCode::SUCCESS;
}

StatusCode Muon::TgcRODReadOut::compare(TgcRdo* rdo, TgcRdo* newRdo)
{
  #ifndef NDEBUG
  MsgStream log(Athena::getMessageSvc(),"Muon::TgcRODReadOut");
  log << MSG::DEBUG << "TgcRODReadOut::compare" << endmsg;
  log << MSG::DEBUG 
	     << " rdo->size()=" << rdo->size() 
	     << " newRdo->size()=" << newRdo->size()
	     << endmsg;
  #endif

  size_t n_data = newRdo->size();
  size_t o_data = rdo->size();
  std::vector<bool> check(o_data, false);

  #ifndef NDEBUG
  log << MSG::DEBUG << "Unmatched in RawData format" << endmsg;
  #endif

  for(size_t ib=0; ib<n_data; ib++){
    TgcRawData* nraw = (*newRdo)[ib];
    #ifndef NDEBUG
    bool matched = false; // matched flag is only needed for DEBUG messages
    #endif
    for(size_t ic=0; ic<o_data; ic++){
      TgcRawData* oraw = (*rdo)[ic];
      if(isMatched(nraw, oraw)){
         check[ic] = true;
         #ifndef NDEBUG
         matched = true;
         #endif
         break;
      }
    }
    #ifndef NDEBUG
    if(!matched) {
      log << MSG::DEBUG << (*nraw) << endmsg;
    }
    #endif
  }

  #ifndef NDEBUG
  log << MSG::DEBUG << "Unmatched in Readout format" << endmsg;
  #endif
  for(size_t ic=0; ic<o_data; ic++){
    if(check[ic]) continue;
    TgcRawData* oraw = (*rdo)[ic];
    #ifndef NDEBUG
    bool matched = false; // matched flag is only needed for DEBUG messages
    #endif
    for(size_t ib=0; ib<n_data; ib++){
      TgcRawData* nraw = (*newRdo)[ib];
      if(isMatched(oraw, nraw)){
        #ifndef NDEBUG
        matched = true;
        #endif
        break;
      }
    }
    #ifndef NDEBUG
    if(!matched) {
      log << MSG::DEBUG << (*oraw) << endmsg;
    }
    #endif
  }

  return StatusCode::SUCCESS;
} 

bool Muon::TgcRODReadOut::isMatched(const TgcRawData* rdo1,
				    const TgcRawData* rdo2) const
{
  if(rdo1->subDetectorId() != rdo2->subDetectorId()) return false;
  if(rdo1->rodId() != rdo2->rodId()) return false;
  if(rdo1->sswId() != rdo2->sswId()) return false;
  if(rdo1->slbId() != rdo2->slbId()) return false;
  if(rdo1->bcTag() != rdo2->bcTag()) return false;
  if(rdo1->type()  != rdo2->type())  return false;

  switch(rdo1->type()){
  case TgcRawData::TYPE_HIT:
    if(rdo1->isAdjacent() != rdo2->isAdjacent())  return false;
    if(rdo1->bitpos()     != rdo2->bitpos())      return false;
    return true;
    break;

  case TgcRawData::TYPE_TRACKLET:
    if(rdo1->segment()   != rdo2->segment())    return false;
    if(rdo1->subMatrix() != rdo2->subMatrix())  return false;
    if(rdo1->segment()   != rdo2->segment())    return false;
    if(rdo1->position()  != rdo2->position())   return false;
    if(rdo1->delta()     != rdo2->delta())      return false;
    return true;
    break;

  case TgcRawData::TYPE_HIPT:
    if(rdo1->isStrip()    != rdo2->isStrip())      return false;
    if(rdo1->isForward()  != rdo2->isForward())    return false;
    if(rdo1->sector()     != rdo2->sector())       return false;
    if(rdo1->chip()       != rdo2->chip())         return false;
    if(rdo1->index()      != rdo2->index())        return false;
    if(rdo1->isHipt()     != rdo2->isHipt())       return false;
    if(rdo1->hitId()      != rdo2->hitId())        return false;
    if(rdo1->hsub()       != rdo2->hsub())         return false;
    if(rdo1->delta()      != rdo2->delta())        return false;
    return true;
    break;

  case TgcRawData::TYPE_SL:
    if(rdo1->isForward()  != rdo2->isForward())    return false;
    if(rdo1->sector()     != rdo2->sector())       return false;
    if(rdo1->index()      != rdo2->index())        return false;
    if(rdo1->isMuplus()   != rdo2->isMuplus())     return false;
    if(rdo1->threshold()  != rdo2->threshold())    return false;
    if(rdo1->roi()        != rdo2->roi())          return false;
    return true;
    break;

  default:
    return true;
    break;
  }
}

// decode ROD data to RDO
StatusCode Muon::TgcRODReadOut::decodeRodToRdo(const ByteStream& vData, 
					       uint16_t subDetectorId,
					       uint16_t rodId, 
					       uint32_t l1Id, 
					       uint16_t bcId)
{

  /////////
  //  decode ROD data part

  // total data fragment size
  //const uint32_t tot_vData = vData.size();  

  //Data Word Count (added by okumura)
  uint32_t sizeRawData=0;
  uint32_t sizeReadOutFormatHit=0;
  uint32_t sizeReadOutFormatTracklet=0;
  //uint32_t sizeChamberFormatHit=0;
  //uint32_t sizeChamberFormatTracklet=0;
  uint32_t sizeHipTWord=0;
  uint32_t sizeSectorLogicWord=0;

  // flag of correct header
  bool isHeaderOK = true;
  // index of vData;
  unsigned int vDataIndex; 
  for(vDataIndex=0; isHeaderOK; vDataIndex++){
    uint32_t rawdata_flag = (vData[vDataIndex] & RawDataFragMask) >>24;
    uint32_t flagmentId = (vData[vDataIndex] & FragmentIdMask) >>24;

    // check HEADER for RawData section 
    if(rawdata_flag==0xFF)   break;
    
    // Words Count
    uint32_t counter=(vData[vDataIndex] & FragmentCountMask );
    
    switch(flagmentId) {
    case 0x01:    //Fragment ID ==1     
      sizeRawData = counter;
      break;
    case 0x02: //Fragmant ID ==2
      sizeReadOutFormatHit = counter;
      break;
    case 0x03: //Fragmant ID ==3
      sizeReadOutFormatTracklet = counter;
      break;
    //case 0x04: //Fragmant ID ==4
      //sizeChamberFormatHit =counter;
      //break;
    //case 0x05: //Fragmant ID ==5
      //sizeChamberFormatTracklet =counter;
      //break;
    case 0x08: //Fragmant ID ==8
      sizeHipTWord =counter;
      break;
    case 0x09: //Fragmant ID ==9
      sizeSectorLogicWord =counter;
      break;
    default:
      isHeaderOK = false;
      break;
    }
  }

  MsgStream log(Athena::getMessageSvc(),"Muon::TgcRODReadOut");
  bool t_debug = (log.level()<=MSG::DEBUG);

  if(!isHeaderOK || 
     sizeRawData==0 || 
     sizeRawData>=0x10000 
     ) {
    unsigned int tmpRodId = rodId + (subDetectorId==CSIDE ? NROD/2 : 0);
    if(tmpRodId>NROD) tmpRodId = NROD;

    if((t_debug || !m_failedHeaderSizeRawData[tmpRodId])) {
      log << (!m_failedHeaderSizeRawData[tmpRodId] ? MSG::WARNING : MSG::DEBUG) 
	       << "Corrupted data, Skip decoding of this ROD data" 
	       << " TgcRODReadout : ROD ID= " << rodId 
	       << " RawData Size= " << sizeRawData
	       << endmsg;
      log << (!m_failedHeaderSizeRawData[tmpRodId] ? MSG::WARNING : MSG::DEBUG)
	       << "ReadOut Hit Size= " << sizeReadOutFormatHit 
	       << " ReadOut Tracklet= " << sizeReadOutFormatTracklet 
	       << " ReadOut SL= " << sizeSectorLogicWord 
	       << " vDataIndex = " << vDataIndex
	       << " HipT = " << sizeHipTWord 
	       << endmsg;
    }
    m_failedHeaderSizeRawData[tmpRodId]++;

    return StatusCode::SUCCESS;
  }

  if(t_debug) {
    log << MSG::DEBUG 
	     <<  "TgcRODReadout : ROD ID=" << rodId  
	     << "  RawData Size=" << sizeRawData
	     << "  ReadOut Hit Size= " << sizeReadOutFormatHit 
	     << "  ReadOut Tracklet= " << sizeReadOutFormatTracklet 
	     << "  ReadOut SL= " << sizeSectorLogicWord 
	     << endmsg;
  }

  uint32_t firstRawDataIndex=vDataIndex;

  // RawData Collection.
  std::vector<TgcRawData *>   vCh;

  // decode RAW DATA Fragment process start
  uint16_t fe_bcId=0;
  uint32_t fe_l1Id=0;
  uint16_t sswId= 999;
  uint16_t slbId = 999;
  uint16_t rxId = 999;
  uint16_t mod = 999;
  uint16_t b_error = 0;
  uint16_t rec_type= 999;

  TgcSlbData* slb =0;

  
  for(; vDataIndex < firstRawDataIndex+sizeRawData; ++vDataIndex){
    if(t_debug) {
      log << MSG::DEBUG 
	       << "Tgc BS Raw:" << vDataIndex 
	       << ":  " << std::hex << vData[vDataIndex] 
	       << endmsg;
    }

    // header check
    unsigned int header = vData[vDataIndex] & HeaderMask;
        
    uint32_t rawdata_flag = (vData[vDataIndex] & RawDataFragMask) >> 24;
    if((rawdata_flag == 0xff) || (rawdata_flag == 0xfc)){
      continue;
    }      
    
    switch(header) {
    case HeaderEvent: // Event header
      // get Rec Type
      rec_type =  (vData[vDataIndex] & 0x18000000) >> 27;
      if (rec_type >1) {
	  log << MSG::WARNING 
		   << "Rec Type " << rec_type << " is not supproted " 
		   << " Skip decoding of remaining hits of this event..."
		   << endmsg;
	return StatusCode::SUCCESS;
     }
      
     // get SSW ID
     if (rec_type == 0) {
       // format Oct. 09
      sswId = (vData[vDataIndex] & 0x0F0000) >> 16;
     } else if (rec_type ==1) {
       // original SSW format
      sswId = (vData[vDataIndex] & 0x07800000) >> 23;
     }
     break;
	
    case HeaderSLB10: // SLB header 10
      // Create RDOs by using slb bit array  
      if(slb!=0) {
        m_tgcSlbDataHelper->convertToHits(subDetectorId, rodId, slb, vCh);
        m_tgcSlbDataHelper->convertToCoincidences(subDetectorId, rodId, slb, vCh);
        delete slb; slb = 0;
      }
      // get SLB ID, BCID, L1ID, ModuleType
      fe_l1Id  = (vData[vDataIndex] & 0x0000F000) >> 12;
      fe_bcId  =  vData[vDataIndex] & 0x00000FFF;
      slbId = (vData[vDataIndex] & 0x1F000000) >> 24;

     if (rec_type == 0) {
       // format Oct. 09
       mod   = (vData[vDataIndex] & 0x00070000) >> 16;
       rxId  = (vData[vDataIndex] & 0x00F80000) >> 19;
     } else if (rec_type ==1) {
       // original SSW format
       mod   = (vData[vDataIndex] & 0x000E0000) >> 17;
     }
     
     if(fe_l1Id != l1Id) {	
       if(t_debug) {
	 log << MSG::DEBUG
		  << "l1Id in SLB Header is different from l1Id in ROD "
		  << " for rodId:" << rodId
		  << " slbId:" << slbId 
		  << "   l1Id, bcId (SLB):" << fe_l1Id << ", " << fe_bcId
		  << "   l1Id, bcId (ROD):" << l1Id << ", " << bcId
		  << endmsg;
       }
     }
      b_error = 0;
      break;
      
      
    case HeaderSLB11: // SLB header 11 or Trailer
      if(vData[vDataIndex] & 0x10000000){
	//SLB Trailer
	// decode Bit map
	b_error = vData[vDataIndex] & 0x0FFF;

      } else {
	//SLBHeader 11
	// get RXID
	rxId = (vData[vDataIndex] & 0x7C00000) >> 22;
	
	break;
      }
      /* FALLTHROUGH */
      
      
    case HeaderSLBC: // SLB data
    case HeaderSLBP:
    case HeaderSLBN:
      // create slb
      if(!slb) {
	// create TgcSlbData
	slb = new TgcSlbData(bcId, l1Id, sswId, slbId);
	// set sbLoc by using rxId
	if(!setSbLoc(subDetectorId, rodId, slb, rxId) ){
	  unsigned int tmpRodId = rodId + (subDetectorId==CSIDE ? NROD/2 : 0);
	  if(tmpRodId>NROD) tmpRodId = NROD;

	  if((t_debug || !m_failedSetSbLoc[tmpRodId])) {
	    log << (!m_failedSetSbLoc[tmpRodId] ? MSG::WARNING : MSG::DEBUG)
		     << " Fail to set sbLoc"
		     << " :rodId=" << rodId
		     << " :sswId=" << sswId
		     << " :slbAddress= " << slbId
		     << " : rxId= " << rxId
		     << endmsg;
	    log << (!m_failedSetSbLoc[tmpRodId] ? MSG::WARNING : MSG::DEBUG)
		     << "Corrupted data, Skip decoding of remaining hits of this event..."
		     << endmsg;
	  }
	  m_failedSetSbLoc[tmpRodId]++;
	  delete slb; slb = 0;
	  return StatusCode::SUCCESS;
	}
	if(!m_tgcSlbDataHelper->setType(subDetectorId, rodId, slb, mod)) {
	  unsigned int tmpRodId = rodId + (subDetectorId==CSIDE ? NROD/2 : 0);
          if(tmpRodId>NROD) tmpRodId = NROD;

	  if((t_debug || !m_failedSetType[tmpRodId])) {
	    log << (!m_failedSetType[tmpRodId] ? MSG::WARNING : MSG::DEBUG)
		     << " Fail to set Module Type"
		     << " :rodId=" << rodId
		     << " :sswId=" << sswId
		     << " :slbAddress= " << slbId
		     << endmsg;
	    log << (!m_failedSetType[tmpRodId] ? MSG::WARNING : MSG::DEBUG)
		     << "Corrupted data, Skip decoding of remaining hits of this event..."
		     << endmsg;
          }
	  m_failedSetType[tmpRodId]++;
	  delete slb; slb = 0;
	  return StatusCode::SUCCESS;
	}
	if(b_error) {
	  slb->setError(b_error);
	}
	if(t_debug) {
	  log << MSG::DEBUG 
		   << "TgcRODReadout : slb =" << slb 
		   << " : rodId=" << rodId
		   << " : sswId=" << sswId
		   << " : slbAddress= " << slbId
		   << " : rxId= "<< rxId
		   << endmsg;
	}
      }


      // decode Bit map
      for(int iHL=0; iHL<32; iHL+=16) {// Higher and Lower word 
	uint32_t vd = vData[vDataIndex] << iHL;
	// get BC tag
	uint32_t bcTag;
	if((vd & HeaderMask) == HeaderSLBC) {
	  bcTag=TgcSlbData::BC_CENTRAL;
	} else if((vd & HeaderMask) == HeaderSLBP) { 
	  bcTag=TgcSlbData::BC_PREVIOUS;
	} else if((vd& HeaderMask) == HeaderSLBN) { 
	  bcTag=TgcSlbData::BC_NEXT;
	} else if((vd& HeaderMask) == HeaderSLB11) {
	  // Trailer 
	  slb->setError(vData[vDataIndex] & 0x0FFF);
	  continue;
	} else {
	  continue;
	}
	// Cell Address and Bit map
	unsigned long cellAddr = (vd & 0x1F000000) >> 24;
	unsigned long cellBitMap = (vd & 0x00FF0000) >> 16;

	if(cellAddr < 25) {
	  // set Bit map  
	  slb->setBitmap(bcTag, cellAddr, cellBitMap);
	} else {
	  // PADWORD
	}
      }
      break;

    case HeaderError:    // Error Report
      {
	uint32_t error_context = (vData[vDataIndex] & 0x00FFFFFF);
	uint16_t error_id      = (vData[vDataIndex] & 0x01F000000)>>24;
	if((t_debug && (error_context!=0))) {
	  log << MSG::DEBUG 
		   << "TgcRODReadout : Error Report "  
		   << " : error id =" << error_id
		   << " : context =" << error_context
		   << endmsg;
	}
      }
      break;
      
    default:
      break;
    }
  }
  if(slb!=0) {
    m_tgcSlbDataHelper->convertToHits(subDetectorId, rodId, slb, vCh);
    m_tgcSlbDataHelper->convertToCoincidences(subDetectorId, rodId, slb, vCh);
    delete slb; slb = 0;
  }

  if(t_debug) { 
    uint32_t firstROHitIndex=vDataIndex;
    for(; vDataIndex < firstROHitIndex+sizeReadOutFormatHit; ++vDataIndex){
      log << MSG::DEBUG 
	       << "Tgc BS Readout HIT:" << vDataIndex 
	       << ":  " << std::hex  << vData[vDataIndex] << endmsg;
    }
    uint32_t firstROTrackletIndex=vDataIndex;
    for(; vDataIndex < firstROTrackletIndex+sizeReadOutFormatTracklet; ++vDataIndex){
      log << MSG::DEBUG 
	       << "Tgc BS Readout HIT:" << vDataIndex 
	       << ":  " << std::hex  << vData[vDataIndex] << endmsg;
    }
  }

 
  /////////
  //  append Hits to RDO

  // loop over all hits
  std::vector<TgcRawData *>::iterator ith   = vCh.begin();
  std::vector<TgcRawData *>::iterator ith_e = vCh.end();

  size_t n_vCh=0;
  for(; ith!=ith_e; ++ith){
    // push back
    m_tgcRdo->push_back(*ith);

    n_vCh +=1;

  }
  if(t_debug) {
    log << MSG::DEBUG
	     << n_vCh << "words of were recorded in TgcRdo container" 
	     << endmsg;
  } 

  return StatusCode::SUCCESS;
 
}

// Decode the ROD header
StatusCode Muon::TgcRODReadOut::decodeHeader(const ByteStream& vData)
{ 
  int vDataIndex=0;
  
  // Start Maker
  #ifndef NDEBUG
  MsgStream log(Athena::getMessageSvc(),"Muon::TgcRODReadOut");
  if(vData[vDataIndex] != ROD_START) {
    log << MSG::DEBUG 
	       << "ROD Start of header marker not found" << endmsg;
  }
  #endif
  ++vDataIndex;
  
  // Header Size
  #ifndef NDEBUG
  if(vData[vDataIndex] != ROD_HEADER_SIZE) {
    log << MSG::DEBUG 
	       << "ROD header size doesn't match " << vData[vDataIndex] 
	       << endmsg;
  }
  #endif
  ++vDataIndex;
  
  // Format version
  ++vDataIndex;
  
  // decode the rest of the header
  
  // Source ID 
  uint16_t subDetectorId = ((vData[vDataIndex]) & 0x00FF0000) >> 16;   
  uint16_t rodId =  (vData[vDataIndex]) & 0x000000FF;

  ++vDataIndex;
  
  // Run Number
  ++vDataIndex;

  // L1ID
  uint32_t l1Id = (vData[vDataIndex]) & 0xFFFFFFFF;
  ++vDataIndex;

  // BCID
  uint16_t bcId = (vData[vDataIndex]) & 0x0000FFFF;
  ++vDataIndex;

  // Trigger Type
  uint16_t triggerType = (vData[vDataIndex]) & 0x000000FF;
  ++vDataIndex;

  // set attributes
  if(!setRodAttributes(subDetectorId,rodId,l1Id,bcId,triggerType)){
    #ifndef NDEBUG
    log << MSG::DEBUG << "Can't set ROD Attributes: Skip decoding of remaining hits of this event..." << endmsg;
    #endif
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;
}


uint16_t Muon::TgcRODReadOut::subDetectorId() const {
  return m_tgcRdo->subDetectorId();
}

uint16_t Muon::TgcRODReadOut::rodId() const {
  return m_tgcRdo->rodId();
}

uint32_t Muon::TgcRODReadOut::l1Id() const {
  return m_tgcRdo->l1Id();
}

uint16_t Muon::TgcRODReadOut::bcId() const {
  return m_tgcRdo->bcId();
}

uint16_t Muon::TgcRODReadOut::triggerType() const {
  return m_tgcRdo->triggerType();
}

// set RDO attributes
StatusCode Muon::TgcRODReadOut::setRodAttributes(const uint16_t subDetectorId, 
						 const uint16_t rodId,
						 const uint32_t l1Id,
						 const uint16_t bcId,
						 const uint16_t triggerType)
{
  m_tgcRdo->setOnlineId(subDetectorId, rodId);
  m_tgcRdo->setL1Id(l1Id);
  m_tgcRdo->setBcId(bcId);
  m_tgcRdo->setTriggerType(triggerType);
  return StatusCode::SUCCESS;
}

// set sbLoc to slb 
bool Muon::TgcRODReadOut::setSbLoc(uint16_t subDetectorId,
				   uint16_t rodId, 
				   TgcSlbData * slb, 
				   int rxId) 
{
  ISvcLocator* svcLocator = Gaudi::svcLocator();

  MsgStream log(Athena::getMessageSvc(),"Muon::TgcRODReadOut");
  bool t_debug = (log.level()<=MSG::DEBUG);

  if(m_cabling ==0) {
    // TGCcablingSvc
    // get Cabling Server Service
    const ITGCcablingServerSvc* TgcCabGet = 0;
    StatusCode sc = svcLocator->service("TGCcablingServerSvc", TgcCabGet);
    if(sc.isFailure()) {
      log << MSG::ERROR << " Can't get TGCcablingServerSvc " << endmsg;
      return false;
    }
    
    // get Cabling Service
    sc = TgcCabGet->giveCabling(m_cabling);
    if(sc.isFailure()) {
      log << MSG::ERROR << " Can't get ITGCcablingSvc from TGCcablingServerSvc" << endmsg;
      return false; 
    }
    int maxRodId,maxSswId, maxSbloc,minChannelId, maxChannelId;
    m_cabling->getReadoutIDRanges(maxRodId,maxSswId, maxSbloc,minChannelId, maxChannelId);
    bool isAtlas = (maxRodId==12);
    if(isAtlas) {
      log << MSG::INFO << m_cabling->name() << " is OK" << endmsg ;
    } else {
      log << MSG::ERROR 
		 << "TGCcablingSvc(octant segmentation) can not be used for TgcRODReadOut" 
		 << endmsg ;
      return false;
    }
  }

  // get sbLoc
  int phi = 0;
  bool isAside = false;
  bool isEndcap = false;
  int moduleType = 0;
  int slbId = 0;
  int dummy_subDetectorId = 0;
  int dummy_rodId = 0;
  int dummy_sswId = 0;
  int sbLoc = 0;
  int slbaddress_ret = 0;
  
  int sswId = slb->getSswId();

  if(!m_cabling->getSLBIDfromRxID(phi, 
				  isAside, 
				  isEndcap, 
				  moduleType, 
				  slbId,
				  subDetectorId,
				  rodId,
				  sswId,
				  rxId)) {
    unsigned int tmpRodId = rodId + (subDetectorId==CSIDE ? NROD/2 : 0);
    if(tmpRodId>NROD) tmpRodId = NROD;

    if(t_debug || !m_failedGetSLBIDfromRxID[tmpRodId]) {
      log << (!m_failedGetSLBIDfromRxID[tmpRodId] ? MSG::WARNING : MSG::DEBUG)
		  << " Fail to getSLBIDfromRxID "
		  << " :rodId=" <<rodId
		  << " :sswId="<<sswId
		  << " :rxId= "<<rxId
		  << endmsg;
          }
    m_failedGetSLBIDfromRxID[tmpRodId]++;
    
    return false;
  }

  if(!m_cabling->getReadoutIDfromSLBID(phi,
				       isAside,
				       isEndcap,
				       moduleType,
				       slbId,
				       dummy_subDetectorId,
				       dummy_rodId,
				       dummy_sswId,
				       sbLoc)) {
    unsigned int tmpRodId = rodId + (subDetectorId==CSIDE ? NROD/2 : 0);
    if(tmpRodId>NROD) tmpRodId = NROD;
    
    if((t_debug || !m_failedGetReadoutIDfromSLBID[tmpRodId])) {
      log  << (!m_failedGetReadoutIDfromSLBID[tmpRodId] ? MSG::WARNING : MSG::DEBUG)
		<< " Fail to getReadoutIDfromSLBID "
		<< " :rodId=" <<rodId
		<< " :sswId="<<sswId
		<< " :rxId= "<<rxId
		<< endmsg;
    }
    m_failedGetReadoutIDfromSLBID[tmpRodId]++;
    
    return false;
  }
  
  if(m_cabling->getSLBAddressfromReadoutID(slbaddress_ret,
					   subDetectorId,
					   rodId,
					   sswId,
					   sbLoc)) {
    // check SLB  Address
    if(slbaddress_ret != slb->getSlbId() ) {
      if(t_debug) {
	log << MSG::DEBUG     
		  << " wrong getSLBAddress  :"
		  <<" :sswId = "<<sswId
		  <<" :rxId = " <<rxId  << ":sbLoc = " << sbLoc 
		  << endmsg;	 
	log << MSG::DEBUG     
		  <<" :slbAddress = " << slb->getSlbId() 
		  <<" expected value = " << slbaddress_ret
		  << endmsg;
      }
    } 
  }

  // set sbLoc
  slb->setSbLoc(sbLoc);
     
  return true;
}
