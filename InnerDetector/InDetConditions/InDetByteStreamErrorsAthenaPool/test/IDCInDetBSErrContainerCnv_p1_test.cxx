/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#undef NDEBUG
#include "../src/IDCInDetBSErrContainerCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>
#include <cassert>




void compare (const IDCInDetBSErrContainer& c1,
              const IDCInDetBSErrContainer& c2)
{
  assert (c1.maxSize() == c2.maxSize());
  auto v1 = c1.getAll();
  auto v2 = c2.getAll();
  assert(v1==v2);
}

void testit (const IDCInDetBSErrContainer& trans1)
{
  IDCInDetBSErrContainerCnv_p1 cnv;
  InDetBSErrContainer64_p1 pers;
  MsgStream log (nullptr, "test");
  cnv.transToPers (&trans1, &pers, log);
  IDCInDetBSErrContainer trans2 (10, -9999);
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  IDCInDetBSErrContainer c(10, -9999);

  c.setOrDrop (1, 2);
  c.setOrDrop (3, 4);
  c.setOrDrop (5, 6);
  testit (c);
}


int main()
{
  test1();
  return 0;
}
