// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataRead/versions/GVec.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2014
 * @brief Test for xAOD auto schema evolution.
 */


#ifndef DATAMODELTESTDATAREAD_GVEC_H
#define DATAMODELTESTDATAREAD_GVEC_H


#include "DataModelTestDataRead/versions/GVec_v1.h"


namespace DMTest {


typedef GVec_v1 GVec;


} // namespace DMTest


#include "xAODCore/CLASS_DEF.h"
CLASS_DEF (DMTest::GVec, 9741, 1)


#endif // not DATAMODELTESTDATAREAD_GVEC_H
