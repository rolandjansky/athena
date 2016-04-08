// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataRead/versions/HView.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Test for xAOD schema evolution.
 */


#ifndef DATAMODELTESTDATAREAD_HVIEW_H
#define DATAMODELTESTDATAREAD_HVIEW_H


#include "DataModelTestDataRead/versions/HView_v2.h"


namespace DMTest {


typedef HView_v2 HView;


} // namespace DMTest


VIEWVECTOR_CLASS_DEF (DMTest::HView, 9787)


#endif // not DATAMODELTESTDATAREAD_HVIEW_H
