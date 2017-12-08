///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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


class ITPCnvBase
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 
#if GAUDI_VERSION > CALC_GAUDI_VERSION(25, 3) 
  typedef Gaudi::PluginService::Factory<ITPCnvBase*> Factory;
#else  
  typedef Gaudi::PluginService::Factory0<ITPCnvBase*> Factory;
#endif
  /// Destructor: 
  virtual ~ITPCnvBase(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

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

  /** Convert transient object representation to persistent
      @param trans [IN] void* pointer to the transient object
      @param pers [OUT] void* pointer to the empty persistent object
      @param log [IN] output message stream
  */  
  virtual
  void 
  transToPersUntyped(const void* trans, void* pers, MsgStream& msg) = 0;

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

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
//std::ostream& operator<<( std::ostream& out, const ITPCnvBase& o );



#endif //> !ATHENAKERNEL_ITPCNVBASE_H
