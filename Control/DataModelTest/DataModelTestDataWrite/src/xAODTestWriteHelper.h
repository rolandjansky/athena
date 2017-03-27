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
#include "StoreGate/WriteHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthContainers/ViewVector.h"
#include "AthContainers/ConstDataVector.h"


namespace DMTest {


StatusCode recordView1 (SG::WriteHandle<ConstDataVector<ViewVector<DMTest::CVec> > >& h,
                        std::unique_ptr<ConstDataVector<ViewVector<DMTest::CVec> > > view);


StatusCode recordView2 (SG::WriteHandle<ViewVector<DMTest::HVec> >& h,
                        std::unique_ptr<ViewVector<DMTest::HVec> > view);


} // namespace DMTest



#endif // not DATAMODELTESTDATAWRITE_XAODTESTWRITEHELPER_H
