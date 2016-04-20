// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataReadCnv/src/CVecCnv.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2016
 * @brief POOL coverter for CVec.
 */


#ifndef DATAMODELTESTDATAREADCNV_CVECCNV_H
#define DATAMODELTESTDATAREADCNV_CVECCNV_H


#include "DataModelTestDataCommon/CVec.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"


typedef T_AthenaPoolxAODCnv<DMTest::CVec> CVecCnv;


#endif // not DATAMODELTESTDATAREADCNV_CVECCNV_H
