///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ThinningSvc.h 
// Header file for class ThinningSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENASERVICES_THINNINGSVC_H 
#define ATHENASERVICES_THINNINGSVC_H 
/** @file ThinningSvc.h
 * @brief This service allows to thin 
 * (remove elements from) a DataVector-like container while the
 * container is being written out.  See AthExThinning for an example of usage.

 * $Id: ThinningSvc.h,v 1.12 2008-09-08 17:10:19 binet Exp $
 * @author S.Binet<binet@cern.ch>
*/

// STL includes
#include <string>
#include <vector>

// boost includes
#include <boost/tuple/tuple.hpp>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/AthService.h"

// AthenaKernel includes
#include "AthenaKernel/IThinningHdlr.h"
#include "AthenaKernel/ISlimmingHdlr.h"
#include "AthenaKernel/IThinningSvc.h"

// DataModel includes
#include <unordered_map>
#include <unordered_set>

// Forward declaration
class DataBucketBase;
class ISvcLocator;
class StoreGateSvc;
template <class TYPE> class SvcFactory;

/**
 * @class ThinningSvc
 * @brief This service allows to thin 
 * (remove elements from) a DataVector-like container while the
 * container is being written out.  See AthExThinning for an example of usage.
 */
class ThinningSvc : virtual public IThinningSvc,
                    virtual public IIncidentListener,
	                    public AthService
{ 

protected:
  friend class SvcFactory<ThinningSvc>;

  /////////////////////////////////////////////////////////////////// 
  // Public typedefs: 
  /////////////////////////////////////////////////////////////////// 
 public: 
  /// the type of the map holding the index-to-index conversion
  /// (that is the index before and after the thinning took place)
  typedef std::unordered_map<std::size_t, std::size_t> IndexMap_t;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  ThinningSvc( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~ThinningSvc() override; 

  /// Gaudi Service Implementation
  //@{
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface ) override;
  //@}

  /// get proxy for a given data object address in memory
  virtual SG::DataProxy* proxy( const void* const pTransient ) const override final;

  /// get proxy with given id and key. Returns 0 to flag failure
  virtual SG::DataProxy* proxy( const CLID& id, const std::string& key ) const override final;

  /// Get proxy given a hashed key+clid.
  /// Find an exact match; no handling of aliases, etc.
  /// Returns 0 to flag failure.
  virtual SG::DataProxy* proxy_exact (SG::sgkey_t sgkey) const override final;

  /// return the list of all current proxies in store
  virtual std::vector<const SG::DataProxy*> proxies() const override final;

  /// Add a new proxy to the store.
  virtual StatusCode addToStore (CLID id, SG::DataProxy* proxy) override final;

  /**
   * @brief Record an object in the store.
   * @param obj The data object to store.
   * @param key The key as which it should be stored.
   * @param allowMods If false, the object will be recorded as const.
   * @param returnExisting If true, return proxy if this key already exists.
   *
   * Full-blown record.  @c obj should usually be something
   * deriving from @c SG::DataBucket.
   *
   * Returns the proxy for the recorded object; nullptr on failure.
   * If the requested CLID/key combination already exists in the store,
   * the behavior is controlled by @c returnExisting.  If true, then
   * the existing proxy is returned; otherwise, nullptr is returned.
   * In either case, @c obj is destroyed.
   */
  virtual
  SG::DataProxy* recordObject (SG::DataObjectSharedPtr<DataObject> obj,
                               const std::string& key,
                               bool allowMods,
                               bool returnExisting) override final;

  /**
   * @brief Inform HIVE that an object has been updated.
   * @param id The CLID of the object.
   * @param key The key of the object.
   */
  virtual
  StatusCode updatedObject (CLID id, const std::string& key) override final;


  ///@}

  /// IStringPool implementation.
  //@{


  /**
   * @brief Find the key for a string/CLID pair.
   * @param str The string to look up.
   * @param clid The CLID associated with the string.
   * @return A key identifying the string.
   *         A given string will always return the same key.
   *         Will abort in case of a hash collision!
   */
  virtual
  sgkey_t stringToKey (const std::string& str, CLID clid) override final;


  /**
   * @brief Find the string corresponding to a given key.
   * @param key The key to look up.
   * @return Pointer to the string found, or null.
   *         We can find keys as long as the corresponding string
   *         was given to either @c stringToKey() or @c registerKey().
   */
  virtual
  const std::string* keyToString (sgkey_t key) const override final;


  /**
   * @brief Find the string and CLID corresponding to a given key.
   * @param key The key to look up.
   * @param clid[out] The found CLID.
   * @return Pointer to the string found, or null.
   *         We can find keys as long as the corresponding string
   *         was given to either @c stringToKey() or @c registerKey().
   */
  virtual
  const std::string* keyToString (sgkey_t key,
                                  CLID& clid) const override final;


  /**
   * @brief Remember an additional mapping from key to string/CLID.
   * @param key The key to enter.
   * @param str The string to enter.
   * @param clid The CLID associated with the string.
   * @return True if successful; false if the @c key already
   *         corresponds to a different string.
   *
   * This registers an additional mapping from a key to a string;
   * it can be found later through @c lookup() on the string.
   * Logs an error if @c key already corresponds to a different string.
   */
  virtual
  void registerKey (sgkey_t key,
                    const std::string& str,
                    CLID clid) override final;

  ///@}


  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** @brief Tell clients if any thinning occurred during the event processing
   */
  virtual
  bool thinningOccurred() const  override
  { return m_thinningOccurred; }

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  static const InterfaceID& interfaceID();

  /////////////////////////////////////////////////////////////////// 
  // Protected types and enums: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  struct Policy {
    enum Type {
      Delete = 0,
      DontDelete = 1
    };
  };

  typedef boost::tuple<IThinningSvc::Filter_t,
		       Athena::IThinningHdlr*,
		        ThinningSvc::IndexMap_t> ThinningStoreValue_t;

  typedef std::unordered_map< SG::DataProxy*, 
		 	     ThinningStoreValue_t > ThinningStore_t;

  typedef void* SlimmedObj_t;
  typedef std::unordered_map< SlimmedObj_t,
                              Athena::ISlimmingHdlr* > SlimmingStore_t;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** incident service handle for {Begin,End}Event
   */
  virtual void handle( const Incident& incident ) override;

  /** Register the map of index-before-thinning/index-after-thinning
   *  for all containers
   */
  virtual StatusCode commit() override;

  /** Register the map of index-before-thinning/index-after-thinning
   *  for a given container.
   *  It will also remove the 'to-be-thinned' elements, if asked for.
   */
  StatusCode commit( ThinningStore_t::iterator iStore, 
                     ThinningSvc::Policy::Type deletePolicy );

  /** Rollback the thinning actions:
   *   - restore the requested to-be-thinned elements
   */
  virtual StatusCode rollback() override;

  /** Get the index after thinning of a given container, providing
   *  the old index.
   *  Returns IThinningSvc::RemovedIdx if the element asked-for has been
   *  removed during thinning.
   */
  virtual
  std::size_t index_impl(const SG::DataProxy* proxy, std::size_t idx ) const
    override;

  /** @brief test if a container is thinned
   */
  virtual
  bool is_thinned_impl(const SG::DataProxy* p) const override;

  /** @brief Retrieve the handler (if any) to thin a @c DataProxy
   */
  virtual 
  Athena::IThinningHdlr* handler( SG::DataProxy* proxy ) override;

  /** Build the 'db' of elements to be kept for each container
   */
  virtual 
  StatusCode filter_impl( Athena::IThinningHdlr* handler,
			  SG::DataProxy* proxy,
			  const Filter_t& filter,
			  const IThinningSvc::Operator::Type op ) override;

  /** Helper method to clean-up thinning handlers and store
   */
  void cleanupStore();

  /** @brief register a slimming handler with the @c IThinningSvc
   *  Registering a @c ISlimmingHdlr allows to slim an object and keep the
   *  side effects of slimming it 'guarded' within one output stream.
   *  The original state (before slimming) of the object will be restored after
   *  to-disk serialization took place.
   *  Note: @c IThinningSvc will take ownership of the handler.
   */
  virtual
  StatusCode register_slimmer (Athena::ISlimmingHdlr *handler) override;

  /// Default constructor: 
  ThinningSvc();

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// the store of slimmers
  SlimmingStore_t m_slimmingStore;

  /// the store of everything.
  /// It contains the associations: 
  ///  [originalProxy] : [filter, deleter,index]
  ThinningStore_t m_thinningStore;

  /// keep track of thinning state: if any thinning occured 
  /// (ie: @c ThinningSvc::filter_impl was called) this variable should be true
  bool m_thinningOccurred;

  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
  /// Pointer to @c StoreGateSvc
  StoreGateSvc_t m_storeGate;

  /** The names of output stream(s) we want to apply thinning on
   */
  StringArrayProperty m_thinnedOutStreamNames;

  /// Set of owned handlers.
  typedef std::unordered_set<Athena::IThinningHdlr*> HandlerSet_t;
  HandlerSet_t m_ownedHandlers;
}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline const InterfaceID& ThinningSvc::interfaceID() 
{ 
   return IThinningSvc::interfaceID(); 
}

#endif //> ATHENASERVICES_THINNINGSVC_H
