/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AFP_EventTPCnv/test/AFP_SiDigiCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2019
 * @brief Tests for AFP_SiDigiCnv_p1.
 */


#undef NDEBUG
#include "AFP_EventTPCnv/AFP_SiDigiCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const AFP_SiDigi& p1,
              const AFP_SiDigi& p2)
{
  assert ( p1.m_fADC        ==  p2.m_fADC);
  assert ( p1.m_fTDC        ==  p2.m_fTDC);
  assert ( p1.m_nStationID  ==  p2.m_nStationID);
  assert ( p1.m_nDetectorID ==  p2.m_nDetectorID);
  assert ( p1.m_nPixelRow   ==  p2.m_nPixelRow);
  assert ( p1.m_nPixelCol   ==  p2.m_nPixelCol);
}


void testit (const AFP_SiDigi& trans1)
{
  MsgStream log (0, "test");
  AFP_SiDigiCnv_p1 cnv;
  AFP_SiDigi_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  AFP_SiDigi trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  AFP_SiDigi trans1;
  trans1.m_fADC = 1.5;
  trans1.m_fTDC = 2.5;
  trans1.m_nStationID = 3;
  trans1.m_nDetectorID = 4;
  trans1.m_nPixelRow = 5;
  trans1.m_nPixelCol = 6;

  testit (trans1);
}


int main()
{
  std::cout << "AFP_EventTPCnv/AFP_SiDigiCnv_p1_test\n";
  test1();
  return 0;
}
