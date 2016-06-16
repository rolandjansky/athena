// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataRead/versions/HVec.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Test for xAOD schema evolution.
 */


#ifndef DATAMODELTESTDATAREAD_HVEC_H
#define DATAMODELTESTDATAREAD_HVEC_H


#include "DataModelTestDataRead/versions/HVec_v2.h"


namespace DMTest {


typedef HVec_v2 HVec;


} // namespace DMTest


#include "xAODCore/CLASS_DEF.h"
CLASS_DEF (DMTest::HVec, 9781, 1)


#endif // not DATAMODELTESTDATAREAD_HVEC_H
