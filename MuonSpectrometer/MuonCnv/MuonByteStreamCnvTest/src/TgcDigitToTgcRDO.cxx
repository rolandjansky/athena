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

#include "EventInfo/TagInfo.h"
#include "EventInfoMgt/ITagInfoMgr.h"

#include "MuonDigitContainer/TgcDigitContainer.h"
#include "MuonDigitContainer/TgcDigitCollection.h"
#include "MuonDigitContainer/TgcDigit.h"

#include "MuonRDO/TgcRdoIdHash.h"

#include "MuonByteStreamCnvTest/TgcDigitToTgcRDO.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

TgcDigitToTgcRDO::TgcDigitToTgcRDO(const std::string& name, ISvcLocator* pSvcLocator)
  : Algorithm(name, pSvcLocator),m_activeStore(0),m_EvtStore("StoreGateSvc", name),
    m_tgc_cabling_server("TGCcablingServerSvc", name),m_rdoContainer(0),m_cabling(0), 
    m_tgcIdHelper(0), m_tagInfoMgr(0), m_log(0), m_debug(false), m_verbose(false)
{
  declareProperty ( "isNewTgcDigit", m_isNewTgcDigit = true );
  declareProperty ( "Store", m_EvtStore, "help" );
  declareProperty ( "RDOContainerName", m_rdoContainerName = "TGCRDO" );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TgcDigitToTgcRDO::initialize()
{
  m_log = new MsgStream(msgSvc(), name());
  m_debug = m_log->level() <= MSG::DEBUG;
  m_verbose = m_log->level() <= MSG::VERBOSE;
  if(m_debug) *m_log << MSG::DEBUG << " in initialize()" << endreq;

  // initialize the StoreGate service
  StatusCode sc = service("ActiveStoreSvc", m_activeStore);
  if (sc != StatusCode::SUCCESS )
    {
      *m_log << (sc.isFailure() ? MSG::FATAL : MSG::ERROR) << " Cannot get ActiveStoreSvc " << endreq;
      return sc ;
    }

  // intitialize transient event store
  if (m_EvtStore.retrieve().isFailure()) {
     *m_log << MSG::FATAL << "Could not retrieve  StoreGate Service !" << endreq;
     return StatusCode::FAILURE;
  }

  // get DetectorStore
  StoreGateSvc* detStore = 0;
  sc = service("DetectorStore", detStore);

  if (sc.isFailure())
    {
      *m_log << MSG::FATAL << "DetectorStore service not found !" << endreq;
      return sc;
    }
  

  // get ID helper
  sc = detStore->retrieve( m_tgcIdHelper, "TGCIDHELPER");
  if (sc.isFailure())
    {
      *m_log << MSG::FATAL << "Could not get TgcIdHelper !" << endreq;
      return sc;
    }
  
  
  // Initialize the TGCcablingServerSvc
  sc = m_tgc_cabling_server.retrieve();
  if (sc.isFailure()) {
      *m_log << MSG::FATAL << "Could not get the TGCcablingServerSvc!" << endreq;
      return sc;
  }
  
  if(m_tgc_cabling_server->isConfigured()) {
    if(m_debug) {
      *m_log << MSG::DEBUG << "standard digitization job: " 
	     << "initialize now the TGC cabling and TGC container." << endreq; 
    }
    sc = getCabling();
    if(sc.isFailure()) {
      *m_log << MSG::FATAL << "Could not get ITGCcablingSvc in initialize()." << endreq;
      return sc;
    }
  } else {
    if(m_debug) {
      *m_log << MSG::DEBUG << "TGCcablingServerSvc not yet configured; postpone the " 
	     << "ITGCcablingSvc and the TgcRdocontainer initialization at first event" 
	     << endreq;
    }
  }

  return StatusCode::SUCCESS;
}



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


StatusCode TgcDigitToTgcRDO::execute()
{
  if(!m_cabling && getCabling().isFailure()) return StatusCode::FAILURE;
  
  if ( m_debug ) *m_log << MSG::DEBUG << "in execute()" << endreq;

  m_rdoContainer->cleanup();
  m_activeStore->setStore( &*m_EvtStore );
  StatusCode sc = m_EvtStore->record(m_rdoContainer, m_rdoContainerName);
  if (sc.isFailure()) *m_log << MSG::WARNING << "Fail to record TGC RDO container in TDS" << endreq;

  sc = fill_TGCdata();
  if (sc.isFailure()) *m_log << MSG::WARNING << "Fail to create TGC RDO" << endreq;

  if ( m_debug ) *m_log << MSG::DEBUG << "done execute()" << endreq;
  return StatusCode::SUCCESS;
}


StatusCode TgcDigitToTgcRDO::finalize() {

  *m_log << MSG::INFO << "in finalize()" << endreq;
  delete m_log;
  if(m_rdoContainer) m_rdoContainer->release();
 
  return StatusCode::SUCCESS;
}


StatusCode TgcDigitToTgcRDO::fill_TGCdata()
{
  if ( m_debug ) *m_log << MSG::DEBUG << "fill_TGCdata" << endreq;

  // init
  m_tgcRdoMap.clear();

  typedef TgcDigitContainer::const_iterator collection_iterator;
  typedef TgcDigitCollection::const_iterator digit_iterator;

  // Retrieve the digit container
  std::string key = "TGC_DIGITS";
  m_activeStore->setStore( &*m_EvtStore );
  const TgcDigitContainer* container;
  StatusCode sc = m_EvtStore->retrieve(container,key);

  if (sc.isFailure())
    {
      *m_log << MSG::FATAL << "Cannot retrieve TGC DIGIT Container" << endreq;
      return StatusCode::FAILURE;
    }

  // loop over collections
  collection_iterator it_coll   = container->begin();
  collection_iterator it_coll_e = container->end();
  for ( ; it_coll != it_coll_e; ++it_coll)
    {
      const TgcDigitCollection* tgcCollection = *it_coll;

      // Iterate on the digits of the collection
      digit_iterator it_dig   = tgcCollection->begin();
      digit_iterator it_dig_e = tgcCollection->end();
      for ( ; it_dig != it_dig_e; ++it_dig)
	{
	  const TgcDigit* tgcDigit = *it_dig;
	  Identifier channelId = tgcDigit->identify();
          uint16_t bctag;
          if (m_isNewTgcDigit) {
            bctag = tgcDigit->bcTag();
          }
          else {
            bctag = 0;
          }

	  if (m_tgcIdHelper->valid(channelId))
	    {
	      // Get the online Id of the channel
	      int subDetectorID;
	      int rodID;
	      int sswID;
	      int slbID;
	      int channelID;

	      // repeat two times for Adjacent Channel
	      for (int iAd=0; iAd<2; ++iAd)
		{
		  bool adFlag = false;

		  // check if this channel has Adjacent partner only when 2nd time
		  if (iAd != 0)
		    {
		      bool a_found = m_cabling->hasAdjacentChannel(channelId);

		      // set Adjacent flag
		      if (a_found)
			adFlag=true;
		      else
			continue;
		    }

		  // get Online ID
		  bool status = m_cabling->getReadoutIDfromOfflineID
		    (channelId,subDetectorID,rodID,sswID,slbID,channelID,adFlag);

		  if (!status)
		    {
		      if ( m_debug ) *m_log << MSG::DEBUG
			                    << "ITGCcablingSvc can't return an online ID for the channel : "
			                    << MSG::dec
			                    << " N_" << m_tgcIdHelper->stationName(channelId)
			                    << " E_" << m_tgcIdHelper->stationEta(channelId)
			                    << " P_" << m_tgcIdHelper->stationPhi(channelId)
			                    << " G_" << m_tgcIdHelper->gasGap(channelId)
			                    << " C_" << m_tgcIdHelper->channel(channelId)
			                    << endreq;
		      continue;
		    }

		  // Create the new Tgc RawData
//           TgcRawData* rawData
//             = new TgcRawData(bctag,subDetectorID,rodID,sswID,slbID,0,0,channelID);
          bool isStrip = m_tgcIdHelper->isStrip(channelId);
          std::string name = m_tgcIdHelper->stationNameString(m_tgcIdHelper->stationName(channelId));
          TgcRawData::SlbType type = TgcRawData::SLB_TYPE_UNKNOWN;
          if (name[1] == '4')
              type = isStrip ? TgcRawData::SLB_TYPE_INNER_STRIP : TgcRawData::SLB_TYPE_INNER_WIRE;
          else if (name[1] == '1')
              type = isStrip ? TgcRawData::SLB_TYPE_TRIPLET_STRIP : TgcRawData::SLB_TYPE_TRIPLET_WIRE;
          else
              type = isStrip ? TgcRawData::SLB_TYPE_DOUBLET_STRIP : TgcRawData::SLB_TYPE_DOUBLET_WIRE;
          TgcRawData* rawData = new TgcRawData(bctag,subDetectorID,rodID,sswID,slbID,0,0,type,adFlag,0,channelID);

		  if ( m_debug ) *m_log << MSG::DEBUG << "Adding a new RawData" << endreq;
		  if ( m_debug ) *m_log << MSG::DEBUG << MSG::hex
		                        << " Sub : " << subDetectorID
		                        << " ROD : " << rodID
		                        << " SSW : " << sswID
		                        << " SLB : " << slbID
		                        << " Ch  : " << channelID
		                        << endreq;

		  // Add the RawData to the RDO
		  TgcRdo * tgcRdo = getTgcRdo(rawData);
		  tgcRdo->push_back(rawData);
		}
	    }
	}
    }

  if ( m_debug ) *m_log << MSG::DEBUG << "Add RDOs to the RdoContainer" << endreq;
  // Add RDOs to the RdoContainer

  TgcRdoIdHash hashF;

  std::map<uint16_t, TgcRdo *>::iterator itM  =m_tgcRdoMap.begin();
  std::map<uint16_t, TgcRdo *>::iterator itM_e=m_tgcRdoMap.end();
  for (; itM != itM_e; ++itM)
    {
      m_activeStore->setStore( &*m_EvtStore );
      unsigned int elementHash = hashF( (itM->second)->identify() );
      //const TgcRdo* rdo = itM->second;
      sc = m_rdoContainer->addCollection(itM->second, elementHash);
      if (sc.isFailure())
	*m_log << MSG::WARNING << "Unable to record TGC RDO in IDC" << endreq;
    }
  if ( m_debug ) *m_log << MSG::DEBUG << "Added RDOs to the RdoContainer XXXXXX" << endreq;

  return StatusCode::SUCCESS;
}


TgcRdo * TgcDigitToTgcRDO::getTgcRdo(const TgcRawData * rawData)
{

  TgcRdoIdHash hashF;

  // Get Online collection ID
  uint16_t onlineColId = TgcRdo::identifyRawData(*rawData);

  std::map<uint16_t, TgcRdo *>::iterator it=m_tgcRdoMap.find(onlineColId);
  if (it != m_tgcRdoMap.end())
    return (*it).second;

  // create new TgcRdo
  IdentifierHash hashId = hashF(onlineColId);

  TgcRdo *rdo = new TgcRdo (onlineColId, hashId);
  m_tgcRdoMap[onlineColId] = rdo;

  // set SubDetectorID and ROD ID
  rdo->setOnlineId (rawData->subDetectorId(), rawData->rodId());

  return rdo;
}

StatusCode TgcDigitToTgcRDO::fillTagInfo() const
{
  if (m_tagInfoMgr==0) return StatusCode::FAILURE;
  
  StatusCode sc = m_tagInfoMgr->addTag("TGC_CablingType",m_cablingType);  

  if(sc.isFailure()) {
    *m_log << MSG::WARNING << "TGC_CablingType " << m_cablingType
	   << " not added to TagInfo " << endreq;
    return sc;
  } else {
    if ( m_debug ) {
      *m_log << MSG::DEBUG << "TGC_CablingType " << m_cablingType 
	     << " is Added TagInfo " << endreq;
    }
  }
  
  return StatusCode::SUCCESS;
}

StatusCode TgcDigitToTgcRDO::getCabling() {

  StatusCode sc = m_tgc_cabling_server->giveCabling(m_cabling);
  if (sc.isFailure()) {
    *m_log << MSG::FATAL << "Could not get ITGCcablingSvc from the Server!" << endreq;
    m_cabling = 0;
    return sc;
  } 
  else {
    if ( m_debug ) *m_log << MSG::DEBUG << " Found the ITGCcablingSvc. " << endreq;
  }

  int maxRodId,maxSswId, maxSbloc,minChannelId, maxChannelId;
  m_cabling->getReadoutIDRanges( maxRodId,maxSswId, maxSbloc,minChannelId, maxChannelId);
  if (maxRodId ==12){
    *m_log << MSG::INFO << m_cabling->name() << " (12-fold) is slected " << endreq ;
    m_cablingType = "TGCcabling12Svc";
  } else {
    *m_log << MSG::INFO << "TGCcablingSvc (8-fold) is selected" << endreq ;
    m_cablingType = "TGCcabling8Svc";
  }

   // get TagInfoMgr
  sc = service("TagInfoMgr", m_tagInfoMgr);
  if ( sc.isFailure() || m_tagInfoMgr==0) {
    *m_log << MSG::WARNING << " Unable to locate TagInfoMgr service" << endreq;
  }

  // Fill Tag Info  
  if (fillTagInfo() != StatusCode::SUCCESS) {
    *m_log << MSG::FATAL << "Unable to fill Tag Info " << endreq;
    return StatusCode::FAILURE;
  } else {
    if(m_debug) *m_log << MSG::DEBUG << "Tag info filled successfully" << endreq;
  }


  // create an empty Rdo container and record it
  m_rdoContainer = new TgcRdoContainer();
  m_rdoContainer->addRef();

  return sc;
}
