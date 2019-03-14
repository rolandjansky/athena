/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"

#include "EventInfoMgt/ITagInfoMgr.h"

#include "TGCcablingInterface/ITGCcablingServerSvc.h"
#include "TGCcablingInterface/ITGCcablingSvc.h"

#include "MuonDigitContainer/TgcDigitCollection.h"
#include "MuonDigitContainer/TgcDigit.h"

#include "MuonRDO/TgcRdoIdHash.h"

#include "MuonByteStreamCnvTest/TgcDigitToTgcRDO.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

TgcDigitToTgcRDO::TgcDigitToTgcRDO(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_tgc_cabling_server("TGCcablingServerSvc", name),m_cabling(0), 
    m_tgcIdHelper(0)
{
  declareProperty ( "isNewTgcDigit", m_isNewTgcDigit = true );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TgcDigitToTgcRDO::initialize()
{
  ATH_MSG_DEBUG( " in initialize()"  );
  ATH_CHECK( detStore()->retrieve( m_tgcIdHelper, "TGCIDHELPER") );
  ATH_CHECK( m_tgc_cabling_server.retrieve() );

  if(m_tgc_cabling_server->isConfigured()) {
    ATH_MSG_DEBUG( "standard digitization job: " 
                   << "initialize now the TGC cabling and TGC container."  );
    ATH_CHECK( getCabling() );
  } else {
    ATH_MSG_DEBUG( "TGCcablingServerSvc not yet configured; postpone the " 
                   << "ITGCcablingSvc and the TgcRdocontainer initialization at first event" );
  }

  ATH_CHECK( m_rdoContainerKey.initialize() );
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_rdoContainerKey );
  ATH_CHECK( m_digitContainerKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_digitContainerKey );

  return StatusCode::SUCCESS;
}



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


StatusCode TgcDigitToTgcRDO::execute()
{
  if(!m_cabling && getCabling().isFailure()) return StatusCode::FAILURE;
  
  ATH_MSG_DEBUG( "in execute()"  );

  if(fill_TGCdata().isFailure()) {
    ATH_MSG_WARNING( "Fail to create TGC RDO" );
  }

  ATH_MSG_DEBUG( "done execute()"  );
  return StatusCode::SUCCESS;
}


StatusCode TgcDigitToTgcRDO::fill_TGCdata()
{
  ATH_MSG_DEBUG( "fill_TGCdata"  );

  SG::WriteHandle<TgcRdoContainer> rdoContainer (m_rdoContainerKey);
  ATH_CHECK(rdoContainer.record((std::make_unique<TgcRdoContainer>())));
  ATH_MSG_DEBUG("Recorded TgcRdoContainer called " << rdoContainer.name() << " in store " << rdoContainer.store());
  SG::ReadHandle<TgcDigitContainer> container (m_digitContainerKey);
  if (!container.isValid()) {
    ATH_MSG_ERROR("Could not find TgcDigitContainer called " << container.name() << " in store " << container.store());
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("Found TgcDigitContainer called " << container.name() << " in store " << container.store());

 // init
  m_tgcRdoMap.clear();

  typedef TgcDigitContainer::const_iterator collection_iterator;
  typedef TgcDigitCollection::const_iterator digit_iterator;

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
		      ATH_MSG_DEBUG( "ITGCcablingSvc can't return an online ID for the channel : "
                                     << MSG::dec
                                     << " N_" << m_tgcIdHelper->stationName(channelId)
                                     << " E_" << m_tgcIdHelper->stationEta(channelId)
                                     << " P_" << m_tgcIdHelper->stationPhi(channelId)
                                     << " G_" << m_tgcIdHelper->gasGap(channelId)
                                     << " C_" << m_tgcIdHelper->channel(channelId) );
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

                  ATH_MSG_DEBUG( "Adding a new RawData"  );
                  ATH_MSG_DEBUG( MSG::hex
                                 << " Sub : " << subDetectorID
                                 << " ROD : " << rodID
                                 << " SSW : " << sswID
                                 << " SLB : " << slbID
                                 << " Ch  : " << channelID );

		  // Add the RawData to the RDO
		  TgcRdo * tgcRdo = getTgcRdo(rawData);
		  tgcRdo->push_back(rawData);
		}
	    }
	}
    }

  ATH_MSG_DEBUG( "Add RDOs to the RdoContainer"  );
  // Add RDOs to the RdoContainer

  TgcRdoIdHash hashF;

  std::map<uint16_t, TgcRdo *>::iterator itM  =m_tgcRdoMap.begin();
  std::map<uint16_t, TgcRdo *>::iterator itM_e=m_tgcRdoMap.end();
  for (; itM != itM_e; ++itM)
    {
      unsigned int elementHash = hashF( (itM->second)->identify() );
      //const TgcRdo* rdo = itM->second;
      StatusCode sc = rdoContainer->addCollection(itM->second, elementHash);
      if (sc.isFailure())
	ATH_MSG_WARNING( "Unable to record TGC RDO in IDC"  );
    }
  ATH_MSG_DEBUG( "Added RDOs to the RdoContainer XXXXXX"  );

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
  ServiceHandle<ITagInfoMgr> tagInfoMgr ("TagInfoMgr", name());
  if (tagInfoMgr.retrieve().isFailure())
    return StatusCode::FAILURE;
  
  StatusCode sc = tagInfoMgr->addTag("TGC_CablingType",m_cablingType);  

  if(sc.isFailure()) {
    ATH_MSG_WARNING( "TGC_CablingType " << m_cablingType
                     << " not added to TagInfo "  );
    return sc;
  } else {
    ATH_MSG_DEBUG( "TGC_CablingType " << m_cablingType 
                   << " is Added TagInfo "  );
  }
  
  return StatusCode::SUCCESS;
}

StatusCode TgcDigitToTgcRDO::getCabling() {

  m_cabling = nullptr;
  ATH_CHECK( m_tgc_cabling_server->giveCabling(m_cabling) );

  int maxRodId,maxSswId, maxSbloc,minChannelId, maxChannelId;
  m_cabling->getReadoutIDRanges( maxRodId,maxSswId, maxSbloc,minChannelId, maxChannelId);
  if (maxRodId ==12){
    ATH_MSG_INFO( m_cabling->name() << " (12-fold) is selected "  );
    m_cablingType = "TGCcabling12Svc";
  } else {
    ATH_MSG_INFO( "TGCcablingSvc (8-fold) is selected"  );
    m_cablingType = "TGCcabling8Svc";
  }

  // Fill Tag Info  
  ATH_CHECK( fillTagInfo() );

  return StatusCode::SUCCESS;
}
