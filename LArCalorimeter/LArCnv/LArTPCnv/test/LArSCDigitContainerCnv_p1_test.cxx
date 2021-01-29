/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include "LArTPCnv/LArSCDigitContainerCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>

void compare (const LArSCDigit& p1,
              const LArSCDigit& p2)
{
  assert (p1.channelID() == p2.channelID());
  assert (p1.gain() == p2.gain());
  assert (p1.samples() == p2.samples());
}


void compare (const LArSCDigitContainer& p1,
              const LArSCDigitContainer& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i=0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void testit (const LArSCDigitContainer& trans1)
{
  MsgStream log (0, "test");
  LArSCDigitContainerCnv_p1 cnv;
  LArSCDigitContainer_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  LArSCDigitContainer trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}

void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  LArSCDigitContainer trans1;
  for (int i=0; i < 10; i++) {
    short o = i*10;
    
    trans1.push_back (new LArSCDigit (HWIdentifier (1234+o),
                                      234+o, 123+o,
                                    std::vector<short> {
                                        (short)(1+o),
                                        (short)(2+o),
                                        (short)(3+o)},
                                    std::vector<unsigned short> {
                                        (unsigned short)(5+o),
                                        (unsigned short)(6+o),
                                        (unsigned short)(7+o)}
                                      ));
                                      
  }
    
  testit (trans1);
}


int main()
{
  std::cout << "starting\n"<<std::endl;
  test1();
  return 0;
}
