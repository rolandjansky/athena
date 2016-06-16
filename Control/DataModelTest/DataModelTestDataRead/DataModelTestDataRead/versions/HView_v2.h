// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataRead/versions/HView_v2.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Test for xAOD schema evolution.
 */


#ifndef DATAMODELTESTDATAREAD_HVIEW_V2_H
#define DATAMODELTESTDATAREAD_HVIEW_V2_H


#include "DataModelTestDataRead/versions/HVec_v2.h"
#include "AthContainers/ViewVector.h"


namespace DMTest {


typedef ViewVector<HVec_v2> HView_v2;


} // namespace DMTest


#endif // not DATAMODELTESTDATAREAD_HVIEW_V2_H
