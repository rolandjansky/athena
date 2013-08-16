/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "IOVDbTPCnv/IOVMetaDataContainerCnv_p1.h"
#include "IOVDbTPCnv/IOVPayloadContainerCnv_p1.h"
#include "IOVDbTPCnv/IOVMetaDataContainer_p1.h"
#include "IOVDbTPCnv/IOVPayloadContainer_p1.h"
#include "IOVDbTPCnv/AttrListIndexes.h"

DECLARE_TPCNV_FACTORY(IOVMetaDataContainerCnv_p1,
                      IOVMetaDataContainer,
                      IOVMetaDataContainer_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(IOVPayloadContainerCnv_p1,
                      IOVPayloadContainer,
                      IOVPayloadContainer_p1,
                      Athena::TPCnvVers::Current)
                      
