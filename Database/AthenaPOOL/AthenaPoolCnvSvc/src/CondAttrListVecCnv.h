/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLCNVSVC_CONDATTRLISTVECCNV_H
#define ATHENAPOOLCNVSVC_CONDATTRLISTVECCNV_H

/** @file CondAttrListVecCnv.h
 *  @brief This file contains the class definition for the CondAttrListVecCnv class.
 *  @author Richard Hawkings <richard.hawkings@cern.ch>

 **/

#include "GaudiKernel/Converter.h"

class IOpaqueAddress;
class DataObject;
class StatusCode;
class IAddressCreator;

/// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/** @class CondAttrListVecCnv
 *  @brief This class provides the converter for the AthenaAttributeList class.
 *
 **/
class CondAttrListVecCnv : public Converter {
   friend class CnvFactory<CondAttrListVecCnv>;

public:
   /// Destructor
   virtual ~CondAttrListVecCnv();
   /// Gaudi Service Interface method implementations:
   virtual StatusCode initialize();

   /// Create a transient object from a POOL persistent representation.
   /// @param pAddr [IN] IOpaqueAddress of POOL persistent representation.
   /// @param pObj [OUT] pointer to the transient object.
   virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

   /// Create a POOL persistent representation for a transient object.
   /// @param pObj [IN] pointer to the transient object.
   /// @param pAddr [OUT] IOpaqueAddress of POOL persistent representation.
   virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

   /// @return storage type.
   virtual long int   repSvcType() const;

   /// @return storage type.
   static long storageType();

   /// @return class ID.
   static const CLID& classID();

protected:
   /// Constructor
   CondAttrListVecCnv(ISvcLocator* svcloc);
};
#endif // ATHENAPOOLCNVSVC_CONDATTRLISTVECCNV_H
