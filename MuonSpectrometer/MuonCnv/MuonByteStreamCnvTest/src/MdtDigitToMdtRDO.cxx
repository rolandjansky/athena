/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"

#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IToolSvc.h"

#include "MuonDigitContainer/MdtDigitContainer.h"
#include "MuonDigitContainer/MdtDigitCollection.h"
#include "MuonDigitContainer/MdtDigit.h"

#include "MuonRDO/MdtCsmIdHash.h"

#include "MuonByteStreamCnvTest/MdtDigitToMdtRDO.h"

#include "EventInfo/TagInfo.h"
#include "EventInfoMgt/ITagInfoMgr.h"

#include <algorithm>
#include <cmath>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

MdtDigitToMdtRDO::MdtDigitToMdtRDO(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),m_EvtStore("StoreGateSvc", name),m_csmContainer(0)
{

  declareProperty("Store", m_EvtStore, "help");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MdtDigitToMdtRDO::initialize(){

  m_log = new MsgStream(msgSvc(), name());
  *m_log << MSG::DEBUG << " in initialize()" << endreq;
  m_debug = m_log->level() <= MSG::DEBUG;
  m_verbose = m_log->level() <= MSG::VERBOSE;

  StatusCode sc;

  // initialize the StoreGate service
  sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
  if (sc != StatusCode::SUCCESS ) {
    *m_log << MSG::ERROR << " Cannot get ActiveStoreSvc " << endreq;
    return sc ;
  }
  
   // intitialize transient event store
  if (m_EvtStore.retrieve().isFailure()) {
     *m_log << MSG::FATAL << "Could not retrieve  StoreGate Service !" << endreq;
     return StatusCode::FAILURE;
  }
 
  
  StoreGateSvc* detStore = 0;
  sc = serviceLocator()->service("DetectorStore", detStore);
  if (sc.isFailure()) {
    *m_log << MSG::FATAL << "DetectorStore service not found !" << endreq;
    return StatusCode::FAILURE;
  } 
  
  sc = detStore->retrieve(m_mdtIdHelper,"MDTIDHELPER");
  if (sc.isFailure()) {
    *m_log << MSG::FATAL << "Cannot get MdtIdHelper" << endreq;
    return StatusCode::FAILURE;
  }  
  else {
    *m_log << MSG::DEBUG << " Found the MdtIdHelper. " << endreq;
  }

  // initialize MDT cabling service
  sc = serviceLocator()->service("MuonMDT_CablingSvc", m_cabling);
  if (sc != StatusCode::SUCCESS ) 
    {
      *m_log << MSG::ERROR << " Cannot get MDT cabling Service " << endreq;
      return sc ;
    }


  // get TagInfoMgr
  sc = service("TagInfoMgr", m_tagInfoMgr);
  if ( sc.isFailure() || m_tagInfoMgr==0) {
    *m_log << MSG::WARNING << " Unable to locate TagInfoMgr service" << endreq;
  }

  // fill the taginfo information
  sc = fillTagInfo();
  if (sc != StatusCode::SUCCESS) {
    *m_log << MSG::WARNING << "Could not fill the tagInfo for MDT cabling" << endreq;
    
  }

  // create an empty pad container and record it
  m_csmContainer = new MdtCsmContainer();
  m_csmContainer->addRef();
  
  return StatusCode::SUCCESS;
}
 


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 


StatusCode MdtDigitToMdtRDO::execute() {

  if ( m_debug ) *m_log << MSG::DEBUG << "in execute()" << endreq;

  StatusCode sc;

  m_csmContainer->cleanup();
  std::string key = "MDTCSM";
  m_activeStore->setStore( &*m_EvtStore );
  sc = m_EvtStore->record(m_csmContainer,key);
  if (sc.isFailure()) *m_log << MSG::ERROR << "Fail to record MDT CSM container in TDS" << endreq;

  sc = fill_MDTdata();
  if (sc.isFailure()) *m_log << MSG::ERROR << "MdtDigitiToMdtRDO fail to execute" << endreq;

  return StatusCode::SUCCESS;
}


StatusCode MdtDigitToMdtRDO::finalize() {
 
  *m_log << MSG::DEBUG << "in finalize()" << endreq;
  delete m_log;
  if(m_csmContainer) m_csmContainer->release();
 
  return StatusCode::SUCCESS;
}


StatusCode MdtDigitToMdtRDO::fill_MDTdata() const {

  StatusCode sc;
  if ( m_debug ) *m_log << MSG::DEBUG << "in execute() : fill_MDTdata" << endreq;

  IdContext mdtContext = m_mdtIdHelper->module_context();

  m_activeStore->setStore( &*m_EvtStore );

  typedef MdtDigitContainer::const_iterator collection_iterator;
  typedef MdtDigitCollection::const_iterator digit_iterator;

  // Retrieve the digit container
  std::string key = "MDT_DIGITS";
  const MdtDigitContainer* container;
  sc = m_EvtStore->retrieve(container,key);
  if (sc.isFailure()) {
    *m_log << MSG::ERROR << "Cannot retrieve MDT Container" << endreq;
    return StatusCode::FAILURE;
  }

   MdtCsmIdHash hashF;

  // Iterate on the collections
  collection_iterator it_coll = container->begin();
  for ( ; it_coll != container->end(); ++it_coll) 
    {
      const MdtDigitCollection* mdtCollection = *it_coll;
      IdentifierHash moduleHash = mdtCollection->identifierHash();
      Identifier moduleId;
      m_mdtIdHelper->get_id(moduleHash, moduleId, &mdtContext); 
      int name    = m_mdtIdHelper->stationName(moduleId);
      int eta     = m_mdtIdHelper->stationEta(moduleId);
      int phi     = m_mdtIdHelper->stationPhi(moduleId);
 
      // Get the online ID of the MDT module
      uint8_t subsystem;
      uint8_t mrod;
      uint8_t link;
      uint8_t tdc;
      uint8_t channel;
      
      bool cabling = m_cabling->getOnlineId(name, eta, phi, 
					    1, 1, 1,
					    subsystem, mrod, link, 
					    tdc, channel);
      // Create the new CSM
      MdtCsm* mdtCsm = new MdtCsm(moduleId, moduleHash, subsystem, mrod, link);

      // Iterate on the digits of the collection
      digit_iterator it_dig = mdtCollection->begin();
      for ( ; it_dig != mdtCollection->end() ; ++it_dig) 
	{
	  const MdtDigit* mdtDigit = *it_dig;
	  Identifier channelId = mdtDigit->identify();
	    
	  if (m_mdtIdHelper->valid(channelId)) 
	    {
	      int multilayer = m_mdtIdHelper->multilayer(channelId);
	      int layer      = m_mdtIdHelper->tubeLayer(channelId);
	      int tube       = m_mdtIdHelper->tube(channelId);
	            
	      // Get the online Id of the channel
	      cabling = m_cabling->getOnlineId(name, eta, phi, 
					       multilayer, layer, tube,
					       subsystem, mrod, link, 
					       tdc, channel);
	            
	      if (!cabling) {
		*m_log << MSG::ERROR 
		       << "MDTcabling can't return an online ID for the channel : " << endreq;
		*m_log << MSG::ERROR << name << " "
		       << eta << " " << phi << " "
		       << multilayer << " " << layer << " " << tube << endreq;
		assert(false);
	      } 

	      bool masked = mdtDigit->is_masked();
	      // Create the new AMT hit
	      MdtAmtHit* amtHit = new MdtAmtHit(tdc, channel, masked);
	            
	      // Get coarse time and fine time
	      int tdc_counts = mdtDigit->tdc();

	      uint16_t coarse = (tdc_counts>>5) & 0xfff;
	      uint16_t fine = tdc_counts & 0x1f;
	      uint16_t width = mdtDigit->adc();
	            
	      amtHit->setValues(coarse, fine, width);
	            
	      if ( m_debug ) *m_log << MSG::DEBUG << "Adding a new AmtHit" << endreq;
	      if ( m_debug ) *m_log << MSG::DEBUG << "Subdet : " << (int) subsystem 
		                    << " mrod : " << (int) mrod 
		                    << " link : " << (int) link << endreq;
	      if ( m_debug ) *m_log << MSG::DEBUG << " Tdc : " << (int) tdc
		                    << " Channel : " << (int) channel 
		                    << " Coarse time : " << coarse 
		                    << " Fine time : " << fine 
		                    << " Width : " << width << endreq; 

	      // Add the digit to the CSM
	      mdtCsm->push_back(amtHit);
	            
	    }
	    
	}

      // Add the CSM to the CsmContainer
      unsigned int elementHash = hashF( moduleId );
      m_activeStore->setStore( &*m_EvtStore );
      sc = m_csmContainer->addCollection(mdtCsm, elementHash);
      if (sc.isFailure())
	*m_log << MSG::WARNING << "Unable to record MDT CSM in IDC" << endreq;
        //delete mdtCsm;
    }
  return StatusCode::SUCCESS;

}

StatusCode MdtDigitToMdtRDO::fillTagInfo() const {

  if (m_tagInfoMgr==0) return StatusCode::FAILURE;
  
  std::string cablingType="";
  if (m_cabling->usingOldCabling() ) {
    cablingType="OldMDT_Cabling";
  }
  else {
    cablingType="NewMDT_Cabling";
  }

  StatusCode sc = m_tagInfoMgr->addTag("MDT_CablingType",cablingType); 
  
  if(sc.isFailure()) {
    *m_log << MSG::WARNING << "MDT_CablingType " << cablingType
	   << " not added to TagInfo " << endreq;
    return sc;
  } else {
    if ( m_debug ) {
      *m_log << MSG::DEBUG << "MDT_CablingType " << cablingType
	     << " is Added TagInfo " << endreq;
    }
  }
  
  return StatusCode::SUCCESS;

}
