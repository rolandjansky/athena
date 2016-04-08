/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibStreamCnvSvc/MdtPRDCollCalibStreamCnv.h"
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

#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonCalibEvent/MdtCalibHit.h"
#include "MdtCalibSvc/MdtCalibrationSvc.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"


//#include "MuonDigitContainer/MdtIdHash.h"

#include "StoreGate/StoreGateSvc.h"
#include "CLIDSvc/CLASS_DEF.h"

#include <iostream>
#include <map> 
#include <list> 

using namespace  LVL2_MUON_CALIBRATION;

MdtPRDCollCalibStreamCnv::MdtPRDCollCalibStreamCnv(ISvcLocator* svcloc) :
    Converter(MuonCalibStream_StorageType, classID(),svcloc) {
  MsgStream log(messageService(), "MdtPRDCollCalibStreamCnv");
  log << MSG::DEBUG<< " construct " <<endreq; 
}

MdtPRDCollCalibStreamCnv::~MdtPRDCollCalibStreamCnv() {
}

const CLID& MdtPRDCollCalibStreamCnv::classID() {
  return ClassID_traits<Muon::MdtPrepDataCollection>::ID();
}

StatusCode MdtPRDCollCalibStreamCnv::initialize() {
//  std::cout << "in MdtPRDCollCalibStreamCnv::initialize" <<std::endl;
  StatusCode sc = Converter::initialize(); 
  if (StatusCode::SUCCESS!=sc) { 
    return sc; 
  } 

  MsgStream log(messageService(), "MdtPRDCollCalibStreamCnv");
  log << MSG::DEBUG<< " initialize " <<endreq; 

  // Retrieve the MdtIdHelper
  // and get the detector descriptor manager
  StoreGateSvc* detStore = 0;
  sc = serviceLocator()->service("DetectorStore", detStore);
  if (sc.isSuccess()) {      
    sc = detStore->retrieve(m_mdtIdHelper, "MDTIDHELPER" );
    if (sc.isFailure()) {
      log << MSG::FATAL << " Cannot retrieve MdtIdHelper " << endreq;
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

  // retrieve the dataProviderSvc
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

  // get MDT Calibration service
  sc = serviceLocator()->service("MdtCalibrationSvc", m_calibSvc);
  if (!sc.isSuccess() || 0 == m_calibSvc) {
    log << MSG::ERROR << " Could not initialize MDT Calibration service Service" << endreq;
    return sc;
  }

  return service("StoreGateSvc", m_storeGate);
}

StatusCode MdtPRDCollCalibStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) {
  //std::cout << "in MdtPRDCollCalibStreamCnv::createObj" <<std::endl;
  MsgStream log(messageService(), "MdtPRDCollCalibStreamCnv");

  MuonCalibStreamAddress *pRE_Addr;
  pRE_Addr = dynamic_cast<MuonCalibStreamAddress*>(pAddr); 
  if(!pRE_Addr) {
    log << MSG::ERROR << " Can not cast to MuonCalibStreamAddress " << endreq ; 
    return StatusCode::FAILURE;    
  }
  
  const std::string& nm = *(pRE_Addr->par());

  log << MSG::DEBUG<<" Creating Objects   "<<nm<<endreq; 

  Muon::MdtPrepDataCollection * coll;
  
  StatusCode sc=fillCollections(coll,pRE_Addr->ipar());
  pObj = StoreGateSvc::asStorable( coll); 
  
  return sc;
}

StatusCode MdtPRDCollCalibStreamCnv::createRep(DataObject* /*pObj*/, IOpaqueAddress*& /*pAddr*/) {
  MsgStream log(messageService(), "MdtPRDCollCalibStreamCnv");
  log << MSG::ERROR << " No conversion MdtPrepData to BS " << endreq ; 
  return   StatusCode::FAILURE ; 
}

StatusCode MdtPRDCollCalibStreamCnv::fillCollections(Muon::MdtPrepDataCollection*& mdtCollection, const unsigned long* ipar){
  MsgStream log(messageService(), "MdtPRDCollCalibStreamCnv");
  
/*
  MdtIdHash hashFunc;
  Muon::MdtPrepDataCollection::ID collId = hashFunc.identifier(ipar[0]);
  mdtCollection = new Muon::MdtPrepDataCollection(); 
  mdtCollection->setIdentifier(collId);
*/

  mdtCollection = new Muon::MdtPrepDataCollection(ipar[0]);  

  const CalibEvent* event= m_dataProvider->getEvent(); 
  
 //extract the list of MDT hit tubes
  std::list<MdtCalibData> tubes = (event->mdt())->data();
  std::list<MdtCalibData>::const_iterator tube = tubes.begin();
                                                                                
 //loop over MDT hit tubes 
 //assumes only leading tdc_counts is present on the data
  while ( tube!=tubes.end() ) {
    uint16_t coarse = (*tube).leadingCoarse();
    uint16_t fine   = (*tube).leadingFine();
    int tdc_counts   = fine | ( coarse << 5);
    int StationName,StationEta, StationPhi, MultiLayer, TubeLayer, Tube;
    WordIntoMdtId((*tube).id(),StationName,
		  StationEta, StationPhi,
		  MultiLayer, TubeLayer,
		  Tube);

 // convert the above info to a MdtPrepData object                                                                                
    Identifier elementId=m_mdtIdHelper->elementID(StationName,StationEta,StationPhi);
    if(elementId==mdtCollection->identify()){
      Muon::MdtDriftCircleStatus digitStatus=Muon::MdtStatusDriftTime;
      Identifier channelId = 
	m_mdtIdHelper->channelID(StationName,StationEta,StationPhi,
				 MultiLayer,TubeLayer,Tube); 
//     std::cout<<channelId<<" "<<tdc_counts<<" "<<(*tube).width()<<" "
//                         <<(*tube).phi()<<" "<<
//     StationName<<" "<< StationEta<<" "<< StationPhi
//                <<" "<< MultiLayer<<" "<< TubeLayer
//                <<" "<<Tube <<std::endl;

      double radius(0.); double errRadius(0.);
      const MuonGM::MdtReadoutElement * descriptor = m_muonMgr->getMdtReadoutElement(channelId);
      Amg::Vector3D position = descriptor->tubePos(channelId);
      double measured_perp = position.perp();
      if (descriptor->getStationS() != 0.) {
         measured_perp = sqrt(measured_perp*measured_perp-
                              descriptor->getStationS()*descriptor->getStationS());
      }
      double measured_x = measured_perp * cos( position.phi() );
      double measured_y = measured_perp * sin( position.phi() );
      const Amg::Vector3D measured_position(measured_x, measured_y, position.z());
      MdtCalibHit calib_hit = MdtCalibHit( channelId, tdc_counts, (*tube).width(), measured_position, descriptor );
      double signedTrackLength = measured_position.mag();

      bool drift_ok = m_calibSvc->driftRadiusFromTime(calib_hit, signedTrackLength);
      if (!drift_ok) return StatusCode::FAILURE;
      radius = calib_hit.driftRadius();
      errRadius = calib_hit.sigmaDriftRadius();

      const Amg::Vector2D driftRadius(radius, 0.);
      Amg::MatrixX mat(1,1); mat.setIdentity();
      mat *= errRadius*errRadius;

      IdentifierHash mdtHashId; // should be set properly, Domizia

      Muon::MdtPrepData *newPrepData = new Muon::MdtPrepData(channelId,
                                                mdtHashId,
                                                driftRadius,
                                                &mat,
                                                descriptor, 
                                                tdc_counts, 
                                                (*tube).width(), 
                                                digitStatus);

 // add the MdtPrepData to the collection
      mdtCollection->push_back(newPrepData);
    } 
    ++tube;
  } 

  return StatusCode::SUCCESS;
}
