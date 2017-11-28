/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonMDT_Cabling/MuonMDT_CablingSvc.h"

// include the old cabling service for backwards-compatibility when reading old data
//#include "MDTcabling/IMDTcablingSvc.h"

#include "GaudiKernel/ServiceHandle.h" 
#include "GaudiKernel/IIncidentSvc.h" 

#include "StoreGate/StoreGateSvc.h"
#include "Identifier/IdentifierHash.h"

#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "MuonCablingData/MdtMezzanineType.h"

#include "MuonCondInterface/IMDTCablingDbTool.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "EventInfo/TagInfo.h"
#include "EventInfoMgt/ITagInfoMgr.h"

MuonMDT_CablingSvc::MuonMDT_CablingSvc(const std::string& svcName,ISvcLocator* sl) :
AthService(svcName,sl),
    m_cablingMap(0),
    m_useOldCabling(false),
    m_dbTool("MDTCablingDbTool/MDTCablingDbTool"),
    m_firstAccess(true),
//    m_oldCablingSvc("MDTcablingSvc", svcName),
    m_tagInfoMgr("TagInfoMgr", svcName),
    m_tagsCompared(false),
    m_detStore(0),
    m_forceUse(false)
{
    declareProperty("UseOldCabling", m_useOldCabling);
    declareProperty("DBTool", m_dbTool);
    //    declareProperty("OldCablingSvc", m_oldCablingSvc);
    declareProperty("TagInfoManager", m_tagInfoMgr);
    declareProperty("ForcedUse", m_forceUse);
    declareProperty("doCalStreamInit",m_doCalStreamInit=false);
}

MuonMDT_CablingSvc::~MuonMDT_CablingSvc()
    { }


/** Initialization method */
StatusCode MuonMDT_CablingSvc::initialize()
{

    StatusCode sc = StatusCode::SUCCESS;

    msg(MSG::INFO) << "in initialize() with Configuration: "<<(m_useOldCabling ? "'Old MDT Cabling'" : "'New MDT Cabling'") <<", callback "<< (m_forceUse?"Disabled":"Enabled")<<  endmsg;

    sc = serviceLocator()->service("DetectorStore", m_detStore);
    if ( sc.isFailure()) {
        msg(MSG::WARNING)<< "Could not find det store. Exiting."
            << endmsg;
        return StatusCode::FAILURE;
    }

    msg(MSG::DEBUG)<<"Trying to locate the DB Tool"<<endmsg;
    if (!m_useOldCabling){
      if ( m_dbTool.retrieve().isFailure()) {
        msg(MSG::WARNING)<< "Could not find tool " << m_dbTool << ". Exiting."
            << endmsg;
        return StatusCode::FAILURE;
      } else {
        msg(MSG::DEBUG)<< "Database tool \"" << m_dbTool << "\" retrieved."
            << endmsg;
      }
    } else {
        // for old cabling, don't do this.
        m_firstAccess=false;
    }

    if(m_forceUse) { 
        // Selected cabling is used without comparison 
        m_tagsCompared = true;
        msg(MSG::DEBUG)<< "'ForcedUse' property is set to 'True', so disabling the callback. Configuration fixed to: "<<(m_useOldCabling ? "'Old MDT Cabling'" : "'New MDT Cabling'") << endmsg;     
    } else { 
        // The cabling to be used and the cabling in tag info will be compared by compareTags method
        const DataHandle<TagInfo> tagInfoH;
        std::string tagInfoKey = "";
       // get the key
        if(m_tagInfoMgr.retrieve().isFailure() || m_tagInfoMgr==0) {
            msg(MSG::WARNING) << " Unable to locate TagInfoMgr service" << endmsg; 
        } else {
            tagInfoKey = m_tagInfoMgr->tagInfoKey();
        }
        if(m_detStore->regFcn(&MuonMDT_CablingSvc::compareTags,
            this,
            tagInfoH, 
            tagInfoKey) 
        != StatusCode::SUCCESS) {
            msg(MSG::WARNING)<< "Cannot register compareTags function for key "  << tagInfoKey << endmsg;
        } else {
            msg(MSG::DEBUG)<< "Registered compareTags callback for key: " << tagInfoKey << endmsg;
        }
    }

  /** Get a pointer to the old cabling if needed */
    //if (m_useOldCabling || (!m_forceUse)) {
    //  
    //  if (m_useOldCabling) msg(MSG::DEBUG) << "===> The old cabling service is being used within the new one" << endmsg; 
    //    if (m_oldCablingSvc.retrieve().isFailure()) {
    //        msg(MSG::ERROR) << "Could not get a pointer to the MDTcablingSvc" << endmsg;
    //        return StatusCode::FAILURE;
    //    }
    //}
    //if ( !m_useOldCabling ) 
    //{
    // register the init mapping model for callback against the
    // cabling folders --- only if not using the old   cabling 
    msg(MSG::INFO) << "Registering for callback the initMappingModel function" 
		   << endmsg;
    
    std::string mapFolderName = m_dbTool->mapFolderName();
    msg(MSG::INFO) << "Map folder name: " << m_dbTool->mapFolderName() << endmsg;
    
    const DataHandle<CondAttrListCollection> MapData;
    
    sc = m_detStore->regFcn(&MuonMDT_CablingSvc::initMappingModel,
			    this,
			    MapData, mapFolderName, true);
    
    if (sc.isFailure()) {
      
      msg(MSG::ERROR) 
	<< "Could not register initMapping function for callback against: " << mapFolderName 
	<< endmsg;
      return sc;
    }
    
    // register the mezzanine folder for callback 
    std::string mezzanineFolderName = m_dbTool->mezzanineFolderName();
    msg(MSG::INFO) << "Mezzanine folder name: " 
		   << mezzanineFolderName << endmsg;
    
    const DataHandle<CondAttrListCollection> MapData_mez;
    
    sc = m_detStore->regFcn(&MuonMDT_CablingSvc::initMappingModel,
			    this,MapData_mez, mezzanineFolderName, true);
    
    if (sc.isFailure()) {
      
      msg(MSG::ERROR) 
	<< "Could not register initMapping function for callback against: " << mezzanineFolderName 
	<< endmsg;
      return sc;
    }

    // This is the special initialization needed to run on the calibration stream
    // No use of the callback but initialize the map at service initialization 
    // Activated only when running on the calibration stream
    if ( m_doCalStreamInit ) {
      msg(MSG::INFO) << "Initializing the MDT calibration map to read the calibration stream" << endmsg;
      sc = initMappingModelNoCallback();
      if ( sc.isFailure() ) {
	msg(MSG::ERROR) << "Could not initialize the MDT cabling service for the calibration stream" << endmsg;
	return StatusCode::FAILURE;
      }
    }

    return sc;
}

/** Finalization method */
StatusCode MuonMDT_CablingSvc::finalize()
{
    return StatusCode::SUCCESS;
}



// queryInterface 
StatusCode MuonMDT_CablingSvc::queryInterface(const InterfaceID& riid, void** ppvIF) 
{ 
    if(IID_IMuonMDT_CablingSvc.versionMatch(riid) ) 
    { 
        *ppvIF = (MuonMDT_CablingSvc*)this; 
    } else { 
        return AthService::queryInterface(riid, ppvIF); 
    }
    return StatusCode::SUCCESS;
} 

StatusCode 
    MuonMDT_CablingSvc::compareTags(IOVSVC_CALLBACK_ARGS)
{
    StatusCode sc;
    msg(MSG::DEBUG) << "compareTags() callback triggered. Current configuration is: "<<(m_useOldCabling ? "OldMDT_Cabling" : "NewMDT_Cabling") << endmsg;

    if (m_forceUse) {
        // shouldn't ever be called, as callback shouldn't have been registered.
        m_tagsCompared=true;
        msg(MSG::INFO) <<"Aborting compareTags call back because ForcedUse property is true. Will use current configuration: "<<(m_useOldCabling ? "OldMDT_Cabling" : "NewMDT_Cabling")<<endmsg;
        return StatusCode::SUCCESS;
    }

    // Get TagInfo and retrieve tags
    const TagInfo* tagInfo = 0;
    StoreGateSvc* detStore=0;
    sc = serviceLocator()->service("DetectorStore", detStore);
    sc= detStore->retrieve(tagInfo);

    std::string cablingType;

    if (sc.isFailure() || tagInfo==0) {
        msg(MSG::WARNING)<< "No TagInfo in DetectorStore while attempting to compare tags. Will use current configuration: "<<(m_useOldCabling ? "OldMDT_Cabling" : "NewMDT_Cabling") << endmsg;
        return StatusCode::RECOVERABLE;
    } else {
        tagInfo->findInputTag("MDT_CablingType", cablingType);

        msg(MSG::DEBUG)<< "MDT_CablingType from TagInfo: " << cablingType << endmsg;

	// either have empty cabling tag and set to old cabling, or new and don't use old cabling
        bool tagMatch = (cablingType=="" && m_useOldCabling) || (cablingType == "NewMDT_Cabling"&& !m_useOldCabling);  

        if (!tagMatch) {
          // Trying to use wrong cabling map! Flip configuration
     
         if (m_useOldCabling) {
            m_useOldCabling = false;
            m_firstAccess=true;
            //retrieve tool now to avoid unpleasent surprises later!
            if ( m_dbTool.retrieve().isFailure()) {
               msg(MSG::WARNING)<< "Could not find tool " << m_dbTool << ". Exiting."<< endmsg;
               return StatusCode::FAILURE;
            }
          } else {
            m_useOldCabling = true;
            m_firstAccess=false;
          }

          msg(MSG::INFO)<< "MDT_CablingType : " << cablingType << " is mismatched "
            << "with configuration of this service ."
            << "m_useOldCabling flag is flipped to " << (m_useOldCabling ? "true" : "false") << " ." 
            << endmsg; 
        }
    }

    msg(MSG::INFO)<< "compareTags() callback determined that the taginfo for MDT cabling is: "<< cablingType << " (blank = old). Current configuration is: "
                  <<(m_useOldCabling ? "OldMDT_Cabling" : "NewMDT_Cabling") << endmsg;
    m_tagsCompared=true;
    return StatusCode::SUCCESS;
}


//// to be fixed: get the cabling map directly from the tool
DataHandle<MuonMDT_CablingMap> MuonMDT_CablingSvc::getCablingMap() 
{
  
  return m_cablingMap;
}


uint32_t MuonMDT_CablingSvc::getROBId(int stationName, int stationEta, int stationPhi) 
{

    int rodId = m_cablingMap->getROBId(stationName,stationEta,stationPhi);

    return rodId;
}


uint32_t MuonMDT_CablingSvc::getROBId(const IdentifierHash mdtIdHash) 
{
    int rodId = m_cablingMap->getROBId(mdtIdHash);
    return rodId;
}


std::vector<uint32_t> MuonMDT_CablingSvc::getROBId(const std::vector<IdentifierHash>& mdtHashVector)
{
    std::vector<uint32_t> robVector;

    for ( unsigned int i = 0 ; i<mdtHashVector.size() ; ++i ) {

        int robId = this->getROBId(mdtHashVector[i]);
        if (robId==0) {

            msg(MSG::ERROR) << "ROB id not found for Hash Id: " << mdtHashVector[i] << endmsg;

        } else {
            ATH_MSG_DEBUG("Found ROB id 0x" << MSG::hex << robId << MSG::dec << " for hashId " << mdtHashVector[i]);
        }
        robVector.push_back(robId);
    }
    ATH_MSG_DEBUG("Size of ROB vector is: " << robVector.size());
    return robVector;
}

//
const std::vector<IdentifierHash>  MuonMDT_CablingSvc::getChamberHashVec(const std::vector< uint32_t> &ROBId_list)
{
  return m_cablingMap->getChamberHashVec(ROBId_list);
}

const std::vector<IdentifierHash>&  MuonMDT_CablingSvc::getChamberHashVec(const uint32_t ROBId)
{
  return m_cablingMap->getChamberHashVec(ROBId);
}

/** This function returns the full list of ROBids */
std::vector<uint32_t> MuonMDT_CablingSvc::getAllROBId() 
{
    std::vector<uint32_t> allROBId;

    //    if (m_useOldCabling) {
    //	std::set< uint32_t > setOfRobIds =  m_oldCablingSvc->getROBid(0.,2.*3.141592,-3.,3.);
    //	std::vector< uint32_t > robIds(setOfRobIds.size());
    //	std::copy(setOfRobIds.begin(), setOfRobIds.end(), robIds.begin());
    //	return robIds;
    //    }
    
    allROBId = m_cablingMap->getAllROBId();
    
    return allROBId;
}


/** convert an online id to an offline one */
bool MuonMDT_CablingSvc::getOfflineId(uint8_t subdetectorId,
    uint8_t rodId,
    uint8_t csmId,
    uint8_t tdcId,
    uint8_t channelId,
    int& stationName, int& stationEta, int& stationPhi,
    int& multiLayer, int& layer, int& tube)
{


  //    if (m_useOldCabling) {
  //        bool found_old = m_oldCablingSvc->getOfflineIDfromOnlineID(subdetectorId,
  //            rodId,
  //            csmId,
  //            tdcId,
  //            channelId,
  //            stationName, stationEta, stationPhi,
  //            multiLayer, layer, tube);
  //
  //        if (!found_old) {
  //            msg(MSG::ERROR) << "Can't find subdetector: 0x" << MSG::hex << (int) subdetectorId
  //                << MSG::dec   << " rod: 0x" << MSG::hex << (int) rodId
  //                << MSG::dec   << " rod: 0x" << MSG::hex << (int) csmId
  //                << MSG::dec   << " rod: 0x" << MSG::hex << (int) tdcId
  //                << MSG::dec   << " rod: 0x" << MSG::hex << (int) channelId;
  //        }
  //        return found_old;
  //    }

    bool found = m_cablingMap->getOfflineId(subdetectorId,rodId,csmId,
        tdcId,channelId,
        stationName,stationEta,stationPhi,
        multiLayer,layer,tube);

    return found;
}

/* get the online Id given the offline id */
bool MuonMDT_CablingSvc::getOnlineId(int stationName, int stationEta, int stationPhi,
    int multiLayer, int layer, int tube,
    uint8_t& subdetectorId, uint8_t& rodId, 
    uint8_t& csmId,
    uint8_t& tdcId, uint8_t& channelId) 
{

    
//    if (m_useOldCabling) {
//        short unsigned int subdet=0;
//        short unsigned int rod=0;
//        short unsigned int csm=0;
//        short unsigned int tdc=0;
//        short unsigned int channel=0;
//
//        bool found_old = m_oldCablingSvc->getOnlineIDfromOfflineID(stationName, stationEta, stationPhi,
//            multiLayer,layer,tube,
//            subdet,rod,csm,tdc,channel);
//
//        if (!found_old) {
//            msg(MSG::ERROR)<< "Can't find station: " << stationName
//                << " eta: " << stationEta
//                << " phi: " << stationPhi
//                << " multilayer: " << multiLayer 
//                << " layer: " << layer
//                << " tube: " << tube << endmsg;
//        }
//        else {
//            subdetectorId = (uint8_t) subdet;
//            rodId = (uint8_t) rod;
//            csmId = (uint8_t) csm;
//            tdcId = (uint8_t) tdc;
//            channelId = (uint8_t) channel;
//        }
//        return found_old;
//    }

    bool found = m_cablingMap->getOnlineId(stationName, stationEta, stationPhi,
        multiLayer, layer, tube,
        subdetectorId, rodId, 
        csmId, tdcId, channelId);

    return found;
}

/** callback registered function to initialize the map object */
StatusCode MuonMDT_CablingSvc::initMappingModel(IOVSVC_CALLBACK_ARGS_P(I,keys))
{

  msg(MSG::INFO) << "initMappingModel has been called" << endmsg;
  msg(MSG::INFO) << "ToolHandle in initMappingModel - <" << m_dbTool << ">" << endmsg;
  
  if(!m_useOldCabling) {
    
    StatusCode sc = m_dbTool->loadParameters(I, keys);
    if (sc.isFailure()) {
      msg(MSG::ERROR)<<"Reading Cabling maps from COOL failed; try to read from file"<<endmsg;
      return StatusCode::FAILURE;
    }
  
    // retrieve the map from the detector store
    
    sc = m_detStore->retrieve(m_cablingMap);
    if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "Can't retrieve the cabling map from the detector store" << endmsg;
      return StatusCode::FAILURE;
    }
    else {
      msg(MSG::DEBUG) << "Retrieved map from the detector store" << endmsg;
    }
  
  }
  else {
    msg(MSG::INFO) << "Using the old cabling, no callback" << endmsg;
  }

  return StatusCode::SUCCESS;
}



// initalize the map without using the callback
StatusCode MuonMDT_CablingSvc::initMappingModelNoCallback()
{

  msg(MSG::INFO) << "initMappingModel has been called" << endmsg;
  msg(MSG::INFO) << "ToolHandle in initMappingModel - <" << m_dbTool << ">" << endmsg;
  
  if(!m_useOldCabling) {
    
    std::list<std::string> keys;
    keys.push_back("/MDT/CABLING/MAP_SCHEMA");
    keys.push_back("/MDT/CABLING/MEZZANINE_SCHEMA");
    
    int I=0;

    StatusCode sc = m_dbTool->loadParameters(I, keys);
    if (sc.isFailure()) {
      msg(MSG::ERROR)<<"Reading Cabling maps from COOL failed; try to read from file"<<endmsg;
      return StatusCode::FAILURE;
    }
  
    // retrieve the map from the detector store
    
    sc = m_detStore->retrieve(m_cablingMap);
    if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "Can't retrieve the cabling map from the detector store" << endmsg;
      return StatusCode::FAILURE;
    }
    else {
      msg(MSG::DEBUG) << "Retrieved map from the detector store" << endmsg;
    }
  
  }
  else {
    msg(MSG::INFO) << "Using the old cabling, no callback" << endmsg;
  }

  return StatusCode::SUCCESS;
}



