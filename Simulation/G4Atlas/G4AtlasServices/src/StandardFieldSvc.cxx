/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StandardFieldSvc.h"

//-----------------------------------------------------------------------------
// Constructor for the AtlasField G4 field object
//-----------------------------------------------------------------------------
AtlasField::AtlasField(MagField::IMagFieldSvc* m)
  : m_magFieldSvc(m)
{}

//-----------------------------------------------------------------------------
// Constructor for the StandardFieldSvc
//-----------------------------------------------------------------------------
StandardFieldSvc::StandardFieldSvc(const std::string& name,
                                   ISvcLocator* pSvcLocator)
  : G4MagFieldSvcBase(name, pSvcLocator),
    m_magFieldSvc("MagField::AtlasFieldSvc/AtlasFieldSvc", name)
{
  declareProperty("MagneticFieldSvc", m_magFieldSvc);
}

//-----------------------------------------------------------------------------
// Initialize the service
//-----------------------------------------------------------------------------
StatusCode StandardFieldSvc::initialize()
{
  ATH_MSG_INFO( "Initializing " << name() );
  ATH_CHECK( m_magFieldSvc.retrieve() );
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Create and retrieve the magnetic field
//-----------------------------------------------------------------------------
G4MagneticField* StandardFieldSvc::makeField()
{
  ATH_MSG_INFO( "StandardFieldSvc::makeField" );
  return new AtlasField( &*m_magFieldSvc );
}
