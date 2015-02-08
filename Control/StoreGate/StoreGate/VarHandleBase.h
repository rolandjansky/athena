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

// SGTools includes
#include "SGTools/DataProxy.h"
#include "SGTools/ProxyMap.h"
#include "SGTools/StorableConversions.h"
#include "SGTools/BuiltinsClids.h"
#include "SGTools/StlVectorClids.h"
#include "SGTools/StlMapClids.h"

// Forward declaration
//namespace AthenaInternal { class AthAlgorithmAccessor; }
class IProxyDictWithPool;
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
  const bool __defaultQuiet__(true);
#else
  const bool __defaultQuiet__(false);
#endif
/**
 * @class SG::VarHandleBase
 * @brief a smart pointer to an object of a given type in an @c IProxyDict (such
 * as StoreGateSvc). It d-casts and caches locally the pointed-at object, to 
 * speed-up subsequent accesses.
 * It can be reset by the store for asynchronous updates (IOVSvc)
 */
class VarHandleBase : public IResetable
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
  // Public enums: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /** VarHandle type: Reader|Writer
   */
  enum Mode {
    Reader = 1<<2,
    Writer = 1<<4,
    Updater = Reader | Writer
  };
 
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
			 const std::string& storename = "StoreGateSvc");

  /// Destructor: 
  virtual ~VarHandleBase(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /// \name Proxy validity checks
  //@{
  /// refers to the state of the proxy, not of the handle
  bool isConst() const;
  bool isInitialized() const;    ///<weaker test but it does not touch the disk!
  bool isSet() const { return isInitialized(); }
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

  /// the mode of the underlying handle (reader|writer|updater)
  virtual Mode mode() const =0;

  /// name of the store holding the object we are proxying
  std::string store() const { return m_store.name(); }

  ///get the data object key (proxy name) - IResetable iface
  const std::string& key() const { return this->name(); }

  ///get the data object key (proxy name)
  const std::string& name() const 
  { 
    return m_sgkey;
  }

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  virtual void reset();  ///< reset cached pointers as needed
  virtual void finalReset();  ///< reset cached pointers no matter what

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
  
  /// handle to the store which holds the object
  ServiceHandle<IProxyDictWithPool> m_store;

  void* typeless_dataPointer(bool quiet=__defaultQuiet__) const
  {
    if (m_ptr) { return m_ptr; }
    return typeless_dataPointer_impl(quiet);
  }

  void* typeless_dataPointer_impl(bool quiet) const;
  const void* typeless_cptr() const { return typeless_dataPointer(); }
  void* typeless_ptr(bool quiet=__defaultQuiet__)
  {
    void* p = typeless_dataPointer(quiet);
    return 0 != p && !isConst() ? p : 0;
  }

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
