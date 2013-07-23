/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "ISProperty/ISPropertyMgr.h"

#include <emon/Util.h>

/**
 * Constructor
 */
ISPropertyMgr::ISPropertyMgr( const std::string& name, ISvcLocator* pSvcLocator ):
  m_ISPropSvc("ISPropertySvc",name),
  m_pSvcLocator(pSvcLocator),
  m_propertyMgr( NULL ),
  m_updateTime(1.),
  m_uniqueName(false)
{
}

/**
 * Destructor
 */
ISPropertyMgr::~ISPropertyMgr(){

  //release the ISProperty service
  m_ISPropSvc.release().ignore();

  //Delete ISonly propertyMgr
  if (m_propertyMgr) delete m_propertyMgr;

}


/**
 * publish the properties declared with declareISProperty
 */
StatusCode ISPropertyMgr::publishProperties(){

  //Retrieve ISPropertySvc
  if ( m_ISPropSvc.retrieve().isFailure()) return StatusCode::FAILURE;
 
  //Create the IS class from typeid information
  std::string type_name = System::typeinfoName(typeid(*this));

  //publish list of properties to is under name given from INamedInterface
  return m_ISPropSvc->publish(ISName(),type_name,m_ISProperties,m_updateTime);
}

/**
 * unpublish the properties declared with declareISProperty
 */
StatusCode ISPropertyMgr::unpublishProperties(){

  //Return if there is no ISPropertySvc
  if (! m_ISPropSvc ) return StatusCode::FAILURE;

  //unpublish list of properties with name given from INamedInterface
  return m_ISPropSvc->unpublish(ISName());
}

/**
 * Genarate the name of the IS object
 * - if m_uniqueName=false this is the name if the INamedInterface (the "Gaudi" name)
 * - if m_uniqueName=true, the hostname and PID are appended
 */
const std::string ISPropertyMgr::ISName() {
  //return gaudi name or generate unquie name
  if (!m_uniqueName) return name();
  else return emon::construct_info_name(name());
}

/**
 * return the list of properties
 */
PropertyList ISPropertyMgr::getISProperties(){

  //simply return the local list of property names
  return m_ISProperties;
}

/**
 * Get a local (IS only) property by name
 */
const Property& ISPropertyMgr::getISOnlyProperty( const std::string& pname ) const {
  return m_propertyMgr->getProperty(pname);
}

