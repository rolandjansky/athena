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



TestViewDriver::TestViewDriver(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_roisContainer("RoIsContainer"),
    m_views("Views"), 
    // will use them in future to copy the output
    m_outputClusterContainer("OutputClusters"),
    m_outputClusterContainerAux("OutputClusterAux.")
    //m_outputProxyContainer("Output"), 
    //m_outputProxyContainerAux("OutputAux.") 
{
   
  declareProperty("RoIsContainer", m_roisContainer, "Input RoIs");
  declareProperty("Views", m_views, "Name of the generated view" );

  declareProperty("OutputClusterContainer", m_outputClusterContainer, "Output collection for clusters");
  declareProperty("OutputClusterContainerAux", m_outputClusterContainerAux, "");
  // declareProperty("OutputProxyContainer", m_outputProxyContainer, "Output proxies - this is returned by each fex and can be used to access actual objects");
  // declareProperty("OutputProxyContainerAux", m_outputProxyContainerAux, "");

}

StatusCode TestViewDriver::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode TestViewDriver::execute() {

  if ( not m_roisContainer.isValid() ) {
    ATH_MSG_ERROR("No decisions object from previous stage");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Running on " << m_roisContainer->size() << " RoIs");

  // Create a temporary WriteHandle to populate the views (this should maybe be done with a configurable handle instead)
  SG::WriteHandle< ConstDataVector<TrigRoiDescriptorCollection> > roisForTheView("RegionOfReco");

  // Divide the RoIs into a vector of single-element collections, one for each view
  std::vector< ConstDataVector<TrigRoiDescriptorCollection> > roiCollections;
  for ( const auto roi: *m_roisContainer.cptr() ) {
    ConstDataVector<TrigRoiDescriptorCollection> oneRoI;
    oneRoI.clear(SG::VIEW_ELEMENTS); //Don't delete the RoIs
    oneRoI.push_back( roi );
    roiCollections.push_back( oneRoI );
  }

  // Create the views and populate them
  std::vector<SG::View*> viewVector;
  ViewHelper::MakeAndPopulate( name()+"_view",		// Base name for all views to use
		  		viewVector,		// Vector to store views
				roisForTheView,		// A writehandle to use to access the views (the handle itself, not the contents)
				roiCollections );	// Data to initialise each view - one view will be made per entry

  // Specify algorithms to run in views (data flow info not available yet)
  std::vector< std::string > algorithmNameSequence = { "algInView" };

  // Run the views
  ViewHelper::RunViews( viewVector,				// Vector to store views
		  	algorithmNameSequence,			// Algorithms to run in each view
			Gaudi::Hive::currentContext(),		// Context to attach the views to
			serviceLocator()->service( "ViewAlgPool" ) );	// Service to retrieve algorithms by name (should make the service name configurable)

  // harvest the results into a merged collection
  m_outputClusterContainer = CxxUtils::make_unique< TestClusterContainer >();
  m_outputClusterContainerAux = CxxUtils::make_unique< TestClusterAuxContainer>();
  m_outputClusterContainer->setStore(m_outputClusterContainerAux.ptr());

  // Temporary ReadHandles to access the views
  SG::ReadHandle< TestClusterContainer > viewClusters("Clusters");
  ViewHelper::MergeViewCollection( viewVector,		//Vector to store views
		  		viewClusters,
				*m_outputClusterContainer );


  /*for ( auto context: subEvents ) {
    SG::ReadHandle<TestClusterContainer> r("Clusters");
    CHECK(r.setProxyDict(context.proxy()));

    for ( auto cl: *r.cptr() ) {
      ATH_MSG_DEBUG("Pulling cluster from the view of Et " << TestEDM::getClusterEt(cl) );
    }

    // how to copy the objects to the output?
    // in principle the container from the view should become a view of what is moved to the per-event container
    // the code below shoudl be working .... however it fails because of constnes issue    
    
    //    const size_t sizeSoFar = m_outputClusterContainer->size();
    //    m_outputClusterContainer->resize( sizeSoFar + r.cptr()->size() );
    //    swap(r.ptr()->begin(), r.ptr()->end(), m_outputClusterContainer->begin()+ sizeSoFar);
    
    //for ( auto el: *r.cptr() ) 
    //      m_outputClusterContainer.cptr()->push_back(el);
  }*/

   for ( auto cluster: *m_outputClusterContainer.cptr() ) {
     ATH_MSG_DEBUG("Cluster of ET " << TestEDM::getClusterEt(cluster) );
   }

   // Store the views for re-use/book-keeping
   m_views = CxxUtils::make_unique< std::vector<SG::View*>>( viewVector );

  return StatusCode::SUCCESS;
}

