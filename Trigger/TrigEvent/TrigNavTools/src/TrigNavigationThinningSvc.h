// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigNavTools_TrigNavigationThinningSvc_h
#define TrigNavTools_TrigNavigationThinningSvc_h

#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/AthService.h"
#define protected public // :-)
#include "AthenaKernel/IThinningSvc.h"
#undef protected
#include "TrigNavTools/ITrigNavigationThinningSvc.h"
#include "TrigNavTools/TrigNavigationSlimmingTool.h"




class TrigNavigationThinningSvc : virtual public IThinningSvc, virtual public ITrigNavigationThinningSvc, virtual public AthService, virtual public IIncidentListener {
public:
  TrigNavigationThinningSvc( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode initialize() override;
  virtual StatusCode reinitialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode queryInterface(const InterfaceID& rrid, void** ppvInterface) override; 
  
  // specifics of ITrigNavigationThinningSvc

  virtual StatusCode doSlimming( std::vector<uint32_t>& slimmed_and_serialized ) override
 { return m_slimmingTool->doSlimming(slimmed_and_serialized);  }
  
  // forwards of ThinningSvc
  virtual bool thinningOccurred() const override;
  
  virtual
  StatusCode register_slimmer (Athena::ISlimmingHdlr *handler) override;

  virtual Athena::IThinningHdlr* handler( SG::DataProxy* proxy ) override;

  virtual StatusCode
  filter_impl( Athena::IThinningHdlr* handler,
	       SG::DataProxy* proxy,
	       const IThinningSvc::Filter_t& filter,
	       const IThinningSvc::Operator::Type op = Operator::And ) override;


  virtual 
  StatusCode commit() override;

  virtual 
  StatusCode rollback() override;

  virtual 
  std::size_t index_impl( const SG::DataProxy* objProxy, 
                          std::size_t idx ) const override;


  virtual
  bool is_thinned_impl(const SG::DataProxy* p) const override;

  // IIncidentListener
  void handle( const Incident& incident ) override;



  ///@{ @c IProxyDict interface
  using IProxyDict::proxy;

  /// get proxy for a given data object address in memory
  virtual SG::DataProxy* proxy( const void* const pTransient ) const override;

  /// get proxy with given id and key. Returns 0 to flag failure
  virtual SG::DataProxy* proxy( const CLID& id, const std::string& key ) const override;

  /// Get proxy given a hashed key+clid.
  /// Find an exact match; no handling of aliases, etc.
  /// Returns 0 to flag failure.
  virtual SG::DataProxy* proxy_exact (SG::sgkey_t sgkey) const override; 

  /// return the list of all current proxies in store
  std::vector<const SG::DataProxy*> proxies() const override;

  /// Add a new proxy to the store.
  virtual StatusCode addToStore (CLID id, SG::DataProxy* proxy) override;

  /// Record an object in the store.
  virtual SG::DataProxy* recordObject (SG::DataObjectSharedPtr<DataObject> obj,
                                       const std::string& key,
                                       bool allowMods,
                                       bool returnExisting) override;


  /// Inform HIVE that an object has been updated.
  virtual StatusCode updatedObject (CLID id, const std::string& key) override;

  
  /**
   * @brief Find the key for a string/CLID pair.
   * @param str The string to look up.
   * @param clid The CLID associated with the string.
   * @return A key identifying the string.
   *         A given string will always return the same key.
   *         Will abort in case of a hash collision!
   */
  virtual
  SG::sgkey_t stringToKey (const std::string& str, CLID clid) override;

  /**
   * @brief Find the string corresponding to a given key.
   * @param key The key to look up.
   * @return Pointer to the string found, or null.
   *         We can find keys as long as the corresponding string
   *         was given to either @c stringToKey() or @c registerKey().
   */
  virtual
  const std::string* keyToString (SG::sgkey_t key) const override;

  /**
   * @brief Find the string and CLID corresponding to a given key.
   * @param key The key to look up.
   * @param clid[out] The found CLID.
   * @return Pointer to the string found, or null.
   *         We can find keys as long as the corresponding string
   *         was given to either @c stringToKey() or @c registerKey().
   */
  virtual
  const std::string* keyToString (SG::sgkey_t key,
                                  CLID& clid) const override;

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
  void registerKey (SG::sgkey_t key,
                    const std::string& str,
                    CLID clid) override;

private:
  ServiceHandle<IThinningSvc> m_workerThinning;
  ToolHandle<HLT::TrigNavigationSlimmingTool> m_slimmingTool;

};


#endif
