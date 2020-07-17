/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StandardFieldSvc.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "StoreGate/ReadCondHandle.h"


//-----------------------------------------------------------------------------
// Constructor for the StandardFieldSvc
//-----------------------------------------------------------------------------
StandardFieldSvc::StandardFieldSvc(const std::string& name,
                                   ISvcLocator* pSvcLocator)
    : G4MagFieldSvcBase(name, pSvcLocator)
{}

//-----------------------------------------------------------------------------
// Initialize the service
//-----------------------------------------------------------------------------
StatusCode StandardFieldSvc::initialize()
{
  ATH_MSG_INFO( "Initializing " << name() );

  // Read handle for AtlasFieldCacheCondObj
  ATH_CHECK( m_fieldCacheCondObjInputKey.initialize() );
    
  ATH_MSG_INFO( "Initialized " << m_fieldCacheCondObjInputKey.key() );
  
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Create and retrieve the magnetic field
//-----------------------------------------------------------------------------
G4MagneticField* StandardFieldSvc::makeField()
{
  ATH_MSG_INFO( "StandardFieldSvc::makeField" );

  MagField::AtlasFieldCache fieldCache;
  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle(
    m_fieldCacheCondObjInputKey, Gaudi::Hive::currentContext());
  const AtlasFieldCacheCondObj* fieldCondObj{ *readHandle };
  if (fieldCondObj == nullptr) {
    ATH_MSG_ERROR(
      "G4MagneticField : Failed to retrieve AtlasFieldCacheCondObj with key "
      << m_fieldCacheCondObjInputKey.key());
    return nullptr;
  }

  AtlasField* af = new AtlasField(fieldCondObj);
  return (af);
}
