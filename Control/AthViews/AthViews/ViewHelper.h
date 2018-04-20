/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHVIEWS_VIEWHELPER_HH
#define ATHVIEWS_VIEWHELPER_HH

#include "CxxUtils/make_unique.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IScheduler.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"
#include "AthViews/View.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/AuxElement.h"
#include "AthContainers/AuxStoreInternal.h"

#include "tbb/task.h"

namespace ViewHelper
{

  namespace impl {
    class SaveAndRestoreContext {
    public:
      SaveAndRestoreContext() : m_context(  Gaudi::Hive::currentContext() ) {}
      ~SaveAndRestoreContext() { Gaudi::Hive::setCurrentContext (m_context); }
    private:
      EventContext m_context;
    };
  }

  //Function to create a vector of views, each populated with one data object
  template< typename T >
  inline StatusCode MakeAndPopulate( std::string const& ViewNameRoot, ViewContainer & ViewVector,
				     SG::WriteHandleKey< T > const& PopulateKey, EventContext const& SourceContext, std::vector< T > const& InputData, bool const allowFallThrough=true )
  {
    //Make a WriteHandle to use
    SG::WriteHandle<T> populateHandle( PopulateKey, SourceContext );

    //Loop over all input data
    unsigned int const viewNumber = InputData.size();
    for ( unsigned int viewIndex = 0; viewIndex < viewNumber; ++viewIndex )
      {
	//Create view
	std::string viewName = ViewNameRoot + std::to_string( viewIndex );
	SG::View * outputView = new SG::View( viewName, allowFallThrough );
	ViewVector.push_back( outputView );

	//Attach a handle to the view
	StatusCode sc = populateHandle.setProxyDict( outputView );
	if ( !sc.isSuccess() )
	  {
	    ViewVector.clear();
	    return sc;
	  }

	//Populate the view
	sc = populateHandle.record( CxxUtils::make_unique< T >( InputData[ viewIndex ] ) );
	if ( !sc.isSuccess() )
	  {
	    ViewVector.clear();
	    return sc;
	  }
      }

    return StatusCode::SUCCESS;
  }

  //Function to add data to existing views
  template< typename T >
  inline StatusCode Populate( ViewContainer const& ViewVector,
			      SG::WriteHandleKey< T > const& PopulateKey, EventContext const& SourceContext, std::vector< T > const& InputData )
  {
    //Make a WriteHandle to use
    SG::WriteHandle<T> populateHandle( PopulateKey, SourceContext );

    //Vector length check
    unsigned int const viewNumber = InputData.size();
    if ( viewNumber != ViewVector.size() ) return StatusCode::FAILURE;

    //Loop over all input data
    for ( unsigned int viewIndex = 0; viewIndex < viewNumber; ++viewIndex )
      {
	//Attach the handle to the view
	StatusCode sc = populateHandle.setProxyDict( ViewVector.at( viewIndex ) );
	if ( !sc.isSuccess() )
	  {
	    return sc;
	  }

	//Populate the view
	sc = populateHandle.record( CxxUtils::make_unique< T >( InputData.at( viewIndex ) ) );
	if ( !sc.isSuccess() )
	  {
	    return sc;
	  }
      }

    return StatusCode::SUCCESS;
  }

  //Function to attach a set of views to a graph node
  inline StatusCode ScheduleViews( ViewContainer & ViewVector, std::string const& NodeName,
				   EventContext const& SourceContext, IScheduler * Scheduler )
  {
    //Prevent view nesting - test if source context has view attached
    Atlas::ExtendedEventContext const* extendedContext = SourceContext.template getExtension<Atlas::ExtendedEventContext>();
    if ( dynamic_cast< SG::View* >( extendedContext->proxy() ) )
      {
	return StatusCode::FAILURE;
      }

    //Retrieve the scheduler
    if ( !Scheduler )
      {
	return StatusCode::FAILURE;
      }

    if ( ViewVector.size() )
      {
	for ( SG::View* view : ViewVector )
	  {
	    //Make a context with the view attached
	    EventContext * viewContext = new EventContext( SourceContext );
	    viewContext->setExtension( Atlas::ExtendedEventContext( view, extendedContext->conditionsRun() ) );

	    //Attach the view to the named node
	    StatusCode sc = Scheduler->scheduleEventView( &SourceContext, NodeName, viewContext );
	    if ( !sc.isSuccess() )
	      {
		return StatusCode::FAILURE;
	      }
	  }
      }
    else
      {
	//Disable the node if no views
	return Scheduler->scheduleEventView( &SourceContext, NodeName, 0 );
      }

    return StatusCode::SUCCESS;
  }

  class ViewMerger {
  private:
    StoreGateSvc* m_sg;
    MsgStream& m_msg;
  public:
    ViewMerger(StoreGateSvc* sg, MsgStream& msg ) : m_sg( sg ), m_msg( msg ) {}
    
    //    MsgStream& msg(const MSG::Level lvl) const { return m_msg; }

    //Function merging view data into a single collection
    template< typename T >
    inline StatusCode mergeViewCollection( ViewContainer const& ViewVector, SG::ReadHandleKey< T > const& QueryKey, EventContext const& SourceContext, T & OutputData )
    {
      //Make a ReadHandle to use
      SG::ReadHandle<T> queryHandle( QueryKey, SourceContext );

      //Loop over all views
      for ( SG::View* inputView : ViewVector )
	{
	  //Attach the handle to the view
	  StatusCode sc = queryHandle.setProxyDict( inputView );
	  if ( !sc.isSuccess() )
	    {
	      m_msg << MSG::ERROR <<"Failed to use view " << inputView->name() << " to read " << queryHandle.key() << " resetting output" << endmsg;
	      OutputData.clear();
	      return sc;
	    }

	  //Merge the data
	  T inputData = *queryHandle;
	  OutputData.insert( OutputData.end(), inputData.begin(), inputData.end() );
	}

      return StatusCode::SUCCESS;
    }

    //Function merging view data into a single collection - overload for datavectors to do all the bookkeeping
    //Adds aux data element "viewIndex" to indicate which element of the merged collection came from where
    //Calls remap for ElementLinks so that they point to the merged collection
    template< typename T >
    inline StatusCode mergeViewCollection( ViewContainer const& ViewVector, SG::ReadHandleKey< DataVector< T > > const& QueryKey, EventContext const& SourceContext, DataVector< T > & OutputData )
    {
      //Check that there's a non-const aux store for output bookkeeping
      if ( !OutputData.getStore() )
	{
	  m_msg << MSG::ERROR << "output data does not have the store"  << endmsg;
	  return StatusCode::FAILURE;
	}

      //Make ReadHandle to access views
      SG::ReadHandle< DataVector< T > > queryHandle( QueryKey, SourceContext );

      //Make accessor for bookkeeping
      SG::AuxElement::Accessor< int > viewBookkeeper( "viewIndex" );

      //Loop over all views
      unsigned int offset = 0;
      for ( unsigned int viewIndex = 0; viewIndex < ViewVector.size(); ++viewIndex )
	{
	  SG::View * inputView = ViewVector.at( viewIndex );

	  //Attach the handle to the view
	  StatusCode sc = queryHandle.setProxyDict( inputView );
	  if ( !sc.isSuccess() )
	    {
	      m_msg << MSG::ERROR << "Failed to use view " << inputView->name() << " to read " << queryHandle.key() << " clearing output" << endmsg;
	      OutputData.clear();
	      return sc;
	    }

	  //Nothing to do for empty collections
	  if ( queryHandle->size() == 0 ) {
	    m_msg << MSG::DEBUG << "Empty collection " << queryHandle.key() <<" in a view " << inputView->name() <<endmsg;
	    continue;
	  }

	  //Merge the data
	  for ( const auto inputObject : *queryHandle.cptr() )
	    {
	      //Element-wise copy data
	      T * outputObject = new T();
	      OutputData.push_back( outputObject );
	      *outputObject = *inputObject;

	      //Add aux data for bookkeeping
	      viewBookkeeper( *outputObject ) = viewIndex;
	    }
	  m_msg << MSG::DEBUG << "Copied " << queryHandle->size() << " objects from collection in view  " << inputView->name() << endmsg;

	  //Declare remapping
	  m_sg->remap( ClassID_traits< DataVector< T > >::ID(), inputView->name() + "_" + queryHandle.name(), queryHandle.name(), offset );
	  offset += queryHandle->size();
	}

      return StatusCode::SUCCESS;
    }
  };

  /**
   * @arg unique_index - gets appended to the view name if >= 0
   */
  inline SG::View* makeView( const std::string& common_name, int const unique_index=-1, bool const allowFallThrough = true) {
    return  (( unique_index == -1 ) ?
	     new SG::View( common_name, allowFallThrough ) :
	     new SG::View( common_name+ " "+std::to_string(unique_index), allowFallThrough ) );

  }

  /**
   * @brief - records object in a view
   */
  template< typename T >
  StatusCode addToView( SG::View* view, const std::string& key, T* ptr ) {

    SG::WriteHandle< T > handle( key );

    if( handle.setProxyDict( view ).isFailure() ) {
      return StatusCode::FAILURE;
    }

    if ( handle.record( std::unique_ptr<T>( ptr ) ).isFailure() ) {
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  template< typename T >
  StatusCode addViewCollectionToView(SG::View* view, const std::string& key, const T* src ) {
    auto * viewColl = new ConstDataVector<T>();
    viewColl->clear( SG::VIEW_ELEMENTS ); //make it a view
    viewColl->insert( viewColl->end(), src->begin(), src->end() ); // copy content

    return addToView( view, key, viewColl );
  }


  /**
   * @return nullptr if object of type T is missing in the view
   */
  template< typename T >
  const T* getFromView( SG::View* view, const std::string& key) {

    SG::ReadHandle<T> handle(key);

    if ( handle.setProxyDict(view).isFailure() ) {
      return nullptr;
    }

    return handle.cptr();
  }

} // EOF namspace ViewHelper

#endif
