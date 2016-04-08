// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataCommon/CView.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Class used for testing ViewVector reading/writing.
 */


#ifndef DATAMODELTESTDATACOMMON_CVIEW_H
#define DATAMODELTESTDATACOMMON_CVIEW_H


#include "DataModelTestDataCommon/versions/CView_v1.h"


namespace DMTest {


typedef CView_v1 CView;


} // namespace DMTest


#include "xAODCore/CLASS_DEF.h"
VIEWVECTOR_CLASS_DEF (DMTest::CView, 9779)


#endif // not DATAMODELTESTDATACOMMON_CVIEW_H
