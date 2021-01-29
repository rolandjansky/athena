/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include "LArTPCnv/LArRawSCContainerCnv_p1.h"
#include "LArRawEvent/LArRawSC.h"
#include "LArRawEvent/LArRawSCContainer.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const LArRawSC& p1,
              const LArRawSC& p2)
{
  assert (p1.hardwareID() == p2.hardwareID());
  assert (p1.chan() == p2.chan());
  assert (p1.energies() == p2.energies());
  assert (p1.bcids() == p2.bcids());
}


void compare (const LArRawSCContainer& p1,
              const LArRawSCContainer& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i=0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void testit (const LArRawSCContainer& trans1)
{
  MsgStream log (0, "test");
  LArRawSCContainerCnv_p1 cnv;
  LArRawSCContainer_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  LArRawSCContainer trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  LArRawSCContainer trans1;
  for (int i=0; i < 10; i++) {
    short o = i*10;
    trans1.push_back (new LArRawSC (HWIdentifier (1234+o),
                                    (short)(23+o),(unsigned int)(14+o),
                                    std::vector<int> {
                                        (int)(1+o),
                                        (int)(2+o),
                                        (int)(3+o)},
                                    std::vector<unsigned short> {
                                        (unsigned short)(5+o),
                                        (unsigned short)(6+o),
                                        (unsigned short)(7+o)},
                                    std::vector<bool> {
                                        true,
                                        false,
                                        true}
                                        ));
  }
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
