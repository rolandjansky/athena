///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHVIEWS_SIMPLEVIEW_H
#define ATHVIEWS_SIMPLEVIEW_H 1

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IProxyDict.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthLinks/ElementLink.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include <string>
#include <set>
#include "tbb/concurrent_unordered_set.h"
#include <vector>

namespace SG {
  class DataProxy;
  class View;
}
class DataObject;

/**
 * @brief Very simple pass-through implementation of IProxyDict 
 *
 * Most of the methods are copypasta and don't do anything
 *
 * @author Ben Wynne - ATLAS
 */
class SimpleView : public IProxyDict
{
  public:
    DeclareInterfaceID( SimpleView, 2, 0 );

    SimpleView() = delete;
    SimpleView( std::string Name, bool AllowFallThrough = true, std::string const& storeName = "StoreGateSvc" );
    virtual ~SimpleView();

   /**
    * @brief Construct a key as used in the parent store.
    * @brief key The key as used in the view.
    */
   std::string viewKey (const std::string& key) const;

    /**
    * @brief links to the previously used views
    * through these parent views additional data objects become available
    **/
    virtual void linkParent( const IProxyDict* parent );

    /**
    * @brief Set a filtering rule for anything loaded via fallthrough
    **/
    void setFilter( std::vector< std::string > const& inputFilter )
    { m_fallFilter = inputFilter; }

    /**
    * get proxy for a given data object address in memory,
    * but performs a deep search among all possible 'symlinked' containers
    * TEMPORARY: This method is going away.
    **/
    virtual SG::DataProxy* deep_proxy(const void* const pTransient) const
    { return proxy (pTransient); }

    /**
    * @brief Get proxy given a hashed key+clid.
    * @param sgkey Hashed key to look up.
    *
    * Find an exact match; no handling of aliases, etc.
    * Returns 0 to flag failure.
    */
    virtual SG::DataProxy* proxy_exact (SG::sgkey_t sgkey) const;


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
    virtual SG::DataProxy* proxy(const CLID& id, const std::string& key) const;

    // The real implementation of proxy(), with control for whole-event store access
    virtual SG::DataProxy* findProxy( const CLID& id, const std::string& key, const bool allowFallThrough ) const;

    /**
    * @brief Get a proxy referencing a given transient object.
    * @param pTransient The object to find.
    * 
    * Returns 0 to flag failure
    */
    virtual SG::DataProxy* proxy(const void* const pTransient) const;


    /**
    * @brief Return the list of all current proxies in store.
    */
    virtual std::vector<const SG::DataProxy*> proxies() const;


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
    virtual StatusCode addToStore(CLID id, SG::DataProxy* proxy);


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
    virtual SG::DataProxy* recordObject ( SG::DataObjectSharedPtr<DataObject> obj,
                                          const std::string& key,
                                          bool allowMods,
                                          bool returnExisting);


    /**
    * @brief Tell the store that a handle has been bound to a proxy.
    * @param handle The handle that was bound.
    * The default implementation does nothing.
    */
    virtual void boundHandle (IResetable* handle);

    /**
    * @brief Tell the store that a handle has been unbound from a proxy.
    * @param handle The handle that was unbound.
    * The default implementation does nothing.
    */
    virtual void unboundHandle (IResetable* handle);

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
    virtual bool tryELRemap ( sgkey_t sgkey_in,   size_t index_in,
                              sgkey_t& sgkey_out, size_t& index_out);

    virtual unsigned long addRef();
    virtual unsigned long release();
    virtual StatusCode queryInterface( const InterfaceID &ti, void** pp );
    virtual const std::string& name() const;

    //IStringPool
    virtual IStringPool::sgkey_t stringToKey( const std::string& str, CLID clid );
    virtual const std::string* keyToString( IStringPool::sgkey_t key ) const;
    virtual const std::string* keyToString( IStringPool::sgkey_t key, CLID& clid ) const;
    virtual void registerKey( IStringPool::sgkey_t key, const std::string& str, CLID clid );

    // prints content of the view
    std::string dump( const std::string& indent = "" ) const;

    void setROI(const ElementLink<TrigRoiDescriptorCollection>& roi);
    const ElementLink<TrigRoiDescriptorCollection>& getROI() const;

  protected:
    // Connection to the whole event store
    ServiceHandle< StoreGateSvc > m_store;

    // View properties
    ElementLink<TrigRoiDescriptorCollection> m_roi;
    std::string m_name;
    std::set< const SG::View* > m_parents;
    std::vector< std::string > m_fallFilter;
    bool m_allowFallThrough;
};

#endif // ATHVIEW_SIMPLEVIEW_H
