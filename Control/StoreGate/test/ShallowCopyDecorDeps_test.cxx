/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file StoreGate/test/ShallowCopyDecorDeps_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2020
 * @brief Tests for ShallowCopyDecorDeps.
 */


#undef NDEBUG
#include "StoreGate/ShallowCopyDecorDeps.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/TestStore.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "TestTools/initGaudi.h"
#include "TestOwner.h"
#include <cassert>
#include <iostream>


class MyObj {};
CLASS_DEF (MyObj, 293847295, 1)


void test1 (ISvcLocator* svcloc)
{
  std::cout << "test1\n";

  TestOwner owner;
  SG::ShallowCopyDecorDeps<MyObj> scdd (&owner, "SCDD", {"d1", "d2"}, "doc string");
  SG::ReadHandleKey<MyObj> origKey (&owner, "ReadKey", "myObj");
  SG::WriteHandleKey<MyObj> copyKey (&owner, "WriteKey", "scopy_myObj");

  assert( origKey.initialize().isSuccess() );
  assert( copyKey.initialize().isSuccess() );
  assert( scdd.initialize (origKey, copyKey) );

  assert (owner.getProperty ("SCDD").name() == "SCDD");
  assert (owner.getProperty ("SCDD").documentation() == "doc string");
  assert (owner.getProperty ("SCDD").type_info() == &typeid(SG::VarHandleKeyArray));
  assert (owner.getProperty ("SCDD").toString() == "['StoreGateSvc+scopy_myObj.d1','StoreGateSvc+scopy_myObj.d2']");
  assert (owner.getProperty ("SCDD").ownerTypeName() == "TestOwner");

  assert (owner.getProperty ("SCDDReadKeys").name() == "SCDDReadKeys");
  assert (owner.getProperty ("SCDDReadKeys").documentation() == "[Internal property]");
  assert (owner.getProperty ("SCDDReadKeys").type_info() == &typeid(SG::VarHandleKeyArray));
  assert (owner.getProperty ("SCDDReadKeys").toString() == "['StoreGateSvc+myObj.d1','StoreGateSvc+myObj.d2']");
  assert (owner.getProperty ("SCDDReadKeys").ownerTypeName() == "TestOwner");

  StoreGateSvc* sg = nullptr;
  assert (svcloc->service ("StoreGateSvc", sg).isSuccess());
  assert (sg->record (std::make_unique<MyObj>(), "myObj", false).isSuccess());

  EventContext ctx;
  ctx.setExtension( Atlas::ExtendedEventContext(sg) );

  assert( scdd.linkDecors (origKey, ctx).isSuccess() );

  const MyObj* myObj = nullptr;
  assert (sg->retrieve (myObj, "myObj").isSuccess());
  assert (myObj != nullptr);

  const MyObj* myObj2 = nullptr;
  assert (sg->retrieve (myObj2, "scopy_myObj.d1").isSuccess());
  assert (myObj == myObj2);

  myObj2 = nullptr;
  assert (sg->retrieve (myObj2, "scopy_myObj.d2").isSuccess());
  assert (myObj == myObj2);
}


int main()
{
  std::cout << "StoreGate/ShallowCopyDecorDeps_test\n";

  ISvcLocator* svcloc = nullptr;
  if (!Athena_test::initGaudi("StoreGate/VarHandleBase_test.txt", svcloc)) {
    return 1;
  }

  test1 (svcloc);
  return 0;
}
