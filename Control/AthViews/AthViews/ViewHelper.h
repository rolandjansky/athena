/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHVIEWS_VIEWHELPER_HH
#define ATHVIEWS_VIEWHELPER_HH

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
#include "CxxUtils/checker_macros.h"


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
  inline StatusCode MakeAndPopulate( std::string const& ViewNameRoot, ViewContainer* ViewVector,
                                     SG::WriteHandleKey< T > const& PopulateKey, EventContext const& SourceContext, std::vector< T > const& InputData, bool const allowFallThrough=true )
  {
    //Check for spaces in the name
    if ( ViewNameRoot.find( " " ) != std::string::npos )
    {
      return StatusCode::FAILURE;
    }

    //Make a WriteHandle to use
    SG::WriteHandle<T> populateHandle( PopulateKey, SourceContext );

    //Loop over all input data
    unsigned int const viewNumber = InputData.size();
    for ( unsigned int viewIndex = 0; viewIndex < viewNumber; ++viewIndex )
    {
      //Create view
      SG::View * outputView = new SG::View( ViewNameRoot, viewIndex, allowFallThrough );
      ViewVector->push_back( outputView );

      //Attach a handle to the view
      StatusCode sc = populateHandle.setProxyDict( outputView );
      if ( !sc.isSuccess() )
      {
        ViewVector->clear();
        return sc;
      }

      //Populate the view
      sc = populateHandle.record( std::make_unique< T >( InputData[ viewIndex ] ) );
      if ( !sc.isSuccess() )
      {
        ViewVector->clear();
        return sc;
      }
    }

    return StatusCode::SUCCESS;
  }

  inline StatusCode ScheduleSingleView( SG::View* view, std::string const& NodeName, EventContext const& SourceContext, Atlas::ExtendedEventContext extendedContext, SmartIF<IScheduler> Scheduler ) {

    //Make a context with the view attached
    auto viewContext = std::make_unique< EventContext >( SourceContext );
    if ( view->getROI().isValid() ) {
      Atlas::setExtendedEventContext (*viewContext,
                                      Atlas::ExtendedEventContext( view, extendedContext.conditionsRun(), *view->getROI() ) );
    } else {
      Atlas::setExtendedEventContext (*viewContext,
                                      Atlas::ExtendedEventContext( view, extendedContext.conditionsRun() ) );
    }

    //Attach the view to the named node
    StatusCode sc = Scheduler->scheduleEventView( &SourceContext, NodeName, std::move( viewContext ) );
    if ( !sc.isSuccess() ) {
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  //Function to attach a set of views to a graph node
  inline StatusCode ScheduleViews( ViewContainer * ViewVector, std::string const& NodeName,
                                   EventContext const& SourceContext, SmartIF<IScheduler> Scheduler, bool reverseOrder = false  )
  {
    //Prevent view nesting - test if source context has view attached
    if ( Atlas::hasExtendedEventContext (SourceContext) ) {
      if ( dynamic_cast< SG::View* >( Atlas::getExtendedEventContext (SourceContext).proxy() ) ) {
        return StatusCode::FAILURE;
      }
    }
    // Atlas::ExtendedEventContext const* extendedContext = Atlas::getExtendedEventContext(SourceContext);
    // if ( dynamic_cast< SG::View* >( extendedContext->proxy() ) )
    // {
    //   return StatusCode::FAILURE;
    // }

    //Retrieve the scheduler
    if ( !Scheduler )
    {
      return StatusCode::FAILURE;
    }

    if ( not ViewVector->empty() )
    {
      Atlas::ExtendedEventContext const extendedContext =
        Atlas::getExtendedEventContext (SourceContext);
      if ( reverseOrder ) {
	for ( auto iter = ViewVector->rbegin(); iter != ViewVector->rend(); ++iter ) {
	  if ( ScheduleSingleView( *iter, NodeName, SourceContext, extendedContext, Scheduler ).isFailure() ) {
	    return StatusCode::FAILURE;
	  }
	}
      } else {
	for ( SG::View* view : *ViewVector ) {
	  if( ScheduleSingleView( view, NodeName, SourceContext, extendedContext, Scheduler ).isFailure() ) {
	    return StatusCode::FAILURE;
	  }
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
      SG::AuxElement::Accessor< ElementLink<TrigRoiDescriptorCollection> > viewBookkeeper( "viewIndex" );

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
        if ( queryHandle->size() == 0 )
        {
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
          viewBookkeeper( *outputObject ) = inputView->getROI();
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
  inline SG::View* makeView( const std::string& common_name, int const unique_index=-1, bool const allowFallThrough = true)
  {
    //Check for spaces in the name
    if ( common_name.find( " " ) != std::string::npos )
    {
      return nullptr;
    }

    return new SG::View( common_name, unique_index, allowFallThrough );
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

  /**
   * navigate from the TrigComposite to nearest view and fetch object from it
   * @return handle (can be invalid)
   */


  template<typename T>
  SG::ReadHandle<T> makeHandle( const SG::View* view , const SG::ReadHandleKey<T>& rhKey, const EventContext& context ) {

    SG::View* nview ATLAS_THREAD_SAFE = const_cast<SG::View*>(view);  // we need it until reading from const IProxyDict is not supported

    auto handle = SG::makeHandle( rhKey, context );
    if ( handle.setProxyDict( nview ).isFailure() ) { // we ignore it besause the handle will be invalid anyways if this call is unsuccesfull
      throw std::runtime_error("Can't make ReadHandle of key " + rhKey.key() + " type " + ClassID_traits<T>::typeName() + " in view " + view->name() );
    }
    return handle;
  }


  /**
   * Create EL to a collection in view
   * @warning no checks are made as to the validity of the created EL
   */

  template<typename T>
  ElementLink<T> makeLink(const SG::View* view, const SG::ReadHandle<T>& handle, size_t index ) {
    return ElementLink<T>( view->name()+"_"+handle.key(), index );
  }

} // EOF namspace ViewHelper

#endif
