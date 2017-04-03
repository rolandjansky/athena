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


StatusCode recordView1 (SG::WriteHandle<ConstDataVector<ViewVector<DMTest::CVec> > >& h,
                        std::unique_ptr<ConstDataVector<ViewVector<DMTest::CVec> > > view)
{
  return h.record (std::move (view));
}


StatusCode recordView2 (SG::WriteHandle<ViewVector<DMTest::HVec> >& h,
                        std::unique_ptr<ViewVector<DMTest::HVec> > view)
{
  return h.record (std::move (view));
}


} // namespace DMTest
