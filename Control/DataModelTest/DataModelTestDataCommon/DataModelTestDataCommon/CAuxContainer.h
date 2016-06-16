// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataCommon/CAuxContainer.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Class used for testing xAOD data reading/writing.
 */


#ifndef DATAMODELTESTDATACOMMON_CAUXCONTAINER_H
#define DATAMODELTESTDATACOMMON_CAUXCONTAINER_H


#include "DataModelTestDataCommon/versions/CAuxContainer_v1.h"


namespace DMTest {


typedef CAuxContainer_v1 CAuxContainer;


} // namespace DMTest


#include "xAODCore/CLASS_DEF.h"
CLASS_DEF (DMTest::CAuxContainer, 9722, 1)


#endif // not DATAMODELTESTDATACOMMON_CAUXCONTAINER_H
