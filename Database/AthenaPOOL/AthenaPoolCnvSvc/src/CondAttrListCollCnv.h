/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLCNVSVC_CONDATTRLISTCOLLECTION_H
#define ATHENAPOOLCNVSVC_CONDATTRLISTCOLLECTION_H

/** @file CondAttrListCollCnv.h
 *  @brief This file contains the class definition for the CondAttrListCollCnv class.
 *  @author RD Schaffer <R.D.Schaffer@cern.ch>
 *  $Id: CondAttrListCollCnv.h,v 1.2 2008-12-12 05:47:07 ssnyder Exp $
 **/

#include "GaudiKernel/Converter.h"

class IOpaqueAddress;
class DataObject;
class StatusCode;
class IAddressCreator;

/// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/** @class CondAttrListCollCnv
 *  @brief This class provides the converter for the CondAttrListCollection class.
 *
 **/
class CondAttrListCollCnv : public Converter {
   friend class CnvFactory<CondAttrListCollCnv>;

public:
   /// Destructor
   virtual ~CondAttrListCollCnv();
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
   CondAttrListCollCnv(ISvcLocator* svcloc);
};
#endif
