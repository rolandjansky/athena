/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTRawDataProviderTool.h"
#include "GaudiKernel/IToolSvc.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "ByteStreamData/RawEvent.h" 
#include "SCT_ConditionsServices/ISCT_ByteStreamErrorsSvc.h"
#include "xAODEventInfo/EventInfo.h"
#include "EventInfo/EventInfo.h"
#include "SCT_RawDataByteStreamCnv/ISCT_RodDecoder.h"

//using xAOD::EventInfo;

/// -------------------------------------------------------
/// contructor

SCTRawDataProviderTool::SCTRawDataProviderTool
( const std::string& type, const std::string& name,const IInterface* parent )
  :  AthAlgTool(type,name,parent),
     m_decoder   ("SCT_RodDecoder", this), 
     m_bsErrSvc  ("InDetSCT_ByteStreamErrorsSvc",name),
     m_robIdSet()
{
  declareProperty ("Decoder", m_decoder);
  declareProperty ("ErrorsSvc",m_bsErrSvc);
  declareInterface< ISCTRawDataProviderTool >( this );   
}

/// -------------------------------------------------------
/// destructor 

SCTRawDataProviderTool::~SCTRawDataProviderTool()
{}
 
/// -------------------------------------------------------
/// initialize

StatusCode SCTRawDataProviderTool::initialize()
{

  StatusCode sc = AlgTool::initialize(); 
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Failed to init baseclass" << endmsg;
    return StatusCode::FAILURE;
  }
   
  /** Retrieve decoder */
  if (m_decoder.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_decoder << endmsg;
    return StatusCode::FAILURE;
  } else 
    msg(MSG::DEBUG) << "Retrieved tool " << m_decoder << endmsg;

  /** Get ByteStreamErrorsSvc  */
  if (m_bsErrSvc.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve service " << m_bsErrSvc << endmsg;
    return StatusCode::FAILURE;
  } else
    msg(MSG::DEBUG) << "Retrieved service " << m_bsErrSvc << endmsg;

  IIncidentSvc* incsvc;
  sc = service("IncidentSvc", incsvc);
  int priority = 100;
  if( sc.isSuccess() ) {
    incsvc->addListener( this, "BeginEvent", priority);
  }

  return StatusCode::SUCCESS;
}

/// -------------------------------------------------------
/// finalize

StatusCode SCTRawDataProviderTool::finalize()
{
  StatusCode sc = AlgTool::finalize(); 
  return sc;
}

/// -------------------------------------------------------
/// convert method

void SCTRawDataProviderTool::handle(const Incident& inc) {
  if (inc.type() == "BeginEvent") {
    /** reset list of known robIds */
    m_robIdSet.clear(); 
  }  
}

StatusCode SCTRawDataProviderTool::convert( std::vector<const ROBFragment*>& vecRobs,SCT_RDO_Container* rdoIdc ){
  if(vecRobs.empty()) return StatusCode::SUCCESS;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "SCTRawDataProviderTool::convert()" << endmsg;
  static int DecodeErrCount = 0;

  /**  are we working on a new event ? */
  std::vector<const ROBFragment*>::const_iterator rob_it = vecRobs.begin();  
  
  StatusCode sc = StatusCode::SUCCESS;

  /** loop over the ROB fragments */

  for(; rob_it!=vecRobs.end(); ++rob_it) {
    /** get the ID of this ROB/ROD */
    uint32_t robid = (*rob_it)->rod_source_id();
    /** check if this ROBFragment was already decoded (EF case in ROIs) */
    if (!m_robIdSet.insert(robid).second) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " ROB Fragment with ID  "
					      << std::hex<<robid<<std::dec
					      << " already decoded, skip" << endmsg; 
    } else {
      sc = m_decoder->fillCollection( &**rob_it, rdoIdc);
      if ( sc==StatusCode::FAILURE ) {
	if ( DecodeErrCount < 100 ) {
	  msg(MSG::ERROR) << "Problem with SCT ByteStream Decoding!" << endmsg;
	  DecodeErrCount++;
	} else if ( 100 == DecodeErrCount ) {
	  msg(MSG::ERROR) << "Too many Problem with SCT Decoding messages, turning message off.  "<< endmsg;
	  DecodeErrCount++;
	}
      }
    }
  }  

  if (sc==StatusCode::FAILURE) {
    msg(MSG::ERROR) << "There was a problem with SCT ByteStream conversion" << endmsg;   
    return sc;
  }
  int nLVL1ID = m_bsErrSvc->getErrorSet(SCT_ByteStreamErrors::LVL1IDError)->size();
  int nROBFragment = m_bsErrSvc->getErrorSet(SCT_ByteStreamErrors::ROBFragmentError)->size();

  if (nLVL1ID > 500 or nROBFragment > 1000) {
    //// retrieve EventInfo.  
    /// First the xAOD one
    bool setOK_xAOD = false;
    const xAOD::EventInfo* xevtInfo_const;
    sc = evtStore()->retrieve(xevtInfo_const);  
    if (sc==StatusCode::SUCCESS) {
      xAOD::EventInfo* xevtInfo=0;
      xevtInfo=const_cast<xAOD::EventInfo*>(xevtInfo_const);
      setOK_xAOD = xevtInfo->setErrorState(xAOD::EventInfo::SCT, xAOD::EventInfo::Error);
    } 

    /// Second the old-style one
    bool setOK_old = false;
    const EventInfo* evtInfo_const;
    sc = evtStore()->retrieve(evtInfo_const);  
    if (sc==StatusCode::SUCCESS) {
      EventInfo* evtInfo = 0;
      evtInfo =const_cast<EventInfo*>(evtInfo_const);
      setOK_old = evtInfo->setErrorState(EventInfo::SCT, EventInfo::Error);
    }

    if ((not setOK_xAOD) and (not setOK_old)) {
      msg(MSG::ERROR)<<"Failed to retrieve EventInfo containers or to set error states"<<endmsg;
      return StatusCode::RECOVERABLE;
    }

    sc = StatusCode::SUCCESS;
  } /// 500 LVL1ID errors or 1000 ROBFragment errors

  return sc; 

}

