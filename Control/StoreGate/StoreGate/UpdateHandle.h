///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// UpdateHandle.h 
// Header file for class SG::UpdateHandle<T>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef STOREGATE_SG_UPDATEHANDLE_H
#define STOREGATE_SG_UPDATEHANDLE_H 1

// STL includes
#include <string>

// fwk includes
#include "GaudiKernel/IInterface.h"
#include "AthenaKernel/IProxyDict.h"
#include "AthenaKernel/IResetable.h"

// SGTools includes
#include "SGTools/ClassID_traits.h"

// StoreGate includes
#include "StoreGate/VarHandleBase.h"

// Forward declaration


namespace SG {
/**
 * @class SG::UpdateHandle<T>
 * @brief a smart pointer to an object of a given type in an @c IProxyDict (such
 * as StoreGateSvc). It d-casts and caches locally the pointed-at object, to 
 * speed-up subsequent accesses.
 * It can be reset by the store for asynchronous updates (IOVSvc)
 *
 * @c SG::UpdateHandle<T> can only access non-const proxies in StoreGate, a valid
 * proxy must already exist in StoreGate.
 *
 * Usage example:
 * @code
 *   class MyAlg : public AthAlgorithm
 *   {
 *     SG::UpdateHandle<int> m_int;
 *   };
 *
 *   MyAlg::MyAlg(...) : ..., m_int("MyIntSgKey") {
 *      declareProperty("IntHandle",
 *                      m_int = SG::UpdateHandle<int>("MyIntSgKey"),
 *                      "a handle to an int in StoreGate");
 *   }
 *
 *   StatusCode MyAlg::execute()
 *   {
 *     ATH_MSG_INFO("int value @[" << m_int.name() << "]="
 *                  << *m_int);
 *     m_int = 10;
 *     ATH_MSG_INFO("int value @[" << m_int.name() << "]="
 *                  << *m_int);
 *     return StatusCode::SUCCESS;
 *   }
 * @endcode
 *
 * For more informations have a look under the package
 *     Control/AthenaExamples/AthExHelloWorld
 *
 */
template <class T>
class UpdateHandle : public SG::VarHandleBase
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public typedefs: 
  /////////////////////////////////////////////////////////////////// 
public: 
  typedef T*               pointer_type; // FIXME: better handling of
  typedef const T*   const_pointer_type; //        qualified T type ?
  typedef T&             reference_type;
  typedef const T& const_reference_type;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /// Default constructor: 
  UpdateHandle();

  /// Copy constructor: 
  UpdateHandle( const UpdateHandle& rhs );

  /// Assignment operator: 
  UpdateHandle& operator=( const UpdateHandle& rhs ); 
  UpdateHandle& operator=( const T& data );
  //UpdateHandle& operator=(       T* data );

  /// Constructor with parameters: 

  //UpdateHandle(SG::DataProxy* proxy); ///< 

  /// retrieve a proxy of name `name` from evtStore
  UpdateHandle(const IInterface* component,
            const std::string& name);

  /// retrieve a proxy of name `name` from store `store`
  UpdateHandle(const IInterface* component,
        const std::string& name, 
        const std::string& store);

  /// retrieve a proxy of name `name` from evtStore
  UpdateHandle(const std::string& name);

  /// retrieve a proxy of name `name` from store `store`
  UpdateHandle(const std::string& name, 
        const std::string& store);

  /// retrieve a proxy of name `name` from store `store`
  //UpdateHandle(const std::string& name, IProxyDict* store);

  /// Destructor: 
  virtual ~UpdateHandle(); 

  /// \name access to the underlying ptr
  //@{
  const_pointer_type operator->() const   { return cptr(); }
  pointer_type operator->()               { return ptr();  }

  const_reference_type operator*() const    { return *cptr(); }   
  reference_type operator*()                { return *ptr();  }

   ///< safer explicit ptr accessor 
  const_pointer_type cptr() const
  { return reinterpret_cast<const_pointer_type>(this->typeless_cptr()); }

  ///< safer explicit ptr accessor 
  pointer_type ptr()
  { return reinterpret_cast<pointer_type>(this->typeless_ptr()); }

  //@}

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /// the CLID of the object we are bound to
  virtual CLID clid() const { return ClassID_traits<T>::ID(); }

  /// the mode of the underlying handle (reader|writer|updater)
  virtual Mode mode() const { return SG::VarHandleBase::Updater; }

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
//std::ostream& operator<<( std::ostream& out, const UpdateHandle& o );


} /* namespace SG */

#ifndef STOREGATE_SG_UPDATEHANDLE_ICC
 #include "StoreGate/UpdateHandle.icc"
#endif

#ifndef NO_LEGACY_HANDLES
namespace SG {
  template <class T>
  using RWVar = UpdateHandle<T>;
} 
#endif

#endif //> !STOREGATE_SG_UPDATEHANDLE_H
