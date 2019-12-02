///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ITPCnvBase.h 
// Header file for class ITPCnvBase
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAKERNEL_ITPCNVBASE_H
#define ATHENAKERNEL_ITPCNVBASE_H 1

#include "Gaudi/PluginService.h"
#include "GAUDI_VERSION.h"
#include <typeinfo>

// Forward declaration
class MsgStream;

/** @class  ITPCnvBase
*/
class ITPCnvBase
{ 
 public: 
  typedef Gaudi::PluginService::Factory<ITPCnvBase*()> Factory;

  /// Destructor: 
  virtual ~ITPCnvBase() = default;


  // Methods for invoking conversions on objects given by generic
  // pointers.

  /** Convert persistent object representation to transient
      @param pers [IN] void* pointer to the persistent object
      @param trans [OUT] void* pointer to the empty transient object
      @param log [IN] output message stream
  */
  virtual 
  void 
  persToTransUntyped(const void* pers, void* trans, MsgStream& msg) = 0;


  /** Convert persistent object representation to transient
      @param pers [IN] void* pointer to the persistent object
      @param trans [OUT] void* pointer to the empty transient object
      @param key [IN] SG key of object being read.
      @param log [IN] output message stream
  */
  virtual 
  void 
  persToTransWithKeyUntyped(const void* pers,
                            void* trans,
                            const std::string& /*key*/,
                            MsgStream& msg)
  {
    return persToTransUntyped (pers, trans, msg);
  }


 /** Convert transient object representation to persistent
      @param trans [IN] void* pointer to the transient object
      @param pers [OUT] void* pointer to the empty persistent object
      @param log [IN] output message stream
  */  
  virtual
  void 
  transToPersUntyped(const void* trans, void* pers, MsgStream& msg) = 0;


  /** Convert transient object representation to persistent
      @param trans [IN] void* pointer to the transient object
      @param pers [OUT] void* pointer to the empty persistent object
      @param key [IN] SG key of object being written.
      @param log [IN] output message stream
  */  
  virtual
  void 
  transToPersWithKeyUntyped(const void* trans,
                            void* pers,
                            const std::string& /*key*/,
                            MsgStream& msg)
  {
    return transToPersUntyped (trans, pers, msg);
  }

  
  /** return C++ type id of the transient class this converter is for
      @return std::type_info&
  */
  virtual 
  const std::type_info& transientTInfo() const = 0;

  /** return C++ type id of the persistent class this converter is for
      @return std::type_info&
  */
  virtual
  const std::type_info& persistentTInfo() const = 0;
}; 




#endif //> !ATHENAKERNEL_ITPCNVBASE_H
