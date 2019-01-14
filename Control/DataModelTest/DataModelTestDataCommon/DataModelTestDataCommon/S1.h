// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file DataModelTestDataCommon/S1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2017
 * @brief For symlink tests.
 */


#ifndef DATAMODELTESTDATACOMMON_S1_H
#define DATAMODELTESTDATACOMMON_S1_H


#include "AthenaKernel/CLASS_DEF.h"


namespace DMTest {


class S1
{
public:
  S1 (int x = 0) : m_x (x) {}
  virtual ~S1() {}
  int m_x;
};


} // namespace DMTest


CLASS_DEF (DMTest::S1, 243020043, 0)


#endif // not DATAMODELTESTDATACOMMON_S1_H
