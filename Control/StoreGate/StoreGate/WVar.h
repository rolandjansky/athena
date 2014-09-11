///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// WVar.h 
// Header file for class SG::WVar<T>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef STOREGATE_SG_WVAR_H
#define STOREGATE_SG_WVAR_H 1

// STL includes
#include <string>
#include <memory> /*unique_ptr*/

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
 * @class SG::WVar<T>
 * @brief a smart pointer to an object of a given type in an @c IProxyDict (such
 * as StoreGateSvc). It d-casts and caches locally the pointed-at object, to 
 * speed-up subsequent accesses.
 * It can be reset by the store for asynchronous updates (IOVSvc)
 *
 * @c SG::WVar<T> can only access non-const proxies in StoreGate, no proxy
 * should already exist in StoreGate as SG::WVar<T> will create it.
 *
 * Usage example:
 * @code
 *   class MyAlg : public AthAlgorithm
 *   {
 *     SG::WVar<int> m_int;
 *   };
 *
 *   MyAlg::MyAlg(...) : ..., m_int("MyIntSgKey") {
 *      declareProperty("IntHandle",
 *                      m_int = SG::WVar<int>("MyIntSgKey"),
 *                      "a handle to an int in StoreGate");
 *   }
 *
 *   StatusCode MyAlg::execute()
 *   {
 *     m_int = new int(42);
 *     ATH_MSG_INFO("int value @[" << m_int.name() << "]="
 *                  << *m_int);
 *     *m_int += 10;
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
class WVar : public SG::VarHandleBase
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
  WVar();

  /// Copy constructor: 
  WVar( const WVar& rhs );

  /// Assignment operator: 
  WVar& operator=( const WVar& rhs ); 
  WVar& operator=( const T& data );
  /// take over the object
  WVar& operator=( std::unique_ptr<T> data );

  /// Constructor with parameters: 

  //WVar(SG::DataProxy* proxy); ///< 

  //why? /// retrieve a proxy of name `name` from evtStore
  // WVar(const IInterface* component,
  //      const std::string& name);

  // /// retrieve a proxy of name `name` from store `store`
  // WVar(const IInterface* component,
  //      const std::string& name, 
  //      const std::string& store);
  
  /// retrieve a proxy of name `name` from evtStore
  WVar(const std::string& name);

  /// retrieve a proxy of name `name` from store `store`
  WVar(const std::string& name, 
       const std::string& store);

  /// retrieve a proxy of name `name` from store `store`
  //WVar(const std::string& name, IProxyDict* store);

  /// Destructor: 
  virtual ~WVar(); 

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
  virtual Mode mode() const { return SG::VarHandleBase::Writer; }

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
//std::ostream& operator<<( std::ostream& out, const WVar& o );


} /* namespace SG */

#ifndef STOREGATE_SG_WVAR_ICC
 #include "StoreGate/WVar.icc"
#endif

#endif //> !STOREGATE_SG_WVAR_H
