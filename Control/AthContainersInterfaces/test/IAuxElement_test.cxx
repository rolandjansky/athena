/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthContainersInterfaces/test/IAuxElement_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2021
 * @brief Regression test for IAuxElement.
 */


#undef NDEBUG
#include "AthContainersInterfaces/IAuxElement.h"
#include <iostream>
#include <cassert>


namespace SG {

class ConstAuxElement
  : public IAuxElement
{
public:
  using IAuxElement::IAuxElement;
  using IAuxElement::setIndex;
  using IAuxElement::noPrivateData;
  using IAuxElement::havePrivateData;
  using IAuxElement::hadPrivateData;
  using IAuxElement::setNoPrivateData;
  using IAuxElement::setHavePrivateData;
  using IAuxElement::setHadPrivateData;
};


} // namespace SG


void test1()
{
  std::cout << "test1\n";

  SG::ConstAuxElement e1;
  assert (e1.index() == 0);
  assert (e1.noPrivateData());
  assert (!e1.havePrivateData());
  assert (!e1.hadPrivateData());

  SG::ConstAuxElement e2 (10);
  assert (e2.index() == 10);
  assert (e2.noPrivateData());
  assert (!e2.havePrivateData());
  assert (!e2.hadPrivateData());

  e2.setIndex (11);
  assert (e2.index() == 11);
  assert (e2.noPrivateData());
  assert (!e2.havePrivateData());
  assert (!e2.hadPrivateData());

  e2.setHavePrivateData();
  assert (e2.index() == 11);
  assert (!e2.noPrivateData());
  assert (e2.havePrivateData());
  assert (!e2.hadPrivateData());

  e2.setHadPrivateData();
  assert (e2.index() == 11);
  assert (!e2.noPrivateData());
  assert (!e2.havePrivateData());
  assert (e2.hadPrivateData());

  e2.setNoPrivateData();
  assert (e2.index() == 11);
  assert (e2.noPrivateData());
  assert (!e2.havePrivateData());
  assert (!e2.hadPrivateData());
}


int main()
{
  test1();
  return 0;
}

