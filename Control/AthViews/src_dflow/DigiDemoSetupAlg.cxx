/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DigiDemoSetupAlg.h"
#include "AthViews/ViewHelper.h"

// FrameWork includes
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"

namespace AthViews {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
DigiDemoSetupAlg::DigiDemoSetupAlg( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{
}

// Destructor
///////////////
DigiDemoSetupAlg::~DigiDemoSetupAlg()
{
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode DigiDemoSetupAlg::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  CHECK( m_w_ints.initialize() );
  CHECK( m_w_views.initialize() );
  CHECK( m_scheduler.retrieve() );
  CHECK( m_digiStore.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode DigiDemoSetupAlg::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode DigiDemoSetupAlg::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

#ifdef GAUDI_SYSEXECUTE_WITHCONTEXT 
  const EventContext& ctx = getContext();
#else
  const EventContext& ctx = *getContext();
#endif


  //Get a pointer to the digitisation store
  IProxyDict * digiStorePointer = dynamic_cast< IProxyDict* >( m_digiStore.get() );
  if ( !digiStorePointer )
  {
    ATH_MSG_FATAL( "Unable to retrieve digitisation store" );
    return StatusCode::FAILURE;
  }

  //Hacky way to determine if you've already done the setup
  SG::ReadHandle< int > firstHandle( "FirstTimeFlag" );
  firstHandle.setProxyDict( digiStorePointer );
  if ( firstHandle.isValid() )
  {
    //Skip first time setup
    ATH_MSG_INFO( "Digi store already set up" );
  }
  else
  {
    //Do first time setup
    ATH_MSG_INFO( "Setting up digi store" );
    SG::WriteHandle< int > firstHandleFiller( "FirstTimeFlag" );
    firstHandleFiller.setProxyDict( digiStorePointer );
    firstHandleFiller.record( std::make_unique< int >( 1 ) );

    //Make all the "pileup events"
    for ( int eventIndex = 0; eventIndex < 100; ++eventIndex )
    {
      SG::View * digiView = new SG::View( m_viewBaseName + std::to_string( eventIndex ), false, digiStorePointer->name() );
      SG::WriteHandle< std::vector< int > > digiHandle( m_w_ints );
      digiHandle.setProxyDict( digiView );
      digiHandle.record( std::make_unique< std::vector< int > >( 1, eventIndex ) );
      delete digiView;
    }
  }
  
  //Make a vector of views for "pileup events" to use in this event
  auto viewVector = std::make_unique< ViewContainer >(); //( m_viewNumber, nullptr );
  for ( int viewIndex = 0; viewIndex < m_viewNumber; ++viewIndex )
  {
    int sampleIndex = ( viewIndex + ctx.evt() + 37 ) % 100; // whatever
    SG::View * digiView = new SG::View( m_viewBaseName + std::to_string( sampleIndex ), false, digiStorePointer->name() );
    viewVector->push_back( digiView );
  }

  //Schedule the algorithms in views
  CHECK( ViewHelper::ScheduleViews( *viewVector.get(), //View vector
        m_viewNodeName,                         //Name of node to attach views to
        ctx,                                    //Context to attach the views to
        m_scheduler.get() ) );                  //ServiceHandle for the scheduler

  //Store the collection of views
  SG::WriteHandle< ViewContainer > outputViewHandle( m_w_views, ctx );
  outputViewHandle.record( std::move( viewVector ) );

  return StatusCode::SUCCESS;
}


} //> end namespace AthViews
