// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/src/xAODTestWriteHelper.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Test recording ViewVector w/o having CLASS_DEF visible in
 *        compilation unit.
 */


#ifndef DATAMODELTESTDATAWRITE_XAODTESTWRITEHELPER_H
#define DATAMODELTESTDATAWRITE_XAODTESTWRITEHELPER_H


#include "DataModelTestDataCommon/CVec.h"
#include "DataModelTestDataWrite/HVec.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthContainers/ViewVector.h"


namespace DMTest {


StatusCode recordView1 (StoreGateSvc* svc,
                        std::unique_ptr<ViewVector<DMTest::CVec> > view,
                        const std::string& key);


StatusCode recordView2 (StoreGateSvc* svc,
                        std::unique_ptr<DMTest::HVec> view,
                        const std::string& key);


} // namespace DMTest



#endif // not DATAMODELTESTDATAWRITE_XAODTESTWRITEHELPER_H
