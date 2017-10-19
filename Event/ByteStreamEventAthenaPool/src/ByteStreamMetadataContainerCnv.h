/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMMETADATACONTAINERCNV_H
#define BYTESTREAMMETADATACONTAINERCNV_H

/** @file ByteStreamEventAthenaPool/src/ByteStreamMetadataContainerCnv.h
 *  @brief This file contains the class definition for the ByteStreamMetadataContainerCnv class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ByteStreamMetadataContainerCnv.h,v 1.3 2009-03-18 17:48:04 gemmeren Exp $
 **/

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "ByteStreamData/ByteStreamMetadataContainer.h"
#include "ByteStreamEventTPCnv/ByteStreamMetadata_p1.h"

/// the latest persistent representation type of ByteStreamMetadataContainer
typedef ByteStreamMetadataContainer_p1 ByteStreamMetadataContainer_PERS;
typedef T_AthenaPoolCustomCnv<ByteStreamMetadataContainer, ByteStreamMetadataContainer_PERS> ByteStreamMetadataContainerCnvBase;

/** @class ByteStreamMetadataContainerCnv
 *  @brief This class provides a converter for the ByteStreamMetadataContainer class.
 **/
class ByteStreamMetadataContainerCnv : public ByteStreamMetadataContainerCnvBase {

friend class CnvFactory<ByteStreamMetadataContainerCnv>;

protected:
   ByteStreamMetadataContainerCnv(ISvcLocator* svcloc) : ByteStreamMetadataContainerCnvBase(svcloc) {}

   virtual ByteStreamMetadataContainer_PERS* createPersistent(ByteStreamMetadataContainer* transObj);
   virtual ByteStreamMetadataContainer* createTransient();
};

#endif
