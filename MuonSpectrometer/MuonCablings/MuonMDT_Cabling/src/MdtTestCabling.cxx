/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtTestCabling.h"

MdtTestCabling::MdtTestCabling(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_chronoSvc("ChronoStatSvc", name)
{ 
}

StatusCode MdtTestCabling::initialize()
{
  ATH_CHECK( m_chronoSvc.retrieve() );

  ATH_CHECK( m_idHelperSvc.retrieve() );

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

/** test the map content */
bool MdtTestCabling::testMap(){

  SG::ReadCondHandle<MuonMDT_CablingMap> readHandle{m_readKey};
  const MuonMDT_CablingMap* readCdo{*readHandle};
  if(!readCdo){
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

  const MuonMDT_CablingMap::MapOfItems *listOfSubdet;
  MuonMDT_CablingMap::MapOfItems::const_iterator it_sub;

  const MdtSubdetectorMap::MapOfItems *listOfROD;
  MdtSubdetectorMap::MapOfItems::const_iterator it_rod;

  const MdtRODMap::MapOfItems *listOfCsm;
  MdtRODMap::MapOfItems::const_iterator it_csm;

  const MdtCsmMap::MapOfItems *listOfAmt;
  MdtCsmMap::MapOfItems::const_iterator it_amt;

  listOfSubdet = readCdo->getListOfElements();

  ATH_MSG_DEBUG( "Got the list of subdetectors" );
  ATH_MSG_DEBUG( "Number of subdetectors: " << listOfSubdet->size() );
  
  // loop on the subdetectors
  for (it_sub=listOfSubdet->begin() ; it_sub !=listOfSubdet->end() ; ++it_sub) {
    MuonMDT_CablingMap::CablingData cabling_map;
    cabling_map.subdetectorId = ((*it_sub).second)->moduleId();
    ATH_MSG_DEBUG( "Now in subdetector: 0x" << MSG::hex <<  cabling_map.subdetectorId << MSG::dec);

    // loop on the RODs of this subdetector
    listOfROD = ((*it_sub).second)->getListOfElements();
    for (it_rod=listOfROD->begin() ; it_rod !=listOfROD->end() ; ++it_rod) {

      cabling_map.mrod = ((*it_rod).second)->moduleId();
      ATH_MSG_DEBUG( "Now in ROD: 0x" << MSG::hex <<cabling_map.mrod<< MSG::dec );

      // loop on the CSMs of this ROD
      listOfCsm = ((*it_rod).second)->getListOfElements();
      for (it_csm=listOfCsm->begin() ; it_csm !=listOfCsm->end() ; ++it_csm) {

	        cabling_map.csm = ((*it_csm).second)->moduleId();
	        ATH_MSG_DEBUG( "Now in csm: 0x" << MSG::hex 
                          << cabling_map.csm << MSG::dec );
        

        listOfAmt = ((*it_csm).second)->getListOfElements();
	      for (it_amt=listOfAmt->begin() ; it_amt !=listOfAmt->end() ; ++it_amt) {

	        cabling_map.tdcId = ((*it_amt).second)->moduleId();
	        ATH_MSG_DEBUG( "Now in amt: 0x" << MSG::hex 
                                          << cabling_map.tdcId  << MSG::dec );

	         for (cabling_map.channelId = 0 ; cabling_map.channelId<24 ; ++cabling_map.channelId) {
	           m_chronoSvc->chronoStart(m_chrono1);
             bool cabling = readCdo->getOfflineId(cabling_map, msgStream());
             m_chronoSvc->chronoStop(m_chrono1);
    
	            if (!cabling) {
	             ATH_MSG_DEBUG("No cabling was found");
               continue;
	           } 	      
            ATH_MSG_DEBUG( "Channel converted to: "<<cabling_map);
            MuonMDT_CablingMap::CablingData new_map{cabling_map};
            // test the back-conversion to online indeces
            m_chronoSvc->chronoStart(m_chrono3);
            cabling = readCdo->getOnlineId(new_map, msgStream());
            m_chronoSvc->chronoStop(m_chrono3);

	          if (!cabling) {
	        	  ATH_MSG_DEBUG( "Could not convert back offline->online" );
              continue;
	         }
            if (new_map != cabling_map) {

		         ATH_MSG_ERROR( "=================================" );
		         ATH_MSG_ERROR( "Back conversion gives a different result !" );
		         ATH_MSG_ERROR( "Original online: "<<cabling_map );
		         ATH_MSG_ERROR( "converted to station: " <<new_map );
      	  } // loop on the channels
	}

      }

    }

  }
}
  return true;


}
/** Run a timing test of the MDT map */
bool MdtTestCabling::testMapTiming() {

  MuonMDT_CablingMap::CablingData cabling_map;
  cabling_map.subdetectorId=1;
  cabling_map.mrod=1;
  cabling_map.csm=1;
  cabling_map.tdcId=1;
  cabling_map.channelId=1;
  bool found{false};

  SG::ReadCondHandle<MuonMDT_CablingMap> readHandle{m_readKey};
  const MuonMDT_CablingMap* readCdo{*readHandle};
  if(!readCdo){
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return false;
  }  

  //
  m_chronoSvc->chronoStart(m_chrono1);
  for (int i = 0 ; i<1000 ; i++) {
    cabling_map.channelId=1;
    found = readCdo->getOfflineId(cabling_map, msgStream());
    if (!found) {
      ATH_MSG_FATAL( " coul dnot find the test channel" );
      return found;
    }
    cabling_map.channelId=3;

    found = readCdo->getOfflineId(cabling_map, msgStream());
    if (!found) {
      ATH_MSG_FATAL( " coul dnot find the test channel" );
      return found;
    }
  }
  m_chronoSvc->chronoStop(m_chrono1);
  return true;

}

/** Initialize the test map */
bool MdtTestCabling::initTestMap()  {

 
  return true;
}

