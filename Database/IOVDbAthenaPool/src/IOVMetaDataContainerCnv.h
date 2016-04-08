/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file IOVMetaDataContainerCnv.h
 * @brief AthenaPool converter for IOVMetaDataContainer
 * @author R.D.Schaffer@cern.ch
 */

#ifndef IOVDBATHENAPOOL_IOVMETADATACONTAINERCNV_H
#define IOVDBATHENAPOOL_IOVMETADATACONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "IOVDbDataModel/IOVMetaDataContainer.h"
#include "IOVDbTPCnv/IOVMetaDataContainer_p1.h"

// the latest persistent representation type of IOVMetaDataContainer
typedef  IOVMetaDataContainer_p1  IOVMetaDataContainer_PERS;
typedef  T_AthenaPoolCustomCnv<IOVMetaDataContainer, IOVMetaDataContainer_PERS >   IOVMetaDataContainerCnvBase;

class IOVMetaDataContainerCnv : public IOVMetaDataContainerCnvBase {
friend class CnvFactory<IOVMetaDataContainerCnv >;
protected:
  IOVMetaDataContainerCnv (ISvcLocator* svcloc) : IOVMetaDataContainerCnvBase(svcloc) {}
  virtual IOVMetaDataContainer_PERS*   createPersistent (IOVMetaDataContainer* transObj);
  virtual IOVMetaDataContainer*        createTransient ();
};



#endif // IOVDBATHENAPOOL_IOVMETADATACONTAINERCNV_H

