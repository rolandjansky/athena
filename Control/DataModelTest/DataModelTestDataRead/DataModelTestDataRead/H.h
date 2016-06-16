// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataRead/H.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Test for xAOD schema evolution.
 */


#ifndef DATAMODELTESTDATAREAD_H_H
#define DATAMODELTESTDATAREAD_H_H


#include "DataModelTestDataRead/versions/H_v2.h"


namespace DMTest {


typedef H_v2 H;


} // namespace DMTest


#include "xAODCore/CLASS_DEF.h"
CLASS_DEF (DMTest::H, 9784, 1)


#endif // not DATAMODELTESTDATAREAD_H_H
