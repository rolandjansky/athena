/*
  General-purpose view creation algorithm <bwynne@cern.ch>

  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./EventViewCreatorAlgorithm.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "CxxUtils/make_unique.h"
#include "AthContainers/ConstDataVector.h"
#include "AthViews/ViewHelper.h"


EventViewCreatorAlgorithm::EventViewCreatorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthAlgorithm( name, pSvcLocator ),
    m_viewsKey( "EventViews" ),
    m_inputKey( "InputCollection" ),
    m_inputAuxKey( "InputCollectionAux" ),
    m_outputKey( "OutputCollection" ),
    m_viewAlgorithmPool( "ViewAlgPool" )
{
  //VarHandleKeys
  declareProperty( "OutputViewCollection", m_viewsKey, "Name of output event view collection" );
  declareProperty( "WithinViewCollection", m_outputKey, "Name of the collection to make inside views" );
  declareProperty( "InputCollection", m_inputKey, "Name of the collection to split into views" );
  declareProperty( "InputAuxCollection", m_inputAuxKey, "Name of the aux collection to split into views" );

  //Other properties
  declareProperty( "ViewAlgorithmNames", m_viewAlgorithmNames, "Names of algorithms to run in the views" );
  declareProperty( "ViewAlgorithmPool", m_viewAlgorithmPool, "Name of algorithm pool for views" );
}

StatusCode EventViewCreatorAlgorithm::initialize()
{
  //Initialize VarHandleKeys
  CHECK( m_viewsKey.initialize() );
  CHECK( m_outputKey.initialize() );
  CHECK( m_inputKey.initialize() );
  CHECK( m_inputAuxKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode EventViewCreatorAlgorithm::execute()
{
#ifdef GAUDI_SYSEXECUTE_WITHCONTEXT
  const EventContext& ctx = getContext();
#else
  const EventContext& ctx = *getContext();
#endif

  //Load input data
  SG::ReadHandle< xAOD::TrigCompositeContainer > inputHandle( m_inputKey, ctx );
  SG::ReadHandle< xAOD::TrigCompositeAuxContainer > inputAuxHandle( m_inputAuxKey, ctx );
  if ( !inputHandle.isValid() || !inputAuxHandle.isValid() )
  {
    ATH_MSG_ERROR( "Unable to load input data for view creation" );
    return StatusCode::FAILURE;
  }
  //CHECK( inputHandle->setStore( inputAuxHandle.ptr() ) );
  
  //Split into views
  std::vector< SG::View* > viewVector;
  std::vector< ConstDataVector<xAOD::TrigCompositeContainer> > viewCollections;
  for ( const auto input: *inputHandle.cptr() )
  {
    ConstDataVector<xAOD::TrigCompositeContainer> oneInput;
    oneInput.clear( SG::VIEW_ELEMENTS );
    oneInput.push_back( input );
    viewCollections.push_back( oneInput );
  }

  //Make the views
  SG::WriteHandle< ConstDataVector<xAOD::TrigCompositeContainer> > viewHandle( m_outputKey, ctx );
  CHECK( ViewHelper::MakeAndPopulate( name() + "_view",		// Base name for all views to use
                                      viewVector,		// Vector to store views
                                      viewHandle,		// A writehandle to use to access the views (the handle itself, not the contents)
                                      viewCollections ) );	// Data to initialise each view - one view will be made per entry

  // Run the views
  CHECK( ViewHelper::RunViews( viewVector,				// Vector containing views
                               m_viewAlgorithmNames,			// Algorithms to run in each view
                               ctx,					// Context to attach the views to
                               serviceLocator()->service( m_viewAlgorithmPool ) ) );	// Service to retrieve algorithms by name

  // Store the views for re-use/book-keeping
  SG::WriteHandle< std::vector< SG::View* > > viewsHandle( m_viewsKey, ctx );
  viewsHandle = CxxUtils::make_unique< std::vector< SG::View* > >( viewVector );

  return StatusCode::SUCCESS;
}

