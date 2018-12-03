/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCondSvc/L1CaloCondSvc.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include <list>
#include <set>


L1CaloCondSvc::L1CaloCondSvc(const std::string& name, ISvcLocator* svc) :
  AthService(name, svc),
  m_detStore ("DetectorStore", name),
  m_regSvc ("IOVRegistrationSvc", name)
{
    // declare properties
	declareProperty("StreamName", m_streamName = "COOLStream");
	declareProperty("BeginRun",   m_iovBeginRun   = 0         );
	declareProperty("EndRun",     m_iovEndRun     = 0x7FFFFFFF);
	declareProperty("BeginLB", m_iovBeginLB = 0         );
	declareProperty("EndLB",   m_iovEndLB   = 0xFFFFFFFF);
	declareProperty("BeginTime",   m_iovBeginTime   = 0);
	declareProperty("EndTime",   m_iovEndTime   = 0);
	declareProperty("TimeStamp",   m_bUseTimeStamp   = false);
	declareProperty("WriteNewChannels",   m_bWriteNewChannels   = false);
}

L1CaloCondSvc::~L1CaloCondSvc() {}

StatusCode L1CaloCondSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  ATH_MSG_DEBUG("in queryInterface()");

  if (L1CaloCondSvc::interfaceID().versionMatch(riid)) {
    ATH_MSG_DEBUG("matched L1CaloCondSvc");
    *ppvInterface=(L1CaloCondSvc*)this;
  } else {
    ATH_MSG_DEBUG("Default to Service interface");
    return AthService::queryInterface(riid,ppvInterface);
  }
  return StatusCode::SUCCESS;
}


StatusCode L1CaloCondSvc::initialize()
{
  ATH_CHECK( AthService::initialize() );
  ATH_CHECK( m_detStore.retrieve() );
  ATH_CHECK( m_regSvc.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode L1CaloCondSvc::finalize()
{
	ATH_MSG_DEBUG("in finalize()");

	std::map<std::string, const DataHandle<AthenaAttributeList>* >::iterator it_AttrList = m_mDataHandleAttrList.begin();
	for(;it_AttrList!=m_mDataHandleAttrList.end();++it_AttrList) {
		delete it_AttrList->second;
	}

  	std::map<std::string, const DataHandle<CondAttrListCollection>* >::iterator it_AttrListColl = m_mDataHandleAttrListColl.begin();
	for(;it_AttrListColl!=m_mDataHandleAttrListColl.end();++it_AttrListColl) {
		delete it_AttrListColl->second;
	}

	return StatusCode::SUCCESS;
}

StatusCode L1CaloCondSvc::updateConditions(IOVSVC_CALLBACK_ARGS_K(keys)) {


// 	ATH_MSG_INFO("ACH updateConditions()");
        ATH_MSG_VERBOSE("updateConditions()");

	// set to store the list of objects to be updated.
	std::set<IL1CaloPersistenceCapable*> vToBeUpdated;

	// loop over the keys of folders that have new conditions to be cached
	std::list<std::string>::const_iterator itr;
	for(itr=keys.begin(); itr!=keys.end(); ++itr) {
		std::string key = *itr;
// 		ATH_MSG_INFO("ACH key = " << key);
                ATH_MSG_VERBOSE("key = " << key);

		// find the current key in the map
		std::map<std::string, std::vector<IL1CaloPersistenceCapable*> >::const_iterator it_map = m_mConditions.find(key);
		if(it_map!=m_mConditions.end()) {

			// loop over objects that depend on the current folder/key
			std::vector<IL1CaloPersistenceCapable*> vObjs = it_map->second;
			std::vector<IL1CaloPersistenceCapable*>::const_iterator it_vObjs = vObjs.begin();
			for(;it_vObjs!=vObjs.end();++it_vObjs) {
				IL1CaloPersistenceCapable* pobj = *it_vObjs;
				vToBeUpdated.insert(pobj);
			}
		}
	}

	// form here vToBeUpdated contains the list of objects to be updated
	// loop over them, get the required datahandles and pass them to pobj
	std::set<IL1CaloPersistenceCapable*>::const_iterator it_set = vToBeUpdated.begin();
	for(;it_set!=vToBeUpdated.end();++it_set) {
		IL1CaloPersistenceCapable* pobj = *it_set;

		// get the keys/folders required by current object
		std::vector<std::string> vCoolInputKeys = pobj->coolInputKeys();
// 		ATH_MSG_INFO("ACH vCoolInputKeys size is "<<vCoolInputKeys.size());
                std::vector<std::string> otherkeys = m_map_conditions2key[pobj];
//                 ATH_MSG_INFO("ACH otherkeys size is "<<otherkeys.size());
		vCoolInputKeys.insert(vCoolInputKeys.end(),otherkeys.begin(),otherkeys.end());
// 		ATH_MSG_INFO("ACH vCoolInputKeys size is now "<<vCoolInputKeys.size());
		std::string conditionType = pobj->conditionType();
//                 ATH_MSG_INFO("ACH conditionType = "<<conditionType);

		if(conditionType=="CondAttrListCollection") {

			// will contain pointer to the CondAttrListCollection(s) required by pobj to populate itself.
			std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap;

			// loop onver input keys, to get the corresponding datahandle
			std::vector<std::string>::const_iterator it_coolInputKeys = vCoolInputKeys.begin();
			for(;it_coolInputKeys!=vCoolInputKeys.end();++it_coolInputKeys) {
				std::string key = *it_coolInputKeys;
// 				ATH_MSG_INFO("ACH update key = " << key);
                                std::map<std::string, const DataHandle<CondAttrListCollection>* >::iterator it = m_mDataHandleAttrListColl.find(key);
                                if (it != m_mDataHandleAttrListColl.end()) {
				   const DataHandle<CondAttrListCollection>& dh = *m_mDataHandleAttrListColl[key];
				   const CondAttrListCollection* attrListCollection = &(*dh);
				   if (attrListCollection) {
// 					ATH_MSG_INFO("ACH attrListCollection = "<<attrListCollection);
					condAttrListCollectionMap[key] = (CondAttrListCollection*)attrListCollection;
					}
                                }
			}
// 			ATH_MSG_INFO("ACH condAttrListCollectionMap size is "<<condAttrListCollectionMap.size());
			pobj->makeTransient(condAttrListCollectionMap);

		} else if(conditionType=="AthenaAttributeList"){
/*
			const DataHandle<AthenaAttributeList>& dh = *m_mDataHandleAttrList[key];

			const AthenaAttributeList* athenaAttributeList = &(*dh);
			pobj->makeTransient(athenaAttributeList);
*/
		} else {
			ATH_MSG_ERROR("Condition type " << conditionType<< "not recognized by L1CaloCondSvc");
			return StatusCode::FAILURE;
		}

	}



/*
		std::map<std::string, std::vector<IL1CaloPersistenceCapable*> >::const_iterator it_obj = m_mConditions.find(key);
		if(it_obj!=m_mConditions.end()) {

			IL1CaloPersistenceCapable* pobj = it_obj->second;

			std::string conditionType = pobj->conditionType();
			if(conditionType=="CondAttrListCollection") {

				const DataHandle<CondAttrListCollection>& dh = *m_mDataHandleAttrListColl[key];

				const CondAttrListCollection* attrListCollection = &(*dh);
				pobj->makeTransient(attrListCollection);

			} else if(conditionType=="AthenaAttributeList"){

				const DataHandle<AthenaAttributeList>& dh = *m_mDataHandleAttrList[key];

				const AthenaAttributeList* athenaAttributeList = &(*dh);
				pobj->makeTransient(athenaAttributeList);

			} else {
				log << MSG::ERROR << "Condition type " << conditionType<< "not recognized by L1CaloCondSvc" << endreq;
				return StatusCode::FAILURE;
			}
		}
*/


	ATH_MSG_VERBOSE("end of L1CaloCondSvc::updateConditions(IOVSVC_CALLBACK_ARGS_P(/*I*/, keys))");
	return StatusCode::SUCCESS;
}


//StatusCode L1CaloCondSvc::updateConditions(IOVSVC_CALLBACK_ARGS_P(/*I*/, keys)) {
//	MsgStream log(msgSvc(),name());
//
//	log<<MSG::VERBOSE<<"updateConditions()"<<endreq;
//
//	std::list<std::string>::const_iterator itr;
//	for(itr=keys.begin(); itr!=keys.end(); ++itr) {
//		std::string key = *itr;
//		log << MSG::VERBOSE << key << endreq;
//
//		std::map<std::string, std::vector<IL1CaloPersistenceCapable*> >::const_iterator it_obj = m_mConditions.find(key);
//		if(it_obj!=m_mConditions.end()) {
//
//			IL1CaloPersistenceCapable* pobj = it_obj->second;
//
//			std::string conditionType = pobj->conditionType();
//			if(conditionType=="CondAttrListCollection") {
//
//				const DataHandle<CondAttrListCollection>& dh = *m_mDataHandleAttrListColl[key];
//
//				const CondAttrListCollection* attrListCollection = &(*dh);
//				pobj->makeTransient(attrListCollection);
//
//			} else if(conditionType=="AthenaAttributeList"){
//
//				const DataHandle<AthenaAttributeList>& dh = *m_mDataHandleAttrList[key];
//
//				const AthenaAttributeList* athenaAttributeList = &(*dh);
//				pobj->makeTransient(athenaAttributeList);
//
//			} else {
//				log << MSG::ERROR << "Condition type " << conditionType<< "not recognized by L1CaloCondSvc" << endreq;
//				return StatusCode::FAILURE;
//			}
//		}
//	}
//
//	log << MSG::VERBOSE << "end of L1CaloCondSvc::updateConditions(IOVSVC_CALLBACK_ARGS_P(/*I*/, keys))" << endreq;
//	return StatusCode::SUCCESS;
//}

