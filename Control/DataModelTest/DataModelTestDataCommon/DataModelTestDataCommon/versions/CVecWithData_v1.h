// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataCommon/versions/CVecWithData_v1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2015
 * @brief Test writing a container with metadata.
 */


#ifndef DATAMODELTESTDATACOMMON_CVECWITHDATA_V1_H
#define DATAMODELTESTDATACOMMON_CVECWITHDATA_V1_H


#include "DataModelTestDataCommon/versions/CVec_v1.h"


namespace DMTest {


class CVecWithData_v1
  : public CVec_v1
{
public:
  CVecWithData_v1() : meta1(0) {}
  int meta1;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATACOMMON_CVECWITHDATA_V1_H
