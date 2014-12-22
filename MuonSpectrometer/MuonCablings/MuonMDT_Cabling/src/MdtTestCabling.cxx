/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonMDT_Cabling/MdtTestCabling.h"

#include "MuonCablingData/MuonMDT_CablingMap.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

MdtTestCabling::MdtTestCabling(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name,pSvcLocator),
  m_cablingSvc(0),
  m_mdtIdHelper(0),
  m_chronoSvc(0)
{ 

  //  m_log = new MsgStream(msgSvc(),name());
  m_log = new MsgStream(msgSvc(),"MdtTestCabling");

}

MdtTestCabling::~MdtTestCabling() { }


StatusCode MdtTestCabling::initialize()
{

  // initialize the chrono service
  StatusCode sc = service("ChronoStatSvc",m_chronoSvc);
  if (sc != StatusCode::SUCCESS) {
    *m_log << MSG::ERROR << "Could not find the ChronoSvc" << endreq;
    return sc;
  }

  // initialize the cabling service
  sc = service("MuonMDT_CablingSvc",m_cablingSvc);
  if (sc != StatusCode::SUCCESS) {
    *m_log << MSG::ERROR << "Could not find the MuonMDT_CablingSvc" << endreq;
    return sc;
  }


  //  bool init = initTestMap();
  //if (!init) {
  //  *m_log << MSG::FATAL << "could not initialize the test map " << endreq;
  //  return StatusCode::FAILURE;
  //}

  // initialize the pointer to the MdtIdHelper
  StoreGateSvc* detStore;
  sc = service("DetectorStore", detStore);
  if ( sc.isSuccess() ) {
    *m_log << MSG::DEBUG << "Retrieved DetectorStore" << endreq;
  }else{
    *m_log << MSG::ERROR << "Failed to retrieve DetectorStore" << endreq;
    return sc;
  }

  // retrieve the mdt id helper
  sc = detStore->retrieve(m_mdtIdHelper, "MDTIDHELPER" );
  if (!sc.isSuccess()) {
    *m_log << MSG::ERROR << "Can't retrieve MdtIdHelper" << endreq;
    return sc;
  }
  

  m_chrono1 = "newcab";
  m_chrono2 = "oldcab";
  m_chrono3 = "OfflineToOnline";

  return StatusCode::SUCCESS;
}


StatusCode MdtTestCabling::execute()
{

  /** call the function to run a timing test of the test map 
      (test map must have been initialized)  */
  // testMapTiming();

  bool tested = testMap();

  if (!tested) {

  }

  return StatusCode::SUCCESS;
}


StatusCode MdtTestCabling::finalize()
{
  return StatusCode::SUCCESS;
}

/** test the map content */
bool MdtTestCabling::testMap()
{

  // print the list of ROBids
  std::vector<uint32_t> robVector = m_cablingSvc->getAllROBId();
  std::cout << "============================= List of ROBids:" << std::endl;
  for (unsigned int i=0 ; i<robVector.size() ; ++i) {
    std::cout << std::hex << robVector[i] << std::dec << std::endl;
  }
  std::cout << "=============================================" << std::endl;

  // loop on the hardware objects to get online identifiers
  //  if (m_debug) {
  *m_log << MSG::DEBUG << "in testMap()" << endreq;
  //}
  DataHandle<MuonMDT_CablingMap> cablingMap = m_cablingSvc->getCablingMap();
  //if (m_debug) {
  *m_log << MSG::DEBUG << "retrieved the map from the service" << endreq;
    //}

  std::map<uint8_t, MdtSubdetectorMap*, std::less<uint8_t> >* listOfSubdet;
  std::map<uint8_t, MdtSubdetectorMap*, std::less<uint8_t> >::const_iterator it_sub;

  std::map<uint8_t, MdtRODMap*, std::less<uint8_t> >* listOfROD;
  std::map<uint8_t, MdtRODMap*, std::less<uint8_t> >::const_iterator it_rod;

  std::map<uint8_t, MdtCsmMap*, std::less<uint8_t> >* listOfCsm;
  std::map<uint8_t, MdtCsmMap*, std::less<uint8_t> >::const_iterator it_csm;

  std::map<uint8_t, MdtAmtMap*, std::less<uint8_t> >* listOfAmt;
  std::map<uint8_t, MdtAmtMap*, std::less<uint8_t> >::const_iterator it_amt;

  listOfSubdet = cablingMap->getListOfElements();

  //  if (m_debug) {
  *m_log << MSG::DEBUG << "Got the list of subdetectors" << endreq;
  *m_log << MSG::DEBUG << "Number of subdetectors: " << listOfSubdet->size() << endreq;
  //}

  // loop on the subdetectors
  for (it_sub=listOfSubdet->begin() ; it_sub !=listOfSubdet->end() ; ++it_sub) {

    int subdetectorId = ((*it_sub).second)->moduleId();
    *m_log << MSG::DEBUG << "Now in subdetector: 0x" << MSG::hex 
	   << (int) subdetectorId << MSG::dec << endreq;

    // loop on the RODs of this subdetector
    listOfROD = ((*it_sub).second)->getListOfElements();
    for (it_rod=listOfROD->begin() ; it_rod !=listOfROD->end() ; ++it_rod) {

      int rodId = ((*it_rod).second)->moduleId();
      *m_log << MSG::DEBUG << "Now in ROD: 0x" << MSG::hex 
	     << (int)  rodId << MSG::dec << endreq;

      // loop on the CSMs of this ROD
      listOfCsm = ((*it_rod).second)->getListOfElements();
      for (it_csm=listOfCsm->begin() ; it_csm !=listOfCsm->end() ; ++it_csm) {

	int csmId = ((*it_csm).second)->moduleId();
	*m_log << MSG::DEBUG << "Now in csm: 0x" << MSG::hex 
	       << (int) csmId << MSG::dec << endreq;
	

	listOfAmt = ((*it_csm).second)->getListOfElements();
	for (it_amt=listOfAmt->begin() ; it_amt !=listOfAmt->end() ; ++it_amt) {

	  int amtId = ((*it_amt).second)->moduleId();
	  *m_log << MSG::DEBUG << "Now in amt: 0x" << MSG::hex 
		 << (int) amtId  << MSG::dec << endreq;

	  for (int chanId = 0 ; chanId<24 ; ++chanId) {
	    // convert the channel

	    int station=0;
	    int eta=0;
	    int phi=0;
	    int multi=0;
	    int layer=0;
	    int tube=0;

	    m_chronoSvc->chronoStart(m_chrono1);

	    bool cabling = m_cablingSvc->getOfflineId(subdetectorId,rodId,csmId,
						      amtId,chanId,
						      station,eta,phi,multi,
						      layer,tube);

	    m_chronoSvc->chronoStop(m_chrono1);

	    if (!cabling) {
	      //*m_log << MSG::ERROR << "channel not found !" << endreq;
	    }
	    
	    else {
	      
	      *m_log << MSG::DEBUG << "Channel converted to: station: " << station
		     << " eta: " << eta << " phi: "<< phi << " multi: " << multi
		     << " layer: " << layer << " tube: " << tube << endreq;

	      uint8_t newSubdet = 0;
	      uint8_t newRod = 0;
	      uint8_t newCsm = 0;
	      uint8_t newAmt = 0;
	      uint8_t newChan = 0;

	      // test the back-conversion to online indeces
	      m_chronoSvc->chronoStart(m_chrono3);
	      cabling =  m_cablingSvc->getOnlineId(station,eta,phi,multi,layer,tube,
						   newSubdet,newRod,newCsm,newAmt,newChan);
	      m_chronoSvc->chronoStop(m_chrono3);

	      if (!cabling) {
		//		*m_log << MSG::WARNING << "Could not convert back offline->online" << endreq;
	      }
	      else {
		
		if (newSubdet!=subdetectorId || newRod!=rodId || newCsm!=csmId ||
		    newAmt!=amtId || newChan!=chanId) {

		  *m_log << MSG::ERROR << "=================================" << endreq;

		  *m_log << MSG::ERROR << "Back conversion gives a different result !" 
			 << endreq;

		  *m_log << MSG::ERROR << "Original online: subdet 0x" << MSG::hex 
			 << subdetectorId << MSG::dec 
			 << "  rodId 0x" << MSG::hex << rodId << MSG::dec
			 << "  csmId 0x" << MSG::hex << csmId << MSG::dec
			 << "  amtId 0x" << MSG::hex << amtId << MSG::dec 
			 << "  chanId 0x" << MSG::hex << chanId << MSG:: dec << endreq;

		  *m_log << MSG::ERROR << "converted to station: " << station
			 << " name: " << m_mdtIdHelper->stationNameString(station)
			 << " eta: " << eta << " phi: " << phi << " multi: " << multi
			 << " layer: " << layer << " tube: " << tube << endreq;

		  *m_log << MSG::ERROR << "New online: subdet 0x" << MSG::hex 
			 << (int) newSubdet << MSG::dec 
			 << "  rodId 0x" << MSG::hex << (int) newRod << MSG::dec
			 << "  csmId 0x" << MSG::hex << (int) newCsm << MSG::dec
			 << "  amtId 0x" << MSG::hex << (int) newAmt << MSG::dec 
			 << "  chanId 0x" << MSG::hex << (int) newChan << MSG:: dec << endreq;
  
		  *m_log << MSG::ERROR << "=================================" << endreq;

		}
	      }

	    }


	    

	  } // loop on the channels
	  

	}

      }

    }

  }

  return true;
}

/** Run a timing test of the MDT map */
bool MdtTestCabling::testMapTiming() 
{

  int stationName=0;
  int stationEta=0;
  int stationPhi=0;
  int multiLayer=0;
  int layer=0;
  int tube=0;

  bool found;

  //
  m_chronoSvc->chronoStart(m_chrono1);
  for (int i = 0 ; i<1000 ; i++) {
    found = m_cablingSvc->getOfflineId(1,1,1,1,1,
				       stationName,stationEta,stationPhi,
				       multiLayer,layer,tube);
    if (!found) {
      *m_log<< MSG::FATAL << " coul dnot find the test channel" << endreq;
      return found;
    }

    //found = m_cablingSvc->getOfflineId(1,1,1,1,1,
    //			       stationName,stationEta,stationPhi,
    //			       multiLayer,layer,tube);
    found = m_cablingSvc->getOfflineId(1,1,1,1,3,
				       stationName,stationEta,stationPhi,
				       multiLayer,layer,tube);
    if (!found) {
      *m_log<< MSG::FATAL << " coul dnot find the test channel" << endreq;
      return StatusCode::FAILURE;
    }
  }
  m_chronoSvc->chronoStop(m_chrono1);

  return true;

}

/** Initialize the test map */
bool MdtTestCabling::initTestMap() 
{

  // fill the map with dummy numbers
//  for (int subdet = 0 ; subdet < 4 ; subdet++) {
//    MdtSubdetectorMap* subdetectorMap = new MdtSubdetectorMap(subdet);
//    for (int rod=0 ; rod <50 ; rod++) {
//      MdtRODMap* rodMap = new MdtRODMap(rod);
//      for (int csm=0 ; csm<6 ; csm++) {
//	MdtCsmMap* csmMap = new MdtCsmMap(csm);
//	for (int tdc=0 ; tdc<18 ; tdc++) {
//	  MdtAmtMap* amtMap = new MdtAmtMap(tdc);
//	  for (int channel=0 ; channel<24 ; channel++) {
//	    amtMap->setChannel(channel,3,3);
//	  }
//	  csmMap->setAmtMap(tdc,amtMap);
//	}
//	
//	rodMap->setCsmMap(csm,csmMap);
//      }
//      subdetectorMap->setRODMap(rod,rodMap);
//    }
//    m_cablingSvc->getCablingMap()->setSubdetectorMap(subdet,subdetectorMap);    
//  }


  return true;
}

