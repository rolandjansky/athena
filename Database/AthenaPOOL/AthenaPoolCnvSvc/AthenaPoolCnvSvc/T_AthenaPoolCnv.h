/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLCNVSVC_T_ATHENAPOOLCNV_H
#define ATHENAPOOLCNVSVC_T_ATHENAPOOLCNV_H

/** @file T_AthenaPoolCnv.h
 *  @brief his file contains the class definition for the templated T_AthenaPoolCnv class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaPoolCnvSvc/AthenaPoolConverter.h"

#include <string>

class DataObject;
class StatusCode;

/// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/** @class T_AthenaPoolCnv
 *  @brief This templated class provides the converter to translate an object to/from its persistent POOL representation.
 **/
template <class T>
class T_AthenaPoolCnv : public AthenaPoolConverter {
   friend class CnvFactory<T_AthenaPoolCnv<T> >;

protected:
   /// Constructor
   T_AthenaPoolCnv(ISvcLocator* svcloc);

   /// Gaudi Service Interface method implementations:
   virtual StatusCode initialize();

   /// Write an object into POOL.
   /// @param pObj [IN] pointer to the transient object.
   /// @param key [IN] StoreGate key (string) - placement hint to generate POOL container name
   virtual StatusCode DataObjectToPool(DataObject* pObj, const std::string& key);

   /// Read an object from POOL.
   /// @param pObj [OUT] pointer to the transient object.
   /// @param token [IN] POOL token of the persistent representation.
   virtual StatusCode PoolToDataObject(DataObject*& pObj, const std::string& token);

   /// Set POOL placement.
   virtual void setPlacement(const std::string& key = "");

public:
   /// @return class ID.
   static const CLID& classID();
};

#include "AthenaPoolCnvSvc/T_AthenaPoolCnv.icc"
#endif
