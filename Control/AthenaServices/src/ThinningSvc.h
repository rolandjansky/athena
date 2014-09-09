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
#include "CxxUtils/unordered_map.h" // move to STL implementation when available

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
  typedef SG::unordered_map<std::size_t, std::size_t> IndexMap_t;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  ThinningSvc( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~ThinningSvc(); 

  /// Gaudi Service Implementation
  //@{
  StatusCode initialize();
  StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface );
  //@}

  ///@{ @c IProxyDict interface
  /// get proxy for a given data object address in memory,
  /// but performs a deep search among all possible 'symlinked' containers
  virtual SG::DataProxy* deep_proxy(const void* const pTransient) const;

  /// get proxy for a given data object address in memory
  virtual SG::DataProxy* proxy( const void* const pTransient ) const;

  /// get default proxy with given id. Returns 0 to flag failure
  virtual SG::DataProxy* proxy( const CLID& id ) const;

  /// get proxy with given id and key. Returns 0 to flag failure
  virtual SG::DataProxy* proxy( const CLID& id, const std::string& key ) const;

  /// Get proxy given a hashed key+clid.
  /// Find an exact match; no handling of aliases, etc.
  /// Returns 0 to flag failure.
  virtual SG::DataProxy* proxy_exact (SG::sgkey_t sgkey) const;

  /// return the list of all current proxies in store
  std::vector<const SG::DataProxy*> proxies() const;

  /// Add a new proxy to the store.
  virtual StatusCode addToStore (CLID id, SG::DataProxy* proxy);

  ///@}

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** @brief Tell clients if any thinning occurred during the event processing
   */
  virtual
  bool thinningOccurred() const { return m_thinningOccurred; }

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

  typedef SG::unordered_map< SG::DataProxy*, 
			     ThinningStoreValue_t > ThinningStore_t;

  typedef void* SlimmedObj_t;
  typedef SG::unordered_map< SlimmedObj_t,
			     Athena::ISlimmingHdlr* > SlimmingStore_t;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** incident service handle for {Begin,End}Event
   */
  void handle( const Incident& incident );

  /** Register the map of index-before-thinning/index-after-thinning
   *  for all containers
   */
  StatusCode commit();

  /** Register the map of index-before-thinning/index-after-thinning
   *  for a given container.
   *  It will also remove the 'to-be-thinned' elements, if asked for.
   */
  StatusCode commit( ThinningStore_t::iterator iStore, 
                     ThinningSvc::Policy::Type deletePolicy );

  /** Rollback the thinning actions:
   *   - restore the requested to-be-thinned elements
   */
  StatusCode rollback();

  /** Get the index after thinning of a given container, providing
   *  the old index.
   *  Returns IThinningSvc::RemovedIdx if the element asked-for has been
   *  removed during thinning.
   */
  std::size_t index_impl(const SG::DataProxy* proxy, std::size_t idx ) const;

  /** @brief test if a container is thinned
   */
  bool is_thinned_impl(const SG::DataProxy* p) const;

  /** @brief Retrieve the handler (if any) to thin a @c DataProxy
   */
  virtual 
  Athena::IThinningHdlr* handler( SG::DataProxy* proxy );

  /** Build the 'db' of elements to be kept for each container
   */
  virtual 
  StatusCode filter_impl( Athena::IThinningHdlr* handler,
			  SG::DataProxy* proxy,
			  const Filter_t& filter,
			  const IThinningSvc::Operator::Type op );

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
  StatusCode register_slimmer (Athena::ISlimmingHdlr *handler);

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
