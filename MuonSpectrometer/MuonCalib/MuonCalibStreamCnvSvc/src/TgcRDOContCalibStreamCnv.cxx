/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibStreamCnvSvc/TgcRDOContCalibStreamCnv.h"
#include "MuonCalibStreamCnvSvc/MuonCalibStreamAddress.h" 
#include "MuonCalibStreamCnvSvc/MuonCalibStreamDataProviderSvc.h" 
#include "MuCalDecode/CalibEvent.h" 
#include "MuCalDecode/CalibData.h" 
#include "MuCalDecode/CalibUti.h" 

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"

#include "MuonRDO/TgcRdo.h"
#include "MuonRDO/TgcRawData.h"
#include "MuonRDO/TgcRdoContainer.h"
#include "MuonIdHelpers/TgcIdHelper.h"
//#include "TGCcablingInterface/ITGCcablingServerSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "CLIDSvc/CLASS_DEF.h"

#include <iostream>
#include <map> 
#include <list> 

using namespace  LVL2_MUON_CALIBRATION;


TgcRDOContCalibStreamCnv::TgcRDOContCalibStreamCnv(ISvcLocator* svcloc) :
  Converter(MuonCalibStream_StorageType, classID(),svcloc),
  m_rdoContainer(NULL)
{
  MsgStream log ( messageService(),"TgcRDOContCalibStreamCnv") ;
  log << MSG::DEBUG<< " construct " <<endreq; 
}

TgcRDOContCalibStreamCnv::~TgcRDOContCalibStreamCnv() {
}

const CLID& TgcRDOContCalibStreamCnv::classID(){
return ClassID_traits<TgcRdoContainer>::ID() ;
}

int TgcRDOContCalibStreamCnv::getRodIdFromSectorId(int tmp_sectorId){
  return ((tmp_sectorId%12) + 1);
}

uint16_t TgcRDOContCalibStreamCnv::bcTagCnv(uint16_t bcBitMap){
  return (bcBitMap == 4 ? 1:
	  (bcBitMap == 2 ? 2:
	   (bcBitMap == 1 ? 3:
	    0)));
}


StatusCode TgcRDOContCalibStreamCnv::initialize() {

   StatusCode sc = Converter::initialize(); 
   if (StatusCode::SUCCESS!=sc) { 
    return sc; 
   } 

   MsgStream log(messageService(), "TgcRDOContCalibStreamCnv");
   log << MSG::DEBUG<< " initialize " <<endreq; 

   // Retrieve the TgcIdHelper
   // and get the detector descriptor manager
   StoreGateSvc* detStore = 0;
   sc = serviceLocator()->service("DetectorStore", detStore);
   if (sc.isSuccess()) {      
     sc = detStore->retrieve(m_tgcIdHelper, "TGCIDHELPER" );
     if (sc.isFailure()) {
       log << MSG::FATAL << " Cannot retrieve TgcIdHelper " << endreq;
       return sc;
     }
     sc = detStore->retrieve( m_muonMgr );
     if (sc.isFailure()) {
       log << MSG::FATAL << " Cannot retrieve MuonGeoModel " << endreq;
       return sc;
     }
   } else {
     log << MSG::ERROR << " Can't locate DetectorStore " << endreq;  
     return sc;
   }

    // retreive also the dataProviderSvc
    IService* svc ;
    sc = serviceLocator()->getService("MuonCalibStreamDataProviderSvc", svc);
    if(sc != StatusCode::SUCCESS ) {
      log<<MSG::ERROR << " Cant get MuonCalibStreamDataProviderSvc " <<endreq;
      return sc ;
    }
    
    m_dataProvider = dynamic_cast<MuonCalibStreamDataProviderSvc*> (svc);
    if(m_dataProvider == 0 ) {
      log<<MSG::ERROR<< " Cant cast to MuonCalibStreamDataProviderSvc " <<endreq;                                                                                
      return StatusCode::FAILURE ;
    }
    
    // create an empty rdo container and record it
    m_rdoContainer = new TgcRdoContainer(m_muonMgr->tgcIdHelper()->module_hash_max()); 
    m_rdoContainer->addRef();
    
    
    return service("StoreGateSvc", m_storeGate); 
    
}

StatusCode
TgcRDOContCalibStreamCnv::createObj(IOpaqueAddress* /*pAddr*/, DataObject*& pObj) 
{
  MsgStream log(messageService(), "TgcRDOContCalibStreamCnv");

  StatusCode sc;
  
  if(m_rdoContainer) {
    m_rdoContainer->cleanup();
  } else {    
    // should a RodContainer be created??
    return StatusCode::FAILURE;
  }
  // register it in StoreGate.
  std::string key = "TGCRDO";
  sc = m_storeGate->record(m_rdoContainer,key);
  if (sc.isFailure()) log << MSG::ERROR << "Fail to record TGC RDO container in TDS" << endreq;

  sc=fillCollections(pObj); 
  if(!sc.isSuccess()) return sc; 
  pObj = StoreGateSvc::asStorable( m_rdoContainer );
  return sc; 
}

 StatusCode 
TgcRDOContCalibStreamCnv::createRep(DataObject* /*pObj*/, IOpaqueAddress*& /*pAddr*/) {
   MsgStream log(messageService(), "TgcRDOContCalibStreamCnv");
   log << MSG::ERROR << " No conversion TgcRDO to stream " << endreq ; 
   return   StatusCode::FAILURE ; 
}

StatusCode TgcRDOContCalibStreamCnv::fillCollections(DataObject*& pObj) 
{
  MsgStream log(messageService(), "TgcRDOContCalibStreamCnv");
  
  const CalibEvent* event= m_dataProvider->getEvent(); 
  
  int l1Id=event->lvl1_id();
  int bcId=0; // DUMMY BCID
  
  
  if(event->tgc()!=NULL){
    std::list<TgcCalibData> m_listOfTGCCalibData = (event->tgc())->data();
    std::list<TgcCalibData>::iterator m_itOfTGCCalibData = m_listOfTGCCalibData.begin();
    //std::list<TgcRawData> m_listOfRDO;
    
    int tgc_systemId    =(*(event->tgc())).systemId();      
    int tgc_subsystemId =(*(event->tgc())).subsystemId();
    //int tgc_roiNumber   =(*(event->tgc())).roiNumber();
    // sectorId absent in new decoding classes
    //int tgc_sectorId    =(*(event->tgc())).sectorId();
    //int tgc_rodId       =getRodIdFromSectorId(tgc_sectorId);
    // to be checked!!!
    
    int tgc_rodId       =getRodIdFromSectorId((*(event->tgc())).rdoId());
    
    uint16_t rdoId = TgcRdo::calculateOnlineId(tgc_subsystemId,tgc_rodId);
    TgcRdoIdHash rdoIdHash;
    int idHash = rdoIdHash(rdoId);
    
    log << MSG::DEBUG << std::endl << std::hex
	<<" systemId " << tgc_systemId 
	<<" subsystemId " << tgc_subsystemId 
      // <<" sectorId "<< tgc_sectorId
	<<" rodId "<< tgc_rodId
	<<" rdoId "<< std::hex << rdoId 
	<<" idHash "<< idHash 
	<<endreq; 
    
    
    TgcRdo* newrdo = new TgcRdo(tgc_subsystemId,tgc_rodId,bcId,l1Id);
    //TgcRdo* newrdo = new TgcRdo(rdoId, idHash);
    
    //std::list<TgcCalibData> tgcdata = (event->tgc())->data();
    {
      // loop over the list
      // from here onwards you need to insert TGC dependent code 
      // to copy the contents of TgcCalibData objects in RDO objects
      // TgcCalibData can be found in 
      // Trigger/TrigAlgorithms/TrigmuFast/TrigmuFast/CalibData.h
      // looking at tags from TrigmuFast-00-02-76-34 onward (ONLY IN
      // TrigmuFast-00-02-76 BRANCH!!!) 
      // once you have created and filled a TgcRdo it should be added
      // with  
      // StatusCode sc = m_rdoContainer->addCollection(newrdo,elementHash );
      // the main problem here on the athena side is to findout 
      // the proper ids and hashes to be used when creating the new TgcRdo and
      // when adding it to the container...
    while(m_itOfTGCCalibData != m_listOfTGCCalibData.end())
      {
	std::list<TGC_BYTESTREAM_READOUTHIT> m_listOfBsReadoutHit
	  = (*(m_itOfTGCCalibData)).readoutHit();
	std::list<TGC_BYTESTREAM_READOUTTRACKLET> m_listOfBsReadoutTracklet
	  = (*(m_itOfTGCCalibData)).readoutTracklet();
	std::list<TGC_BYTESTREAM_READOUTTRIPLETSTRIP> m_listOfBsReadoutTripletStrip
	  = (*(m_itOfTGCCalibData)).readoutTripletStrip();
	std::list<TGC_BYTESTREAM_HIPT> m_listOfBsHipt
	  = (*(m_itOfTGCCalibData)).readoutHipt();
	std::list<TGC_BYTESTREAM_SL> m_listOfBsSL
	  = (*(m_itOfTGCCalibData)).readoutSL();
	
	// Filling Readout Format Hit Data
	std::list<TGC_BYTESTREAM_READOUTHIT>::iterator m_itOfBsReadoutHit
	  = m_listOfBsReadoutHit.begin();
	
	while(m_itOfBsReadoutHit != m_listOfBsReadoutHit.end())
	  {
	    TGC_BYTESTREAM_READOUTHIT roh= *(m_itOfBsReadoutHit);
	    
	   
 	    //printf("raw data %08x \n",roh);
	    log << MSG::DEBUG << std::hex
		<<"TgcRawData READOUT FORMATTED HIT "<<std::endl
		<<" bcTag " <<bcTagCnv(roh.bcBitmap)
		<<" subDetectorId " <<newrdo->subDetectorId()
		<<" rodId " <<newrdo->rodId()
		<<" sswId " <<roh.ldbId
		<<" sbId " <<roh.sbId
		<<" l1Id " <<newrdo->l1Id()
		<<" bcId " <<newrdo->bcId()
		<<" sbType " <<(TgcRawData::SlbType)roh.sbType
		<<" adjucent " <<(bool)roh.adj
		<<" associate tracklet " <<roh.tracklet
		<<" bitPos " <<roh.channel+40 // is it fixed or not ? (yasuyuki)
		<<endreq; 
	    
	    
	    
	    TgcRawData* raw = new TgcRawData(bcTagCnv(roh.bcBitmap),
					     newrdo->subDetectorId(),
					     newrdo->rodId(),
					     roh.ldbId,
					     roh.sbId,
					     newrdo->l1Id(),
					     newrdo->bcId(),
					     (TgcRawData::SlbType)roh.sbType,
					     (bool)roh.adj,
					     roh.tracklet,
					     roh.channel+40 // is it fixed or not ? (yasuyuki)
					     );
	    newrdo->push_back(raw);
	    m_itOfBsReadoutHit++;
	  }
	// ---------------------
	
	
	// Filling Readout Format Triplet Strip Hit
	std::list<TGC_BYTESTREAM_READOUTTRIPLETSTRIP>::iterator m_itOfBsReadoutTripletStrip
	  = m_listOfBsReadoutTripletStrip.begin();
	
	
	while(m_itOfBsReadoutTripletStrip != m_listOfBsReadoutTripletStrip.end())
	  {
	    TGC_BYTESTREAM_READOUTTRIPLETSTRIP rostrip= *(m_itOfBsReadoutTripletStrip);

	    //printf("raw data %08x \n",rostrip);
	    log << MSG::DEBUG << std::hex
	      //<<"raw data :: "<<(unsigned long)(*rostrip)<<std::endl
		<<"TgcRawData READOUT TRIPLET STRIP "<<std::endl
		<<" bcTag " <<bcTagCnv(rostrip.bcBitmap)
		<<" subDetectorId " <<newrdo->subDetectorId()
		<<" rodId " <<newrdo->rodId()
		<<" sswId " <<rostrip.ldbId
		<<" sbId " <<rostrip.sbId
		<<" l1Id " <<newrdo->l1Id()
		<<" bcId " <<newrdo->bcId()
		<<" sbType " <<TgcRawData::SLB_TYPE_TRIPLET_STRIP
		<<" adjucent " <<"0"
		<<" seg " <<rostrip.seg
		<<" subc " <<rostrip.subc
		<<" phi " <<rostrip.phi
		<<endreq; 
	    
	    TgcRawData* raw = new TgcRawData(bcTagCnv(rostrip.bcBitmap),
					     newrdo->subDetectorId(),
					     newrdo->rodId(),
					     rostrip.ldbId,
					     rostrip.sbId,
					     newrdo->l1Id(),
					     newrdo->bcId(),
					     TgcRawData::SLB_TYPE_TRIPLET_STRIP,
					     0,
					     rostrip.seg,
					     rostrip.subc,
					     rostrip.phi
					     );
	    
	    newrdo->push_back(raw);
	    m_itOfBsReadoutTripletStrip++;
	  }      
	// ---------------------
	
	
	// Filling Readout Format Tracklet Data
	std::list<TGC_BYTESTREAM_READOUTTRACKLET>::iterator m_itOfBsReadoutTracklet
	  = m_listOfBsReadoutTracklet.begin();
	
	while(m_itOfBsReadoutTracklet != m_listOfBsReadoutTracklet.end())
	  {
	    TGC_BYTESTREAM_READOUTTRACKLET rotrk = *(m_itOfBsReadoutTracklet);

	    //printf("raw data %08x \n",rotrk);
	    log << MSG::DEBUG << std::hex
	      //<<"raw data :: "<<(unsigned long)(*rotrk)<<std::endl
		<<"TgcRawData READOUT TRACKLET "<<std::endl
		<<" bcTag " <<bcTagCnv(rotrk.bcBitmap)
		<<" subDetectorId " <<newrdo->subDetectorId()
		<<" rodId " <<newrdo->rodId()
		<<" sswId " <<rotrk.ldbId
		<<" sbId " <<rotrk.sbId
		<<" l1Id " <<newrdo->l1Id()
		<<" bcId " <<newrdo->bcId()
		<<" sbType " <<((rotrk.slbType == 4) ? TgcRawData::SLB_TYPE_INNER_STRIP : (TgcRawData::SlbType)rotrk.slbType)
		<<" adjucent " <<"0"
		<<" seg " <<rotrk.seg
		<<" zero " <<"0"
		<<" r phi " <<rotrk.rphi
		<<endreq; 

	    TgcRawData* raw = new TgcRawData(bcTagCnv(rotrk.bcBitmap),
					     newrdo->subDetectorId(),
					     newrdo->rodId(),
					     rotrk.ldbId,
					     rotrk.sbId,
					     newrdo->l1Id(),
					     newrdo->bcId(),
					     (rotrk.slbType == 4) ? TgcRawData::SLB_TYPE_INNER_STRIP : (TgcRawData::SlbType)rotrk.slbType,
					     rotrk.delta,
					     rotrk.seg,
					     0,
					     rotrk.rphi
					     );
	    newrdo->push_back(raw);
	    m_itOfBsReadoutTracklet++;
	  }
	// ---------------------
	
	
	// Filling Hipt Data
	std::list<TGC_BYTESTREAM_HIPT>::iterator m_itOfBsHipt
	  = m_listOfBsHipt.begin();
	
	while(m_itOfBsHipt != m_listOfBsHipt.end())
	  {
	    TGC_BYTESTREAM_HIPT hpt = *(m_itOfBsHipt);

	    //printf("raw data %08x \n",hpt);	    
	    log << MSG::DEBUG << std::hex
	      //<<"raw data :: "<<(unsigned long)(*hpt)<<std::endl
		<<"TgcRawData HPT "<<std::endl
		<<" bcTag " <<bcTagCnv(hpt.bcBitmap)
		<<" subDetectorId " <<newrdo->subDetectorId()
		<<" rodId " <<newrdo->rodId()
		<<" l1Id " <<newrdo->l1Id()
		<<" bcId " <<newrdo->bcId()
		<<" strip " <<hpt.strip
		<<" forward " <<hpt.fwd
		<<" sector " <<hpt.sector
		<<" chip " <<hpt.chip
		<<" cand " <<hpt.cand
		<<" hipt " <<hpt.hipt
		<<" hitId " <<hpt.hitId
		<<" sub " <<hpt.sub
		<<" delta " <<hpt.delta
		<<endreq; 

	    
	    TgcRawData* raw = new TgcRawData(bcTagCnv(hpt.bcBitmap),
					     newrdo->subDetectorId(),
					     newrdo->rodId(),
					     newrdo->l1Id(),
					     newrdo->bcId(),
					     hpt.strip,
					     hpt.fwd,
					     hpt.sector,
					     hpt.chip,
					     hpt.cand,
					     hpt.hipt,
					     hpt.hitId,
					     hpt.sub,
					     hpt.delta
					     );
	    newrdo->push_back(raw);	    
	    m_itOfBsHipt++;      
	  }
	// ---------------------
	
	
	// Filling SL Data
	std::list<TGC_BYTESTREAM_SL>::iterator m_itOfBsSL
	  = m_listOfBsSL.begin();
	
	while(m_itOfBsSL != m_listOfBsSL.end())
	  {
	    TGC_BYTESTREAM_SL sl = *(m_itOfBsSL); 

	    //printf("raw data %08x \n",sl);	    
	    log << MSG::DEBUG << std::hex
		<<"TgcRawData SL "
	      //<<"raw data :: "<<(unsigned long)(*sl)
		<<" bcTag " <<bcTagCnv(sl.bcBitmap)
		<<" subDetectorId " <<newrdo->subDetectorId()
		<<" rodId " <<newrdo->rodId()
		<<" l1Id " <<newrdo->l1Id()
		<<" bcId " <<newrdo->bcId()
		<<" cand2pluse " <<sl.cand2plus
		<<" fwd " <<sl.fwd
		<<" sector " <<sl.sector
		<<" cand " <<sl.cand
		<<" sign " <<sl.sign
		<<" thereshold " <<sl.threshold
		<<" overlap " <<sl.overlap
		<<" roi " <<sl.roi
		<<endreq; 
	    
	    TgcRawData* raw = new TgcRawData(bcTagCnv(sl.bcBitmap),
					     newrdo->subDetectorId(),
					     newrdo->rodId(),
					     newrdo->l1Id(),
					     newrdo->bcId(),
					     sl.cand2plus,
					     sl.fwd,
					     sl.sector,
					     sl.cand,
					     sl.sign,
					     sl.threshold,
					     sl.overlap,
                                             false,
					     sl.roi
					     );
	    
	    newrdo->push_back(raw);	    
	    m_itOfBsSL++;            
	  }
	// ---------------------      
	
	
	StatusCode sc = m_rdoContainer->addCollection(newrdo, idHash);
	if(!sc.isSuccess())
		{
		log << MSG::ERROR <<"Failed to add to collection"<<endreq;
		}
	m_itOfTGCCalibData++;
      }      
      
      
      
      
      
    }
    pObj= m_rdoContainer;
    
  }
  return StatusCode::SUCCESS; 
}

