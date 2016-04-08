// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataCommon/CVecWithData.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2015
 * @brief Test writing a container with metadata.
 */


#ifndef DATAMODELTESTDATACOMMON_CVECWITHDATA_H
#define DATAMODELTESTDATACOMMON_CVECWITHDATA_H


#include "DataModelTestDataCommon/versions/CVecWithData_v1.h"


namespace DMTest {


typedef CVecWithData_v1 CVecWithData;


} // namespace DMTest


#include "xAODCore/CLASS_DEF.h"
CLASS_DEF (DMTest::CVecWithData, 9771, 1)


#endif // not DATAMODELTESTDATACOMMON_CVECWITHDATA_H
