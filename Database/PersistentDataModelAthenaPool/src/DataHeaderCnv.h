/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAHEADERCNV_H
#define DATAHEADERCNV_H

/**
 *  @file DataHeaderCnv.h
 *  @brief This file contains the class definition for the DataHeaderCnv class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: DataHeaderCnv.h,v 1.9 2009-04-21 22:04:51 gemmeren Exp $
 **/

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "PersistentDataModel/DataHeader.h"
#include "PersistentDataModelTPCnv/DataHeader_p5.h"
#include "PersistentDataModelTPCnv/DataHeaderCnv_p5.h"

/** @class DataHeaderCnv
 *  @brief This class provides the converter to customize the saving of DataHeader.
 **/
class DataHeaderCnv : public T_AthenaPoolCustomCnv<DataHeader, DataHeader_p5> {
   friend class CnvFactory<DataHeaderCnv>;
public:
   DataHeaderCnv(ISvcLocator* svcloc);
   ~DataHeaderCnv();

   /// Extend base-class conversion methods
   virtual StatusCode updateRep(IOpaqueAddress* pAddress, DataObject* pObject);
   virtual StatusCode updateRepRefs(IOpaqueAddress* pAddress, DataObject* pObject);

   virtual StatusCode DataObjectToPool(DataObject* pObj, const std::string& tname);

   virtual DataHeader_p5* poolReadObject_p5();

   virtual DataHeader_p5* createPersistent(DataHeader* transObj);
   virtual DataHeader* createTransient();

private:
   DataHeaderCnv_p5 m_TPconverter;
   RootType m_mapClassDesc;
   mutable std::string m_dhFormMdx;
   mutable DataHeaderForm_p5* m_dhForm;
};

#endif
