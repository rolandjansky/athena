/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "AthViews/SimpleView.h"

using namespace std;

SimpleView::SimpleView() :
	m_store( "StoreGateSvc", "SimpleView" ),
	m_name( "SimpleView" )
{
}

SimpleView::SimpleView( std::string Name ) :
	m_store( "StoreGateSvc", "SimpleView" ),
	m_name( Name )
{
}

SimpleView::~SimpleView()
{
}

void SimpleView::linkParent( const IProxyDict* parent ) {
  m_parents.push_back( parent );
}


/**
 * @brief Get proxy given a hashed key+clid.
 * @param sgkey Hashed key to look up.
 *
 * Find an exact match; no handling of aliases, etc.
 * Returns 0 to flag failure.
 */
SG::DataProxy * SimpleView::proxy_exact( SG::sgkey_t sgkey ) const
{
	cout << "Not implemented: SimpleView::proxy_exact" << endl;
	//TODO - view rename
	return m_store->proxy_exact( sgkey );
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
	const std::string viewKey = m_name + "_" + key;
	auto dp =  m_store->proxy( id, viewKey );
	if ( dp != nullptr ) return dp;
	for ( auto parent: m_parents ) {
	  dp = parent->proxy( id, key );
	  if ( dp ) return dp;
	}
	return dp; // can be the nullptr still
}


/**
 * @brief Get a proxy referencing a given transient object.
 * @param pTransient The object to find.
 * 
 * Returns 0 to flag failure
 */
SG::DataProxy * SimpleView::proxy( const void* const pTransient ) const
{
	cout << "Not implemented: SimpleView::proxy" << endl;
	//TODO - view rename
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
	const std::string viewKey = m_name + "_" + proxy->name();
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
	cout << "Not implemented: SimpleView::tryELRemap" << endl;
	return m_store->tryELRemap( sgkey_in, index_in, sgkey_out, index_out ); //TODO
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
	const std::string viewKey = m_name + "_" + key;
	return m_store->recordObject( obj, viewKey, allowMods, returnExisting );
}

/**
 * @brief Inform HIVE that an object has been updated.
 * @param id The CLID of the object.
 * @param key The key of the object.
 */
StatusCode SimpleView::updatedObject( CLID id, const std::string& key )
{
	const std::string viewKey = m_name + "_" + key;
	return m_store->updatedObject( id, viewKey );
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
	cout << "Not implemented: SimpleView::addRef" << endl;
	return 0; //TODO
}
unsigned long SimpleView::release()
{
	cout << "Not implemented: SimpleView::release" << endl;
	return 0; //TODO
}
StatusCode SimpleView::queryInterface( const InterfaceID &/*ti*/, void** /*pp*/ )
{
	cout << "Not implemented: SimpleView::queryInterface" << endl;
	return StatusCode::FAILURE; //TODO
}
const std::string& SimpleView::name() const
{
	return m_name;
}

//IStringPool
IStringPool::sgkey_t SimpleView::stringToKey( const std::string& str, CLID clid )
{
	const std::string viewKey = m_name + "_" + str;
	return m_store->stringToKey( viewKey, clid );
}
const std::string* SimpleView::keyToString( IStringPool::sgkey_t key ) const
{
	cout << "Not implemented: SimpleView::keyToString" << endl;
	//TODO - view rename maybe?
	return m_store->keyToString( key );
}
const std::string* SimpleView::keyToString( IStringPool::sgkey_t key, CLID& clid ) const
{
	cout << "Not implemented: SimpleView::keyToString" << endl;
	//TODO - view rename maybe?
	return m_store->keyToString( key, clid ); 
}
void SimpleView::registerKey( IStringPool::sgkey_t key, const std::string& str, CLID clid )
{
	const std::string viewKey = m_name + "_" + str;
	m_store->registerKey( key, viewKey, clid );
}
