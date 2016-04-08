// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataRead/HAuxContainer.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Test for xAOD schema evolution.
 */


#ifndef DATAMODELTESTDATAREAD_HAUXCONTAINER_H
#define DATAMODELTESTDATAREAD_HAUXCONTAINER_H


#include "DataModelTestDataRead/versions/HAuxContainer_v2.h"


namespace DMTest {


typedef HAuxContainer_v2 HAuxContainer;


} // namespace DMTest


#include "xAODCore/CLASS_DEF.h"
CLASS_DEF (DMTest::HAuxContainer, 9782, 1)


#endif // not DATAMODELTESTDATAREAD_HAUXCONTAINER_H
