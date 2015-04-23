/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"

#include "MuonDigitContainer/MdtDigitContainer.h"
#include "MuonDigitContainer/MdtDigitCollection.h"
#include "MuonDigitContainer/MdtDigit.h"

#include "MuonMDT_Cabling/MuonMDT_CablingSvc.h"

#include "MuonRDO/MdtCsmIdHash.h"
#include "MuonRDO/MdtCsmContainer.h"
#include "MuonRDO/MdtCsm.h"

#include "MuonByteStreamCnvTest/MdtDigitToMdtRDO.h"

#include "EventInfo/TagInfo.h"
#include "EventInfoMgt/ITagInfoMgr.h"

#include <algorithm>
#include <cmath>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

MdtDigitToMdtRDO::MdtDigitToMdtRDO(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_activeStore("ActiveStoreSvc", name), 
  m_cabling("MuonMDT_CablingSvc", name),
  m_csmContainer(0), m_mdtIdHelper(0),
  m_BMEpresent(false)
{
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MdtDigitToMdtRDO::initialize()
{
  ATH_MSG_DEBUG( " in initialize()"  );
  ATH_CHECK( m_activeStore.retrieve() );
  ATH_CHECK( detStore()->retrieve(m_mdtIdHelper,"MDTIDHELPER") );
  ATH_CHECK( m_cabling.retrieve() );

  if ( fillTagInfo().isFailure() ) {
    ATH_MSG_WARNING( "Could not fill the tagInfo for MDT cabling"  );
  }

  // create an empty pad container and record it
  m_csmContainer = new MdtCsmContainer();
  m_csmContainer->addRef();

  // check if the layout includes elevator chambers
  m_BMEpresent = m_mdtIdHelper->stationNameIndex("BME") != -1;
  if ( m_BMEpresent )
    ATH_MSG_INFO( "Processing configuration for layouts with BME chambers."  );
  
  return StatusCode::SUCCESS;
}
 


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 


StatusCode MdtDigitToMdtRDO::execute() {

  ATH_MSG_DEBUG( "in execute()"  );

  m_csmContainer->cleanup();
  std::string key = "MDTCSM";
  m_activeStore->setStore( &*evtStore() );
  StatusCode sc = evtStore()->record(m_csmContainer,key);
  if (sc.isFailure()) ATH_MSG_ERROR( "Fail to record MDT CSM container in TDS"  );

  sc = fill_MDTdata();
  if (sc.isFailure()) ATH_MSG_ERROR( "MdtDigitiToMdtRDO fail to execute"  );

  return StatusCode::SUCCESS;
}


StatusCode MdtDigitToMdtRDO::finalize() {
 
  ATH_MSG_DEBUG( "in finalize()"  );
  if(m_csmContainer) m_csmContainer->release();
  return StatusCode::SUCCESS;
}


StatusCode MdtDigitToMdtRDO::fill_MDTdata() const {

  ATH_MSG_DEBUG( "in execute() : fill_MDTdata"  );

  IdContext mdtContext = m_mdtIdHelper->module_context();

  m_activeStore->setStore( &*evtStore() );

  typedef MdtDigitContainer::const_iterator collection_iterator;
  typedef MdtDigitCollection::const_iterator digit_iterator;

  // Retrieve the digit container
  std::string key = "MDT_DIGITS";
  const MdtDigitContainer* container;
  ATH_CHECK( evtStore()->retrieve(container,key) );

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

      if (!cabling) {
	ATH_MSG_ERROR( "MDTcabling can't return an online ID for the channel : "  );
	ATH_MSG_ERROR( name << " "
                       << eta << " " << phi << " "
                       << "and dummy multilayer=1, layer=1, tube=1 ."  );
        std::abort();
      } 

      Identifier chid1, chid2;
      if ( m_BMEpresent ){
	// 1st ML channel get_id
	chid1 = m_mdtIdHelper->channelID(m_mdtIdHelper->stationName(moduleId),
					 m_mdtIdHelper->stationEta(moduleId),
					 m_mdtIdHelper->stationPhi(moduleId),
					 1, 1, 1 );
	// 2nd ML channel id
	if ( name == 53 ) {
	  chid2 = m_mdtIdHelper->channelID(m_mdtIdHelper->stationName(moduleId),
					   m_mdtIdHelper->stationEta(moduleId),
					   m_mdtIdHelper->stationPhi(moduleId),
					   2, 1, 1 );
        
        }
      }

      unsigned int elementHash = 0, elementHash_2nd = 0;
      MdtCsm* mdtCsm = 0;
      MdtCsm* mdtCsm_2nd = 0;

      // elevator chambers are read out by 2 CSMs
      // they are split in the middle (for both multilayers)
      // the first tube read out by the 2nd CSM is (offline!) tube 43

      if ( !m_BMEpresent ) {
	elementHash = hashF( moduleId );
	mdtCsm = new MdtCsm(moduleId, moduleHash, subsystem, mrod, link);

      } else {

	elementHash = hashF( chid1 ); 
	elementHash_2nd = hashF( chid2 );
	mdtCsm = new MdtCsm(chid1, moduleHash, subsystem, mrod, link);

        if ( name == 53 ) {
	  uint8_t subsystem_2ndcsm, mrod_2ndcsm, link_2ndcsm, tdc_2ndcsm, channel_2ndcsm;

	  cabling = m_cabling->getOnlineId(name, eta, phi, 1, 1, 43,
					   subsystem_2ndcsm, mrod_2ndcsm,
					   link_2ndcsm, tdc_2ndcsm, channel_2ndcsm);

	  if (!cabling) {
	    ATH_MSG_ERROR( "MDTcabling can't return an online ID for the channel : "  );
	    ATH_MSG_ERROR( name << " "
                           << eta << " " << phi << " "
                           << " and dummy multilayer=1, layer=1, tube=1 ."  );
            std::abort();
	  } 

	  mdtCsm_2nd = new MdtCsm(chid2, elementHash_2nd, subsystem_2ndcsm, mrod_2ndcsm, link_2ndcsm);

	}
      }

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
		ATH_MSG_ERROR( "MDTcabling can't return an online ID for the channel : "  );
		ATH_MSG_ERROR( name << " "
                               << eta << " " << phi << " "
                               << multilayer << " " << layer << " " << tube  );
                std::abort();
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
	            
	      ATH_MSG_DEBUG( "Adding a new AmtHit"  );
	      ATH_MSG_DEBUG( "Subdet : " << (int) subsystem 
                             << " mrod : " << (int) mrod 
                             << " link : " << (int) link  );
	      ATH_MSG_DEBUG( " Tdc : " << (int) tdc
                             << " Channel : " << (int) channel 
                             << " Coarse time : " << coarse 
                             << " Fine time : " << fine 
                             << " Width : " << width  );

	      // Add the digit to the CSM
	      if( name != 53 ) mdtCsm->push_back(amtHit);
	      else {
		if( link == mdtCsm->CsmId() ) mdtCsm->push_back(amtHit);
		else if( link == mdtCsm_2nd->CsmId() ) mdtCsm_2nd->push_back(amtHit);
		else {
		  delete amtHit; amtHit = NULL;
		  ATH_MSG_ERROR( "There's a BME digit that doesn't match a CSM"  );
		}
	      }
	            
	    }
	    
	}

      // Add the CSM to the CsmContainer
      m_activeStore->setStore( &*evtStore() );
      StatusCode sc = m_csmContainer->addCollection(mdtCsm, elementHash);
      if (sc.isFailure())
	ATH_MSG_WARNING( "Unable to record MDT CSM in IDC"  );
        //delete mdtCsm;
      if ( name == 53 && m_BMEpresent) {
	sc = m_csmContainer->addCollection(mdtCsm_2nd, elementHash_2nd);
	if (sc.isFailure()) 
	  ATH_MSG_WARNING( "Unable to record MDT CSM in IDC 2nd"  );
      }
    }
  return StatusCode::SUCCESS;

}

StatusCode MdtDigitToMdtRDO::fillTagInfo() const {

  ServiceHandle<ITagInfoMgr> tagInfoMgr ("TagInfoMgr", name());
  if (tagInfoMgr.retrieve().isFailure())
    return StatusCode::FAILURE;
  
  std::string cablingType="";
  if (m_cabling->usingOldCabling() ) {
    cablingType="OldMDT_Cabling";
  }
  else {
    cablingType="NewMDT_Cabling";
  }

  StatusCode sc = tagInfoMgr->addTag("MDT_CablingType",cablingType); 
  
  if(sc.isFailure()) {
    ATH_MSG_WARNING( "MDT_CablingType " << cablingType
                     << " not added to TagInfo "  );
    return sc;
  } else {
    ATH_MSG_DEBUG( "MDT_CablingType " << cablingType
                   << " is Added TagInfo "  );
  }
  
  return StatusCode::SUCCESS;

}
