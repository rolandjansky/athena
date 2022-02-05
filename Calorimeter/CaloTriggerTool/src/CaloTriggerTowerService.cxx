/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTriggerTool/CaloTriggerTowerService.h"

#include <cstdio>
#include <fstream>
#include <iostream>

#include "Gaudi/Property.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnlID_Exception.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TClass.h"

//===========================================================
CaloTriggerTowerService::CaloTriggerTowerService( const std::string& type,
						const std::string& name,
						const IInterface* parent )
  : AthAlgTool(type,name,parent),
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

  const CaloIdManager*	caloMgr;
  StatusCode status = detStore()->retrieve(caloMgr);
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

  status = detStore()->retrieve(m_onlineHelper, "LArOnlineID");
  if (status.isFailure()) {
    msg() << MSG::ERROR << "Could not access LArOnlineID helper" << endmsg;
    return StatusCode::FAILURE;
  } else {
    msg() << MSG::DEBUG << "Successfully accessed LArOnlineID helper" << endmsg;
  }

  // Make sure the dictionaries for the LArTTCellMap persistent classes
  // are available.  We used to read this object via a conditions callback,
  // but callbacks are not thread-friendly, so this was changed to retrieving
  // it from detStore during event processing.  However, this meant that
  // the TClass's for the persistent objects were also being loaded
  // at that time.  As of root 6.22.00, at least, TClass can sometimes
  // fail to properly load a dictionary when it's being run in a multithreaded
  // context.  So force the needed dictionaries to load now.
  TClass::GetClass ("LArTTCell_P");
  TClass::GetClass ("LArTTCell_P::LArTTCell_P_t");
  TClass::GetClass ("std::vector<LArTTCell_P::LArTTCell_P_t>");

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
  return getTTCellMap() != nullptr && getCaloTTOnOffIdMap() != nullptr && getCaloTTOnAttrIdMap() != nullptr;
}


//===========================================================
HWIdentifier  CaloTriggerTowerService::createTTChannelID(const Identifier & id, bool bQuiet) const
{

	HWIdentifier invalidId (0);

        const CaloTTOnOffIdMap* caloTTOnOffIdMap = getCaloTTOnOffIdMap();
	if(!caloTTOnOffIdMap ) {// no mapping object
		msg() << MSG::ERROR << " No CaloTTOnOffIdMap !" << endmsg;
		msg() << MSG::ERROR << " Has the DB folder holding the CaloTTOnOffIdMap been added to IOVDbSvc ? " << endmsg;
		msg() << MSG::ERROR << " IOVDbSvc.Folders+=[ FolderName + DBConnection + \"<tag>\"+TagSpec+\"</tag>\" ] " << endmsg;
		return invalidId;

	} else {

		// have mapping object, forward the call
                HWIdentifier sid = caloTTOnOffIdMap->createSignalChannelID( id ) ;
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
Identifier  CaloTriggerTowerService::cnvToIdentifier(const HWIdentifier & id, bool bQuiet) const
{
	Identifier invalidId (0);

        const CaloTTOnOffIdMap* caloTTOnOffIdMap = getCaloTTOnOffIdMap();
	if(!caloTTOnOffIdMap ) {
		msg() << MSG::ERROR << " No CaloTTOnOffIdMap !" << endmsg;
		msg() << MSG::ERROR << " Has the DB folder holding the CaloTTOnOffIdMap been added to IOVDbSvc ? " << endmsg;
		msg() << MSG::ERROR << " IOVDbSvc.Folders+=[ FolderName + DBConnection + \"<tag>\"+TagSpec+\"</tag>\" ] " << endmsg;
		return invalidId;

	} else {
		// mapping object exist, forward the call
                Identifier offid = caloTTOnOffIdMap->cnvToIdentifier(id, bQuiet);
		if(bQuiet) {
			return offid;

		} else {
			if(!offid.is_valid()) {
				CaloID_Exception except;
				except.code(6) ;
				except.message(" Online ID not found in map ") ;
				throw except ;
			}
			return offid;
		}
	}
}

L1CaloCoolChannelId CaloTriggerTowerService::cnvRxIdToCoolChannelId(const L1CaloRxCoolChannelId& rxCoolChannelId) const {

   L1CaloCoolChannelId invalidId;

   const CaloTTPpmRxIdMap* caloTTPpmRxIdMap = getCaloTTPpmRxIdMap();
   if(!caloTTPpmRxIdMap ) {
     msg() << MSG::ERROR << " No CaloTTPpmRxIdMap !" << endmsg;
     msg() << MSG::ERROR << " Has the DB folder holding the CaloTTPpmRxIdMap been added to IOVDbSvc ? " << endmsg;
     msg() << MSG::ERROR << " IOVDbSvc.Folders+=[ FolderName + DBConnection + \"<tag>\"+TagSpec+\"</tag>\" ] " << endmsg;
     return invalidId;

   } else {
     // mapping object exist, forward the call
     L1CaloCoolChannelId ppmId = caloTTPpmRxIdMap->rxToPpmId(rxCoolChannelId);
     if(ppmId==L1CaloCoolChannelId()) {
         CaloID_Exception except;
         except.code(6) ;
         except.message(" Rx channel ID not found in map ") ;
         throw except ;
       }
     return ppmId;
   }
}

std::vector<L1CaloRxCoolChannelId> CaloTriggerTowerService::cnvCoolChannelIdToRxId(const L1CaloCoolChannelId& ppmCoolChannelId) const {

   const CaloTTPpmRxIdMap* caloTTPpmRxIdMap = getCaloTTPpmRxIdMap();
   if(!caloTTPpmRxIdMap ) {
     msg() << MSG::ERROR << " No CaloTTPpmRxIdMap !" << endmsg;
     msg() << MSG::ERROR << " Has the DB folder holding the CaloTTPpmRxIdMap been added to IOVDbSvc ? " << endmsg;
     msg() << MSG::ERROR << " IOVDbSvc.Folders+=[ FolderName + DBConnection + \"<tag>\"+TagSpec+\"</tag>\" ] " << endmsg;
     return std::vector<L1CaloRxCoolChannelId>();

   } else {
     // mapping object exist, forward the call
     std::vector<L1CaloRxCoolChannelId> rxChannels = caloTTPpmRxIdMap->ppmToRxId(ppmCoolChannelId);
     if(rxChannels.empty()) {
         CaloID_Exception except;
         except.code(6) ;
         except.message(" Ppm channel ID not found in map ") ;
         throw except ;
       }
     return rxChannels;
   }
}

//==========================================================================
unsigned int CaloTriggerTowerService::barrel_endcap_fcal(const HWIdentifier & id) const {

  const CaloTTOnAttrIdMap* caloTTOnAttrIdMap = getCaloTTOnAttrIdMap();
  if(!caloTTOnAttrIdMap) {
     msg() << MSG::ERROR << " No TTOnAttrIdMap !" << endmsg;
     msg() << MSG::ERROR << " Has the DB folder holding the TTOnAttrIdMap been added to IOVDbSvc ? " << endmsg;
     msg() << MSG::ERROR << " IOVDbSvc.Folders+=[ FolderName + DBConnection + \"<tag>\"+TagSpec+\"</tag>\" ] " << endmsg;
     return (0);
    } else {
        unsigned int barrel_endcap_fcal = caloTTOnAttrIdMap->barrel_endcap_fcal(id);
        return barrel_endcap_fcal;
    }
}

//==========================================================================
unsigned int CaloTriggerTowerService::em_had(const HWIdentifier & id) const {

    const CaloTTOnAttrIdMap* caloTTOnAttrIdMap = getCaloTTOnAttrIdMap();
    if(!caloTTOnAttrIdMap) {
     msg() << MSG::ERROR << " No TTOnAttrIdMap !" << endmsg;
     msg() << MSG::ERROR << " Has the DB folder holding the TTOnAttrIdMap been added to IOVDbSvc ? " << endmsg;
     msg() << MSG::ERROR << " IOVDbSvc.Folders+=[ FolderName + DBConnection + \"<tag>\"+TagSpec+\"</tag>\" ] " << endmsg;
     return (0);
    } else {
        unsigned int em_had = caloTTOnAttrIdMap->em_had(id);
        return em_had;
    }
}

//==========================================================================
unsigned int CaloTriggerTowerService::pos_neg(const HWIdentifier & id) const {

    const CaloTTOnAttrIdMap* caloTTOnAttrIdMap = getCaloTTOnAttrIdMap();
    if(!caloTTOnAttrIdMap) {
     msg() << MSG::ERROR << " No TTOnAttrIdMap !" << endmsg;
     msg() << MSG::ERROR << " Has the DB folder holding the TTOnAttrIdMap been added to IOVDbSvc ? " << endmsg;
     msg() << MSG::ERROR << " IOVDbSvc.Folders+=[ FolderName + DBConnection + \"<tag>\"+TagSpec+\"</tag>\" ] " << endmsg;
     return (0);
    } else {
        unsigned int pos_neg = caloTTOnAttrIdMap->pos_neg(id);
        return pos_neg;
    }
}

unsigned int CaloTriggerTowerService::module_type(const HWIdentifier & id) const {


    const CaloTTOnAttrIdMap* caloTTOnAttrIdMap = getCaloTTOnAttrIdMap();
    if(!caloTTOnAttrIdMap) {
     msg() << MSG::ERROR << " No TTOnAttrIdMap !" << endmsg;
     msg() << MSG::ERROR << " Has the DB folder holding the TTOnAttrIdMap been added to IOVDbSvc ? " << endmsg;
     msg() << MSG::ERROR << " IOVDbSvc.Folders+=[ FolderName + DBConnection + \"<tag>\"+TagSpec+\"</tag>\" ] " << endmsg;
     return (0);
    } else {
        unsigned int module_type = caloTTOnAttrIdMap->module_type(id);
        return module_type;
    }
}


//===========================================================
L1CaloCoolChannelId CaloTriggerTowerService::createL1CoolChannelId( const HWIdentifier & id ) const {

    const CaloTTOnAttrIdMap* caloTTOnAttrIdMap = getCaloTTOnAttrIdMap();
    if(caloTTOnAttrIdMap) {

        unsigned int crate       = m_ttonlineHelper->crate(id);
        unsigned int module      = m_ttonlineHelper->module(id); // this returns the physical slot position !
        unsigned int submodule   = m_ttonlineHelper->submodule(id);
        unsigned int channel     = m_ttonlineHelper->channel(id);

        unsigned int module_type = caloTTOnAttrIdMap->module_type(id);

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
HWIdentifier CaloTriggerTowerService::cnvCoolChannelIdToHWID( const L1CaloCoolChannelId& coolChannelId ) const {

    unsigned int       crate = coolChannelId.crate();
    unsigned int      module = coolChannelId.module(false); // get the physical module position
    unsigned int   submodule = coolChannelId.subModule();
    unsigned int     channel = coolChannelId.channel();

    // create Athena Online Id - the physical module position is expected here (not the logical)
    HWIdentifier channelId = m_ttonlineHelper->channelId(crate,module,submodule,channel);

    return channelId;
}


//==========================================================================
std::vector<Identifier>
CaloTriggerTowerService::createCellIDvecTT(const Identifier& id) const 
//==========================================================================
{
  std::vector<Identifier> vec ;

  if(m_emHelper->dictionaryVersion() == "fullAtlas" ||
     m_emHelper->dictionaryVersion() == "H8TestBeam") {

    const LArTTCellMap* TTCellMap = getTTCellMap();
    if(!TTCellMap) {
      msg() << MSG::ERROR << " No TTCellMap  !" << endmsg;
      return vec;
    }

    Identifier ttId=m_lvl1Helper->tower_id(id);
    Identifier regId=m_lvl1Helper->region_id(id);
    int maxLay=m_lvl1Helper->layer_max(regId);

    for(int iLay=0;iLay<=maxLay;++iLay) {
      // Rem: not all iLay correspond to physically existing layers
      Identifier layId = m_lvl1Helper->layer_id(ttId,iLay);
      std::vector<Identifier> vecp = TTCellMap->createCellIDvec(layId);
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
CaloTriggerTowerService::createCellIDvecLayer(const Identifier& id) const
//==========================================================================
{
  std::vector<Identifier> vec ;
  const LArTTCellMap* TTCellMap = getTTCellMap();

  if(m_emHelper->dictionaryVersion() == "fullAtlas" ||
     m_emHelper->dictionaryVersion() == "H8TestBeam") {
    if(!TTCellMap) {
      msg() << MSG::ERROR << " No TTCellMap  !" << endmsg;
      return vec;
    }
    vec = TTCellMap->createCellIDvec(id);
  }

  return vec;
}

//==========================================================================
Identifier CaloTriggerTowerService::whichTTID(const Identifier & id) const
//==========================================================================
{
  Identifier sid(0);

  if(m_emHelper->dictionaryVersion() == "fullAtlas" ||
     m_emHelper->dictionaryVersion() == "H8TestBeam") {

    const LArTTCellMap* TTCellMap = getTTCellMap();
    if(!TTCellMap) {
      msg() << MSG::ERROR << " No TTCellMap  !" << endmsg;
      return sid;
    }
    sid = TTCellMap->whichTTID( id ) ;
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
bool CaloTriggerTowerService::is_in_lvl1(const Identifier & id) const
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


const LArTTCellMap* CaloTriggerTowerService::getTTCellMap() const
{
  if (!m_TTCellMap.get()) {
    const LArTTCellMap* TTCellMap = nullptr;
    if (detStore()->retrieve (TTCellMap, m_TTCellMapKey).isSuccess()) {
      m_TTCellMap.set (TTCellMap);
    }
  }
  return m_TTCellMap.get();
}


const CaloTTOnOffIdMap* CaloTriggerTowerService::getCaloTTOnOffIdMap() const
{
  if (!m_caloTTOnOffIdMap.get()) {
    const CaloTTOnOffIdMap* caloTTOnOffIdMap = nullptr;
    if (detStore()->retrieve (caloTTOnOffIdMap, m_caloTTOnOffIdMapKey).isSuccess()) {
      m_caloTTOnOffIdMap.set (caloTTOnOffIdMap);
    }
  }
  return m_caloTTOnOffIdMap.get();
}


const CaloTTOnAttrIdMap* CaloTriggerTowerService::getCaloTTOnAttrIdMap() const
{
  if (!m_caloTTOnAttrIdMap.get()) {
    const CaloTTOnAttrIdMap* caloTTOnAttrIdMap = nullptr;
    if (detStore()->retrieve (caloTTOnAttrIdMap, m_caloTTOnAttrIdMapKey).isSuccess()) {
      m_caloTTOnAttrIdMap.set (caloTTOnAttrIdMap);
    }
  }
  return m_caloTTOnAttrIdMap.get();
}


const CaloTTPpmRxIdMap* CaloTriggerTowerService::getCaloTTPpmRxIdMap() const
{
  if (!m_caloTTPpmRxIdMap.get()) {
    const CaloTTPpmRxIdMap* caloTTPpmRxIdMap = nullptr;
    if (detStore()->retrieve (caloTTPpmRxIdMap, m_caloTTPpmRxIdMapKey).isSuccess()) {
      m_caloTTPpmRxIdMap.set (caloTTPpmRxIdMap);
    }
  }
  return m_caloTTPpmRxIdMap.get();
}
