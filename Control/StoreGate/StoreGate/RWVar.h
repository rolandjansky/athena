///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RWVar.h 
// Header file for class SG::RWVar<T>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef STOREGATE_SG_RWVAR_H
#define STOREGATE_SG_RWVAR_H 1

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
 * @class SG::RWVar<T>
 * @brief a smart pointer to an object of a given type in an @c IProxyDict (such
 * as StoreGateSvc). It d-casts and caches locally the pointed-at object, to 
 * speed-up subsequent accesses.
 * It can be reset by the store for asynchronous updates (IOVSvc)
 *
 * @c SG::RWVar<T> can only access non-const proxies in StoreGate, a valid
 * proxy must already exist in StoreGate.
 *
 * Usage example:
 * @code
 *   class MyAlg : public AthAlgorithm
 *   {
 *     SG::RWVar<int> m_int;
 *   };
 *
 *   MyAlg::MyAlg(...) : ..., m_int("MyIntSgKey") {
 *      declareProperty("IntHandle",
 *                      m_int = SG::RWVar<int>("MyIntSgKey"),
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
class RWVar : public SG::VarHandleBase
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
  RWVar();

  /// Copy constructor: 
  RWVar( const RWVar& rhs );

  /// Assignment operator: 
  RWVar& operator=( const RWVar& rhs ); 
  RWVar& operator=( const T& data );
  //RWVar& operator=(       T* data );

  /// Constructor with parameters: 

  //RWVar(SG::DataProxy* proxy); ///< 

  /// retrieve a proxy of name `name` from evtStore
  RWVar(const IInterface* component,
            const std::string& name);

  /// retrieve a proxy of name `name` from store `store`
  RWVar(const IInterface* component,
        const std::string& name, 
        const std::string& store);

  /// retrieve a proxy of name `name` from evtStore
  RWVar(const std::string& name);

  /// retrieve a proxy of name `name` from store `store`
  RWVar(const std::string& name, 
        const std::string& store);

  /// retrieve a proxy of name `name` from store `store`
  //RWVar(const std::string& name, IProxyDict* store);

  /// Destructor: 
  virtual ~RWVar(); 

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
//std::ostream& operator<<( std::ostream& out, const RWVar& o );


} /* namespace SG */

#ifndef STOREGATE_SG_RWVAR_ICC
 #include "StoreGate/RWVar.icc"
#endif

#endif //> !STOREGATE_SG_RWVAR_H
