/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibStreamCnvSvc/RpcRDOContCalibStreamCnv.h"
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

#include "MuonPrepRawData/MuonPrepDataCollection.h"

#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonRDO/RpcPadContainer.h"
#include "MuonRDO/RpcPad.h"
#include "MuonRDO/RpcFiredChannel.h"
#include "MuonRDO/RpcCoinMatrix.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "RPCcablingInterface/RpcPadIdHash.h"

#include "RPCcablingInterface/IRPCcablingServerSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "CLIDSvc/CLASS_DEF.h"

#include <iostream>
#include <map> 
#include <list> 

using namespace  LVL2_MUON_CALIBRATION;

RpcRDOContCalibStreamCnv::RpcRDOContCalibStreamCnv(ISvcLocator* svcloc) :
    Converter(MuonCalibStream_StorageType, classID(),svcloc),
    m_padContainer(NULL) {
   MsgStream log(messageService(), "RpcRDOContCalibStreamCnv");
   log << MSG::DEBUG<< " construct " <<endreq; 
}

RpcRDOContCalibStreamCnv::~RpcRDOContCalibStreamCnv() {
}

const CLID& RpcRDOContCalibStreamCnv::classID(){
return ClassID_traits<RpcPadContainer>::ID() ;
}


StatusCode RpcRDOContCalibStreamCnv::initialize() {

   StatusCode sc = Converter::initialize(); 
   if (StatusCode::SUCCESS!=sc) { 
    return sc; 
   } 

   MsgStream log(messageService(), "RpcRDOContCalibStreamCnv");
   log << MSG::DEBUG<< " initialize " <<endreq; 

   // Retrieve the RpcIdHelper
   // and get the detector descriptor manager
   StoreGateSvc* detStore = 0;
   sc = serviceLocator()->service("DetectorStore", detStore);
   if (sc.isSuccess()) {      
     sc = detStore->retrieve(m_rpcIdHelper, "RPCIDHELPER" );
     if (sc.isFailure()) {
       log << MSG::FATAL << " Cannot retrieve RpcIdHelper " << endreq;
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

   // get RPC cablingSvc
   const IRPCcablingServerSvc* RpcCabGet = 0;
   sc = service("RPCcablingServerSvc", RpcCabGet);
   if (sc.isFailure()) {
        log << MSG::WARNING << "Could not get RPCcablingServerSvc !" << endreq;
        return StatusCode::FAILURE;
   }

   sc = RpcCabGet->giveCabling(m_cabling);
   if (sc.isFailure()) {
        log << MSG::WARNING << "Could not get RPCcablingSvc from the Server !" << endreq;
        m_cabling = 0;
        return StatusCode::FAILURE;
   } 
   else {
        log << MSG::DEBUG << " Found the RPCcablingSvc. " << endreq;
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

   // initialize the offset (should be moved to job options)
   m_rpcOffset=24000;

   // create an empty pad container and record it
   m_padContainer = new RpcPadContainer(600);
   m_padContainer->addRef();
 
   return service("StoreGateSvc", m_storeGate); 
   
}

StatusCode
RpcRDOContCalibStreamCnv::createObj(IOpaqueAddress* /*pAddr*/, DataObject*& pObj) 
{
  MsgStream log(messageService(), "RpcRDOContCalibStreamCnv");

  StatusCode sc;

  if(m_padContainer) {
    m_padContainer->cleanup();
  } else {    
    // should a PadContainer be created??
    return StatusCode::FAILURE;
  }
  // register it in StoreGate.
  std::string key = "RPCPAD";
  sc = m_storeGate->record(m_padContainer,key);
  if (sc.isFailure()) log << MSG::ERROR << "Fail to record RPC Pad container in TDS" << endreq;

  sc=fillCollections(pObj); 
  if(sc==StatusCode::FAILURE) return sc; 
  pObj = StoreGateSvc::asStorable( m_padContainer );
  return sc; 
}

 StatusCode 
RpcRDOContCalibStreamCnv::createRep(DataObject* /*pObj*/, IOpaqueAddress*& /*pAddr*/) {
   MsgStream log(messageService(), "RpcRDOContCalibStreamCnv");
   log << MSG::ERROR << " No conversion RpcPrepData to BS " << endreq ; 
   return   StatusCode::FAILURE ; 
}

StatusCode RpcRDOContCalibStreamCnv::fillCollections(DataObject*& pObj) 
 {
  MsgStream log(messageService(), "RpcRDOContCalibStreamCnv");

  const CalibEvent* event= m_dataProvider->getEvent(); 
  RpcPadIdHash hashF;
 
  if(event->rpc()!=NULL){
    //extract the list of RPC pad
    std::list<RpcCalibData> calib_pad = (event->rpc())->data();

    log<<MSG::DEBUG<<"number of PADS= "<<(event->rpc())->size()<<endreq;
    log<<MSG::DEBUG<<"number of matrices= "<<calib_pad.size()<<endreq;

    //std::vector<RpcPad*>* m_rpcpads;
    // Iterate on the readout PADS
    int max_pad=1;
    for (int j=0; j<max_pad; ++j){

      // Identifier elements
      int name = 0;
      int eta = 0;
      int phi = 0;
      int doublet_r = 0;
      int doublet_z = 0;
      int doublet_phi = 0;
      int gas_gap = 0;
      int measures_phi = 0;
      int strip = 0;

      unsigned int sector=(event->rpc())->sectorId();
      unsigned int subsys=(event->rpc())->subsystemId();
      unsigned int pad_id=(event->rpc())->padId();
      unsigned int status=(event->rpc())->status();
      unsigned int errorCode=(event->rpc())->error_code();
      if(subsys==0x65) sector=sector+32;

      log<<MSG::DEBUG<<"pad no= "<<j<<" sector= "<<sector<<" subsys= "<<subsys<<" pad id= "<<pad_id<<" status= "<<status<<" error code= "<<errorCode<<endreq;


      int side = (sector < 32) ? 0:1;
      int logic_sector = sector%32;
      int key = side*10000+logic_sector*100+pad_id;
	
      // Retrieve the identifier elements from the map
      const CablingRPCBase::RDOmap& pad_map = m_cabling->give_RDOs();
      RDOindex index = (*pad_map.find(key)).second;
      index.offline_indexes(name, eta, phi, doublet_r, doublet_z, doublet_phi, gas_gap, 
			    measures_phi, strip);
      
      // Build the pad offline identifier
      Identifier id = m_rpcIdHelper->padID(name,eta,phi,doublet_r,doublet_z,doublet_phi);

      //IdentifierHash padHash;
      //m_rpcIdHelper->get_detectorElement_hash(id, padHash);
      
      RpcPad* newpad = new RpcPad(id,index.hash(),pad_id,status,errorCode,sector);

      //iterate on the matrices
      std::list<RpcCalibData>::const_iterator calib_matrix;
      std::list<RpcCalibData>::const_iterator calib_matrix_begin = calib_pad.begin();
      std::list<RpcCalibData>::const_iterator calib_matrix_end = calib_pad.end();
      for(calib_matrix=calib_matrix_begin; calib_matrix!=calib_matrix_end; ++calib_matrix){

	  unsigned int cmid=calib_matrix->onlineId();
	  unsigned int crc=calib_matrix->crc();
	  unsigned int fel1id=calib_matrix->fel1Id();
	  unsigned int febcid=calib_matrix->febcId();
	  log<<MSG::DEBUG<<"matrix no= "<<cmid<<" crc= "<<crc<<" fel1id= "<<fel1id<<" febcid= "<<febcid<<endreq;
	  RpcCoinMatrix* matrix=new RpcCoinMatrix(id,cmid,crc,fel1id,febcid);
	  //iterate on the fired channels
	  for(short unsigned int i=0; i<calib_matrix->hitNum(); ++i){
	       RpcFiredChannel* rpcfiredchannel=0;

	       uint16_t bcid=0;
	       uint16_t time=0;
	       uint16_t ijk=0;
	       uint16_t channel=0;
	       uint16_t ovl=0;
	       uint16_t thr=0;
	       calib_matrix->giveHit(i,bcid,time,ijk,channel,ovl,thr); 
	       if(ijk<7){
		 rpcfiredchannel=new RpcFiredChannel(bcid,time,ijk,channel);
	       } else if(ijk==7){
		 rpcfiredchannel=new RpcFiredChannel(bcid,time,ijk,thr,ovl);
	       }
	       log<<MSG::DEBUG<<"hit no= "<<i<<" bcid= "<<bcid<<" time= "<<time<<" ijk= "<<ijk<<" channel="<<channel<<" ovl= "<<ovl<<" thr= "<<thr<<endreq;
	       matrix->push_back(rpcfiredchannel);
	       log<<MSG::DEBUG<<"hit added to the matrix "<<endreq;
	  }
	  //add the matrix to the pad
	  newpad->push_back(matrix); 
	  log<<MSG::DEBUG<<"matrix added to the pad "<<endreq;
      }
      // Push back the decoded pad in the vector

      Identifier padId = newpad->identify();

      RpcPadIdHash padHashFunction =
	*(m_cabling->padHashFunction()); 

      IdentifierHash elementHash = padHashFunction(padId);

      //int elementHash = hashF( padId );
      
      //m_rpcIdHelper->get_detectorElement_hash(padId, elementHash);

      StatusCode sc = m_padContainer->addCollection(newpad,elementHash );
      if (sc.isFailure())
	log << MSG::ERROR << "Unable to record RPC Pad "<<padId<<" in IDC" << endreq; 

    //      m_rpcpads->push_back(newpad); 
      log<<MSG::DEBUG<<"pad added to the pad vector "<<endreq;
    }
    pObj= m_padContainer;

  }
  return StatusCode::SUCCESS; 
}

