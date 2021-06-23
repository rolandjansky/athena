/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/TPCnvFactory.h"
#include "ByteStreamEventTPCnv/ByteStreamMetadataCnv_p1.h"

DECLARE_TPCNV_FACTORY(ByteStreamMetadataCnv_p1,
                      ByteStreamMetadata,
                      ByteStreamMetadata_p1,
                      Athena::TPCnvVers::Current)
