/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonMDT_Cabling/MdtTestCabling.h"

#include "StoreGate/StoreGateSvc.h"

MdtTestCabling::MdtTestCabling(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_mdtIdHelper(0),
  m_chronoSvc("ChronoStatSvc", name)
{ 
}

MdtTestCabling::~MdtTestCabling() { }


StatusCode MdtTestCabling::initialize()
{
  ATH_CHECK( m_chronoSvc.retrieve() );

  // initialize the pointer to the MdtIdHelper
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", name());
  ATH_CHECK( detStore.retrieve() );
  ATH_CHECK( detStore->retrieve(m_mdtIdHelper, "MDTIDHELPER" ) );

  m_chrono1 = "newcab";
  m_chrono2 = "oldcab";
  m_chrono3 = "OfflineToOnline";

  ATH_CHECK( m_readKey.initialize() );

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

  SG::ReadCondHandle<MuonMDT_CablingMap> readHandle{m_readKey};
  const MuonMDT_CablingMap* readCdo{*readHandle};
  if(readCdo==nullptr){
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return false;
  }

  // print the list of ROBids
  std::vector<uint32_t> robVector = readCdo->getAllROBId();
  ATH_MSG_DEBUG( "============================= List of ROBids:" );
  for (unsigned int i=0 ; i<robVector.size() ; ++i) {
    ATH_MSG_DEBUG( std::hex << robVector[i] << std::dec );
  }
  ATH_MSG_DEBUG( "=============================================" );

  // loop on the hardware objects to get online identifiers

  ATH_MSG_DEBUG( "in testMap()" );
  ATH_MSG_DEBUG( "retrieved the map from the service" );

  std::map<uint8_t, MdtSubdetectorMap*, std::less<uint8_t> >* listOfSubdet;
  std::map<uint8_t, MdtSubdetectorMap*, std::less<uint8_t> >::const_iterator it_sub;

  std::map<uint8_t, MdtRODMap*, std::less<uint8_t> >* listOfROD;
  std::map<uint8_t, MdtRODMap*, std::less<uint8_t> >::const_iterator it_rod;

  std::map<uint8_t, MdtCsmMap*, std::less<uint8_t> >* listOfCsm;
  std::map<uint8_t, MdtCsmMap*, std::less<uint8_t> >::const_iterator it_csm;

  std::map<uint8_t, MdtAmtMap*, std::less<uint8_t> >* listOfAmt;
  std::map<uint8_t, MdtAmtMap*, std::less<uint8_t> >::const_iterator it_amt;

  listOfSubdet = readCdo->getListOfElements();

  //  if (m_debug) {
  ATH_MSG_DEBUG( "Got the list of subdetectors" );
  ATH_MSG_DEBUG( "Number of subdetectors: " << listOfSubdet->size() );
  //}

  // loop on the subdetectors
  for (it_sub=listOfSubdet->begin() ; it_sub !=listOfSubdet->end() ; ++it_sub) {

    int subdetectorId = ((*it_sub).second)->moduleId();
    ATH_MSG_DEBUG( "Now in subdetector: 0x" << MSG::hex 
                   << (int) subdetectorId << MSG::dec);

    // loop on the RODs of this subdetector
    listOfROD = ((*it_sub).second)->getListOfElements();
    for (it_rod=listOfROD->begin() ; it_rod !=listOfROD->end() ; ++it_rod) {

      int rodId = ((*it_rod).second)->moduleId();
      ATH_MSG_DEBUG( "Now in ROD: 0x" << MSG::hex 
                     << (int)  rodId << MSG::dec );

      // loop on the CSMs of this ROD
      listOfCsm = ((*it_rod).second)->getListOfElements();
      for (it_csm=listOfCsm->begin() ; it_csm !=listOfCsm->end() ; ++it_csm) {

	int csmId = ((*it_csm).second)->moduleId();
	ATH_MSG_DEBUG( "Now in csm: 0x" << MSG::hex 
                       << (int) csmId << MSG::dec );
	

	listOfAmt = ((*it_csm).second)->getListOfElements();
	for (it_amt=listOfAmt->begin() ; it_amt !=listOfAmt->end() ; ++it_amt) {

	  int amtId = ((*it_amt).second)->moduleId();
	  ATH_MSG_DEBUG( "Now in amt: 0x" << MSG::hex 
                         << (int) amtId  << MSG::dec );

	  for (int chanId = 0 ; chanId<24 ; ++chanId) {
	    // convert the channel

	    int station=0;
	    int eta=0;
	    int phi=0;
	    int multi=0;
	    int layer=0;
	    int tube=0;

	    m_chronoSvc->chronoStart(m_chrono1);
	    bool cabling = readCdo->getOfflineId(subdetectorId,rodId,csmId,
						      amtId,chanId,
						      station,eta,phi,multi,
						      layer,tube);

	    m_chronoSvc->chronoStop(m_chrono1);

	    if (!cabling) {
	      //ATH_MSG_ERROR( "channel not found !" );
	    }
	    
	    else {
	      
	      ATH_MSG_DEBUG( "Channel converted to: station: " << station
                             << " eta: " << eta << " phi: "<< phi << " multi: " << multi
                             << " layer: " << layer << " tube: " << tube );

	      uint8_t newSubdet = 0;
	      uint8_t newRod = 0;
	      uint8_t newCsm = 0;
	      uint8_t newAmt = 0;
	      uint8_t newChan = 0;

	      // test the back-conversion to online indeces
	      m_chronoSvc->chronoStart(m_chrono3);
	      cabling = readCdo->getOnlineId(station,eta,phi,multi,layer,tube,newSubdet,newRod,newCsm,newAmt,newChan);

	      m_chronoSvc->chronoStop(m_chrono3);

	      if (!cabling) {
		//		ATH_MSG_WARNING( "Could not convert back offline->online" );
	      }
	      else {
		
		if (newSubdet!=subdetectorId || newRod!=rodId || newCsm!=csmId ||
		    newAmt!=amtId || newChan!=chanId) {

		  ATH_MSG_ERROR( "=================================" );

		  ATH_MSG_ERROR( "Back conversion gives a different result !" );

		  ATH_MSG_ERROR( "Original online: subdet 0x" << MSG::hex 
                                 << subdetectorId << MSG::dec 
                                 << "  rodId 0x" << MSG::hex << rodId << MSG::dec
                                 << "  csmId 0x" << MSG::hex << csmId << MSG::dec
                                 << "  amtId 0x" << MSG::hex << amtId << MSG::dec 
                                 << "  chanId 0x" << MSG::hex << chanId << MSG:: dec );

		  ATH_MSG_ERROR( "converted to station: " << station
                                 << " name: " << m_mdtIdHelper->stationNameString(station)
                                 << " eta: " << eta << " phi: " << phi << " multi: " << multi
                                 << " layer: " << layer << " tube: " << tube );

		  ATH_MSG_ERROR( "New online: subdet 0x" << MSG::hex 
                                 << (int) newSubdet << MSG::dec 
                                 << "  rodId 0x" << MSG::hex << (int) newRod << MSG::dec
                                 << "  csmId 0x" << MSG::hex << (int) newCsm << MSG::dec
                                 << "  amtId 0x" << MSG::hex << (int) newAmt << MSG::dec 
                                 << "  chanId 0x" << MSG::hex << (int) newChan << MSG:: dec );
  
		  ATH_MSG_ERROR( "=================================" );

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

  SG::ReadCondHandle<MuonMDT_CablingMap> readHandle{m_readKey};
  const MuonMDT_CablingMap* readCdo{*readHandle};
  if(readCdo==nullptr){
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return false;
  }  

  //
  m_chronoSvc->chronoStart(m_chrono1);
  for (int i = 0 ; i<1000 ; i++) {
    found = readCdo->getOfflineId(1,1,1,1,1,
				       stationName,stationEta,stationPhi,
				       multiLayer,layer,tube);
    if (!found) {
      ATH_MSG_FATAL( " coul dnot find the test channel" );
      return found;
    }

    found = readCdo->getOfflineId(1,1,1,1,3,
				  stationName,stationEta,stationPhi,
				  multiLayer,layer,tube);
    if (!found) {
      ATH_MSG_FATAL( " coul dnot find the test channel" );
      return false;
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

