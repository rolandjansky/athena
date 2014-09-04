/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     LArBadFebMaskingTool
PACKAGE:  offline/Calorimeter/LArCellRec

AUTHORS: G.Unal
CREATION:  23 april 2009

PURPOSE:

********************************************************************/

#include "LArCellRec/LArBadFebMaskingTool.h"

#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h" 
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "CaloEvent/CaloCellContainer.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArRawEvent/LArFebErrorSummary.h" 
#include "LArIdentifier/LArOnlineID.h" 
#include "LArRecConditions/ILArBadChanTool.h"
#include "LArRecConditions/LArBadFeb.h"
#include "LArTools/LArCablingService.h"
#include "LArRecEvent/LArEventBitInfo.h"
#include "xAODEventInfo/EventInfo.h"

using xAOD::EventInfo;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

LArBadFebMaskingTool::LArBadFebMaskingTool(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  :AlgTool(type, name, parent),
   m_badChannelTool(""),m_maskParity(true),m_maskSampleHeader(true),m_maskEVTID(true),m_maskScacStatus(true),
   m_maskScaOutOfRange(true),m_maskGainMismatch(true),m_maskTypeMismatch(true),m_maskNumOfSamples(true),
   m_maskEmptyDataBlock(true),m_maskDspBlockSize(true),m_maskCheckSum(true),m_maskMissingHeader(true),
   m_maskBadGain(true),
   m_larFebErrorSummaryKey("LArFebErrorSummary")

{ 
  declareInterface<ICaloCellMakerTool>(this); 

  declareProperty("badChannelTool",m_badChannelTool);
  declareProperty("maskParity",m_maskParity);
  declareProperty("maskSampleHeader",m_maskSampleHeader);
  declareProperty("maskEVTID",m_maskEVTID);
  declareProperty("maskScacStatus",m_maskScacStatus);
  declareProperty("maskScaOutOfRange",m_maskScaOutOfRange);
  declareProperty("maskGainMismatch",m_maskGainMismatch);
  declareProperty("maskTypeMismatch",m_maskTypeMismatch);
  declareProperty("maskNumOfSamples",m_maskNumOfSamples);
  declareProperty("maskEmptyDataBlock",m_maskEmptyDataBlock);
  declareProperty("maskDspBlockSize",m_maskDspBlockSize);
  declareProperty("maskCheckSum",m_maskCheckSum);
  declareProperty("maskMissingHeader",m_maskMissingHeader);
  declareProperty("maskBadGain",m_maskBadGain);
  declareProperty("larFebErrorSummaryKey",m_larFebErrorSummaryKey);
}


/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
/////////////////////////////////////////////////////////////////////

StatusCode LArBadFebMaskingTool::initialize()
{
  MsgStream  log(msgSvc(),name());

  m_evt=0;
  m_mask=0;

  m_errorToMask = 0;
  if (m_maskParity)         m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::Parity);
  if (m_maskSampleHeader)   m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::SampleHeader);
  if (m_maskEVTID)          m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::EVTID);
  if (m_maskScacStatus)     m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::ScacStatus);
  if (m_maskScaOutOfRange)  m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::ScaOutOfRange);
  if (m_maskGainMismatch)   m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::GainMismatch);
  if (m_maskTypeMismatch)   m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::TypeMismatch);
  if (m_maskNumOfSamples)   m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::NumOfSamples);
  if (m_maskEmptyDataBlock) m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::EmptyDataBlock);
  if (m_maskDspBlockSize)   m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::DspBlockSize);
  if (m_maskCheckSum)       m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::CheckSum);
  if (m_maskMissingHeader)  m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::MissingHeader);
  if (m_maskBadGain)        m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::BadGain);

  log << MSG::INFO << " bit mask for errors to mask " << m_errorToMask << endreq;

  //initialize the StoreGateSvc ptr
  StatusCode sc = service("StoreGateSvc", m_storeGate); 
  if (sc.isFailure()) {
    log << MSG::ERROR << " Cannot retrieve StoreGateSvc" << endreq;
    return sc;
  } 

  //initialize the detectorStore ptr
  sc = service("DetectorStore", m_detStore); 
  if (sc.isFailure()) {
    log << MSG::ERROR << " Cannot retrieve detectorStore" << endreq;
    return sc;
  } 

  // callback to GeoModel to retrieve identifier helpers, etc..
  const IGeoModelSvc *geoModel=0;
  sc = service("GeoModelSvc", geoModel);
  if(sc.isFailure())
  {
    log << MSG::ERROR << "Could not locate GeoModelSvc" << endreq;
    return sc;
  }

  // dummy parameters for the callback:
  int dummyInt=0;
  std::list<std::string> dummyList;

  if (geoModel->geoInitialized())
  {
    return geoInit(dummyInt,dummyList);
  }
  else
  {
    sc = m_detStore->regFcn(&IGeoModelSvc::geoInit,
                          geoModel,
                          &LArBadFebMaskingTool::geoInit,this);
    if(sc.isFailure())
    {
      log << MSG::ERROR << "Could not register geoInit callback" << endreq;
      return sc;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode LArBadFebMaskingTool::geoInit(IOVSVC_CALLBACK_ARGS)
{

  MsgStream  log(msgSvc(),name());

  const  CaloIdManager* caloIdMgr;
  StatusCode sc = m_detStore->retrieve( caloIdMgr );
  if (sc.isFailure()) {
   log << MSG::ERROR << "Unable to retrieve CaloIdMgr " << endreq;
   return sc;
  }
  m_calo_id = caloIdMgr->getCaloCell_ID();


  // translate offline ID into online ID
  sc = m_cablingService.retrieve();
  if(sc.isFailure()){
    log << MSG::ERROR << "Could not retrieve LArCablingService Tool" << endreq;
    return sc;
  }
  
  sc = m_detStore->retrieve(m_onlineID, "LArOnlineID");
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not get LArOnlineID helper !" << endreq; 
    return sc;
  }

  if (!m_badChannelTool.empty()) {
    sc = m_badChannelTool.retrieve();
    if (sc.isFailure()) {
     log << MSG::ERROR << "Could not retrieve bad channel tool " << endreq;
     return sc;
    }
  }

  return StatusCode::SUCCESS;

}

StatusCode LArBadFebMaskingTool::finalize()
{
   MsgStream  log(msgSvc(),name());
   log << MSG::INFO << "  ---- Summary from LArBadFebMaskingTool " << endreq;
   log << MSG::INFO << "   Number of events processed    " << m_evt << endreq;
   log << MSG::INFO << "   Number of masked Feb total    " << m_mask << endreq;
   float ratio=0.;
   if (m_evt>0) ratio=((float)(m_mask))/((float)(m_evt));
   log << MSG::INFO << "   Number of masked Feb per event " << ratio << endreq;

   return StatusCode::SUCCESS;
}

StatusCode LArBadFebMaskingTool::process(CaloCellContainer * theCont )
{
	
  MsgStream  log(msgSvc(),name());
  bool dump=false;
  if (log.level()<=MSG::DEBUG) dump=true;

  m_evt++;


  if(dump) log << MSG::DEBUG << " in  LArBadFebMaskingTool::process " << endreq;
  const LArFebErrorSummary* larFebErrorSummary;
  StatusCode sc = m_storeGate->retrieve(larFebErrorSummary,m_larFebErrorSummaryKey);
  if (sc.isFailure()) {
    log << MSG::WARNING << " cannot retrieve Feb error summary.  Skip  LArBadFebMaskingTool::process " << endreq;
    return StatusCode::SUCCESS;
  }

  // retrieve map of Feb-errors
  const std::map<unsigned int,uint16_t>& febMap = larFebErrorSummary->get_all_febs();

  if(dump) log << MSG::DEBUG << " Number of Febs " << febMap.size() << endreq;

  // retrieve EventInfo
  const EventInfo* eventInfo_c=0;
  sc = m_storeGate->retrieve(eventInfo_c);
  if (sc.isFailure()) {
     log << MSG::WARNING << " cannot retrieve EventInfo, will not set LAr bit information " << endreq;
  }
  EventInfo* eventInfo=0;
  if (eventInfo_c) {
    eventInfo = const_cast<EventInfo*>(eventInfo_c);
  }
  
  bool flagBadEvent = false;   // flag bad event = Feb error not known in database

  // catch cases of empty LAR container  => severe problem in decoding => flag event as in ERROR
  unsigned int nLar = theCont->nCellsCalo(CaloCell_ID::LAREM)+theCont->nCellsCalo(CaloCell_ID::LARHEC)+theCont->nCellsCalo(CaloCell_ID::LARFCAL);
  if (nLar==0) {
     if(dump) log << MSG::DEBUG <<  " empty lar cell container " << endreq;
     flagBadEvent = true;
  }

// loop over all Febs

  std::vector<HWIdentifier>::const_iterator feb = m_onlineID->feb_begin();
  std::vector<HWIdentifier>::const_iterator feb_end = m_onlineID->feb_end();


  for ( ; feb != feb_end; feb++) {

      bool toMask1 = false;   // mask because of bad error
      bool inError = false;   // mask because Feb listed in database as being to mask
      bool isDead  = false;   // set to true if Feb is deadAll or deadReadout 

// for debug
      HWIdentifier febId = (*feb);
      unsigned int ifeb = febId.get_identifier32().get_compact();
      if(dump) log << MSG::DEBUG << " process Feb: " << ifeb << " ";

      std::map<unsigned int,uint16_t>::const_iterator it1 = febMap.find(ifeb);
      if (it1 != febMap.end()) {
        uint16_t ierror = (*it1).second;
        if (ierror & m_errorToMask) toMask1=true;
        if(dump) log << MSG::DEBUG << " ierror,toMask " << ierror << " " << toMask1 << " ";
      }

      if (!m_badChannelTool.empty()) {
         LArBadFeb febstatus = m_badChannelTool->febStatus(febId);
         inError = febstatus.inError();
         isDead = ( febstatus.deadReadout() | febstatus.deadAll() );
         if(dump) log << MSG::DEBUG << " inError, isDead "  << inError << " " << isDead;
      }
      if (dump) log << MSG::DEBUG << endreq;

      if (toMask1 && !inError && !isDead) flagBadEvent=true;

      if (toMask1 || inError) {
         m_mask++;
         for (int ch=0; ch<128; ++ch) {
           HWIdentifier hwid = m_onlineID->channel_Id(febId, ch);
           if (m_cablingService->isOnlineConnected(hwid)) {
              Identifier id = m_cablingService->cnvToIdentifier( hwid);
              IdentifierHash theCellHashID = m_calo_id->calo_cell_hash(id);
              int index = theCont->findIndex(theCellHashID);
              if (index<0) {
                 if(dump) log << MSG::DEBUG << " cell " << hwid.get_compact() << " " << id.get_compact() << " is not in the container " << endreq;
                 continue;
              }
              CaloCell* aCell = theCont->at(index);

              if (aCell) {
                if(dump) log << MSG::DEBUG << "  mask cell hwid= " << hwid.get_compact() << " offlineid = " << id.get_compact()
                  << " " << aCell->ID().get_compact() << endreq;
                aCell->setEnergy(0.);
                aCell->setTime(0.);
                uint16_t qua=0;
                aCell->setQuality(qua);
                uint16_t provenance = (aCell->provenance() | 0x0800);
                aCell->setProvenance(provenance);
              }

           }  // isConnected
         }    // loop over channels in Feb
      }       // toMask

  }       // loop over Febs in error


  if (eventInfo && flagBadEvent) {
    if (dump) log << MSG::DEBUG << " set error bit for LAr for this event " << endreq;
    if (!eventInfo->setErrorState(EventInfo::LAr,EventInfo::Error)) {
         log << MSG::WARNING << " cannot set error state for LAr " << endreq;
    }
    if (!eventInfo->setEventFlagBit(EventInfo::LAr,LArEventBitInfo::DATACORRUPTED)) {
        log << MSG::WARNING << " cannot set event bit info for LAr " << endreq;
    }
  }
 

  return StatusCode::SUCCESS;
}
