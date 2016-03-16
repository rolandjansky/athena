/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file InDetEventTPCnv/test/SiWidthCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "InDetEventTPCnv/InDetPrepRawData/SiWidthCnv_p1.h"
#include "InDetEventTPCnv/PixelClusterContainerCnv_tlp1.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const InDet::SiWidth& p1,
              const InDet::SiWidth& p2)
{
  assert (p1.colRow() == p2.colRow());
  assert (p1.widthPhiRZ() == p2.widthPhiRZ());
}


void testit (const InDet::SiWidth& trans1)
{
  MsgStream log (0, "test");
  SiWidthCnv_p1 cnv;
  PixelClusterContainerCnv_tlp1 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  InDet::SiWidth_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  InDet::SiWidth trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  InDet::SiWidth trans1 (Amg::Vector2D (1, 2),
                         Amg::Vector2D (3.5, 4.5));

  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
