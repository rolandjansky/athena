/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataReadCnv/src/HAuxContainerCnv_v1.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Test for xAOD schema evolution.
 */


#include "HAuxContainerCnv_v1.h"
#include "DataModelTestDataRead/versions/HVec_v1.h"
#include "DataModelTestDataRead/H.h"
#include "DataModelTestDataRead/HVec.h"


namespace DMTest {


HAuxContainerCnv_v1::HAuxContainerCnv_v1()
{
}


void HAuxContainerCnv_v1::persToTrans( const DMTest::HAuxContainer_v1* oldObj,
                                       DMTest::HAuxContainer* newObj,
                                       MsgStream& /*log*/ )
{
  newObj->resize(0);

  DMTest::HVec_v1 oldInt;
  for (size_t i = 0; i < oldObj->size(); i++)
    oldInt.push_back (new DMTest::H_v1);
  oldInt.setStore (oldObj);

  DMTest::HVec newInt;
  newInt.setStore (newObj);

  for( const DMTest::H_v1* oldh : oldInt ) {
    DMTest::H* newh = new DMTest::H;
    newInt.push_back( newh );

    newh->setAFloat (oldh->anInt() + 0.5);
  }
}


void HAuxContainerCnv_v1::transToPers( const DMTest::HAuxContainer*,
                                       DMTest::HAuxContainer_v1*,
                                       MsgStream&  )
{
  std::abort();
}


} // namespace DMTest
