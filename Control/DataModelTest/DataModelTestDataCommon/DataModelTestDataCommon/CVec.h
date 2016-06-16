// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataCommon/CVec.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Class used for testing xAOD data reading/writing.
 */


#ifndef DATAMODELTESTDATACOMMON_CVEC_H
#define DATAMODELTESTDATACOMMON_CVEC_H


#include "DataModelTestDataCommon/versions/CVec_v1.h"


namespace DMTest {


typedef CVec_v1 CVec;


} // namespace DMTest


#include "xAODCore/CLASS_DEF.h"
CLASS_DEF (DMTest::CVec, 9721, 1)


#endif // not DATAMODELTESTDATACOMMON_CVEC_H
