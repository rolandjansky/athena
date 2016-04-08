// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/H.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Test for xAOD schema evolution.
 */


#ifndef DATAMODELTESTDATAWRITE_H_H
#define DATAMODELTESTDATAWRITE_H_H


#include "DataModelTestDataWrite/versions/H_v1.h"


namespace DMTest {


typedef H_v1 H;


} // namespace DMTest


#include "xAODCore/CLASS_DEF.h"
CLASS_DEF (DMTest::H, 9784, 1)


#endif // not DATAMODELTESTDATAWRITE_H_H
