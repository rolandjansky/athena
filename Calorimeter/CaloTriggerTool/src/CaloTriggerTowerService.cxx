/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTriggerTool/CaloTriggerTowerService.h"

#include <stdio.h>
#include <fstream>
#include <iostream>

#include "GaudiKernel/Property.h"
#include "LArIdentifier/LArIdManager.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnlID_Exception.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "LArCabling/LArCablingService.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

//===========================================================
CaloTriggerTowerService::CaloTriggerTowerService( const std::string& type,
						const std::string& name,
						const IInterface* parent )
  : AthAlgTool(type,name,parent),
    m_larcablingSvc(nullptr) ,
    m_onlineHelper(nullptr) ,
    m_emHelper(nullptr) ,
    m_lvl1Helper(nullptr) ,
    m_ttonlineHelper(nullptr)
{

  // Declare additional interface
  declareInterface<CaloTriggerTowerService>(this);

  declareProperty("LArTTCellMapKey",m_TTCellMapKey="LArTTCellMapAtlas");
  declareProperty("CaloTTOnOffIdMapKey",m_caloTTOnOffIdMapKey="CaloTTOnOffIdMapAtlas");
  declareProperty("CaloTTOnAttrIdMapKey",m_caloTTOnAttrIdMapKey="CaloTTOnAttrIdMapAtlas");
  declareProperty("CaloTTPpmRxIdMapKey",m_caloTTPpmRxIdMapKey="CaloTTPpmRxIdMapAtlas");

}


//===========================================================
StatusCode CaloTriggerTowerService::initialize ()
{

  msg().setLevel(msgLevel());

  msg()<<MSG::INFO<<" => CaloTriggerTowerService::initialize() "<< endmsg;


  ServiceHandle<StoreGateSvc> detStore("DetectorStore",name());
  StatusCode status = detStore.retrieve();
  if(status.isSuccess())
  {
    const CaloIdManager*	caloMgr;
    status = detStore->retrieve(caloMgr);
    if (status.isFailure()) {
      msg() << MSG::ERROR << "Unable to retrieve CaloIdManager from DetectorStore" << endmsg;
      return StatusCode::FAILURE;
    } else {
      msg() << MSG::DEBUG << "Successfully retrieved CaloIdManager from DetectorStore" << endmsg;
    }
    m_emHelper = caloMgr->getEM_ID();
    if (!m_emHelper) {
	msg() << MSG::ERROR << "Could not access LArEM_ID helper" << endmsg;
	return StatusCode::FAILURE;
    } else {
	msg() << MSG::DEBUG << "Successfully accessed LArEM_ID helper" << endmsg;
    }
    m_lvl1Helper = caloMgr->getLVL1_ID();
    if (!m_lvl1Helper) {
	msg() << MSG::ERROR << "Could not access CaloLVL1_ID helper" << endmsg;
	return StatusCode::FAILURE;
    } else {
	msg() << MSG::DEBUG << "Successfully accessed CaloLVL1_ID helper" << endmsg;
    }
    m_ttonlineHelper = caloMgr->getTTOnlineID();
    if (!m_ttonlineHelper) {
	msg() << MSG::ERROR << "Could not access TTOnlineID helper" << endmsg;
	return StatusCode::FAILURE;
    } else {
	msg() << MSG::DEBUG << "Successfully accessed CaloLVL1_ID helper" << endmsg;
    }

    const LArIdManager*	larMgr;
    status = detStore->retrieve(larMgr);
    if (status.isFailure()) {
      msg() << MSG::ERROR << "Unable to retrieve LArIdManager from DetectorStore" << endmsg;
      return StatusCode::FAILURE;
    } else {
      msg() << MSG::DEBUG << "Successfully retrieved LArIdManager from DetectorStore" << endmsg;
    }
    m_onlineHelper = larMgr->getOnlineID();
    if (!m_onlineHelper) {
	msg() << MSG::ERROR << "Could not access LArOnlineID helper" << endmsg;
	return StatusCode::FAILURE;
    } else {
	msg() << MSG::DEBUG << "Successfully accessed LArOnlineID helper" << endmsg;
    }


    status= detStore->regFcn(&CaloTriggerTowerService::iovCallBack,this,
			     m_TTCellMap,m_TTCellMapKey);
    if (status.isFailure()) {
      msg() << MSG::ERROR << "Unable to regFcn for "<<m_TTCellMapKey << endmsg;
      return StatusCode::FAILURE;
    }


    status= detStore->regFcn(&CaloTriggerTowerService::iovCallBack,this,
			     m_caloTTOnOffIdMap,m_caloTTOnOffIdMapKey);
    if (status.isFailure()) {
      msg() << MSG::ERROR << "Unable to regFcn for "<<m_caloTTOnOffIdMapKey << endmsg;
      return StatusCode::FAILURE;
    }


    status= detStore->regFcn(&CaloTriggerTowerService::iovCallBack,this,
			     m_caloTTOnAttrIdMap,m_caloTTOnAttrIdMapKey);
    if (status.isFailure()) {
      msg() << MSG::ERROR << "Unable to regFcn for "<< m_caloTTOnAttrIdMapKey << endmsg;
      return StatusCode::FAILURE;
    }

    status= detStore->regFcn(&CaloTriggerTowerService::iovCallBack,this,
			     m_caloTTPpmRxIdMap,m_caloTTPpmRxIdMapKey);
    if (status.isFailure()) {
      msg() << MSG::ERROR << "Unable to regFcn for "<< m_caloTTPpmRxIdMapKey << endmsg;
      return StatusCode::FAILURE;
    }

  }
  else
    {
      msg()<<MSG::ERROR<<" => Failed to get DetectorStore "<< endmsg;
    }

  IToolSvc* toolSvc;
  status   = service( "ToolSvc",toolSvc  );
  if(status.isSuccess()) {
    status = toolSvc->retrieveTool("LArCablingService",m_larcablingSvc);
    if(status.isFailure()) {
      msg() << MSG::ERROR << "Could not retrieve LArCablingService"<< endmsg;
      return(StatusCode::FAILURE);
    }
  } else    {
    msg() << MSG::ERROR << "Could not get ToolSvc"<< endmsg;
    return(StatusCode::FAILURE);
  }

  msg()<<MSG::INFO<<" ====> ...CaloTriggerTowerService::init() OK "<< endmsg;
  return StatusCode::SUCCESS;
}



//===========================================================
StatusCode CaloTriggerTowerService::finalize ()
{
  msg()<<MSG::INFO<<" => CaloTriggerTowerService::finalize() "<< endmsg;
  return StatusCode::SUCCESS;
}

//===========================================================
bool CaloTriggerTowerService::is_initialized () const
{
  return m_TTCellMap.isValid()&&m_caloTTOnOffIdMap.isValid()&&m_caloTTOnAttrIdMap.isValid();
}


//===========================================================
HWIdentifier  CaloTriggerTowerService::createTTChannelID(const Identifier & id, bool bQuiet)
{

	HWIdentifier invalidId (0);

	if(!m_caloTTOnOffIdMap ) {// no mapping object
		msg() << MSG::ERROR << " No CaloTTOnOffIdMap !" << endmsg;
		msg() << MSG::ERROR << " Has the DB folder holding the CaloTTOnOffIdMap been added to IOVDbSvc ? " << endmsg;
		msg() << MSG::ERROR << " IOVDbSvc.Folders+=[ FolderName + DBConnection + \"<tag>\"+TagSpec+\"</tag>\" ] " << endmsg;
		return invalidId;

	} else {

		// have mapping object, forward the call
		HWIdentifier sid = m_caloTTOnOffIdMap->createSignalChannelID( id ) ;
		if(bQuiet) {
			return sid;

		} else {
			if(sid == invalidId) {
				CaloID_Exception except;
				except.code(5) ;
				except.message("HWIdentifier not found in map ") ;
				throw except ;
			}
			return sid;
		}
	}
}


//===========================================================================
Identifier  CaloTriggerTowerService::cnvToIdentifier(const HWIdentifier & id, bool bQuiet)
{
	Identifier invalidId (0);

	if(!m_caloTTOnOffIdMap ) {
		msg() << MSG::ERROR << " No CaloTTOnOffIdMap !" << endmsg;
		msg() << MSG::ERROR << " Has the DB folder holding the CaloTTOnOffIdMap been added to IOVDbSvc ? " << endmsg;
		msg() << MSG::ERROR << " IOVDbSvc.Folders+=[ FolderName + DBConnection + \"<tag>\"+TagSpec+\"</tag>\" ] " << endmsg;
		return invalidId;

	} else {
		// mapping object exist, forward the call
		Identifier offid = m_caloTTOnOffIdMap->cnvToIdentifier(id, bQuiet);
		if(bQuiet) {
			return offid;

		} else {
			static Identifier def = Identifier()  ;
			if(offid==def) {
				CaloID_Exception except;
				except.code(6) ;
				except.message(" Online ID not found in map ") ;
				throw except ;
			}
			return offid;
		}
	}
}

L1CaloCoolChannelId CaloTriggerTowerService::cnvRxIdToCoolChannelId(const L1CaloRxCoolChannelId& rxCoolChannelId) {

   L1CaloCoolChannelId invalidId;

   if(!m_caloTTPpmRxIdMap ) {
     msg() << MSG::ERROR << " No CaloTTPpmRxIdMap !" << endmsg;
     msg() << MSG::ERROR << " Has the DB folder holding the CaloTTPpmRxIdMap been added to IOVDbSvc ? " << endmsg;
     msg() << MSG::ERROR << " IOVDbSvc.Folders+=[ FolderName + DBConnection + \"<tag>\"+TagSpec+\"</tag>\" ] " << endmsg;
     return invalidId;

   } else {
     // mapping object exist, forward the call
     L1CaloCoolChannelId ppmId = m_caloTTPpmRxIdMap->rxToPpmId(rxCoolChannelId);
     if(ppmId==L1CaloCoolChannelId()) {
         CaloID_Exception except;
         except.code(6) ;
         except.message(" Rx channel ID not found in map ") ;
         throw except ;
       }
     return ppmId;
   }
}

std::vector<L1CaloRxCoolChannelId> CaloTriggerTowerService::cnvCoolChannelIdToRxId(const L1CaloCoolChannelId& ppmCoolChannelId) {

   if(!m_caloTTPpmRxIdMap ) {
     msg() << MSG::ERROR << " No CaloTTPpmRxIdMap !" << endmsg;
     msg() << MSG::ERROR << " Has the DB folder holding the CaloTTPpmRxIdMap been added to IOVDbSvc ? " << endmsg;
     msg() << MSG::ERROR << " IOVDbSvc.Folders+=[ FolderName + DBConnection + \"<tag>\"+TagSpec+\"</tag>\" ] " << endmsg;
     return std::vector<L1CaloRxCoolChannelId>();

   } else {
     // mapping object exist, forward the call
     std::vector<L1CaloRxCoolChannelId> rxChannels = m_caloTTPpmRxIdMap->ppmToRxId(ppmCoolChannelId);
     if(rxChannels.size()==0) {
         CaloID_Exception except;
         except.code(6) ;
         except.message(" Ppm channel ID not found in map ") ;
         throw except ;
       }
     return rxChannels;
   }
}

//==========================================================================
unsigned int CaloTriggerTowerService::barrel_endcap_fcal(const HWIdentifier & id) {

    if(!m_caloTTOnAttrIdMap) {
     msg() << MSG::ERROR << " No TTOnAttrIdMap !" << endmsg;
     msg() << MSG::ERROR << " Has the DB folder holding the TTOnAttrIdMap been added to IOVDbSvc ? " << endmsg;
     msg() << MSG::ERROR << " IOVDbSvc.Folders+=[ FolderName + DBConnection + \"<tag>\"+TagSpec+\"</tag>\" ] " << endmsg;
     return (0);
    } else {
        unsigned int barrel_endcap_fcal = m_caloTTOnAttrIdMap->barrel_endcap_fcal(id);
        return barrel_endcap_fcal;
    }
}

//==========================================================================
unsigned int CaloTriggerTowerService::em_had(const HWIdentifier & id) {

    if(!m_caloTTOnAttrIdMap) {
     msg() << MSG::ERROR << " No TTOnAttrIdMap !" << endmsg;
     msg() << MSG::ERROR << " Has the DB folder holding the TTOnAttrIdMap been added to IOVDbSvc ? " << endmsg;
     msg() << MSG::ERROR << " IOVDbSvc.Folders+=[ FolderName + DBConnection + \"<tag>\"+TagSpec+\"</tag>\" ] " << endmsg;
     return (0);
    } else {
        unsigned int em_had = m_caloTTOnAttrIdMap->em_had(id);
        return em_had;
    }
}

//==========================================================================
unsigned int CaloTriggerTowerService::pos_neg(const HWIdentifier & id) {

    if(!m_caloTTOnAttrIdMap) {
     msg() << MSG::ERROR << " No TTOnAttrIdMap !" << endmsg;
     msg() << MSG::ERROR << " Has the DB folder holding the TTOnAttrIdMap been added to IOVDbSvc ? " << endmsg;
     msg() << MSG::ERROR << " IOVDbSvc.Folders+=[ FolderName + DBConnection + \"<tag>\"+TagSpec+\"</tag>\" ] " << endmsg;
     return (0);
    } else {
        unsigned int pos_neg = m_caloTTOnAttrIdMap->pos_neg(id);
        return pos_neg;
    }
}

unsigned int CaloTriggerTowerService::module_type(const HWIdentifier & id) {


    if(!m_caloTTOnAttrIdMap) {
     msg() << MSG::ERROR << " No TTOnAttrIdMap !" << endmsg;
     msg() << MSG::ERROR << " Has the DB folder holding the TTOnAttrIdMap been added to IOVDbSvc ? " << endmsg;
     msg() << MSG::ERROR << " IOVDbSvc.Folders+=[ FolderName + DBConnection + \"<tag>\"+TagSpec+\"</tag>\" ] " << endmsg;
     return (0);
    } else {
        unsigned int module_type = m_caloTTOnAttrIdMap->module_type(id);
        return module_type;
    }
}


//===========================================================
L1CaloCoolChannelId CaloTriggerTowerService::createL1CoolChannelId( const HWIdentifier & id ) {

    if(m_caloTTOnAttrIdMap) {

        unsigned int crate       = m_ttonlineHelper->crate(id);
        unsigned int module      = m_ttonlineHelper->module(id); // this returns the physical slot position !
        unsigned int submodule   = m_ttonlineHelper->submodule(id);
        unsigned int channel     = m_ttonlineHelper->channel(id);

        unsigned int module_type = m_caloTTOnAttrIdMap->module_type(id);

		// since the module argument is the physical position and not the logical one, the logical flag is set to false
        return L1CaloCoolChannelId(crate, L1CaloModuleType( (L1CaloModuleType::L1CaloModuleEnum) module_type), module, submodule, channel, false);

    } else {

        msg() << MSG::ERROR << " No CaloTTOnAttrIdMap !" << endmsg;
        msg() << MSG::ERROR << " Has the DB folder holding the CaloTTOnAttrIdMap been added to IOVDbSvc ? " << endmsg;
        msg() << MSG::ERROR << " IOVDbSvc.Folders+=[ FolderName + DBConnection + \"<tag>\"+TagSpec+\"</tag>\" ] " << endmsg;
        return 0;
    }
}


//===========================================================
HWIdentifier CaloTriggerTowerService::cnvCoolChannelIdToHWID( const L1CaloCoolChannelId& coolChannelId ) {

    unsigned int       crate = coolChannelId.crate();
    unsigned int      module = coolChannelId.module(false); // get the physical module position
    unsigned int   submodule = coolChannelId.subModule();
    unsigned int     channel = coolChannelId.channel();

    // create Athena Online Id - the physical module position is expected here (not the logical)
    HWIdentifier channelId = m_ttonlineHelper->channelId(crate,module,submodule,channel);

    return channelId;
}


//==========================================================================
std::vector<HWIdentifier> CaloTriggerTowerService::createChannelIDvec(const HWIdentifier & id, int extTt)
{
  std::vector<HWIdentifier> channel_id_vec;
  std::vector<HWIdentifier> febs_of_tt;
  std::vector<Identifier> cell_id_vec;

  if(m_emHelper->dictionaryVersion() == "fullAtlas" ||
     m_emHelper->dictionaryVersion() == "H8TestBeam") {

    if(!m_TTCellMap) {
      msg() << MSG::ERROR << " No TTCellMap  !" << endmsg;
      return channel_id_vec;
    }

    Identifier ttId=cnvToIdentifier(id);
    if(extTt) {
      cell_id_vec=createCellIDvecLayer(ttId);
    } else {
      cell_id_vec=createCellIDvecTT(ttId);
    }
    std::vector<Identifier>::const_iterator it  = cell_id_vec.begin();
    std::vector<Identifier>::const_iterator it_e  = cell_id_vec.end();
    for (; it!=it_e; ++it)      {
      HWIdentifier onlId=m_larcablingSvc->createSignalChannelID(*it);
      channel_id_vec.push_back(onlId);
    }

  }

  return channel_id_vec;
}



//==========================================================================
HWIdentifier CaloTriggerTowerService::whichTTChannelID(const HWIdentifier & id)
{
//==========================================================================
//
// Input : channel Online identifier
// Output: trigger tower Online identifier
//
//==========================================================================

  HWIdentifier triggerTower(0);

  if(m_emHelper->dictionaryVersion() == "fullAtlas" ||
     m_emHelper->dictionaryVersion() == "H8TestBeam") {

    if(!m_TTCellMap) {
      msg() << MSG::ERROR << " No TTCellMap  !" << endmsg;
      return triggerTower;
    }

    Identifier cellId=m_larcablingSvc->cnvToIdentifier(id);
    //Identifier ttId=whichTTID(cellId);

    // WhichTTID returns a layer_id which embed the layer information
    // while createTTChannelID expect a tower_id
    Identifier ttLayerId=whichTTID(cellId);
    Identifier ttId = m_lvl1Helper->tower_id(ttLayerId);

    triggerTower=createTTChannelID(ttId);

  }

  return triggerTower;
}


//==========================================================================
std::vector<Identifier>
CaloTriggerTowerService::createCellIDvecTT(const Identifier& id)
//==========================================================================
{
  std::vector<Identifier> vec ;

  if(m_emHelper->dictionaryVersion() == "fullAtlas" ||
     m_emHelper->dictionaryVersion() == "H8TestBeam") {

    if(!m_TTCellMap) {
      msg() << MSG::ERROR << " No TTCellMap  !" << endmsg;
      return vec;
    }

    Identifier ttId=m_lvl1Helper->tower_id(id);
    Identifier regId=m_lvl1Helper->region_id(id);
    int maxLay=m_lvl1Helper->layer_max(regId);

    for(int iLay=0;iLay<=maxLay;++iLay) {
      // Rem: not all iLay correspond to physically existing layers
      Identifier layId = m_lvl1Helper->layer_id(ttId,iLay);
      std::vector<Identifier> vecp = m_TTCellMap->createCellIDvec(layId);
      std::vector<Identifier>::const_iterator it  = vecp.begin();
      std::vector<Identifier>::const_iterator it_e  = vecp.end();
      for (; it!=it_e; ++it)      {
	vec.push_back(*it);
      }
    }

  }

  return vec;
}

//==========================================================================
std::vector<Identifier>
CaloTriggerTowerService::createCellIDvecLayer(const Identifier& id)
//==========================================================================
{
  std::vector<Identifier> vec ;

  if(m_emHelper->dictionaryVersion() == "fullAtlas" ||
     m_emHelper->dictionaryVersion() == "H8TestBeam") {
    if(!m_TTCellMap) {
      msg() << MSG::ERROR << " No TTCellMap  !" << endmsg;
      return vec;
    }
    vec = m_TTCellMap->createCellIDvec(id);
  }

  return vec;
}

//==========================================================================
Identifier CaloTriggerTowerService::whichTTID(const Identifier & id)
//==========================================================================
{
  Identifier sid(0);

  if(m_emHelper->dictionaryVersion() == "fullAtlas" ||
     m_emHelper->dictionaryVersion() == "H8TestBeam") {

    if(!m_TTCellMap) {
      msg() << MSG::ERROR << " No TTCellMap  !" << endmsg;
      return sid;
    }
    sid = m_TTCellMap->whichTTID( id ) ;
    Identifier invalidId (0);
    if(sid == invalidId ){
      LArID_Exception except;
      except.message("Identifier not found in map ") ;
      throw except ;
    }
  }

  return sid;
}


//=============================================================
bool CaloTriggerTowerService::is_in_lvl1(const HWIdentifier & id)
//=============================================================
//
// input = channel online id
// some channels are mapped to a TT although not in lvl1
// 2 cases: barrel end and last compartment of hec.
//
//=============================================================
{

  Identifier cellId=m_larcablingSvc->cnvToIdentifier(id);
  bool lvl1 = is_in_lvl1(cellId);

  return lvl1;

}

//=============================================================
bool CaloTriggerTowerService::is_in_lvl1(const Identifier & id)
//=============================================================
//
// input = cell offline id
// some channels are mapped to a TT although not in lvl1
// 2 cases: barrel end and last compartment of hec.
//
//=============================================================
{

  bool lvl1 = true ;

  if(m_emHelper->dictionaryVersion() == "fullAtlas" ||
	m_emHelper->dictionaryVersion() == "H8TestBeam") {

    if(m_emHelper->is_em_barrel(id)) {
      int samp=m_emHelper->sampling(id);
      if(samp==0) {
	    int eta=m_emHelper->eta(id);
        if(eta==60) {
		  lvl1 = false ;
	    }
	  }
    } else {
      Identifier ttId=whichTTID(id);
      if(m_lvl1Helper->is_hec(ttId)) {
        int layer = m_lvl1Helper->layer(ttId);
        if(layer == 3){
          lvl1 = false ;
        }
      }
	}
  } else {
    lvl1 = false ;
  }

  return lvl1;

}

StatusCode CaloTriggerTowerService::iovCallBack(IOVSVC_CALLBACK_ARGS) {

  msg()<<MSG::INFO<<" ====> iovCallBack " << endmsg;

  return StatusCode::SUCCESS;

}

