///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ITPCnvSvc.h 
// Header file for class ITPCnvSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAKERNEL_ITPCNVSVC_H 
#define ATHENAKERNEL_ITPCNVSVC_H 1 

/** @class ITPCnvSvc
 */


#include "AthenaKernel/TPCnvFactory.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ClassID.h"
#include <string>


class ITPCnvBase;


class ITPCnvSvc
  : virtual public ::IService
{ 
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 
  DeclareInterfaceID(ITPCnvSvc,1,0);


  /** Destructor: 
   */
  virtual ~ITPCnvSvc();

  /** @brief load the T/P converter class named `cls`
   *  return NULL on failure.
   *  the converter is OWNED by the T/P converter service
   */
  virtual
  ITPCnvBase*
  load_tpcnv(const std::string& cls) = 0;

  /** @brief return the T/P converter for a transient class (NULL if failure)
   *         `ITPCnvSvc` owns the `ITPCnvBase` pointer
   */ 
  virtual
  ITPCnvBase* 
  t2p_cnv(const std::string& transClassName,
          Athena::TPCnvType::Value type = Athena::TPCnvType::Athena) = 0;

  /** @brief return the T/P converter for a transient class (NULL if failure)
   *         `ITPCnvSvc` owns the `ITPCnvBase` pointer
   */ 
  virtual
  ITPCnvBase* 
  t2p_cnv(const CLID& transClid,
          Athena::TPCnvType::Value type = Athena::TPCnvType::Athena) = 0;

  /** @brief return the T/P converter for a persistent class (NULL if failure)
   *         `ITPCnvSvc` owns the `ITPCnvBase` pointer
   */ 
  virtual
  ITPCnvBase* 
  p2t_cnv(const std::string& persClassName,
          Athena::TPCnvType::Value type = Athena::TPCnvType::Athena) = 0;

}; 

#endif //> !ATHENAKERNEL_ITPCNVSVC_H
