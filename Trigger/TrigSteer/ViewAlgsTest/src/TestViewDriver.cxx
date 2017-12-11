/*
  Copyright ( C ) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include "CxxUtils/make_unique.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "AthContainers/ConstDataVector.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "AthViews/ViewHelper.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"


#include "./SchedulerProxy.h"
#include "./TestViewDriver.h"

enum RoIsInView { BareRoIDescriptor = 0, CollectionWithOneElement = 1, CollectionWithAllElements = 2, SuperRoI=3 };

TestViewDriver::TestViewDriver( const std::string& name, ISvcLocator* pSvcLocator )
  : AthAlgorithm( name, pSvcLocator ) {
}

StatusCode TestViewDriver::initialize( ) {
  CHECK( m_scheduler.retrieve( ) );
  CHECK( m_roisContainer.initialize( ) );
  CHECK( m_roisViewOutput.initialize( ) );
  m_clustersViewInputHandle.assign( m_clustersViewInput );
  CHECK( m_clustersViewInputHandle.initialize( ) );
  CHECK( m_views.initialize( ) );
  return StatusCode::SUCCESS;
}

StatusCode TestViewDriver::execute( ) {
  auto roisContainer = SG::makeHandle( m_roisContainer );

  //  this code is to be replaced by set of tools which can customize what is placed in the view
  auto contexts = std::vector<EventContext>( );
  auto viewVector = std::make_unique<std::vector<SG::View*>>( );
  unsigned int viewCounter = 0;
  unsigned int conditionsRun = getContext().getExtension<Atlas::ExtendedEventContext>()->conditionsRun();
  for ( const auto roi: *roisContainer.cptr( ) ) {

    contexts.push_back( getContext( ) );
    viewVector->push_back( ViewHelper::makeView( name( )+"_view", viewCounter++ ) );
    contexts.back( ).setExtension( Atlas::ExtendedEventContext( viewVector->back( ),
                                                                conditionsRun));


    auto oneRoIColl = std::make_unique< ConstDataVector<TrigRoiDescriptorCollection> >( );
    oneRoIColl->clear( SG::VIEW_ELEMENTS ); //Don't delete the RoIs
    oneRoIColl->push_back( roi );
    auto handle = SG::makeHandle( m_roisViewOutput, contexts.back( ) );
    handle.setProxyDict( viewVector->back( ) );
    CHECK( handle.record( std::move( oneRoIColl ) ) );
    ATH_MSG_DEBUG( "Placed RoICollection with a single RoI " <<  *roi << " in the view " << viewVector->back( )->impl( )->name( ) );
  } 


  // Run the views
  CHECK( ViewHelper::ScheduleViews( *viewVector,
                                    m_viewNodeName,
                                    getContext(),
                                    m_scheduler.get() ) );

  ATH_MSG_DEBUG( "Execution in " << viewVector->size( ) << " Views performed" );
  
  // Harvest the results into a merged collection - currently impossible due to issue with TrigComposite
  //  auto outputClusterContainer = std::make_unique< TestClusterContainer >( );
  //  auto outputClusterContainerAux = std::make_unique< TestClusterAuxContainer>( );
  //  outputClusterContainer->setStore( outputClusterContainerAux.get( ) );


  for ( auto& viewCtx: contexts ) {
    auto handle = SG::makeHandle( m_clustersViewInputHandle, viewCtx );
    if ( not handle.isValid( ) ) {
      ATH_MSG_ERROR( "Can not bind handle to a view context" );
      return StatusCode::FAILURE;
    }
    for ( auto cluster: *handle.get( ) ) {
      ATH_MSG_DEBUG( "Cluster of ET " << TestEDM::getClusterEt( cluster ) );
      //outputClusterContainer->push_back(cluster);  // FIXME this is not as simple, we need some trick to do copy
    }
  }

  // for now we are not outputting
  //  {
  //    auto handle = SG::makeHandle( m_outputClusterContainer );
  //    CHECK( handle.record( std::move( outputClusterContainer ), std::move( outputClusterContainerAux ) ) );
  //  }
  // Store the views for re-use/book-keeping
  {
    auto handle = SG::makeHandle( m_views );
    CHECK( handle.record( std::move( viewVector ) ) );
    ATH_MSG_DEBUG( "Recorded views under the key: " << m_views.key() );
  }

  return StatusCode::SUCCESS;
}

