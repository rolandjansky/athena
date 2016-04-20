// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CAuxContainerCnv.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2016
 * @brief POOL converter for CAuxContainer.
 */


#ifndef DATAMODELTESTDATAREADCNV_CAUXCONTAINERCNV_H
#define DATAMODELTESTDATAREADCNV_CAUXCONTAINERCNV_H


#include "DataModelTestDataCommon/CAuxContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"


typedef T_AthenaPoolAuxContainerCnv<DMTest::CAuxContainer> CAuxContainerCnv;


#endif // not DATAMODELTESTDATAREADCNV_CAUXCONTAINERCNV_H
