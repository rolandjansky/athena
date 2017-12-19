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


#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"
#include "ByteStreamData/ByteStreamMetadataContainer.h"
#include "ByteStreamEventTPCnv/ByteStreamMetadataCnv_p1.h"


typedef T_AthenaPoolTPCnvCnv<ByteStreamMetadataContainer,
                             ByteStreamMetadataContainerCnv_p1>
  ByteStreamMetadataContainerCnv;


#endif
