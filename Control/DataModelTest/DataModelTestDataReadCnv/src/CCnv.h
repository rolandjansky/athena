// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataReadCnv/src/CCnv.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2016
 * @brief POOL coverter for C.
 */


#ifndef DATAMODELTESTDATAREADCNV_CCNV_H
#define DATAMODELTESTDATAREADCNV_CCNV_H


#include "DataModelTestDataCommon/C.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"


typedef T_AthenaPoolxAODCnv<DMTest::C> CCnv;


#endif // not DATAMODELTESTDATAREADCNV_CCNV_H
