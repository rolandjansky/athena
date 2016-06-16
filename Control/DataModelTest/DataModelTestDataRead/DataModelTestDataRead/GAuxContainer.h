// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataRead/GAuxContainer.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2014
 * @brief Test for xAOD auto schema evolution.
 */


#ifndef DATAMODELTESTDATAREAD_GAUXCONTAINER_H
#define DATAMODELTESTDATAREAD_GAUXCONTAINER_H


#include "DataModelTestDataRead/versions/GAuxContainer_v1.h"


namespace DMTest {


typedef GAuxContainer_v1 GAuxContainer;


} // namespace DMTest


#include "xAODCore/CLASS_DEF.h"
CLASS_DEF (DMTest::GAuxContainer, 9742, 1)


#endif // not DATAMODELTESTDATAREAD_GAUXCONTAINER_H
