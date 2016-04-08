// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataCommon/versions/CView_v1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Class used for testing ViewVector reading/writing.
 */


#ifndef DATAMODELTESTDATACOMMON_CVIEW_V1_H
#define DATAMODELTESTDATACOMMON_CVIEW_V1_H


#include "DataModelTestDataCommon/versions/CVec_v1.h"
#include "AthContainers/ViewVector.h"


namespace DMTest {


typedef ViewVector<CVec_v1> CView_v1;


} // namespace DMTest


#endif // not DATAMODELTESTDATACOMMON_CVIEW_V1_H
