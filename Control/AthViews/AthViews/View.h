///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHVIEWS_VIEW_H
#define ATHVIEWS_VIEW_H

#include "AthenaKernel/IProxyDict.h"
#include "AthViews/SimpleView.h"
#include "AthViews/DebugView.h"
#include "AthLinks/ElementLink.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

// DECLARATIONS
namespace SG {
  class DataProxy;
}
class DataObject;

// rewire implementations in the constructor
// this is PIMPL for now
namespace SG {
class View : public IProxyDict {
public:
  View () = delete;
  View (const std::string& name, const int index, const bool AllowFallThrough = true, std::string const& storeName = "StoreGateSvc");
  virtual ~View ();
  View (const View&) = delete;
  View& operator= (const View&) = delete;

   /**
    * @brief Construct a key as used in the parent store.
    * @brief key The key as used in the view.
    */
   std::string viewKey (const std::string& key) const
   {
     return m_implementation->viewKey (key);
   }


#ifdef ATHVIEWS_DEBUG
  void impl ( DebugView* impl ) { m_implementation = impl; }
  DebugView* impl (void ) { return m_implementation; }
  const DebugView* impl ( void ) const { return m_implementation; }
#else
  void impl ( SimpleView* impl ) { m_implementation = impl; }
  SimpleView* impl (void ) { return m_implementation; }
  const SimpleView* impl ( void ) const { return m_implementation; }
#endif
  size_t viewID() const{ return m_index; }

  /**
   * for printing the content of the view
   * @warning - expensive call
   **/
  std::string dump( const std::string& indent = "" ) const {
    return m_implementation->dump( indent );
  }

  /*virtual SG::DataProxy* proxy(const CLID& id) const { 
    return m_implementation->proxy(id); 
  }*/

  void linkParent( const IProxyDict* parent) {
    m_implementation->linkParent( parent );
  }

  void setFilter( std::vector< std::string > const& inputFilter ) {
    m_implementation->setFilter( inputFilter );
  }
  
  virtual SG::DataProxy* deep_proxy(const void* const pTransient) const { 
    return m_implementation->proxy (pTransient); 
  }

  virtual SG::DataProxy* proxy_exact (SG::sgkey_t sgkey) const {
    return m_implementation->proxy_exact(sgkey);
  }

  virtual SG::DataProxy* proxy(const CLID& id, const std::string& key) const {
    return m_implementation->proxy(id, key);
  }

  virtual SG::DataProxy* proxy(const void* const pTransient) const {
    return m_implementation->proxy(pTransient);
  }


  virtual std::vector<const SG::DataProxy*> proxies() const {
    return m_implementation->proxies();
  }


  virtual StatusCode addToStore(CLID id, SG::DataProxy* proxy) {
    return m_implementation->addToStore(id, proxy);
  }


  /*virtual SG::DataProxy* recordObject (std::unique_ptr<DataObject> obj,
				       const std::string& key,
				       bool allowMods) {
    return m_implementation->recordObject( std::move( obj ), key, allowMods );
  }*/
  virtual SG::DataProxy* recordObject (SG::DataObjectSharedPtr<DataObject> obj,
                                       const std::string& key,
                                       bool allowMods,
                                       bool returnExisting) {
    return m_implementation->recordObject( obj, key, allowMods, returnExisting );
  }
  

  
  virtual void boundHandle (IResetable* handle) {
    return m_implementation->boundHandle(handle);
  }
  
  virtual void unboundHandle (IResetable* handle) {
    return m_implementation->unboundHandle(handle);
  }

  virtual bool tryELRemap (sgkey_t sgkey_in, size_t index_in,
			   sgkey_t& sgkey_out, size_t& index_out) {
    return m_implementation->tryELRemap(sgkey_in, index_in, sgkey_out, index_out);
  }

  virtual unsigned long addRef(){ return m_implementation->addRef(); }
  virtual unsigned long release(){ return m_implementation->release(); }
  virtual StatusCode queryInterface(const InterfaceID &ti, void** pp){ return m_implementation->queryInterface(ti, pp); };
  virtual const std::string& name() const { return m_implementation->name(); }

  //IStringPool
  virtual IStringPool::sgkey_t stringToKey( const std::string& str, CLID clid ){ return m_implementation->stringToKey( str, clid ); }
  virtual const std::string* keyToString( IStringPool::sgkey_t key ) const{ return m_implementation->keyToString( key ); }
  virtual const std::string* keyToString( IStringPool::sgkey_t key, CLID& clid ) const{ return m_implementation->keyToString( key, clid ); }
  virtual void registerKey( IStringPool::sgkey_t key, const std::string& str, CLID clid ){ m_implementation->registerKey( key, str, clid ); }

  void setROI(const ElementLink<TrigRoiDescriptorCollection>& roi) { m_implementation->setROI(roi); };
  const ElementLink<TrigRoiDescriptorCollection>& getROI() const { return m_implementation->getROI(); };

private:

#ifdef ATHVIEWS_DEBUG
  DebugView *m_implementation;
#else
  SimpleView *m_implementation;
#endif
  size_t m_index;
};
} // EOF SG namespace


#include "AthenaKernel/CLASS_DEF.h"

// Do we need to do this?
class ViewContainer {
  typedef std::vector<SG::View*> T;
  T m_data;
public:

  typedef T::const_iterator const_iterator;
  typedef T::iterator iterator;
  typedef T::reverse_iterator reverse_iterator;
  typedef T::const_reference const_reference;
  typedef T::reference reference;
  typedef T::value_type value_type;


  ~ViewContainer() { 
    std::for_each(m_data.begin(), m_data.end(), [](SG::View* v){ delete v; } ); 
  }
  void push_back( SG::View* ptr ) { m_data.push_back( ptr ); }
  size_t size() const { return m_data.size(); }
  bool empty() const { return m_data.empty(); }
  void clear() {     
    std::for_each(m_data.begin(), m_data.end(), [](SG::View* v){ delete v; } );   m_data.clear(); 
  }
  const_iterator begin() const { return m_data.begin(); }
  const_iterator end() const { return m_data.end(); }
  iterator begin() { return m_data.begin(); }
  iterator end() { return m_data.end(); }
  reverse_iterator rbegin() { return m_data.rbegin(); }
  reverse_iterator rend() { return m_data.rend(); }

  const_reference at(size_t pos) const { return m_data.at(pos); }
  reference at(size_t pos) { return m_data.at(pos); }
  const_reference back() const { return m_data.back(); }
  reference back() { return m_data.back(); }
};

#include "AthLinks/DeclareIndexingPolicy.h"
CONTAINER_IS_SEQUENCE(ViewContainer)

CLASS_DEF( ViewContainer , 1160627009 , 1 )

#endif
