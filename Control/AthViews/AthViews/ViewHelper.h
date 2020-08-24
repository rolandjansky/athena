/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHVIEWS_VIEWHELPER_HH
#define ATHVIEWS_VIEWHELPER_HH

#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IScheduler.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"
#include "AthViews/View.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/AuxElement.h"


namespace ViewHelper
{
  //Function to create a vector of views, each populated with one data object
  template< typename T >
  inline StatusCode makeAndPopulate( std::string const& viewNameRoot, ViewContainer* viewVector,
                                     SG::WriteHandleKey< T > const& populateKey, EventContext const& sourceContext,
                                     std::vector< T > const& inputData, bool const allowFallThrough = true )
  {
    //Check for spaces in the name
    if ( viewNameRoot.find( " " ) != std::string::npos )
    {
      return StatusCode::FAILURE;
    }

    //Make a WriteHandle to use
    SG::WriteHandle<T> populateHandle( populateKey, sourceContext );

    //Loop over all input data
    unsigned int const viewNumber = inputData.size();
    for ( unsigned int viewIndex = 0; viewIndex < viewNumber; ++viewIndex )
    {
      //Create view
      SG::View * outputView = new SG::View( viewNameRoot, viewIndex, allowFallThrough );
      viewVector->push_back( outputView );

      //Attach a handle to the view
      StatusCode sc = populateHandle.setProxyDict( outputView );
      if ( !sc.isSuccess() )
      {
        viewVector->clear();
        return sc;
      }

      //Populate the view
      sc = populateHandle.record( std::make_unique< T >( inputData[ viewIndex ] ) );
      if ( !sc.isSuccess() )
      {
        viewVector->clear();
        return sc;
      }
    }

    return StatusCode::SUCCESS;
  }

  //Function to pass a single view to the scheduler
  inline StatusCode scheduleSingleView( SG::View* view, std::string const& nodeName, EventContext const& sourceContext,
                                        EventIDBase::number_type conditionsRun, SmartIF<IScheduler> scheduler )
  {
    //Make a new context with the view attached
    auto viewContext = std::make_unique< EventContext >( sourceContext );
    if ( view->getROI().isValid() ) {
      Atlas::setExtendedEventContext( *viewContext,
                                      Atlas::ExtendedEventContext( view, conditionsRun, *view->getROI() ) );
    } else {
      Atlas::setExtendedEventContext( *viewContext,
                                      Atlas::ExtendedEventContext( view, conditionsRun ) );
    }

    //Attach the view to the named node
    return scheduler->scheduleEventView( &sourceContext, nodeName, std::move( viewContext ) );
  }

  //Function to attach a set of views to a graph node
  inline StatusCode scheduleViews( ViewContainer * viewVector, std::string const& nodeName,
                                   EventContext const& sourceContext, SmartIF<IScheduler> scheduler, bool reverseOrder = false  )
  {
    //Require the information of the extended context (should always be there, but check anyway)
    if ( !Atlas::hasExtendedEventContext( sourceContext ) ) {
      return StatusCode::FAILURE;
    }
    Atlas::ExtendedEventContext const extendedContext = Atlas::getExtendedEventContext( sourceContext );

    //Prevent view nesting - test if source context has view attached
    if ( dynamic_cast< SG::View* >( extendedContext.proxy() ) ) {
      return StatusCode::FAILURE;
    }

    //Retrieve the scheduler
    if ( !scheduler ) {
      return StatusCode::FAILURE;
    }

    if ( viewVector->empty() ) {

      //Disable the node if no views
      return scheduler->scheduleEventView( &sourceContext, nodeName, 0 );
    }
    else {
      if ( reverseOrder ) {
        for ( auto iter = viewVector->rbegin(); iter != viewVector->rend(); ++iter ) {

          //Schedule each view in reverse (should make no difference, just a debugging tool)
          if ( scheduleSingleView( *iter, nodeName, sourceContext,
                                   extendedContext.conditionsRun(), scheduler ).isFailure() ) {
            return StatusCode::FAILURE;
          }
        }
      }
      else {
        for ( SG::View* view : *viewVector ) {

          //Schedule each view
          if ( scheduleSingleView( view, nodeName, sourceContext,
                                   extendedContext.conditionsRun(), scheduler ).isFailure() ) {
            return StatusCode::FAILURE;
          }
        }
      }
    }

    return StatusCode::SUCCESS;
  }

  class ViewMerger {
  private:
    StoreGateSvc* m_sg;
    MsgStream& m_msg;
  public:
    ViewMerger(StoreGateSvc* sg, MsgStream& msg ) : m_sg( sg ), m_msg( msg ) {}

    //Function merging view data into a single collection
    template< typename T >
    inline StatusCode mergeViewCollection( ViewContainer const& viewVector, SG::ReadHandleKey< T > const& queryKey, EventContext const& sourceContext, T & outputData )
    {
      //Make a ReadHandle to use
      SG::ReadHandle<T> queryHandle( queryKey, sourceContext );

      //Loop over all views
      for ( SG::View* inputView : viewVector )
      {
        //Attach the handle to the view
        StatusCode sc = queryHandle.setProxyDict( inputView );
        if ( !sc.isSuccess() )
        {
          m_msg << MSG::ERROR <<"Failed to use view " << inputView->name() << " to read " << queryHandle.key() << " resetting output" << endmsg;
          outputData.clear();
          return sc;
        }

        //Merge the data
        T inputData = *queryHandle;
        outputData.insert( outputData.end(), inputData.begin(), inputData.end() );
      }

      return StatusCode::SUCCESS;
    }

    //Function merging view data into a single collection - overload for datavectors to do all the bookkeeping
    //Adds aux data element "viewIndex" to indicate which element of the merged collection came from where
    //Calls remap for ElementLinks so that they point to the merged collection
    template< typename T >
    inline StatusCode mergeViewCollection( ViewContainer const& viewVector, SG::ReadHandleKey< DataVector< T > > const& queryKey, EventContext const& sourceContext, DataVector< T > & outputData )
    {
      //Check that there's a non-const aux store for output bookkeeping
      if ( !outputData.getStore() )
      {
        m_msg << MSG::ERROR << "output data does not have the store"  << endmsg;
        return StatusCode::FAILURE;
      }

      //Make ReadHandle to access views
      SG::ReadHandle< DataVector< T > > queryHandle( queryKey, sourceContext );

      //Make accessor for bookkeeping
      SG::AuxElement::Accessor< ElementLink< TrigRoiDescriptorCollection > > viewBookkeeper( "viewIndex" );

      //Loop over all views
      unsigned int offset = 0;
      for ( unsigned int viewIndex = 0; viewIndex < viewVector.size(); ++viewIndex )
      {
        SG::View * inputView = viewVector.at( viewIndex );

        //Attach the handle to the view
        StatusCode sc = queryHandle.setProxyDict( inputView );
        if ( !sc.isSuccess() )
        {
          m_msg << MSG::ERROR << "Failed to use view " << inputView->name() << " to read " << queryHandle.key() << " clearing output" << endmsg;
          outputData.clear();
          return sc;
        }

        //Nothing to do for empty collections
        if ( queryHandle->size() == 0 )
        {
          if (m_msg.level() <= MSG::DEBUG) {
            m_msg << MSG::DEBUG << "Empty collection " << queryHandle.key() <<" in a view " << inputView->name() <<endmsg;
            continue;
          }
        }

        //Merge the data
        for ( const auto inputObject : *queryHandle.cptr() )
        {
          //Element-wise copy data (looks a bit weird, but necessary for xAOD objects)
          T * outputObject = new T();
          outputData.push_back( outputObject );
          *outputObject = *inputObject;

          //Add aux data for bookkeeping
          viewBookkeeper( *outputObject ) = inputView->getROI();
        }

        if (m_msg.level() <= MSG::DEBUG) {
          m_msg << MSG::DEBUG << "Copied " << queryHandle->size() << " objects from collection in view  " << inputView->name() << endmsg;
        }

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
  inline SG::View* makeView( const std::string& common_name, int const unique_index = -1, bool const allowFallThrough = true )
  {
    //Check for spaces in the name
    if ( common_name.find( " " ) != std::string::npos )
    {
      return nullptr;
    }

    return new SG::View( common_name, unique_index, allowFallThrough );
  }


  /**
   * navigate from the TrigComposite to nearest view and fetch object from it
   * @return handle (can be invalid)
   */
  template<typename T>
  SG::ReadHandle<T> makeHandle( const SG::View* view , const SG::ReadHandleKey<T>& rhKey, const EventContext& context )
  {
    SG::View* nview ATLAS_THREAD_SAFE = const_cast< SG::View* >( view ); //We need it until reading from const IProxyDict is supported

    auto handle = SG::makeHandle( rhKey, context );
    if ( handle.setProxyDict( nview ).isFailure() ) {
      //We ignore it because the handle will be invalid anyway if this call is unsuccessful
      throw std::runtime_error( "Can't make ReadHandle of key " + rhKey.key() + " type " + ClassID_traits<T>::typeName() + " in view " + view->name() );
    }
    return handle;
  }


  /**
   * Create EL to a collection in view
   * @warning no checks are made as to the validity of the created EL
   */
  template<typename T>
  ElementLink<T> makeLink( const SG::View* view, const SG::ReadHandle<T>& handle, size_t index )
  {
    return ElementLink<T>( view->name() + "_" + handle.key(), index );
  }

} // EOF namspace ViewHelper

#endif
