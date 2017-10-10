/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMEVENTATHENAPOOL_BYTESTREAMMETADATACNV_P1_H
#define BYTESTREAMEVENTATHENAPOOL_BYTESTREAMMETADATACNV_P1_H

/** @file ByteStreamEventTPCnv/ByteStreamMetadataCnv_p1.h
 *  @brief This file contains the class definition for the ByteStreamMetadataCnv_p1 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ByteStreamMetadataCnv_p1.h,v 1.1 2009-03-18 17:42:44 gemmeren Exp $
 **/

#include "ByteStreamData/ByteStreamMetadataContainer.h"
#include "ByteStreamEventTPCnv/ByteStreamMetadata_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

/** @class ByteStreamMetadataCnv_p1
 *  @brief This class provides the converter to customize the saving of ByteStreamMetadata_p1.
 **/
class ByteStreamMetadataCnv_p1 : public T_AthenaPoolTPCnvBase<ByteStreamMetadata, ByteStreamMetadata_p1>  {
public:
   ByteStreamMetadataCnv_p1() {}

   virtual void persToTrans(const ByteStreamMetadata_p1* persObj, ByteStreamMetadata* transObj, MsgStream &log);
   virtual void transToPers(const ByteStreamMetadata* transObj, ByteStreamMetadata_p1* persObj, MsgStream &log);
};

template<>
class T_TPCnv<ByteStreamMetadata, ByteStreamMetadata_p1> : public ByteStreamMetadataCnv_p1 {
public:
};

class ByteStreamMetadataContainerCnv_p1 : public T_AthenaPoolTPCnvVector<ByteStreamMetadataContainer, ByteStreamMetadataContainer_p1, ByteStreamMetadataCnv_p1> {
};

template<>
class T_TPCnv<ByteStreamMetadataContainer, ByteStreamMetadataContainer_p1> : public ByteStreamMetadataContainerCnv_p1 {
public:
};

#endif
