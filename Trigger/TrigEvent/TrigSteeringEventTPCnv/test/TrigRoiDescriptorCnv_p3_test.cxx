/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigSteeringEventTPCnv/test/TrigRoiDescriptorCnv_p3_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigRoiDescriptorCnv_p3.
 */


#undef NDEBUG
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCnv_p3.h"
#include "TestTools/random.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const IRoiDescriptor& p1,
              const IRoiDescriptor& p2)
{
  assert (p1.phi() == p2.phi());
  assert (p1.eta() == p2.eta());
  assert (p1.zed() == p2.zed());
  assert (p1.phiPlus() == p2.phiPlus());
  assert (p1.etaPlus() == p2.etaPlus());
  assert (p1.zedPlus() == p2.zedPlus());
  assert (p1.phiMinus() == p2.phiMinus());
  assert (p1.etaMinus() == p2.etaMinus());
  assert (p1.zedMinus() == p2.zedMinus());
  assert (p1.isFullscan() == p2.isFullscan());
  assert (p1.composite() == p2.composite());
  assert (p1.version() == p2.version());

  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (*p1.at(i), *p2.at(i));
}


void compare (const TrigRoiDescriptor& p1,
              const TrigRoiDescriptor& p2)
{
  compare (static_cast<const IRoiDescriptor&>(p1),
           static_cast<const IRoiDescriptor&>(p2));

  assert (0 == p2.l1Id());
  assert (p1.roiId() == p2.roiId());
  assert (p1.roiWord() == p2.roiWord());
  assert (true == p2.manageConstituents());
}


void testit (const TrigRoiDescriptor& trans1)
{
  MsgStream log (0, "test");
  TrigRoiDescriptorCnv_p3 cnv;
  TrigRoiDescriptor_p3 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigRoiDescriptor trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TrigRoiDescriptor trans1 (123, 234, 345,
                            2.5, 2.4, 2.6,
                            1.5, 1.3, 1.7,
                            6.5, 6.0, 7.0);
  trans1.manageConstituents (true);

  for (int i=0; i < 10; i++) {
    trans1.push_back( new TrigRoiDescriptor( 1.1 + i*0.1,
                                             2.1 + i*0.1,
                                             3.1 + i*0.1,
                                             -3.9 + i*0.1,
                                             -2.9 + i*0.1,
                                             -1.9 + i*0.1,
                                             10.1 + i*0.1,
                                             10.2 + i*0.1,
                                             10.2 + i*0.1) );
  }

  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
