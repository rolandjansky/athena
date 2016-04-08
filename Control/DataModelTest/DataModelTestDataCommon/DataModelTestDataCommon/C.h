// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataCommon/C.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Class used for testing xAOD data reading/writing.
 */


#ifndef DATAMODELTESTDATACOMMON_C_H
#define DATAMODELTESTDATACOMMON_C_H


#include "DataModelTestDataCommon/versions/C_v1.h"


namespace DMTest {


typedef C_v1 C;


} // namespace DMTest


#include "xAODCore/CLASS_DEF.h"
CLASS_DEF (DMTest::C, 9724, 1)


#endif // not DATAMODELTESTDATACOMMON_C_H
