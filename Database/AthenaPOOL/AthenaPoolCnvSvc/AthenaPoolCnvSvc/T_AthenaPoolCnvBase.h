/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLCNVSVC_T_ATHENAPOOLCNVBASE_H
#define ATHENAPOOLCNVSVC_T_ATHENAPOOLCNVBASE_H

/** @file T_AthenaPoolCnvBase.h
 *  @brief his file contains the class definition for the templated T_AthenaPoolCnvBase class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaPoolCnvSvc/AthenaPoolConverter.h"

#include <string>

class DataObject;
class StatusCode;

/** @class T_AthenaPoolCnvBase
 *  @brief This templated class provides the converter to translate an object to/from its persistent POOL representation.
 **/
template <class T>
class T_AthenaPoolCnvBase : public AthenaPoolConverter {
public:
   /// Constructor
   T_AthenaPoolCnvBase(ISvcLocator* svcloc,
                       const char* name = nullptr);

protected:

   /// Gaudi Service Interface method implementations:
   virtual StatusCode initialize();

   /// Convert an object into Persistent.
   /// @param pObj [IN] pointer to the transient object.
   virtual StatusCode DataObjectToPers(DataObject* pObj, IOpaqueAddress*& pAddr);

   /// Write an object into POOL.
   /// @param pObj [IN] pointer to the transient object.
   virtual StatusCode DataObjectToPool(IOpaqueAddress* pAddr, DataObject* pObj);

   /// Read an object from POOL.
   /// @param pObj [OUT] pointer to the transient object.
   /// @param token [IN] POOL token of the persistent representation.
   /// @param key [IN] SG key of the object being read.
   virtual StatusCode PoolToDataObject(DataObject*& pObj, const Token* token,
                                       const std::string& key);

   /// Set POOL placement.
   virtual Placement setPlacement(const std::string& key, const std::string& output);

public:
   /// @return class ID.
   static const CLID& classID();
};


#include "AthenaPoolCnvSvc/T_AthenaPoolCnvBase.icc"

#endif
