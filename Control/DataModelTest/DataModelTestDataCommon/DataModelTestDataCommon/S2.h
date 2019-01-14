// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file DataModelTestDataCommon/S2.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2017
 * @brief For symlink tests.
 */


#ifndef DATAMODELTESTDATACOMMON_S2_H
#define DATAMODELTESTDATACOMMON_S2_H


#include "DataModelTestDataCommon/S1.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/BaseInfo.h"


namespace DMTest {


class S2
  : public S1
{
public:
  S2 (int x = 0) : S1(x) {}
};


} // namespace DMTest


CLASS_DEF (DMTest::S2, 243020042, 0)
SG_ADD_BASE (DMTest::S2, DMTest::S1);


#endif // not DATAMODELTESTDATACOMMON_S2_H
