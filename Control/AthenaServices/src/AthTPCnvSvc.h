///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// AthTPCnvSvc.h 
// Header file for class AthTPCnvSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENASERVICES_ATHTPCNVSVC_H
#define ATHENASERVICES_ATHTPCNVSVC_H 1

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IClassIDSvc.h"
#include "AthenaKernel/ITPCnvSvc.h"
#include "AthenaKernel/ITPCnvBase.h"
#include "AthenaBaseComps/AthService.h"

// Forward declaration
class ICLIDSvc;


class AthTPCnvSvc
  : public extends1<AthService, ITPCnvSvc>
{ 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  AthTPCnvSvc( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~AthTPCnvSvc() override; 

  //@}

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** @brief load the T/P converter class named `cls`
   *  return NULL on failure.
   *  the converter is OWNED by the T/P converter service
   */
  virtual
  ITPCnvBase*
  load_tpcnv(const std::string& cls) override;

  /** @brief return the T/P converter for a transient class (NULL if failure)
   *         `ITPCnvSvc` owns the `ITPCnvBase` pointer
   */ 
  virtual
  ITPCnvBase* 
  t2p_cnv(const std::string& transClassName,
          Athena::TPCnvType::Value type = Athena::TPCnvType::Athena) override;

  /** @brief return the T/P converter for a transient class (NULL if failure)
   *         `ITPCnvSvc` owns the `ITPCnvBase` pointer
   */ 
  virtual
  ITPCnvBase* 
  t2p_cnv(const CLID& transClid,
          Athena::TPCnvType::Value type = Athena::TPCnvType::Athena) override;

  /** @brief return the T/P converter for a persistent class (NULL if failure)
   *         `ITPCnvSvc` owns the `ITPCnvBase` pointer
   */ 
  virtual
  ITPCnvBase* 
  p2t_cnv(const std::string& persClassName,
          Athena::TPCnvType::Value type = Athena::TPCnvType::Athena) override;

  /** @brief return the T/P converter for a transient class (NULL if failure)
   *         Ownership is returned to the caller.
   */ 
  virtual
  std::unique_ptr<ITPCnvBase>
  t2p_cnv_unique(const std::string& transClassName) const override;

  /** @brief Return the T/P converter for a transient class.
   *         Returns null on failure (with no warning printed).
   *         Ownership is returned to the caller.
   */ 
  virtual
  std::unique_ptr<ITPCnvBase>
  t2p_cnv_unique(const CLID transClid) const override;


  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 
  /// Default constructor: 
  AthTPCnvSvc();

  /** handle to a @c IClassIDSvc to handle loading of types by CLID
   */
  ServiceHandle<IClassIDSvc> m_clidSvc;

  typedef std::vector<ITPCnvBase*> TpCnvs_t;
  /** a registry of ITPCnvBase* instances
   */
  TpCnvs_t m_cnvs;
  
}; 


#endif //> !ATHENASERVICES_ATHTPCNVSVC_H
