///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PerfMonTestBasicAlg.cxx 
// Implementation file for class PerfMonTest::BasicAlg
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "PerfMonKernel/IPerfMonSvc.h"

// StoreGate
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/CLASS_DEF.h"

// PerfMonTests includes
#include "PerfMonTestBasicAlg.h"

namespace PerfMonTest {
  struct Data {
    std::vector<int> m_data;
  };
}

CLASS_DEF( PerfMonTest::Data, 208557492, 1 )


using namespace PerfMonTest;

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
BasicAlg::BasicAlg( const std::string& name, 
		  ISvcLocator* pSvcLocator ) : 
  Algorithm   ( name,    pSvcLocator ),
  m_storeGate ( "StoreGateSvc", name ),
  m_msg       ( msgSvc(),       name )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty( "DataSize",
		   m_dataSize = 10,
		   "Number of 'Data' objects to be stored each event" );

  declareProperty( "MyBool",
		   m_my_bool = false,
		   "dummy boolean to exercize IPerfMonSvc::declareInfo" );

  declareProperty( "MyInt",
		   m_my_int = 10,
		   "dummy integer to exercize IPerfMonSvc::declareInfo" );

  declareProperty( "MyLong",
		   m_my_long = 100,
		   "dummy long to exercize IPerfMonSvc::declareInfo" );

  declareProperty( "MyDouble",
		   m_my_double = 600.,
		   "dummy double to exercize IPerfMonSvc::declareInfo" );

  declareProperty( "MyString",
		   m_my_string = "foo",
		   "dummy string to exercize IPerfMonSvc::declareInfo" );

  declareProperty( "MyPair",
		   m_my_pair = std::make_pair<double,double>(10.,42.),
		   "dummy pair to exercize IPerfMonSvc::declareInfo" );
}

// Destructor
///////////////
BasicAlg::~BasicAlg()
{ 
  m_msg << MSG::DEBUG << "Calling destructor" << endreq;
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode BasicAlg::initialize()
{
  // configure our MsgStream
  m_msg.setLevel( outputLevel() );

  m_msg << MSG::INFO 
	<< "Initializing " << name() << "..." 
	<< endreq;

  // Get pointer to StoreGateSvc and cache it :
  if ( !m_storeGate.retrieve().isSuccess() ) {
    m_msg << MSG::ERROR 	
	  << "Unable to retrieve pointer to StoreGateSvc"
	  << endreq;
    return StatusCode::FAILURE;
  }
  
  m_dataOutputName = "PerfMonTest_Data_" + name();

  // retrieve a handle to the IPerfMonSvc
  ServiceHandle<IPerfMonSvc> svc( "PerfMonSvc", this->name() );
  if ( !svc.retrieve().isSuccess() ) {
    m_msg << MSG::ERROR
	  << "Could not retrieve [" << svc << "] !!" 
	  << endreq;
    return StatusCode::FAILURE;
  }

  svc->declareInfo( "my_bool",   m_my_bool,   "a boolean",         this );
  svc->declareInfo( "my_int",    m_my_int,    "an integer",        this );
  svc->declareInfo( "my_long",   m_my_long,   "a long",            this );
  svc->declareInfo( "my_double", m_my_double, "a double",          this );
  svc->declareInfo( "my_string", m_my_string, "an std::string",    this );
  svc->declareInfo( "my_pair",   m_my_pair,   "a pair of doubles", this );

  return StatusCode::SUCCESS;
}

StatusCode BasicAlg::finalize()
{
  m_msg << MSG::INFO 
	<< "Finalizing " << name() << "..." 
	<< endreq;

  return StatusCode::SUCCESS;
}

StatusCode BasicAlg::execute()
{  
  m_msg << MSG::DEBUG << "Executing " << name() << "..." 
	<< endreq;

  if ( 0 == m_dataSize ) {
    return StatusCode::SUCCESS;
  }

  Data * data = new Data;
  if ( !m_storeGate->record(data, m_dataOutputName).isSuccess() ) {
    m_msg << MSG::ERROR
          << "Could not store data at [" << m_dataOutputName << "] !!"
          << endreq;
    delete data;
    data = 0;
    return StatusCode::RECOVERABLE;
  }
  if ( !m_storeGate->setConst(data).isSuccess() ) {
    m_msg << MSG::WARNING << "Could not setConst data at ["
	  << m_dataOutputName << "] !!"
          << endreq;
  }  

  // testing scoped-monitoring
  {
    PerfMon::ScopedMonitor mon(*this, "filling_data");
    // filling data...
    data->m_data.reserve( m_dataSize );
    for ( std::size_t i = 0; i != static_cast<std::size_t>(m_dataSize); ++i ) {
      data->m_data.push_back( i );
    }
  }

  m_my_bool  = !m_my_bool;
  m_my_int  += 10;
  m_my_long += 11;
  m_my_double += 42.;
  m_my_string += ( m_my_bool ? "0" : "1" );
  m_my_pair.first  += 666.;
  m_my_pair.second -= 666.;

  return StatusCode::SUCCESS;
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

