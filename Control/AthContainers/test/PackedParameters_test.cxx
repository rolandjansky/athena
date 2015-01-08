/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/test/PackedParameters_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2014
 * @brief Regression tests for PackedParameters
 */


#undef NDEBUG
#include "AthContainers/PackedParameters.h"
#include <cassert>
#include <iostream>


void test1()
{
  std::cout << "test1\n";

  SG::PackedParameters p1;
  assert (p1.nbits() == 32);
  assert (p1.nmantissa() == 23);
  assert (p1.scale() == 0);
  assert (p1.flags() == 0);
  assert (!p1.isSigned());
  assert (!p1.isFloat());
  assert (!p1.hasScale());
  assert (!p1.rounding());

  SG::PackedParameters p2 (27, SG::PackedParameters::FLAG_IS_SIGNED);
  assert (p2.nbits() == 27);
  assert (p2.nmantissa() == 26);
  assert (p2.scale() == 0);
  assert (p2.flags() == SG::PackedParameters::FLAG_IS_SIGNED);
  assert ( p2.isSigned());
  assert (!p2.isFloat());
  assert (!p2.hasScale());
  assert (!p2.rounding());

  SG::PackedParameters p3 (static_cast<uint16_t>(0));
  assert (p3.nbits() == 16);
  assert (p3.nmantissa() == 16);
  assert (p3.scale() == 0);
  assert (!p3.isSigned());
  assert (!p3.isFloat());
  assert (!p3.hasScale());
  assert (!p3.rounding());

  SG::PackedParameters p4 (static_cast<int32_t>(0));
  assert (p4.nbits() == 32);
  assert (p4.nmantissa() == 23);
  assert (p4.scale() == 0);
  assert ( p4.isSigned());
  assert (!p4.isFloat());
  assert (!p4.hasScale());
  assert (!p4.rounding());

  SG::PackedParameters p5 (static_cast<float>(0));
  assert (p5.nbits() == 32);
  assert (p5.nmantissa() == 23);
  assert (p5.scale() == 0);
  assert ( p5.isSigned());
  assert ( p5.isFloat());
  assert (!p5.hasScale());
  assert (!p5.rounding());

  SG::PackedParameters p6 (static_cast<double>(0));
  assert (p6.nbits() == 32);
  assert (p6.nmantissa() == 23);
  assert (p6.scale() == 0);
  assert ( p6.isSigned());
  assert ( p6.isFloat());
  assert (!p6.hasScale());
  assert (!p6.rounding());

  assert (p1.setNbits (16));
  assert (p1.nbits() == 16);
  assert (p1.nmantissa() == 16);
  assert (!p1.isSigned());
  assert (!p1.setNbits (0));
  assert (!p1.setNbits (33));
  assert (p1.nbits() == 16);
  assert (p1.setNbits (1));
  assert (p1.nbits() == 1);
  assert (p1.nmantissa() == 1);
  assert (!p1.isSigned());
  assert (!p1.setSigned(true));
  assert (p1.setNbits (7));
  assert (p1.setNmantissa (7));
  assert (p1.nbits() == 7);
  assert (p1.nmantissa() == 7);
  assert (!p1.isSigned());
  assert (p1.setSigned(true));
  assert (p1.nbits() == 7);
  assert (p1.nmantissa() == 6);
  assert (p1.isSigned());
  assert (p1.setSigned(false));
  assert (p1.nbits() == 7);
  assert (p1.nmantissa() == 6);
  assert (!p1.isSigned());
  assert (!p1.setNmantissa (0));
  assert (!p1.setNmantissa (8));
  assert (p1.nmantissa() == 6);

  assert (!p1.hasScale());
  assert (p1.scale() == 0);
  assert (p1.setScale(2));
  assert (p1.scale() == 2);
  assert (p1.hasScale());
  assert (p1.setScale(1));
  assert (p1.scale() == 1);
  assert (!p1.hasScale());
  assert (p1.setScale(0));
  assert (p1.scale() == 0);
  assert (!p1.hasScale());

  assert (!p1.rounding());
  assert (p1.setRounding(true));
  assert (p1.rounding());
  assert (p1.setRounding(false));
  assert (!p1.rounding());

  assert (!p1.isFloat());
  assert (p1.setFloat(true));
  assert (p1.isFloat());
  assert (p1.setFloat(false));
  assert (!p1.isFloat());
}


bool testvalid (const std::string& s)
{
  return SG::PackedParameters::isValidOption (SG::AuxDataOption (s, 0));
}
void test2()
{
  std::cout << "test2\n";

  assert (testvalid ("nbits"));
  assert (testvalid ("nmantissa"));
  assert (testvalid ("scale"));
  assert (testvalid ("signed"));
  assert (testvalid ("rounding"));
  assert (testvalid ("float"));
  assert (!testvalid ("xxx"));
}


void test3()
{
  std::cout << "test3\n";

  SG::PackedParameters p;
  
  assert (p.nbits() == 32);
  assert (p.setOption (SG::AuxDataOption ("nbits", 17)));
  assert (p.nbits() == 17);
  assert (!p.setOption (SG::AuxDataOption ("nbits", 33)));

  assert (p.nmantissa() == 17);
  assert (p.setOption (SG::AuxDataOption ("nmantissa", 13)));
  assert (p.nmantissa() == 13);
  assert (!p.setOption (SG::AuxDataOption ("nmantissa", 23)));

  assert (p.scale() == 0);
  assert (!p.hasScale());
  assert (p.setOption (SG::AuxDataOption ("scale", 2)));
  assert (p.scale() == 2);
  assert (p.hasScale());

  assert (!p.isSigned());
  assert (p.setOption (SG::AuxDataOption ("signed", 1)));
  assert (p.isSigned());
  assert (p.setOption (SG::AuxDataOption ("signed", 0)));
  assert (!p.isSigned());

  assert (!p.rounding());
  assert (p.setOption (SG::AuxDataOption ("rounding", 1)));
  assert (p.rounding());
  assert (p.setOption (SG::AuxDataOption ("rounding", 0)));
  assert (!p.rounding());

  assert (!p.isFloat());
  assert (p.setOption (SG::AuxDataOption ("float", 1)));
  assert (p.isFloat());
  assert (p.setOption (SG::AuxDataOption ("float", 0)));
  assert (!p.isFloat());

  assert (!p.setOption (SG::AuxDataOption ("xxx", 1)));
}


int main()
{
  test1();
  test2();
  test3();
  return 0;
}
