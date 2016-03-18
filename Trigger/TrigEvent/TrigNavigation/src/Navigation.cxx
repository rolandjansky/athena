/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include <iostream>
#include <algorithm>
#include <iterator> // remove it (it is here to help with debugging)

#include <boost/lexical_cast.hpp>

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/System.h"

#include "TrigNavigation/Navigation.h"

using namespace HLT;
using namespace HLTNavDetails;

Navigation::Navigation(  const std::string& type, const std::string& name,
                         const IInterface* parent )
  : AthAlgTool(type, name, parent),
    m_serializerServiceHandle("TrigSerializeCnvSvc", name),
    m_storeGateHandle("StoreGateSvc", name),
    m_clidSvc("ClassIDSvc", name),
    m_fullholderfactory(m_objectsKeyPrefix)
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
  declareProperty("Dlls",  m_dlls, "Libraries to load (with trigger EDM)");
  declareProperty("ObjectsKeyPrefix", m_objectsKeyPrefix="HLT", "The prefix which all Trigger EDM objects will get, by default it is HLT");
  declareProperty("ObjectsIndexOffset", m_objectsIndexOffset=0, "The offset with which the objects idx is be shifted.");
  declareProperty("ReadonlyHolders", m_readonly = false, "read only flag for holders (cannot create new feature containers");
  declareInterface<Navigation>(this);
}

Navigation::~Navigation() {
}


/*****************************************************************************
 *
 * INITIALIZATION and FINALIZATION
 *
 *****************************************************************************/
StatusCode Navigation::initialize() {
  // message log
  delete m_log;
  m_log = new MsgStream(msgSvc(), name() );

 // get StoreGate
  StatusCode sc = m_storeGateHandle.retrieve();
  if(sc.isFailure()) {
    (*m_log) << MSG::FATAL << "Unable to get pointer to StoreGate Service: "
             << m_storeGateHandle << endreq;
    return StatusCode::FAILURE;
  }
  m_storeGate = m_storeGateHandle.operator->();

  StatusCode scnv = m_serializerServiceHandle.retrieve();
  if (scnv.isFailure()){
    *m_log << MSG::FATAL << "Navigation::initialize() cannot get TrigSerializeCnvSvc"
           << endreq;
  } else {
    if (m_log->level() <= MSG::DEBUG )
      *m_log << MSG::DEBUG << "Navigation::initialize() got TrigSerializeCnvSvc"
             << endreq;
  }
  m_serializerSvc = m_serializerServiceHandle.operator->();

  m_fullholderfactory.prepare(m_storeGate,m_serializerSvc,m_readonly);
  m_holderfactory = &m_fullholderfactory;

  CHECK(m_clidSvc.retrieve());

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

    if ( m_clidSvc->getIDOfTypeName(type, clid).isFailure() ) {
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
  return StatusCode::SUCCESS;
}



MsgStream& HLT::operator<< ( MsgStream& m, const Navigation& nav ) {
  m << (NavigationCore&)nav;
  return m;
}
