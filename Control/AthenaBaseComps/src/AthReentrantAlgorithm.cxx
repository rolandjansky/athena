///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthReentrantAlgorithm.cxx 
// Implementation file for class AthReentrantAlgorithm
// Author: C. Leggett
/////////////////////////////////////////////////////////////////// 

// AthenaBaseComps includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

// FrameWork includes
#include "GaudiKernel/Property.h"
#ifndef REENTRANT_GAUDI
#include "GaudiKernel/ThreadLocalContext.h"
#endif

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
AthReentrantAlgorithm::AthReentrantAlgorithm( const std::string& name, 
			    ISvcLocator* pSvcLocator,
			    const std::string& version ) : 
  ::ReEntAlgorithm   ( name, pSvcLocator, version ),
  ::AthMessaging( msgSvc(), name ),
  m_evtStore    ( "StoreGateSvc/StoreGateSvc",  name ),
  m_detStore    ( "StoreGateSvc/DetectorStore", name ),
  m_userStore   ( "UserDataSvc/UserDataSvc", name )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  auto props = getProperties();
  for( Property* prop : props ) {
    if( prop->name() != "OutputLevel" ) {
      continue;
    }
    prop->declareUpdateHandler
      (&AthReentrantAlgorithm::msg_update_handler, this);
    break;
  }

  declareProperty( "EvtStore",
                   m_evtStore = StoreGateSvc_t ("StoreGateSvc", name),
                   "Handle to a StoreGateSvc instance: it will be used to "
                   "retrieve data during the course of the job" );

  declareProperty( "DetStore",
                   m_detStore = StoreGateSvc_t ("StoreGateSvc/DetectorStore", name),
                   "Handle to a StoreGateSvc/DetectorStore instance: it will be used to "
                   "retrieve data during the course of the job" );

  declareProperty( "UserStore",
                   m_userStore = UserDataSvc_t ("UserDataSvc/UserDataSvc", name),
                   "Handle to a UserDataSvc/UserDataSvc instance: it will be used to "
                   "retrieve user data during the course of the job" );
}

// Destructor
///////////////
AthReentrantAlgorithm::~AthReentrantAlgorithm()
{ 
  ATH_MSG_DEBUG ("Calling destructor");
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void 
AthReentrantAlgorithm::msg_update_handler( Property& outputLevel ) 
{
   // We can't just rely on the return value of msgLevel() here. Since it's
   // not well defined whether the base class gets updated with the new
   // output level first, or this class. So by default just use the property
   // itself. The fallback is only there in case Gaudi changes its property
   // type at one point, to be able to fall back on something.
   IntegerProperty* iprop = dynamic_cast< IntegerProperty* >( &outputLevel );
   if( iprop ) {
      msg().setLevel( iprop->value() );
   } else {
      msg().setLevel( msgLevel() );
   }
}


#ifndef REENTRANT_GAUDI
/**
 * @brief Standard Gaudi execute method.
 *
 * Find the event context and call @c execute_r.
 */
StatusCode AthReentrantAlgorithm::execute()
{
  return execute_r (Gaudi::Hive::currentContext());
}
#endif
