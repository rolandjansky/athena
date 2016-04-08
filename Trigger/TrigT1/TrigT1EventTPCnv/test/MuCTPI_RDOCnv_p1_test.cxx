/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigT1EventTPCnv/test/MuCTPI_RDOCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for MuCTPI_RDOCnv_p1.
 */


#undef NDEBUG
#include "TrigT1EventTPCnv/MuCTPI_RDOCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const MuCTPI_RDO& p1,
              const MuCTPI_RDO& p2)
{
  assert (p1.getAllCandidateMultiplicities() == p2.getAllCandidateMultiplicities());
  assert (p1.dataWord() == p2.dataWord());
}


void testit (const MuCTPI_RDO& trans1)
{
  MsgStream log (0, "test");
  MuCTPI_RDOCnv_p1 cnv;
  MuCTPI_RDO_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  MuCTPI_RDO trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  MuCTPI_RDO trans1 (std::vector<unsigned int> {1, 2, 3},
                     std::vector<unsigned int> {7, 8, 9});
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
