///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AnotherConcreteSvc.cxx 
// Implementation file for class AnotherConcreteSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes

// FrameWork includes
#include "Gaudi/Property.h"

// AthExJobOptions includes
#include "AnotherConcreteSvc.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
AnotherConcreteSvc::AnotherConcreteSvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService ( name, pSvcLocator )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty, "descr" );

  declareProperty( "Qotd",
		   m_qotd = "Quote is not available for today!",
		   "Very important message the service has to advertise" );
}

// Destructor
///////////////
AnotherConcreteSvc::~AnotherConcreteSvc()
{}

// Athena service's Hooks
////////////////////////////
StatusCode
AnotherConcreteSvc::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  ATH_MSG_INFO ("Quote of the day: [" << m_qotd.value() << "]");
  return StatusCode::SUCCESS;
}

StatusCode AnotherConcreteSvc::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode 
AnotherConcreteSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IVirtualSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IVirtualSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

StatusCode AnotherConcreteSvc::qotd( std::string& quote )
{
  quote = m_qotd.value();
  return StatusCode::SUCCESS;
}
