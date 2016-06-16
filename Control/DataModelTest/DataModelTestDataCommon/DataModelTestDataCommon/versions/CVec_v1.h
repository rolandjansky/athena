// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataCommon/versions/CVec_v1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Class used for testing xAOD data reading/writing.
 */


#ifndef DATAMODELTESTDATACOMMON_CVEC_V1_H
#define DATAMODELTESTDATACOMMON_CVEC_V1_H


#include "DataModelTestDataCommon/versions/C_v1.h"
#include "AthContainers/DataVector.h"


namespace DMTest {


typedef DataVector<C_v1> CVec_v1;


} // namespace DMTest



#endif // not DATAMODELTESTDATACOMMON_CVEC_V1_H
