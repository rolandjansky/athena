/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Base class
#include "FastSimulationSvc.h"

FastSimulationSvc::FastSimulationSvc( const std::string& name, ISvcLocator* pSvcLocator )
  : AthService(name,pSvcLocator)
  , m_FastSimList(this)
{
  declareProperty( "FastSimulations" , m_FastSimList , "Tool handle array of fast simulation tools" );
}

StatusCode FastSimulationSvc::initialize(){
  // Loop through list of fast simulations and retrieve them
  //  This forces a call to initialize() for each of them
  ATH_MSG_INFO( "Initializing list of " << m_FastSimList.size() << " fast simulation tools in " << name() );
  CHECK( m_FastSimList.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode FastSimulationSvc::finalize(){
  // Nothing to do here
  return StatusCode::SUCCESS;
}

StatusCode FastSimulationSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IFastSimulationSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IFastSimulationSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

