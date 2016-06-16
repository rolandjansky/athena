// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/versions/GVec_v1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2014
 * @brief Test for xAOD auto schema evolution.
 */


#ifndef DATAMODELTESTDATAWRITE_GVEC_V1_H
#define DATAMODELTESTDATAWRITE_GVEC_V1_H


#include "DataModelTestDataWrite/versions/G_v1.h"
#include "AthContainers/DataVector.h"


namespace DMTest {


typedef DataVector<G_v1> GVec_v1;


} // namespace DMTest



#endif // not DATAMODELTESTDATAWRITE_GVEC_V1_H
