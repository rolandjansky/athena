/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include "CxxUtils/make_unique.h"
#include "AthContainers/ConstDataVector.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "AthViews/ViewHelper.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"


#include "./SchedulerProxy.h"
#include "./TestViewDriver.h"

enum RoIsInView { BareRoIDescriptor = 0, CollectionWithOneElement = 1, CollectionWithAllElements = 2, SuperRoI=3 };

TestViewDriver::TestViewDriver(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_roisContainer("RoIsContainer"),
    m_roisViewOutput("RoIsViewOutput"),
    m_views("Views"), 
    m_outputClusterContainer("OutputClusters"),
    m_viewAlgorithmsPool("AlgResourcePool/ViewAlgPool", name) {
   
  declareProperty("RoIsContainer", m_roisContainer, "Input RoIs");
  declareProperty("RoIsViewOutput", m_roisViewOutput, "Name of the collection that will be placed in each view");
  declareProperty("Views", m_views, "Name of the generated view" );
  declareProperty("OutputClusterContainer", m_outputClusterContainer, "Output collection for clusters");

  declareProperty( "ViewAlgorithmsPool", m_viewAlgorithmsPool);
  declareProperty( "ViewAlgorithmNames", m_viewAlgorithmNames, "Names of algorithms to run in the views" );
  declareProperty("RoITypeInViews", m_roITypeInViews = 1, "0 - place TrigRoiDesciptor in views, 1 - place Collections wiht single RoI, 2 - place entrie collection in the view, 3 - place SuperRoI in single view ");
}

StatusCode TestViewDriver::initialize() {
  CHECK( m_viewAlgorithmsPool.retrieve() );
  CHECK( m_roisContainer.initialize() );
  CHECK( m_roisViewOutput.initialize() );
  CHECK( m_views.initialize() );
  CHECK( m_outputClusterContainer.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode TestViewDriver::execute() {
  auto roisContainer = SG::makeHandle(m_roisContainer);

  //  this code is to be replaced by set of tools which are loaded depending on the way the RoI need to be placed in the views


  auto contexts = std::vector<EventContext>();
  auto viewVector = std::make_unique<std::vector<SG::View*>>();
  unsigned int viewCounter = 0;
  for ( const auto roi: *roisContainer.cptr() ) {

    contexts.push_back( getContext() );    
    viewVector->push_back( ViewHelper::makeView( name()+"_view", viewCounter++) );
    contexts.back().setProxy( viewVector->back() );

    
    auto oneRoIColl = std::make_unique< ConstDataVector<TrigRoiDescriptorCollection> >();    
    // Divide the RoIs into a vector of single-element collections, one for each view
    //new ConstDataVector<TrigRoiDescriptorCollection>;
    oneRoIColl->clear(SG::VIEW_ELEMENTS); //Don't delete the RoIs
    oneRoIColl->push_back( roi );
    auto handle = SG::makeHandle( m_roisViewOutput, newCtx );
    CHECK( handle.record( std::move( oneRoIColl ) ) );


    ATH_MSG_DEBUG("Placed RoICollection with a single RoI " <<  *roi << " in the view");
    // just an RoI descriptor in the view
  } 
  // missing is super RoI, and not covering the case when this alg can actually comsume more than on RoIs input collections

  
  // Run the views
  CHECK( ViewHelper::RunViews( contexts
			       m_viewAlgorithmNames,				// Algorithms to run in each view
			       serviceLocator()->service( "ViewAlgPool" ) ) );	//FIXME this should realy be service handle, else we do costly retrival each execution, needs api change of ViewHelper

  ATH_MSG_DEBUG("Execution in " << viewVector->size() << " Views performed");
  
  // Harvest the results into a merged collection - currently impossible due to issue with TrigComposite
  auto outputClusterContainer = std::make_unique< TestClusterContainer >();
  auto outputClusterContainerAux = std::make_unique< TestClusterAuxContainer>();
  outputClusterContainer->setStore(outputClusterContainerAux.get());
  /*CHECK( ViewHelper::MergeViewCollection( viewVector,
				viewClusters,
				*m_outputClusterContainer ) );*/

  // for ( auto view : *viewVector ) {
  //   const TestClusterContainer* viewClusters = ViewHelper::getFromView<TestClusterContainer>(view, "Clusters");
    
  //   for ( auto cl: *viewClusters ) {
  //     ATH_MSG_DEBUG("Pulling cluster from the view of Et " << TestEDM::getClusterEt(cl) );
  //   }

    // how to copy the objects to the output?
    // in principle the container from the view should become a view of what is moved to the per-event container
    // the code below shoudl be working .... however it fails because of constnes issue    
    
    //    const size_t sizeSoFar = m_outputClusterContainer->size();
    //    m_outputClusterContainer->resize( sizeSoFar + r.cptr()->size() );
    //    swap(r.ptr()->begin(), r.ptr()->end(), m_outputClusterContainer->begin()+ sizeSoFar);
    
    //for ( auto el: *r.cptr() ) 
    //      m_outputClusterContainer.cptr()->push_back(el);
  //  }

  for ( auto cluster: *outputClusterContainer.get() ) {
    ATH_MSG_DEBUG("Cluster of ET " << TestEDM::getClusterEt(cluster) );
  }
  {
    auto handle = SG::makeHandle(m_outputClusterContainer);
    CHECK( handle.record( std::move(outputClusterContainer), std::move( outputClusterContainerAux) ) );
  }
  // Store the views for re-use/book-keeping
  {
    auto handle = SG::makeHandle( m_views );
    CHECK( handle.record( std::move( viewVector ) ) );
  }

  return StatusCode::SUCCESS;
}

