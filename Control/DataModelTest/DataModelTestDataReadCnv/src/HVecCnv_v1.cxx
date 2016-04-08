/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataReadCnv/src/HVecCnv_v1.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Test for xAOD schema evolution.
 */


#include "HVecCnv_v1.h"
#include "DataModelTestDataRead/H.h"


namespace DMTest {


HVecCnv_v1::HVecCnv_v1()
{
}


void HVecCnv_v1::persToTrans( const DMTest::HVec_v1* oldObj,
                              DMTest::HVec* newObj,
                              MsgStream& /*log*/ )
{
  newObj->clear();
  for (size_t i = 0; i < oldObj->size(); i++)
    newObj->push_back (new DMTest::H);
}


void HVecCnv_v1::transToPers( const DMTest::HVec*,
                              DMTest::HVec_v1*,
                              MsgStream&  )
{
  std::abort();
}


} // namespace DMTest
