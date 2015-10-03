///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// VarHandleBase.h 
// Header file for class SG::VarHandleBase
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef STOREGATE_SG_VARHANDLEBASE_H
#define STOREGATE_SG_VARHANDLEBASE_H 1

// STL includes
#include <string>

// fwk includes
#include "AthenaKernel/IProxyDict.h"
#include "AthenaKernel/IResetable.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/DataObjID.h"
#include "GaudiKernel/DataHandle.h"

// SGTools includes
#include "SGTools/DataProxy.h"
#include "SGTools/ProxyMap.h"
#include "SGTools/StorableConversions.h"
#include "SGTools/BuiltinsClids.h"
#include "SGTools/StlVectorClids.h"
#include "SGTools/StlMapClids.h"

// Forward declaration
//namespace AthenaInternal { class AthAlgorithmAccessor; }
class StoreGateSvc;
class VarHandleProperty;
namespace SG { class VarHandleBase; }
namespace Gaudi { namespace Parsers {
    StatusCode parse(SG::VarHandleBase&, const std::string&);
  }
}
namespace Athena_test {
  void varHandleTest(void);
  void resetableTest(void);
  void refCountTest(void);
}
namespace SG {
#ifdef NDEBUG
  const bool defaultQuiet = true;
#else
  const bool defaultQuiet = false;
#endif
/**
 * @class SG::VarHandleBase
 * @brief a smart pointer to an object of a given type in an @c IProxyDict (such
 * as StoreGateSvc). It d-casts and caches locally the pointed-at object, to 
 * speed-up subsequent accesses.
 * It can be reset by the store for asynchronous updates (IOVSvc)
 */
class VarHandleBase : public IResetable, public Gaudi::DataHandle
{ 
  //friend class AthenaInternal::AthAlgorithmAccessor;
  friend class VarHandleProperty;
  friend StatusCode Gaudi::Parsers::parse(SG::VarHandleBase&, 
                                          const std::string&);
  friend void Athena_test::varHandleTest(void);
  friend void Athena_test::resetableTest(void);
  friend void Athena_test::refCountTest(void);
  friend std::ostream& operator<<( std::ostream&, const VarHandleBase&);
 
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /// Default constructor: 
  VarHandleBase();

  /// Copy constructor: 
  VarHandleBase( const VarHandleBase& rhs );
  VarHandleBase( VarHandleBase&& rhs );

  /// Assignment operator: 
  VarHandleBase& operator=( const VarHandleBase& rhs ); 
  VarHandleBase& operator=( VarHandleBase&& rhs ); 

  /// Constructor with parameters: 

  //explicit VarHandleBase(SG::DataProxy* proxy); ///< 

  explicit VarHandleBase(const std::string& sgkey,
			 const Gaudi::DataHandle::Mode& mode,
			 const std::string& storename = "StoreGateSvc");

  /// Destructor: 
  virtual ~VarHandleBase() override;

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /// \name Proxy validity checks
  //@{
  /// refers to the state of the proxy, not of the handle
  bool isConst() const;
  bool isInitialized() const;    ///<weaker test but it does not touch the disk!
  virtual bool isSet() const override final { return isInitialized(); }
  //@}

  /// \name validity checks
  //@{
  /// retrieves the @c DataObject to check it is valid and locked
  virtual bool isValid() const = 0;

#ifdef ATHENA_USE_CXX11
  explicit operator bool() const { return isValid(); }
#endif
  //@}

  StatusCode setState() const;

  /// the CLID of the object we are bound to
  /// it is extracted from the templated derived class instead of
  /// from the datamember proxy because we use this clid() method to
  /// know which proxy to retrieve from the IProxyDict store.
  virtual CLID clid() const =0;

  /// name of the store holding the object we are proxying
  std::string store() const;

  ///get the data object key (proxy name) - IResetable iface
  virtual const std::string& key() const override final { return this->name(); }

  ///get the data object key (proxy name)
  const std::string& name() const 
  { 
    return m_sgkey;
  }

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 


  /**
   * @brief Explicitly set the event store.
   * @param store The new event store.
   *
   * This implicitly does a reset().
   */
  StatusCode setStore (IProxyDictWithPool* store);

  using IResetable::reset;
  virtual void reset (bool hard) override final;  ///< reset cached pointers as needed
  virtual void finalReset() override final;  ///< reset cached pointers no matter what

  /// set the 'const' bit for the bound proxy in the store
  void setConst() { m_proxy->setConst(); }

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
protected: 

  StatusCode setState(SG::DataProxy* proxy) const;
  StatusCode setState(IProxyDict* store, const std::string& name) const;

  /// helper functions to bind ourselves to a DataProxy
  static bool bindToProxy(      SG::VarHandleBase* v);
  static bool bindToProxy(const SG::VarHandleBase* v)
  { 
    return SG::VarHandleBase::bindToProxy(const_cast<SG::VarHandleBase*>(v));
  }

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
protected: 

  /// the object we are bound to
  mutable void* m_ptr;

  /// the proxy holding the object we are bound to
  mutable SG::DataProxy* m_proxy;

  /// storegate key we will be bound to
  std::string m_sgkey;
  
  /// Pointer to the store that owns the object.
  mutable IProxyDictWithPool* m_store;

  /// Handle to the store.  Used if the store pointer is not set explicitly.
  ServiceHandle<IProxyDictWithPool> m_storeHandle;

  void* typeless_dataPointer(bool quiet=defaultQuiet) const
  {
    if (m_ptr) { return m_ptr; }
    return typeless_dataPointer_impl(quiet);
  }

  void* typeless_dataPointer_impl(bool quiet) const;
  const void* typeless_cptr() const { return typeless_dataPointer(); }
  void* typeless_ptr(bool quiet=defaultQuiet)
  {
    void* p = typeless_dataPointer(quiet);
    return 0 != p && !isConst() ? p : 0;
  }

#ifdef ATHENAHIVE
  /////////////////////////////////////////////////////////////////
  //
  //// From DataHandle
  //
public:

  virtual void setKey(const DataObjID& key) {
    m_TID = key;
  }

  virtual const std::string& objKey() const {
    return key();
  }
  virtual void updateKey(const std::string& /*key*/) {};
  
  virtual const DataObjID& fullKey() const {
    if (!m_idValid) {
      m_TID = DataObjID(m_sgkey,clid());
      m_idValid = true;
    }
    return m_TID;
  }
  
#endif

private:
  mutable DataObjID m_TID;

  // ugly hack since we need to know the clid() before setState is called
  // and it's not yet available in the constructor
  mutable bool m_idValid;

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline
std::ostream& operator<<( std::ostream& out, const VarHandleBase& o ) {
  out << "VarHandleBase @" << &o
      << " store=" <<o.store()
      << ", clid=" <<o.clid()
      << ", key=" <<o.key()
      << "----------- ptr@" << o.m_ptr 
      << ", proxy@" << o.m_proxy 
      << ", DataObject@" << o.m_proxy->object();
  return out;
}

inline 
bool operator==(const VarHandleBase& l, const VarHandleBase& r) {
  return (l.clid() == r.clid() &&
	  l.mode() == r.mode() &&
	  l.name() == r.name() &&
	  l.store() == r.store());
}
inline 
bool operator!=(const VarHandleBase& l, const VarHandleBase& r) {
  return !(l==r);
}


} /* namespace SG */

#endif //> !STOREGATE_SG_VARHANDLEBASE_H
