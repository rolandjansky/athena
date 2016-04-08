/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibStreamCnvSvc/MdtPRDContCalibStreamCnv.h"
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
#include "MuonCalibEvent/MdtCalibHit.h"
#include "MdtCalibSvc/MdtCalibrationSvc.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"

#include "StoreGate/StoreGateSvc.h"
#include "CLIDSvc/CLASS_DEF.h"

#include <iostream>
#include <map> 
#include <list> 

using namespace  LVL2_MUON_CALIBRATION;

// function to sort the Mdt hits accordingly to the LVL2 identifier
bool CompareIds(const MdtCalibData data1, const MdtCalibData data2) {
   return data1.id()>data2.id();
}

MdtPRDContCalibStreamCnv::MdtPRDContCalibStreamCnv(ISvcLocator* svcloc) :
  Converter(MuonCalibStream_StorageType, classID(),svcloc),
  m_container(NULL) {
  MsgStream log(messageService(), "MdtPRDContCalibStreamCnv");
  log << MSG::DEBUG<< " construct " <<endreq; 
}

MdtPRDContCalibStreamCnv::~MdtPRDContCalibStreamCnv() {
}

const CLID& MdtPRDContCalibStreamCnv::classID() {
  return ClassID_traits<Muon::MdtPrepDataContainer>::ID();
}

StatusCode MdtPRDContCalibStreamCnv::initialize() {

  //std::cout << "in MdtPRDContCalibStreamCnv::initialize" <<std::endl;
  StatusCode sc = Converter::initialize(); 
  if (StatusCode::SUCCESS!=sc) { 
    return sc; 
  } 

  MsgStream log(messageService(), "MdtPRDContCalibStreamCnv");
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

  // retrieve also the dataProviderSvc
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
  // initialize the offset (should be moved to job options)
  m_mdtOffset=20000;

  sc= service("StoreGateSvc", m_storeGate); 
  
  return sc; 
}

StatusCode MdtPRDContCalibStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) {
  //std::cout << "in MdtPRDContCalibStreamCnv::createObj" <<std::endl;
  MsgStream log(messageService(), "MdtPRDContCalibStreamCnv");

  StatusCode sc;
  //  typedef Muon::MdtPrepDataContainer::ID CollID; 
  
  MuonCalibStreamAddress *pRE_Addr;
  pRE_Addr = dynamic_cast<MuonCalibStreamAddress*>(pAddr); 
  if(!pRE_Addr) {
    log << MSG::ERROR << " Can not cast to MuonCalibStreamAddress " << endreq ; 
    return StatusCode::FAILURE;    
  }
  
  const std::string& nm = *(pRE_Addr->par());

// create MdtPRDContainer if it does not exist
  if( !m_container) {
    m_container = new Muon::MdtPrepDataContainer(m_mdtIdHelper->module_hash_max());
    m_container ->addRef();
    
    //   m_container->setAllCollections(m_mdtIdHelper->module_hash_max());

    // create all address Objects
    //   const MuonDetElemHash& hashF = m_container->hashFunc();
    //   int n = haxshF.max();

    //   log << MSG::DEBUG << "max number of hsh ids: "
    //   << n << endreq;

 // Loop over collection identifiers
    MdtIdHelper::const_id_iterator id_it = m_mdtIdHelper->module_begin();
    for (int i=0; id_it != m_mdtIdHelper->module_end(); id_it++, i++ ) {
            
      log << MSG::DEBUG << "Identifier is " << (*id_it).getString() << endreq;

      // reverse lookup.
      //Muon::MdtPrepDataCollection::ID collID = *id_it;
	//        log << MSG::DEBUG << "Identifier is "
	//	    << collID.getString() << endreq;
        //std::cout<<"Identifier is "
        //    << collID.getString() <<std::endl;

      //Muon::MdtPrepDataContainer::KEY
      // std::string nm= m_container->key(collID);
      
      // create an Address object,
      MuonCalibStreamAddress* addr =new
	MuonCalibStreamAddress(ClassID_traits<Muon::MdtPrepDataCollection>::ID(),"","",i,0);
      // register it in StoreGate.
      sc=m_storeGate->recordAddress(addr, true);
      if(sc==StatusCode::FAILURE) return sc;
    }  //end loop over collection identifiers
  }    //end if !m_container
  
  m_container->cleanup();

  log << MSG::DEBUG<<" Creating Objects   "<<nm<<endreq;
  
  //m_container->setAllCollections();
  sc=fillCollections(pAddr,pObj);
  if(sc==StatusCode::FAILURE) return sc;
  pObj = StoreGateSvc::asStorable( m_container ) ;

  return sc; 
}

StatusCode MdtPRDContCalibStreamCnv::createRep(DataObject* /*pObj*/, IOpaqueAddress*& /*pAddr*/) {
  MsgStream log(messageService(), "MdtPRDContCalibStreamCnv");
  log << MSG::ERROR << " No conversion MdtPrepData to BS " << endreq ; 
  return   StatusCode::FAILURE ; 
}

StatusCode MdtPRDContCalibStreamCnv::fillCollections(IOpaqueAddress* /*pAddr*/, DataObject*& /*pObj*/) {
  MsgStream log(messageService(), "MdtPRDContCalibStreamCnv");
  const CalibEvent* event= m_dataProvider->getEvent(); 

  // Skip events with no MDT hits (otherwise crash below)
  if (!event->mdt() ) {
    log << MSG::WARNING << " NO MDT hits in event!" << endreq;
    return StatusCode::SUCCESS;
  }

 //extract the list of MDT hit tubes
  std::list<MdtCalibData> tubes = (event->mdt())->data();
  // sort the list using CompareIds function
  tubes.sort(CompareIds);
  std::list<MdtCalibData>::const_iterator tube = tubes.begin();
                                                                                
  Muon::MdtPrepDataCollection * mdtCollection=0;
  Identifier previousId, elementId;

 //loop over MDT hit tubes 
 //assumes only leading tdc_counts is present on the data
  for (;tube!=tubes.end();tube++ ) {
    uint16_t coarse = (*tube).leadingCoarse();
    uint16_t fine   = (*tube).leadingFine();
    int tdc_counts   = fine | ( coarse << 5);
    int StationName,StationEta, StationPhi, MultiLayer, TubeLayer, Tube ;           
    WordIntoMdtId((*tube).id(),StationName,
		  StationEta, StationPhi,
		  MultiLayer, TubeLayer,
		  Tube);

 // convert the above info to a MdtPrepData object                                                                                
    Muon::MdtDriftCircleStatus digitStatus=Muon::MdtStatusDriftTime;
    Identifier channelId = 
      m_mdtIdHelper->channelID(StationName,StationEta,StationPhi,
			       MultiLayer,TubeLayer,Tube); 
    //std::cout<<channelId<<" "<<tdc_counts<<" "<<(*tube).width()<<" "
    //                    <<(*tube).phi()<<" "<<
    //StationName<<" "<< StationEta<<" "<< StationPhi
    //           <<" "<< MultiLayer<<" "<< TubeLayer
    //           <<" "<<Tube <<std::endl;


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

    /*bool drift_ok = */m_calibSvc->driftRadiusFromTime(calib_hit, signedTrackLength);
    /*     if (!drift_ok) continue;  */
    radius = calib_hit.driftRadius();
    errRadius = calib_hit.sigmaDriftRadius();

    Amg::Vector2D  driftRadius(radius,0);
    Amg::MatrixX *mat = new Amg::MatrixX(1,1);
    (*mat)(0,0) = (errRadius*errRadius);
    
    IdentifierHash mdtHashId;

// check wether the collection exists already or not, if not add it
    elementId=m_mdtIdHelper->elementID(StationName,StationEta,StationPhi);
    if(elementId != previousId) {
      if(mdtCollection) {

	IdentifierHash mdtHashId;
	m_mdtIdHelper->get_module_hash(mdtCollection->identify(), mdtHashId);	 
	//         Muon::MdtPrepDataContainer::KEY key = m_container->key(previousId);
	if (StatusCode::SUCCESS != m_container->addCollection(mdtCollection, mdtHashId)) {
	  log << MSG::WARNING
	      << "Couldn't record MDT PD Collection with key=" /*<< key*/
	      << " in StoreGate! "<<mdtCollection->size()
	      <<" hits will be lost " << endreq;
	  //return StatusCode::FAILURE;
	}
      }
      previousId=elementId;
      IdContext mdtContext = m_mdtIdHelper->module_context();
      if(m_mdtIdHelper->get_hash(elementId, mdtHashId, &mdtContext)) {
	log << MSG::WARNING << "Unable to get MDT hash id from MDT PD collection "
	    << "context begin_index = " << mdtContext.begin_index()
	    << " context end_index  = " << mdtContext.end_index()
	    << " the identifier is "
	    << endreq;
	elementId.show();
	return StatusCode::FAILURE;
      }
      mdtCollection=new Muon::MdtPrepDataCollection(mdtHashId);
      mdtCollection->setIdentifier(elementId);
    }
    
    Muon::MdtPrepData *newPrepData = new Muon::MdtPrepData(channelId,
							   mdtHashId,
							   driftRadius,
							   mat,
							   descriptor, 
							   tdc_counts, 
							   (*tube).width(), 
							   digitStatus);
     
    // add the MdtPrepData to the collection
    mdtCollection->push_back(newPrepData);
     
  }  //end loop over tubes

  // store last collection
  if(mdtCollection) {
    
    //         Muon::MdtPrepDataContainer::KEY key = m_container->key(elementId);
    
    IdentifierHash mdtHashId;
    m_mdtIdHelper->get_module_hash(mdtCollection->identify(), mdtHashId);
    
    if (StatusCode::SUCCESS != m_container->addCollection(mdtCollection, mdtHashId)) {
      log << MSG::WARNING
	  << "Couldn't record MDT PD Collection with key=" /*<< key*/
	  << " in StoreGate! "<<mdtCollection->size()
	  <<" hits will be lost " << endreq;
      //return StatusCode::FAILURE;
    }
  }
 
  return StatusCode::SUCCESS;
}
