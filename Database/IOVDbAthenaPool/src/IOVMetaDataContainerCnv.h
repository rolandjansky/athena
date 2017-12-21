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

#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"
#include "IOVDbDataModel/IOVMetaDataContainer.h"
#include "IOVDbTPCnv/IOVMetaDataContainerCnv_p1.h"


typedef T_AthenaPoolTPCnvCnv<IOVMetaDataContainer, IOVMetaDataContainerCnv_p1>
  IOVMetaDataContainerCnv;


#endif // IOVDBATHENAPOOL_IOVMETADATACONTAINERCNV_H

