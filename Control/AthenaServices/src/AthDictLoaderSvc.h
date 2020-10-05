///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// AthDictLoaderSvc.h 
// Header file for class AthDictLoaderSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENASERVICES_ATHDICTLOADERSVC_H
#define ATHENASERVICES_ATHDICTLOADERSVC_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthService.h"

// the Type type
#include "DataModelRoot/RootType.h"

// AthenaKernel
#include "AthenaKernel/IDictLoaderSvc.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/DsoDb.h"

// Forward declaration
class ISvcLocator;
template <class TYPE> class SvcFactory;

class ATLAS_CHECK_THREAD_SAFETY AthDictLoaderSvc
  : virtual public ::IDictLoaderSvc,
            public ::AthService
{ 
  friend class SvcFactory<AthDictLoaderSvc>;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  AthDictLoaderSvc( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~AthDictLoaderSvc(); 

  // Assignment operator: 
  //AthDictLoaderSvc &operator=(const AthDictLoaderSvc &alg); 

  /// Gaudi Service Implementation
  //@{
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface );
  //@}

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  static const InterfaceID& interfaceID();

  /** @brief check a @c Reflex dictionary exists for a given type
   */
  virtual 
  bool has_type (const std::string& type_name);

  /** @brief check a @c Reflex dictionary exists for a given type
   */
  virtual 
  bool has_type (const std::type_info& typeinfo);

  /** @brief check a @c Reflex dictionary exists for a given type
   */
  virtual 
  bool has_type (CLID clid);

  /** @brief retrieve a @c Reflex::Type by name (auto)loading the dictionary
   *         by any necessary means.
   */
  virtual
  const RootType load_type (const std::string& type_name);

  /** @brief retrieve a @c Reflex::Type by @c std::type_info (auto)loading the
   *         dictionary by any necessary means.
   *         This method is preferred over the above one as it is guaranteed to
   *         succeed *IF* the dictionary for that type has been generated.
   */
  virtual
  const RootType load_type (const std::type_info& typeinfo);

  /** @brief retrieve a @c Reflex::Type by name (auto)loading the dictionary
   *         by any necessary means.
   */
  virtual
  const RootType load_type (CLID clid);

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  AthDictLoaderSvc();

  // Containers
  
  /** dictionary of all known (reflex) types
   */
  const Ath::DsoDb* m_dsodb;

  /** switch to recursively load (or not) all dictionaries for all types
   *  composing a given one. (ie: load dict of Bar in struct Foo {Bar*b;};)
   *  Default is 'true'.
   */
  bool m_doRecursiveLoad;

  /** handle to a @c IClassIDSvc to handle loading of types by CLID
   */
  ServiceHandle<IClassIDSvc> m_clidSvc;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline 
const InterfaceID& 
AthDictLoaderSvc::interfaceID() 
{ 
  return IDictLoaderSvc::interfaceID(); 
}



#endif //> !ATHENASERVICES_ATHDICTLOADERSVC_H
