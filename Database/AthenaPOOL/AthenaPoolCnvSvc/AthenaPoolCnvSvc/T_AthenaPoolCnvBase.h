/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

protected:
   /// Constructor
   T_AthenaPoolCnvBase(ISvcLocator* svcloc);

   /// Gaudi Service Interface method implementations:
   virtual StatusCode initialize();

   /// Write an object into POOL.
   /// @param pObj [IN] pointer to the transient object.
   /// @param key [IN] StoreGate key (string) - placement hint to generate POOL container name
   virtual StatusCode DataObjectToPool(DataObject* pObj, const std::string& key);

   /// Read an object from POOL.
   /// @param pObj [OUT] pointer to the transient object.
   /// @param token [IN] POOL token of the persistent representation.
   virtual StatusCode PoolToDataObject(DataObject*& pObj, const Token* token);

   /// Set POOL placement.
   virtual void setPlacement(const std::string& key = "");

public:
   /// @return class ID.
   static const CLID& classID();
};


#include "AthenaPoolCnvSvc/T_AthenaPoolCnvBase.icc"

#endif
