// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/versions/HView.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Test for xAOD schema evolution.
 */


#ifndef DATAMODELTESTDATAWRITE_HVIEW_H
#define DATAMODELTESTDATAWRITE_HVIEW_H


#include "DataModelTestDataWrite/versions/HView_v1.h"


namespace DMTest {


typedef HView_v1 HView;


} // namespace DMTest


VIEWVECTOR_CLASS_DEF (DMTest::HView, 9787)


#endif // not DATAMODELTESTDATAWRITE_HVIEW_H
