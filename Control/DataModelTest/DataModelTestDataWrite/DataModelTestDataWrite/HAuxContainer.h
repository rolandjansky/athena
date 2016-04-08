// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/HAuxContainer.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Test for xAOD schema evolution.
 */


#ifndef DATAMODELTESTDATAWRITE_HAUXCONTAINER_H
#define DATAMODELTESTDATAWRITE_HAUXCONTAINER_H


#include "DataModelTestDataWrite/versions/HAuxContainer_v1.h"


namespace DMTest {


typedef HAuxContainer_v1 HAuxContainer;


} // namespace DMTest


#include "xAODCore/CLASS_DEF.h"
CLASS_DEF (DMTest::HAuxContainer, 9782, 1)


#endif // not DATAMODELTESTDATAWRITE_HAUXCONTAINER_H
