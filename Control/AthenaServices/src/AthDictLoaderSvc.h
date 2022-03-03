///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/IClassIDSvc.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthService.h"

// the Type type
#include "DataModelRoot/RootType.h"

// AthenaKernel
#include "AthenaKernel/IDictLoaderSvc.h"
#include "AthenaKernel/DsoDb.h"

// Forward declaration
class ITPCnvSvc;
class ISvcLocator;
template <class TYPE> class SvcFactory;

class AthDictLoaderSvc
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
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface ) override;
  //@}

  static const InterfaceID& interfaceID();

  /** @brief check a @c Reflex dictionary exists for a given type
   */
  virtual 
  bool has_type (const std::string& type_name) override;

  /** @brief check a @c Reflex dictionary exists for a given type
   */
  virtual 
  bool has_type (const std::type_info& typeinfo) override;

  /** @brief check a @c Reflex dictionary exists for a given type
   */
  virtual 
  bool has_type (CLID clid) override;

  /** @brief retrieve a @c Reflex::Type by name (auto)loading the dictionary
   *         by any necessary means.
   *         If @c recursive is true, then recursively load contained types.
   */
  virtual
  const RootType load_type (const std::string& type_name, bool recursive = false) override;

  /** @brief retrieve a @c Reflex::Type by @c std::type_info (auto)loading the
   *         dictionary by any necessary means.
   *         This method is preferred over the above one as it is guaranteed to
   *         succeed *IF* the dictionary for that type has been generated.
   *         If @c recursive is true, then recursively load contained types.
   */
  virtual
  const RootType load_type (const std::type_info& typeinfo, bool recursive = false) override;

  /** @brief retrieve a @c Reflex::Type by name (auto)loading the dictionary
   *         by any necessary means.
   *         If @c recursive is true, then recursively load contained types.
   */
  virtual
  const RootType load_type (CLID clid, bool recursive = false) override;

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

  /** handle to a @c IClassIDSvc to handle loading of types by CLID
   */
  ServiceHandle<IClassIDSvc> m_clidSvc;

  ServiceHandle<ITPCnvSvc>   m_tpCnvSvc;

  void load_recursive (const RootType& typ);
  using Memo_t = std::unordered_set<std::string>;
  void load_recursive1 (const std::string& tnam, Memo_t& memo);
  void load_recursive1 (const RootType& typ, Memo_t& memo);
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
