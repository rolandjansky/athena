///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthService.cxx 
// Implementation file for class AthService
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes

// AthenaBaseComps includes
#include "AthenaBaseComps/AthService.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
AthService::AthService( const std::string& name, 
			ISvcLocator* pSvcLocator ) : 
  ::Service      ( name, pSvcLocator )
{    
}

// Destructor
///////////////
AthService::~AthService()
{
}

StatusCode AthService::sysInitialize()
{
  return Service::sysInitialize();
}

StatusCode AthService::initialize()
{
  return StatusCode::SUCCESS;
}

StatusCode AthService::sysReinitialize()
{
  return Service::sysReinitialize();
}

StatusCode AthService::reinitialize()
{
  return StatusCode::SUCCESS;
}

StatusCode AthService::sysFinalize()
{
  return Service::sysFinalize();
}

StatusCode AthService::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode 
AthService::queryInterface( const InterfaceID& riid, 
			    void**             ppvi )
{
  return Service::queryInterface( riid, ppvi );
}
