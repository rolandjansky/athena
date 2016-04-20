// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataReadCnv/src/HVecCnv_v1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Test for xAOD schema evolution.
 */


#ifndef DATAMODELTESTDATAREADCNV_HVECCNV_V1_H
#define DATAMODELTESTDATAREADCNV_HVECCNV_V1_H


#include "DataModelTestDataRead/HVec.h"
#include "DataModelTestDataRead/versions/HVec_v1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"


namespace DMTest {


class HVecCnv_v1
  : public T_AthenaPoolTPCnvBase< DMTest::HVec,
                                  DMTest::HVec_v1 >
{
public:
   HVecCnv_v1();

   virtual void persToTrans( const DMTest::HVec_v1* oldObj,
                             DMTest::HVec* newObj,
                             MsgStream& log ) override;

   virtual void transToPers( const DMTest::HVec*,
                             DMTest::HVec_v1*,
                             MsgStream& log ) override;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAREADCNV_HVECCNV_V1_H
