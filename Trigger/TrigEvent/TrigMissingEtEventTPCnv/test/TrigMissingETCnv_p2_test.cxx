/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigMissingEtEventTPCnv/test/TrigMissingETCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigMissingETCnv_p2.
 */


#undef NDEBUG
#include "TrigMissingEtEventTPCnv/TrigMissingETCnv_p2.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>
#include <sstream>


void compare (const TrigMissingET& p1,
              const TrigMissingET& p2)
{
  assert (p1.ex() == p2.ex());
  assert (p1.ey() == p2.ey());
  assert (p1.ez() == p2.ez());
  assert (p1.sumEt() == p2.sumEt());
  assert (p1.sumE() == p2.sumE());
  assert (p1.getFlag() == p2.getFlag());
  assert (p1.RoIword() == p2.RoIword());
  assert (p1.getNumOfComponents() == p2.getNumOfComponents());
  for (unsigned int i = 0; i < p1.getNumOfComponents(); i++) {
    assert (p1.getNameOfComponent(i) == p2.getNameOfComponent(i));
    assert (p1.getExComponent(i) == p2.getExComponent(i));
    assert (p1.getEyComponent(i) == p2.getEyComponent(i));
    assert (p1.getEzComponent(i) == p2.getEzComponent(i));
    assert (p1.getSumEtComponent(i) == p2.getSumEtComponent(i));
    assert (p1.getSumEComponent(i) == p2.getSumEComponent(i));
    assert (p1.getSumOfSigns(i) == p2.getSumOfSigns(i));
    assert (p1.getUsedChannels(i) == p2.getUsedChannels(i));
    assert (p1.getStatus(i) == p2.getStatus(i));
    assert (p1.getComponentCalib0(i) == p2.getComponentCalib0(i));
    assert (p1.getComponentCalib1(i) == p2.getComponentCalib1(i));
  }
}


void testit (const TrigMissingET& trans1)
{
  MsgStream log (0, "test");
  TrigMissingETCnv_p2 cnv;
  TrigMissingET_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigMissingET trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TrigMissingET trans1 (4);
  trans1.setEx (40000);
  trans1.setEy (41000);
  trans1.setEz (42000);
  trans1.setSumEt (43000);
  trans1.setSumE (44000);
  trans1.setFlag (123);
  trans1.set_RoIword (234);
  
  for (int i=0; i < 4; i++) {
    std::ostringstream os;
    os << "comp" << i;
    trans1.setNameOfComponent(i, os.str().c_str());
    trans1.setExComponent (i, 30000 + i*100);
    trans1.setEyComponent (i, 30010 + i*100);
    trans1.setEzComponent (i, 30020 + i*100);
    trans1.setSumEtComponent (i, 30030 + i*100);
    trans1.setSumEComponent (i, 30040 + i*100);
    trans1.setComponentCalib0 (i, 30050 + i*100);
    trans1.setComponentCalib1 (i, 30060 + i*100);
    trans1.setSumOfSigns (i, i*100);
    trans1.setUsedChannels (i, 1000 + i*100);
    trans1.setStatus (i, 2000 + i*100);
  }
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
