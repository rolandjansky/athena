/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigCombinedEventTPCnv/test/TrigCompositeCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigCompositeCnv_p1.
 */


#undef NDEBUG
#include "TrigCombinedEventTPCnv/TrigCompositeCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


bool operator== (const TrigFeatureLink& l1,
                 const TrigFeatureLink& l2)
{
  if (l1.clid() != l2.clid()) return false;
  if (l1.subTypeIndex() != l2.subTypeIndex()) return false;
  if (l1.index() != l2.index()) return false;
  return true;
}


void compare (const TrigComposite& p1,
              const TrigComposite& p2)
{
  assert (p1.name() == p2.name());
  assert (p1.allDetails<TrigFeatureLink>() ==
          p2.allDetails<TrigFeatureLink>());
  assert (p1.allDetails<float>() ==
          p2.allDetails<float>());
  assert (p1.allDetails<int>() ==
          p2.allDetails<int>());
  assert (p1.allDetails<std::string>() ==
          p2.allDetails<std::string>());
  assert (p1.allDetails<std::vector<float> >() ==
          p2.allDetails<std::vector<float> >());
  assert (p1.allDetails<std::vector<int> >() ==
          p2.allDetails<std::vector<int> >());
  assert (p1.allDetails<std::vector<std::string> >() ==
          p2.allDetails<std::vector<std::string> >());
}


void testit (const TrigComposite& trans1)
{
  MsgStream log (0, "test");
  TrigCompositeCnv_p1 cnv;
  TrigComposite_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigComposite trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TrigComposite trans1 ("comp");
  trans1.addDetail ("i1", 1);
  trans1.addDetail ("i2", 2);
  trans1.addDetail ("f1", 3.5f);
  trans1.addDetail ("f2", 4.5f);
  trans1.addDetail ("f3", 5.5f);
  trans1.addDetail ("s1", std::string("s1val"));
  trans1.addDetail ("iv1", std::vector<int>{6, 7});
  trans1.addDetail ("iv2", std::vector<int>{9, 9, 10});
  trans1.addDetail ("fv1", std::vector<float>{11.5});
  trans1.addDetail ("fv2", std::vector<float>{12.5, 13.5});
  trans1.addDetail ("fv3", std::vector<float>{14.5, 15.5, 16.5});
  trans1.addDetail ("sv1", std::vector<std::string>{"s", "t"});

  trans1.addObject ("l1", TrigFeatureLink (123, 10, 1));
  trans1.addObject ("l2", TrigFeatureLink (124, 11, 2));
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
