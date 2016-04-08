/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/src/xAODTestWriteHelper.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Test recording ViewVector w/o having CLASS_DEF visible in
 *        compilation unit.
 */


#include "xAODTestWriteHelper.h"


namespace DMTest {


StatusCode recordView1 (StoreGateSvc* svc,
                        std::unique_ptr<ViewVector<DMTest::CVec> > view,
                        const std::string& key)
{
  return svc->record (std::move(view), key, false);
}


StatusCode recordView2 (StoreGateSvc* svc,
                        std::unique_ptr<DMTest::HVec> view,
                        const std::string& key)
{
  return svc->record (std::move(view), key, false);
}


} // namespace DMTest
