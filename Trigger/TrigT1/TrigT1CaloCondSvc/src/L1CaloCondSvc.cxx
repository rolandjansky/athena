/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrigT1CaloCondSvc/L1CaloCondSvc.h"

#include <list>
#include <set>


L1CaloCondSvc::L1CaloCondSvc(const std::string& name, ISvcLocator* svc) :
  Service(name, svc),
  m_detStore(0),
  m_regSvc(0),
  m_streamer(0)
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

const InterfaceID& L1CaloCondSvc::type() const
{
	return this->interfaceID();
}

StatusCode L1CaloCondSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "in queryInterface()" << endreq;

  if (L1CaloCondSvc::interfaceID().versionMatch(riid)) {
    log << MSG::DEBUG << "matched L1CaloCondSvc" << endreq;
    *ppvInterface=(L1CaloCondSvc*)this;
  } else {
    log << MSG::DEBUG << "Default to Service interface" << endreq;
    return Service::queryInterface(riid,ppvInterface);
  }
  return StatusCode::SUCCESS;
}

StatusCode L1CaloCondSvc::initialize()
{
  // service initialisation
  MsgStream log(msgSvc(),name());

  if (StatusCode::SUCCESS!=Service::initialize()) log << MSG::ERROR <<
	    "Service initialisation failed" << endreq;

  log << MSG::DEBUG << "in initialize()" << endreq;

  // get detector store
  if (StatusCode::SUCCESS!=service("DetectorStore", m_detStore)) {
    log << MSG::FATAL << "Detector store not found" << endreq;
    return StatusCode::FAILURE;
  }

	// get IOV registration service
	StatusCode sc = service("IOVRegistrationSvc", m_regSvc);
	if (sc.isFailure()) {
	    log << MSG::INFO << "Unable to find IOVRegistrationSvc " << endreq;
	    return StatusCode::FAILURE;
	} else {
	 log << MSG::DEBUG << "Found IOVRegistrationSvc "  << endreq;
	}

	ISvcLocator* svcLoc = Gaudi::svcLocator( );
	IToolSvc* toolSvc = 0;

	sc = svcLoc->service( "ToolSvc",toolSvc  );
	if(sc.isSuccess()) {
		sc = toolSvc->retrieveTool("AthenaPoolOutputStreamTool", m_streamName, m_streamer);
		if (sc.isFailure()) {
		    log << MSG::INFO << "Unable to find AthenaOutputStreamTool" << endreq;
		    return StatusCode::FAILURE;
		}

	} else {
		log << MSG::FATAL << "Could not retrieve ToolSvc" << endreq;
	}

  return StatusCode::SUCCESS;
}

StatusCode L1CaloCondSvc::finalize()
{
	MsgStream log(msgSvc(),name());
	log << MSG::DEBUG << "in finalize()" << endreq;

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
	MsgStream log(msgSvc(),name());

	log<<MSG::VERBOSE<<"updateConditions()"<<endreq;

	// set to store the list of objects to be updated.
	std::set<IL1CaloPersistenceCapable*> vToBeUpdated;

	// loop over the keys of folders that have new conditions to be cached
	std::list<std::string>::const_iterator itr;
	for(itr=keys.begin(); itr!=keys.end(); ++itr) {
		std::string key = *itr;
		log << MSG::VERBOSE << "key = " << key << endreq;

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
		std::string conditionType = pobj->conditionType();

		if(conditionType=="CondAttrListCollection") {

			// will contain pointer to the CondAttrListCollection(s) required by pobj to populate itself.
			std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap;

			// loop onver input keys, to get the corresponding datahandle
			std::vector<std::string>::const_iterator it_coolInputKeys = vCoolInputKeys.begin();
			for(;it_coolInputKeys!=vCoolInputKeys.end();++it_coolInputKeys) {
				std::string key = *it_coolInputKeys;
                                std::map<std::string, const DataHandle<CondAttrListCollection>* >::iterator it = m_mDataHandleAttrListColl.find(key);
                                if (it != m_mDataHandleAttrListColl.end()) {
				   const DataHandle<CondAttrListCollection>& dh = *m_mDataHandleAttrListColl[key];
				   const CondAttrListCollection* attrListCollection = &(*dh);
				   if (attrListCollection) condAttrListCollectionMap[key] = (CondAttrListCollection*)attrListCollection;
                                }
			}
			pobj->makeTransient(condAttrListCollectionMap);

		} else if(conditionType=="AthenaAttributeList"){
/*
			const DataHandle<AthenaAttributeList>& dh = *m_mDataHandleAttrList[key];

			const AthenaAttributeList* athenaAttributeList = &(*dh);
			pobj->makeTransient(athenaAttributeList);
*/
		} else {
			log << MSG::ERROR << "Condition type " << conditionType<< "not recognized by L1CaloCondSvc" << endreq;
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


	log << MSG::VERBOSE << "end of L1CaloCondSvc::updateConditions(IOVSVC_CALLBACK_ARGS_P(/*I*/, keys))" << endreq;
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

