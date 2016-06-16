// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/versions/GVec.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2014
 * @brief Test for xAOD auto schema evolution.
 */


#ifndef DATAMODELTESTDATAWRITE_GVEC_H
#define DATAMODELTESTDATAWRITE_GVEC_H



#include "DataModelTestDataWrite/versions/GVec_v1.h"


namespace DMTest {


typedef GVec_v1 GVec;


} // namespace DMTest


#include "xAODCore/CLASS_DEF.h"
CLASS_DEF (DMTest::GVec, 9741, 1)


#endif // not DATAMODELTESTDATAWRITE_GVEC_H
