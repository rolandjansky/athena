/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file JetTagInfoTPCnv/test/ISvxAssociationCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2018
 * @brief Regression tests.
 */

#undef NDEBUG
#include "JetTagInfoTPCnv/ISvxAssociationCnv_p1.h"
#include "JetTagInfoTPCnv/ISvxAssociation_p1.h"
#include "JetTagInfoTPCnv/JetTagInfoCnv_tlp3.h"
#include "TestTools/leakcheck.h"
#include "TestTools/initGaudi.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const JetAssociationBase& p1,
              const JetAssociationBase& p2)
{
  assert (p1.name() == p2.name());
  assert (p1.keyIndex() == p2.keyIndex());
}


void compare (const Analysis::ISvxAssociation& p1,
              const Analysis::ISvxAssociation& p2)
{
  compare (static_cast<const JetAssociationBase&>(p1),
           static_cast<const JetAssociationBase&>(p2));
  // Not saved.
  assert (p2.vertexInfo() == nullptr);
}


void testit (const Analysis::ISvxAssociation& trans1)
{
  MsgStream log (0, "test");
  Analysis::ISvxAssociationCnv_p1 cnv;
  Analysis::JetTagInfoCnv_tlp3 tlcnv;
  cnv.setTopConverter (&tlcnv, TPObjRef::typeID_t());
  Analysis::ISvxAssociation_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Analysis::ISvxAssociation trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Analysis::ElectronAssociation trans0b ("name1");

  Athena_test::Leakcheck check;

  Analysis::ISvxAssociation trans1 ("name1");
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "JetTagInfoTPCnv/ISvxAssociationCnv_p1_test\n";

  test1();
  return 0;
}
