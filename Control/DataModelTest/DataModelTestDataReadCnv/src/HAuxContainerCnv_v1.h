// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataReadCnv/src/HAuxContainerCnv_v1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Test for xAOD schema evolution.
 */


#ifndef DATAMODELTESTDATAREADCNV_HAUXCONTAINERCNV_V1_H
#define DATAMODELTESTDATAREADCNV_HAUXCONTAINERCNV_V1_H


#include "DataModelTestDataRead/HAuxContainer.h"
#include "DataModelTestDataRead/versions/HAuxContainer_v1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"


namespace DMTest {


class HAuxContainerCnv_v1
  : public T_AthenaPoolTPCnvBase< DMTest::HAuxContainer,
                                  DMTest::HAuxContainer_v1 >
{
public:
   HAuxContainerCnv_v1();

   virtual void persToTrans( const DMTest::HAuxContainer_v1* oldObj,
                             DMTest::HAuxContainer* newObj,
                             MsgStream& log ) override;

   virtual void transToPers( const DMTest::HAuxContainer*,
                             DMTest::HAuxContainer_v1*,
                             MsgStream& log ) override;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAREADCNV_HAUXCONTAINERCNV_V1_H
