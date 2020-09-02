///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ConcreteSvc.cxx 
// Implementation file for class ConcreteSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes

// FrameWork includes
#include "Gaudi/Property.h"

// AthExJobOptions includes
#include "ConcreteSvc.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
ConcreteSvc::ConcreteSvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService ( name, pSvcLocator )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty, "descr" );

  declareProperty( "Qotd",
		   m_qotd = "Your day will be somewhat dictated by authority",
		   "Very important message the service has to advertise" );
}

// Destructor
///////////////
ConcreteSvc::~ConcreteSvc()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ConcreteSvc::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");
  ATH_MSG_INFO ("Quote of the day: [" << m_qotd.value() << "]");
  return StatusCode::SUCCESS;
}

StatusCode ConcreteSvc::finalize()
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
ConcreteSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
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

StatusCode ConcreteSvc::qotd( std::string& quote )
{
  quote = m_qotd.value();
  return StatusCode::SUCCESS;
}
