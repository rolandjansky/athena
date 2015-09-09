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

  StatusCode doSlimming( std::vector<uint32_t>& slimmed_and_serialzied ) { return m_slimmingTool->doSlimming(slimmed_and_serialzied);  }
  
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
  /// get proxy for a given data object address in memory,
  /// but performs a deep search among all possible 'symlinked' containers
  virtual SG::DataProxy* deep_proxy(const void* const pTransient) const override;

  /// get proxy for a given data object address in memory
  virtual SG::DataProxy* proxy( const void* const pTransient ) const override;

  /// get default proxy with given id. Returns 0 to flag failure
  virtual SG::DataProxy* proxy( const CLID& id ) const override;

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





  
private:
  ServiceHandle<IThinningSvc> m_workerThinning;
  ToolHandle<HLT::TrigNavigationSlimmingTool> m_slimmingTool;

};


#endif
