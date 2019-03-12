/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMDATA_BYTESTREAMMETADATACONTAINER_H
#define BYTESTREAMDATA_BYTESTREAMMETADATACONTAINER_H

/** @file ByteStreamMetadataContainer.h
 *  @brief This file contains the class definition for the ByteStreamMetadataContainer class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ByteStreamMetadataContainer.h,v 1.2 2009-04-02 14:47:40 gemmeren Exp $
 **/

#include "ByteStreamData/ByteStreamMetadata.h"
#include "AthContainers/DataVector.h"

/** @class ByteStreamMetadataContainer 
 *  @brief This class is the StoreGate data object for bytestream metadata. 
 **/
class ByteStreamMetadataContainer : public DataVector<ByteStreamMetadata> {
};

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(ByteStreamMetadataContainer, 1076128893, 1)
#ifndef XAOD_STANDALONE
#include "AthenaKernel/MetaCont.h"
CLASS_DEF( MetaCont<ByteStreamMetadataContainer> , 1174128703 , 1 )
#include "SGTools/BaseInfo.h"
SG_BASE( MetaCont<ByteStreamMetadataContainer>, MetaContBase );
#endif // not XAOD_STANDALONE

#endif
