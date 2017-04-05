/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataReadCnv/src/DataModelTestDataReadTPCnv.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Test for xAOD schema evolution.
 */


#include "AthenaKernel/TPCnvFactory.h"

#include "HVecCnv_v1.h"
#include "HAuxContainerCnv_v1.h"


DECLARE_TPCNV_FACTORY( DMTest::HVecCnv_v1,
                       DMTest::HVec,
                       DMTest::HVec_v1,
                       Athena::TPCnvVers::Old )
// Alternate names needed for BS reading.
DECLARE_TPCNV_FACTORY( DMTest::HVecCnv_v1,
                       DataVector<DMTest::H>,
                       DataVector<DMTest::H_v1>,
                       Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( DMTest::HAuxContainerCnv_v1,
                       DMTest::HAuxContainer,
                       DMTest::HAuxContainer_v1,
                       Athena::TPCnvVers::Old )
