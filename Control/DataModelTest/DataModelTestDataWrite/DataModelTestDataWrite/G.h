// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/G.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2014
 * @brief Test for xAOD auto schema evolution.
 */


#ifndef DATAMODELTESTDATAWRITE_G_H
#define DATAMODELTESTDATAWRITE_G_H


#include "DataModelTestDataWrite/versions/G_v1.h"


namespace DMTest {


typedef G_v1 G;


} // namespace DMTest



#include "xAODCore/CLASS_DEF.h"
CLASS_DEF (DMTest::G, 9744, 1)


#endif // not DATAMODELTESTDATAWRITE_G_H
