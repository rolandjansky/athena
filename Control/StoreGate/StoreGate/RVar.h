///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RVar.h 
// Header file for class SG::RVar<T>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef STOREGATE_SG_RVAR_H
#define STOREGATE_SG_RVAR_H 1

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
 * @class SG::RVar<T>
 * @brief a smart pointer to an object of a given type in an @c IProxyDict (such
 * as StoreGateSvc). It d-casts and caches locally the pointed-at object, to 
 * speed-up subsequent accesses.
 * It can be reset by the store for asynchronous updates (IOVSvc)
 *
 * @c SG::RVar<T> can access const and non-const proxies in StoreGate but
 * cannot modify them (ie: it is actually a const T*).
 * A valid proxy must already exist in StoreGate.
 *
 * Usage example:
 * @code
 *   class MyAlg : public AthAlgorithm
 *   {
 *     SG::RVar<int> m_int;
 *   };
 *
 *   MyAlg::MyAlg(...) : ..., m_int("MyIntSgKey") {
 *      declareProperty("IntHandle",
 *                      m_int = SG::RVar<int>("MyIntSgKey"),
 *                      "a handle to an int in StoreGate");
 *   }
 *
 *   StatusCode MyAlg::execute()
 *   {
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
class RVar : public SG::VarHandleBase
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
  RVar();

  /// Copy constructor: 
  RVar( const RVar& rhs );

  /// Assignment operator: 
  RVar& operator=( const RVar& rhs ); 

  /// Constructor with parameters: 

  //RVar(SG::DataProxy* proxy); ///< 

  /// retrieve a proxy of name `name` from evtStore (via component `component`)
  RVar(const IInterface* component,
       const std::string& name);

  /// retrieve a proxy of name `name` from store `store`
  RVar(const IInterface* component,
       const std::string& name, 
       const std::string& store);

  /// retrieve a proxy of name `name` from evtStore
  RVar(const std::string& name);

  /// retrieve a proxy of name `name` from store `store`
  RVar(const std::string& name, 
       const std::string& store);

  /// retrieve a proxy of name `name` from store `store`
  //RVar(const std::string& name, IProxyDict* store);

  /// Destructor: 
  virtual ~RVar(); 

  /// \name access to the underlying ptr
  //@{
  const_pointer_type  operator->() const   { return  cptr(); }
  const_reference_type operator*() const   { return *cptr(); }   

   ///< safer explicit ptr accessor 
  const_pointer_type cptr() const
  { return reinterpret_cast<const_pointer_type>(this->typeless_cptr()); }

  ///< safer explicit ptr accessor 
  const_pointer_type ptr() 
  { return cptr(); }

  //@}

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /// the CLID of the object we are bound to
  virtual CLID clid() const { return ClassID_traits<T>::ID(); }

  /// the mode of the underlying handle (reader|writer|updater)
  virtual Mode mode() const { return SG::VarHandleBase::Reader; }

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
//std::ostream& operator<<( std::ostream& out, const RVar& o );


} /* namespace SG */

#ifndef STOREGATE_SG_RVAR_ICC
 #include "StoreGate/RVar.icc"
#endif

#endif //> !STOREGATE_SG_RVAR_H
