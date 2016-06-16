// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/versions/HVec_v1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Test for xAOD schema evolution.
 */


#ifndef DATAMODELTESTDATAWRITE_HVEC_V1_H
#define DATAMODELTESTDATAWRITE_HVEC_V1_H


#include "DataModelTestDataWrite/versions/H_v1.h"
#include "AthContainers/DataVector.h"


namespace DMTest {


typedef DataVector<H_v1> HVec_v1;


} // namespace DMTest


#endif // not DATAMODELTESTDATAWRITE_HVEC_V1_H
