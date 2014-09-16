/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include <iostream>
#include <algorithm>
#include <iterator> // remove it (it is here to help with debugging)



//#include <boost/cstdint.hpp>
//#include <stdint.h>
#include <boost/lexical_cast.hpp>

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/System.h"
//#include "GaudiKernel/IClassManager.h"
//#include "GaudiKernel/SmartIF.h"

#include "TrigNavigation/Navigation.h"
using namespace HLT;
using namespace HLTNavDetails;


//std::map<CLID, IHolder* > Navigation::m_types;



//Navigation::Navigation( TriggerElementFactory& factory )
Navigation::Navigation(  const std::string& type, const std::string& name,
                         const IInterface* parent )
  : AthAlgTool(type, name, parent),
    m_serializerServiceHandle("TrigSerializeCnvSvc", name),
    m_storeGateHandle("StoreGateSvc", name)
{

  declareProperty("ReferenceAllClasses", m_referenceAllClasses=false,
                  "Flag anabling all features referenceing.");

  declareProperty("ClassesToPayload", m_classesToPayloadProperty,
                  "List of classes which need to be serialized together with the Navigation.");

  declareProperty("ClassesFromPayload", m_classesFromPayloadProperty,
                  "List of classes which need to be de-serialized together with the Navigation.");
  
  declareProperty("ClassesToPayload_DSonly", m_classesToPayloadProperty_DSonly,
                  "List of classes which need to be serialized together with the Navigation (Only in DataScouting collection).");
  
  declareProperty("ClassesToPreregister", m_classesToPreregisterProperty,
                  "List of classes which need to be put in SG independently if they appear in event.");

  //  declareProperty("SerializeSpaceLimit",  m_serializeSpaceLimit=8000000,
  //                  "Limit of the size which navigation can use once serialized.");

  declareProperty("Dlls",  m_dlls, "Libraries to load (with trigger EDM)");
  declareProperty("ObjectsKeyPrefix", m_objectsKeyPrefix="HLT", "The prefix which all Trigger EDM objects will get, by default it is HLT");
  declareProperty("ObjectsIndexOffset", m_objectsIndexOffset=0, "The offset with which the objects idx is be shifted.");
  declareProperty("EvtStore", m_storeGateHandle);

  declareInterface<Navigation>(this);
}

Navigation::~Navigation() {}


/*****************************************************************************
 *
 * INITIALIZATION and FINALIZATION
 *
 *****************************************************************************/
StatusCode Navigation::initialize() {
  // message log
  delete m_log;
  m_log = new MsgStream(msgSvc(), name() );

  // factor of TEs
  /* in CORE m_factory = new TriggerElementFactory;
  if ( m_factory->listOfProduced().size() != 0 ) {
    *m_log << MSG::FATAL << "Navigation::Navigation(), factory size not 0 but: " 
	   << m_factory->listOfProduced().size() << endreq;
    return StatusCode::FAILURE;
  }

  */
 // get StoreGate
  StatusCode sc = m_storeGateHandle.retrieve();
  if(sc.isFailure()) {
    (*m_log) << MSG::FATAL << "Unable to get pointer to StoreGate Service: "
             << m_storeGateHandle << endreq;
    return StatusCode::FAILURE;
  }
  //  m_storeGate = m_storeGateHandle.operator->();
  //  AccessProxy* ap = new AccessProxy(0, m_storeGateHandle.operator->());
  setAccessProxy(m_storeGateHandle.operator->());

  // create the Serializer
  //  m_serializer = NULL;
  /*
    if ( m_serializerToolHandle.retrieve().isFailure() ) {
    (*m_log) << MSG::FATAL << "failed to retrieve serializer tool: "
    << m_serializerToolHandle<< endreq;
    return  StatusCode::FAILURE;
    } else {
    (*m_log) << MSG::DEBUG << "successfully retrieved serializer tool: "
    << m_serializerTool << endreq;
    //    m_serializer = m_serializerTool.operator->();
    }
    m_serializerTool = m_serializerToolHandle.operator->();
  */

  StatusCode scnv = m_serializerSvc.retrieve();
  if (scnv.isFailure()){
    *m_log << MSG::FATAL << "Navigation::initialize() cannot get TrigSerializeCnvSvc"
           << endreq;
  } else {
    if (m_log->level() <= MSG::DEBUG )
      *m_log << MSG::DEBUG << "Navigation::initialize() got TrigSerializeCnvSvc"
             << endreq;
  }

  // payload def
  if ( classKey2CLIDKey(m_classesToPayloadProperty,  m_classesToPayload).isFailure() ) {
    (*m_log) << MSG::FATAL << "failed to decode property ClassesToPayload: "
             << m_classesToPayloadProperty << endreq;
    return  StatusCode::FAILURE;
  }

  if ( classKey2CLIDKey(m_classesToPayloadProperty_DSonly,  m_classesToPayload_DSonly).isFailure() ) {
    (*m_log) << MSG::FATAL << "failed to decode property ClassesToPayload: " 
	     << m_classesToPayloadProperty_DSonly << endreq;
    
    return  StatusCode::FAILURE;
  }
  
  // initialize converters
  for (size_t icl=0; icl<m_classesToPayload.size(); icl++){
    CLID cl = m_classesToPayload.at(icl).first;
    StatusCode stmp = m_serializerSvc->addConverter(cl);
    if (stmp.isFailure())
      *m_log << MSG::WARNING << "Initialization of a converter for CLID=" << cl << " failed" << endreq;
  }


  // preregistration def
  if ( classKey2CLIDKey(m_classesToPreregisterProperty,  m_classesToPreregister).isFailure() ) {
    (*m_log) << MSG::FATAL << "failed to decode property ClassesToPreregister: "
             << m_classesToPreregisterProperty << endreq;
    return  StatusCode::FAILURE;
  }


  // create string specific serializer
  //in CORE m_stringSerializer = new StringSerializer();

  // print out registered holders
  HLT::TypeMaps::CLIDtoHolderMap::const_iterator holderIt;
  if (m_log->level() <= MSG::VERBOSE ) {
    for ( holderIt = HLT::TypeMaps::holders().begin(); holderIt != HLT::TypeMaps::holders().end(); ++holderIt ) {
      if(!holderIt->second){
	(*m_log) << MSG::FATAL << "static type information not intialized. Holder is null pointer" << endreq;
      }
      (*m_log) << MSG::VERBOSE << *(holderIt->second) << endreq;
    }
  }

  // load libraries
  std::vector<std::string>::const_iterator dlIt;
  for ( dlIt = m_dlls.begin(); dlIt != m_dlls.end(); ++dlIt ) {
    System::ImageHandle handle = 0;
    if ( System::loadDynamicLib( *dlIt, &handle)  != 1 ) {
      (*m_log) << MSG::WARNING << "failed to load " << *dlIt << endreq;
    } else {
      if (m_log->level() <= MSG::DEBUG )
        (*m_log) << MSG::DEBUG << "forcibly loaded library " << *dlIt << endreq;
    }
  }

  // translate Class names into CLID numbers
  if (m_log->level() <= MSG::DEBUG )
    (*m_log) << MSG::DEBUG << " successfully initialized Navigation "
             << endreq;

  return StatusCode::SUCCESS;
}

StatusCode
Navigation::classKey2CLIDKey(const std::vector<std::string> property,
                             std::vector<std::pair<CLID,
                             std::string> >& decoded ) {
  // translate Class names into CLID numbers
  IClassIDSvc* clidSvc;
  if( service("ClassIDSvc", clidSvc).isFailure() ) {
    (*m_log) << MSG::FATAL << "Unable to get pointer to CLIDSvc Service" << endreq;
    return StatusCode::FAILURE;
  }

  std::vector<std::string>::const_iterator it;
  for ( it = property.begin(); it != property.end(); ++it ) {
    CLID clid;
    std::string key;
    std::string type;

    if ( it->find("#") != std::string::npos ) {
      type = it->substr(0, it->find("#") );
      key  = it->substr(it->find("#")+1 );
    } else {
      type = *it;
      key = "";
    }

    if ( clidSvc->getIDOfTypeName(type, clid).isFailure() ) {
      (*m_log) << MSG::FATAL << "Unable to get CLID for class: " << *it
               << " check property" << endreq;
      return StatusCode::FAILURE;
    }

    if (m_log->level() <= MSG::DEBUG )
      (*m_log) << MSG::DEBUG << "Recognized CLID : " << type << " and key: " << key
               << endreq;

    decoded.push_back(std::make_pair(clid, key));
  }
  return StatusCode::SUCCESS;
}

StatusCode Navigation::finalize() {
  if (m_log->level() <= MSG::DEBUG )
    *m_log << MSG::DEBUG << "Navigation finalize" << endreq;
  //  reset();

  /* in CORE elete m_stringSerializer;
  delete m_factory;
  delete m_log;
  */
  /*
    for (std::map< CLID, std::vector<uint32_t>* > ::iterator it = m_serializedFeatures.begin();
    it != m_serializedFeatures.end(); ++it) {
    delete it->second;
    }
  */
  return StatusCode::SUCCESS;
}



MsgStream& HLT::operator<< ( MsgStream& m, const Navigation& nav ) {
  m << (NavigationCore&)nav;
  return m;
}
