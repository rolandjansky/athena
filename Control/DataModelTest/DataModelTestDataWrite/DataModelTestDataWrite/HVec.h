// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/versions/HVec.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Test for xAOD schema evolution.
 */


#ifndef DATAMODELTESTDATAWRITE_HVEC_H
#define DATAMODELTESTDATAWRITE_HVEC_H


#include "DataModelTestDataWrite/versions/HVec_v1.h"


namespace DMTest {


typedef HVec_v1 HVec;


} // namespace DMTest


#include "xAODCore/CLASS_DEF.h"
CLASS_DEF (DMTest::HVec, 9781, 1)


#endif // not DATAMODELTESTDATAWRITE_HVEC_H
