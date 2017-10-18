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

 // get StoreGate
  StatusCode sc = m_storeGateHandle.retrieve();
  if(sc.isFailure()) {
    ATH_MSG_FATAL("Unable to get pointer to StoreGate Service: " << m_storeGateHandle);
    return StatusCode::FAILURE;
  }
  m_storeGate = m_storeGateHandle.operator->();

  StatusCode scnv = m_serializerServiceHandle.retrieve();
  if (scnv.isFailure()){
    ATH_MSG_FATAL("Navigation::initialize() cannot get TrigSerializeCnvSvc");
  } else {
    ATH_MSG_DEBUG("Navigation::initialize() got TrigSerializeCnvSvc");
  }
  m_serializerSvc = m_serializerServiceHandle.operator->();

  m_fullholderfactory.prepare(m_storeGate,m_serializerSvc,m_readonly);
  m_holderfactory = &m_fullholderfactory;

  CHECK(m_clidSvc.retrieve());

  // payload def
  if ( classKey2CLIDKey(m_classesToPayloadProperty,  m_classesToPayload).isFailure() ) {
    ATH_MSG_FATAL("failed to decode property ClassesToPayload: " << m_classesToPayloadProperty);
    return  StatusCode::FAILURE;
  }

  if ( classKey2CLIDKey(m_classesToPayloadProperty_DSonly,  m_classesToPayload_DSonly).isFailure() ) {
    ATH_MSG_FATAL("failed to decode property ClassesToPayload: " << m_classesToPayloadProperty_DSonly);
    
    return  StatusCode::FAILURE;
  }
  
  // initialize converters
  for (size_t icl=0; icl<m_classesToPayload.size(); icl++){
    CLID cl = m_classesToPayload.at(icl).first;
    StatusCode stmp = m_serializerSvc->addConverter(cl);
    if (stmp.isFailure())
      ATH_MSG_WARNING("Initialization of a converter for CLID=" << cl << " failed");
  }


  // preregistration def
  if ( classKey2CLIDKey(m_classesToPreregisterProperty,  m_classesToPreregister).isFailure() ) {
    ATH_MSG_FATAL("failed to decode property ClassesToPreregister: " << m_classesToPreregisterProperty);
    return  StatusCode::FAILURE;
  }

  // print out registered holders
  HLT::TypeMaps::CLIDtoHolderMap::const_iterator holderIt;
  if (msgLvl(MSG::VERBOSE) ) {
    for ( holderIt = HLT::TypeMaps::holders().begin(); holderIt != HLT::TypeMaps::holders().end(); ++holderIt ) {
      if(!holderIt->second){
        ATH_MSG_FATAL("static type information not intialized. Holder is null pointer");
      }
      ATH_MSG_VERBOSE(*(holderIt->second));
    }
  }

  // load libraries
  std::vector<std::string>::const_iterator dlIt;
  for ( dlIt = m_dlls.begin(); dlIt != m_dlls.end(); ++dlIt ) {
    System::ImageHandle handle = 0;
    if ( System::loadDynamicLib( *dlIt, &handle)  != 1 ) {
      ATH_MSG_WARNING("failed to load " << *dlIt);
    } else {
        ATH_MSG_DEBUG("forcibly loaded library " << *dlIt);
    }
  }

  // translate Class names into CLID numbers
  ATH_MSG_DEBUG(" successfully initialized Navigation ");

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
      ATH_MSG_FATAL("Unable to get CLID for class: " << *it << " check property");
      return StatusCode::FAILURE;
    }

      ATH_MSG_DEBUG("Recognized CLID : " << type << " and key: " << key);

    decoded.push_back(std::make_pair(clid, key));
  }
  return StatusCode::SUCCESS;
}

StatusCode Navigation::finalize() {
    ATH_MSG_DEBUG("Navigation finalize");
  return StatusCode::SUCCESS;
}



MsgStream& HLT::operator<< ( MsgStream& m, const Navigation& nav ) {
  m << (NavigationCore&)nav;
  return m;
}
