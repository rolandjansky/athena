// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataReadCnv/src/HVecCnv.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Test for xAOD schema evolution.
 */


#ifndef DATAMODELTESTDATAREADCNV_HVECCNV_H
#define DATAMODELTESTDATAREADCNV_HVECCNV_H


#include "HVecCnv_v1.h"
#include "DataModelTestDataRead/HVec.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"


typedef T_AthenaPoolxAODCnv<DMTest::HVec, DMTest::HVecCnv_v1> HVecCnv;


#endif // not DATAMODELTESTDATAREADCNV_HVECCNV_H
