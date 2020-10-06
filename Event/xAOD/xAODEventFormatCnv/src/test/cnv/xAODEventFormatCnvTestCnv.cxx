/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODEventFormatCnv/testAthenaPoolTPCnv.cxx 619884 2014-10-03 15:00:40Z christos $

// Gaudi/Athena include(s):
#include "AthenaKernel/TPCnvFactory.h"

// EDM include(s):
#include "xAODEventFormatCnv/test/AAuxContainer.h"
#include "xAODEventFormatCnv/test/versions/AAuxContainer_v1.h"

// Local include(s):
#include "xAODAAuxContainerCnv_v1.h"

// Declare the T/P converter(s):
DECLARE_TPCNV_FACTORY( xAODAAuxContainerCnv_v1,
                       xAODMakerTest::AAuxContainer,
                       xAODMakerTest::AAuxContainer_v1,
                       Athena::TPCnvVers::Old )
