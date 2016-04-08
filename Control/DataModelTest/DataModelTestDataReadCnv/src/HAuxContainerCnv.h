// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataReadCnv/src/HAuxContainerCnv.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Test for xAOD schema evolution.
 */


#ifndef DATAMODELTESTDATAREADCNV_HAUXCONTAINERCNV_H
#define DATAMODELTESTDATAREADCNV_HAUXCONTAINERCNV_H


#include "DataModelTestDataRead/HAuxContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"
#include "HAuxContainerCnv_v1.h"

typedef T_AthenaPoolAuxContainerCnv<DMTest::HAuxContainer, DMTest::HAuxContainerCnv_v1> HAuxContainerCnv;

#endif // not DATAMODELTESTDATAREADCNV_HAUXCONTAINERCNV_H
