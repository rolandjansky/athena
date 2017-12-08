/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RoiCollectionToViews.h"
#include "AthViews/ViewHelper.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "CxxUtils/make_unique.h"
#include "AthContainers/ConstDataVector.h"

namespace AthViews {

///////////////////////////////////////////////////////////////////
// Public methods: 
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
RoiCollectionToViews::RoiCollectionToViews( const std::string& name,
                      ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{
}

// Destructor
///////////////
RoiCollectionToViews::~RoiCollectionToViews()
{
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode RoiCollectionToViews::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  CHECK( m_trigRoIs.initialize() );
  CHECK( m_viewRoIs.initialize() );
  CHECK( m_w_views.initialize() );
  CHECK( m_scheduler.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode RoiCollectionToViews::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode RoiCollectionToViews::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

#ifdef GAUDI_SYSEXECUTE_WITHCONTEXT 
  const EventContext& ctx = getContext();
#else
  const EventContext& ctx = *getContext();
#endif

  //Load the collection of RoI descriptors
  SG::ReadHandle< TrigRoiDescriptorCollection > inputRoIs( m_trigRoIs, ctx );
  ATH_CHECK( inputRoIs.isValid() );
 
  //Skip if there's nothing to do
  //if ( inputRoIs->empty() ) return StatusCode::SUCCESS;
  //NB: don't do this any more. Calling the helper with an empty collection is now correct
  //as this will deactivate the node in the scheduler

  std::vector< ConstDataVector<TrigRoiDescriptorCollection> > outputRoICollectionVector;
  for ( auto roi: *inputRoIs )
  {
    ATH_MSG_DEBUG( "RoI Eta: " << roi->eta() << " Phi: " << roi->phi() << " RoIWord: " << roi->roiWord() );
    
    ConstDataVector<TrigRoiDescriptorCollection> oneRoIColl (SG::VIEW_ELEMENTS);
    oneRoIColl.push_back( roi );
    outputRoICollectionVector.push_back( std::move(oneRoIColl) );
  }

  //Create the views and populate them
  std::vector< SG::View* > viewVector;
  SG::WriteHandle< ConstDataVector<TrigRoiDescriptorCollection> > outputRoIs( m_viewRoIs, ctx );
  CHECK( ViewHelper::MakeAndPopulate( m_viewBaseName, //Base name for all views to use
          viewVector,                                 //Vector to store views
          outputRoIs,                                 //A writehandle to use to access the views (the handle itself, not the contents)
          outputRoICollectionVector,                  //Data to initialise each view - one view will be made per entry
          m_viewFallThrough ) );                      //Allow fall through from view to storegate

  //Run the algorithms in views
  if ( m_algorithmNameSequence.empty() )
  {
    CHECK( ViewHelper::ScheduleViews( viewVector, //View vector
            m_viewNodeName,                       //CF node to attach views to
            ctx,                                  //Context to attach the views to
            m_scheduler.get() ) );                //Scheduler
  }
  else
  {
    ATH_MSG_WARNING( "This method of EventView scheduling (specifying algorithm names) is DEPRECATED" );
    ATH_MSG_WARNING( "Please use the scheduler EventView handling by specifying a CF node name" );
    CHECK( ViewHelper::RunViews( viewVector,                //View vector
            m_algorithmNameSequence,                        //CF node to attach views to
            ctx,                                            //Context to attach the views to
            serviceLocator()->service( m_algPoolName ) ) ); //Service to retrieve algorithms by name
  }

  //Store the collection of views
  SG::WriteHandle< std::vector< SG::View* > > outputViewHandle( m_w_views, ctx );
  outputViewHandle.record( CxxUtils::make_unique< std::vector< SG::View* > >( viewVector ) );

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

} //> end namespace AthViews
