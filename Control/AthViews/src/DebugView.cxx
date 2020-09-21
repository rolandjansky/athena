/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AthViews/DebugView.h"
#include "AthViews/View.h"
#include "AthenaKernel/getMessageSvc.h"


DebugView::DebugView( std::string Name, bool AllowFallThrough, std::string const& storeName ) :
  SimpleView( Name, AllowFallThrough, storeName ),
  AthMessaging( Athena::getMessageSvc(), Name )
{
}

DebugView::~DebugView()
{
  // Debugging info
  ATH_MSG_INFO( "Loaded via fallthrough from view " << m_name << ": " );
  for ( auto const& key : m_fallList ) ATH_MSG_INFO( key );

  ATH_MSG_INFO( "View " << m_name << " has " << m_parents.size() << " parents" );
}

void DebugView::linkParent( const IProxyDict* parent ) {

  // Check the parent is a view
  auto castParent = dynamic_cast< const SG::View* >( parent );
  if ( castParent ) {

    // Check for uniqueness
    auto result = m_parents.insert( castParent );
    if ( std::get<1>( result ) ) {
      ATH_MSG_INFO( "Adding parent view " << parent->name() << " to view " << name() << " with " << m_parents.size() - 1 << " existing parents" );
    }
    else {
      ATH_MSG_WARNING( "Attempted to add duplicate parent " << parent->name() << " to view " << name() );
    }
  }
  else {
    ATH_MSG_ERROR( "Unable to link parent view that cannot be cast to SG::View" );
  }
}

// Calling proxy locally can have allowFallThrough true
SG::DataProxy * DebugView::proxy( const CLID& id, const std::string& key ) const
{
  return findProxy( id, key, m_allowFallThrough );
}

SG::DataProxy * DebugView::findProxy( const CLID& id, const std::string& key, const bool allowFallThrough ) const
{
  auto isValid = [](const SG::DataProxy* p) { return p != nullptr and p->isValid(); };
  auto localProxy = m_store->proxy( id, viewKey(key) );
  if ( isValid( localProxy ) ) {
    return localProxy;
  }

  // Locate any parent views containing the data
  SG::DataProxy* parentProxy = nullptr;
  std::vector< const SG::View* > validParents;
  for ( auto& parent: m_parents ) {

    // Don't allow parents to access whole-event store independently of this view
    auto inParentProxy = parent->impl()->findProxy( id, key, false );

    // Record parents that contain the data
    if ( isValid( inParentProxy ) ) {

      // Warn if two different data objects conflict
      if ( parentProxy && ( parentProxy->object() != inParentProxy->object() ) ) {
        ATH_MSG_WARNING( "Parent view " << parent->name() << " contains conflicting data (" << inParentProxy->object() << ") vs " << parentProxy->object() << " found first" );
      }

      // Retrive the first data found
      if ( !parentProxy ) parentProxy = inParentProxy;

      // Make list of all parents returning data
      validParents.push_back( parent );
    }
  }

  // Return results from parent view search
  if ( parentProxy ) {

    // Warn if multiple parents returned result
    if ( validParents.size() > 1 ) {

      std::string validNames = "";
      for ( auto& parent : validParents ) validNames += parent->name() + " ";

      ATH_MSG_WARNING( "Key " << key << " in multiple parents (" << validParents.size() << "): " << validNames );
    }
    return parentProxy;
  }
  
  // Look in the default store if cound not find in any view - for instance for event-wise IDCs
  if ( (not isValid( localProxy )) and allowFallThrough ) {

    // Apply filtering
    if ( m_fallFilter.size() ) {
      bool filterPass = false;

      // Filter passes if the key contains one of the possible values
      for ( auto& entry : m_fallFilter ) {
        if ( key.find( entry ) != std::string::npos ) {
          filterPass = true;
          break;
        }
      }

      if ( !filterPass ) {
        ATH_MSG_INFO( "Key " << key << " not found in filter for view " << m_name << ":  returning 0" );
        return nullptr;
      }
    }

    auto mainStoreProxy = m_store->proxy( id, key );
    if ( mainStoreProxy ) {
      ATH_MSG_INFO( "Key " << key << " fell through to main store from view " << m_name << ":  returning " << mainStoreProxy );
      m_fallList.insert( key );
    }
    return mainStoreProxy;
  }
  return nullptr;
}

SG::DataProxy * DebugView::proxy( const void* const pTransient ) const
{
  throw std::runtime_error( "Not implemented: SimpleView::proxy" );
  return m_store->proxy( pTransient );
}

SG::DataProxy * DebugView::recordObject( SG::DataObjectSharedPtr<DataObject> obj, const std::string& key, bool allowMods, bool returnExisting )
{
  // Warn if fallthrough already happened for the object
  if ( m_fallList.find( key ) != m_fallList.end() ) {
    ATH_MSG_WARNING( "Key " << key << " was recorded to view " << m_name << " but was previously retrieved via fallthrough" );
  }

  return m_store->recordObject( obj, viewKey(key), allowMods, returnExisting );
}
