// Dear emacs, this is -*- c++ -*-
/* Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration */
#ifndef XAODEVENTFORMATCNV_TEST_XAODAAUXCONTAINERCNV_H
#define XAODEVENTFORMATCNV_TEST_XAODAAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// EDM include(s):
#include "xAODEventFormatCnv/test/AAuxContainer.h"
#include "xAODAAuxContainerCnv_v1.h"

/// Base class for the converter
typedef T_AthenaPoolAuxContainerCnv< xAODMakerTest::AAuxContainer,
                                     xAODAAuxContainerCnv_v1 >
   xAODAAuxContainerCnv;


#endif  // XAODEVENTFORMATCNV_TEST_XAODAAUXCONTAINERCNV_H
