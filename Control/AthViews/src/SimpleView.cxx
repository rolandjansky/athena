/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <stdexcept>
#include "AthViews/SimpleView.h"
#include "AthViews/View.h"
#include "SGTools/transientKey.h"

SimpleView::SimpleView( std::string Name, bool AllowFallThrough, std::string const& storeName ) :
  m_store( storeName, Name ),
  m_roi(),
  m_name( Name ),
  m_allowFallThrough( AllowFallThrough )
{
}

SimpleView::~SimpleView()
{
}


/**
 * @brief Construct a key as used in the parent store.
 * @brief key The key as used in the view.
 */
std::string SimpleView::viewKey (const std::string& key) const
{
  return SG::transientKey (m_name + "_" + key);
}


void SimpleView::linkParent( const IProxyDict* parent ) {
  auto castParent = dynamic_cast< const SG::View* >( parent );
  if ( castParent ) {
    m_parents.insert( castParent );
  }
  else {
    throw std::runtime_error( "Unable to link parent view that cannot be cast to SG::View" );
  }
}


/**
 * @brief Get proxy given a hashed key+clid.
 * @param sgkey Hashed key to look up.
 *
 * Find an exact match; no handling of aliases, etc.
 * Returns 0 to flag failure.
 */
SG::DataProxy * SimpleView::proxy_exact( SG::sgkey_t /*sgkey*/ ) const
{
  return nullptr;
}


/**
 * @brief Get proxy with given id and key.
 * @param id The @c CLID of the desired object.
 * @param key The key of the desired object.
 *
 * If the key is a null string, then it is a @em default key.
 * Finding a proxy via the default key should succeed only if there
 * is exactly one object with the given @c CLID in the store.
 * Finding a proxy via a default key is considered deprecated
 * for the case of the event store.
 * 
 * Returns 0 to flag failure
 */
SG::DataProxy * SimpleView::proxy( const CLID& id, const std::string& key ) const
{
  return findProxy( id, key, m_allowFallThrough );
}

SG::DataProxy * SimpleView::findProxy( const CLID& id, const std::string& key, const bool allowFallThrough ) const
{
  auto isValid = [](const SG::DataProxy* p) { return p != nullptr and p->isValid(); };
  auto localProxy = m_store->proxy( id, viewKey(key) );
  if ( isValid( localProxy ) ) {
    return localProxy;
  }

  for ( auto parent: m_parents ) {
    // Don't allow parents to access whole-event store independently of this view
    auto inParentProxy = parent->impl()->findProxy( id, key, false );
    if ( isValid( inParentProxy ) ) {
      return inParentProxy;
    }
  }
  
  //Look in the default store if cound not find in any view - for instance for event-wise IDCs
  if ( (not isValid( localProxy )) and allowFallThrough ) {

    //Apply filtering
    if ( m_fallFilter.size() ) {
      bool filterPass = false;

      //Filter passes if the key contains one of the possible values
      for ( auto& entry : m_fallFilter ) {
        if ( key.find( entry ) != std::string::npos ) {
          filterPass = true;
          break;
        }
      }

      if ( !filterPass ) return nullptr;
    }

    return m_store->proxy( id, key );
  }
  return nullptr;
}


/**
 * @brief Get a proxy referencing a given transient object.
 * @param pTransient The object to find.
 * 
 * Returns 0 to flag failure
 */
SG::DataProxy * SimpleView::proxy( const void* const pTransient ) const
{
  throw std::runtime_error( "Not implemented: SimpleView::proxy" );
  return m_store->proxy( pTransient );
}


/**
 * @brief Return the list of all current proxies in store.
 */
std::vector< const SG::DataProxy* > SimpleView::proxies() const
{
  return m_store->proxies();
}


/**
 * @brief Add a new proxy to the store.
 * @param id CLID as which the proxy should be added.
 * @param proxy The proxy to add.
 *
 * Simple addition of a proxy to the store.  The key is taken as the
 * primary key of the proxy.  Does not handle things
 * like overwrite, history, symlinks, etc.  Should return failure
 * if there is already an entry for this clid/key.
 */
StatusCode SimpleView::addToStore( CLID id, SG::DataProxy * proxy )
{
  return m_store->addToStore( id, proxy );
}


/**
 * @brief Test to see if the target of an ElementLink has moved.
 * @param sgkey_in Original hashed key of the EL.
 * @param index_in Original index of the EL.
 * @param sgkey_out[out] New hashed key for the EL.
 * @param index_out[out] New index for the EL.
 * @return True if there is a remapping; false otherwise.
 *
 * The default implementation here always returns false.
 */
bool SimpleView::tryELRemap( sgkey_t sgkey_in, size_t index_in, sgkey_t & sgkey_out, size_t & index_out )
{
  throw std::runtime_error( "Not implemented: SimpleView::tryELRemap" );
  return m_store->tryELRemap( sgkey_in, index_in, sgkey_out, index_out );
}

/**
 * @brief Record an object in the store.
 * @param obj The data object to store.
 * @param key The key as which it should be stored.
 * @param allowMods If false, the object will be recorded as const.
 *
 * Full-blown record.  @c obj should usually be something
 * deriving from @c SG::DataBucket.
 *
 * Returns the proxy for the recorded object; nullptr on failure.
 */
SG::DataProxy * SimpleView::recordObject( SG::DataObjectSharedPtr<DataObject> obj, const std::string& key, bool allowMods, bool returnExisting )
{
  return m_store->recordObject( obj, viewKey(key), allowMods, returnExisting );
}

/**
 * @brief Tell the store that a handle has been bound to a proxy.
 * @param handle The handle that was bound.
 * The default implementation does nothing.
 */
void SimpleView::boundHandle( IResetable * handle )
{
  return m_store->boundHandle( handle );
}

/**
 * @brief Tell the store that a handle has been unbound from a proxy.
 * @param handle The handle that was unbound.
 * The default implementation does nothing.
 */
void SimpleView::unboundHandle( IResetable * handle )
{
  return m_store->unboundHandle( handle );
}

unsigned long SimpleView::addRef()
{
  throw std::runtime_error( "Not implemented: SimpleView::addRef" );
  return 0;
}
unsigned long SimpleView::release()
{
  throw std::runtime_error( "Not implemented: SimpleView::release" );
  return 0;
}
StatusCode SimpleView::queryInterface( const InterfaceID &/*ti*/, void** /*pp*/ )
{
  throw std::runtime_error( "Not implemented: SimpleView::queryInterface" );
  return StatusCode::FAILURE;
}
const std::string& SimpleView::name() const
{
  return m_name;
}

//IStringPool
IStringPool::sgkey_t SimpleView::stringToKey( const std::string& str, CLID clid )
{
  return m_store->stringToKey( viewKey(str), clid );
}
const std::string* SimpleView::keyToString( IStringPool::sgkey_t key ) const
{
  throw std::runtime_error( "Not implemented: SimpleView::keyToString" );
  return m_store->keyToString( key );
}
const std::string* SimpleView::keyToString( IStringPool::sgkey_t key, CLID& clid ) const
{
  throw std::runtime_error( "Not implemented: SimpleView::keyToString" );
  return m_store->keyToString( key, clid );
}
void SimpleView::registerKey( IStringPool::sgkey_t key, const std::string& str, CLID clid )
{
	m_store->registerKey( key, viewKey(str), clid );
}

void SimpleView::setROI(const ElementLink<TrigRoiDescriptorCollection>& roi) {
  m_roi = roi;
}

const ElementLink<TrigRoiDescriptorCollection>& SimpleView::getROI() const {
  return m_roi;
}

std::string SimpleView::dump( const std::string& indent ) const {

  // Dump view contents
  std::string ret = indent + "Dump " + name() + "\n";
  ret += indent + "[";
  for ( const SG::DataProxy* dp: proxies() ) {
    if ( dp->name().find( name() ) == 0 ) 
      ret += " " + dp->name();
  }
  ret += " ]\n";

  // Dump parent views
  if ( m_parents.size() ) ret += indent + "Parents:\n";
  for ( auto p : m_parents ) {
    ret += p->dump( indent + "  " );
  }

  // Fallthrough
  if ( indent == "" ) {
    if ( m_allowFallThrough ) {
      ret += indent + "May access main store: " + m_store->name();
    } else {
      ret += indent + "May not access main store";
    }
  }
  return ret;
}
