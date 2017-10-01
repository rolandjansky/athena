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
  ::AthAlgorithm( name, pSvcLocator ),
  m_trigRoIs( "input_rois" ),
  m_w_views( "all_views" ),
  m_viewRoIs( "output_rois" ),
  m_algorithmNameSequence( std::vector< std::string >() ),
  m_algPoolName( "" ),
  m_viewBaseName( "" )
{
  //
  // Property declaration
  // 

  declareProperty( "InputRoICollection", m_trigRoIs, "Collection of RoIs to split into views" );
  
  declareProperty( "OutputRoICollection", m_viewRoIs, "RoI collection to use inside views" );
  
  declareProperty( "AllViews", m_w_views, "All views" );

  declareProperty( "ViewBaseName", m_viewBaseName, "Name to use for all views - number will be appended" );

  declareProperty( "AlgorithmNameSequence", m_algorithmNameSequence, "Names of algorithms to run in the views" );

  declareProperty( "AlgPoolName", m_algPoolName, "Name for the algorithm pool service to use with the views" );
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
  if ( inputRoIs->empty() ) return StatusCode::SUCCESS;

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
  CHECK( ViewHelper::RunViews( viewVector,              //View vector
          m_algorithmNameSequence,                        //Algorithms to run in each view
          ctx,                                            //Context to attach the views to
          serviceLocator()->service( m_algPoolName ) ) ); //Service to retrieve algorithms by name

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
