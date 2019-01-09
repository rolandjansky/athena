// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
// $Id$
/**
 * @file DataModelTestDataCommon/S3.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2018
 * @brief For mixed condcont tests.
 */


#ifndef DATAMODELTESTDATACOMMON_S3_H
#define DATAMODELTESTDATACOMMON_S3_H


#include "AthenaKernel/CLASS_DEF.h"


namespace DMTest {


class S3
{
public:
  S3 (int x = 0) : m_x (x) {}
  virtual ~S3() {}
  int m_x;
};


} // namespace DMTest


CLASS_DEF (DMTest::S3, 131160325, 0)


#endif // not DATAMODELTESTDATACOMMON_S3_H
