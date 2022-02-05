/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthContainers/test/exceptions_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2013
 * @brief Regression tests for exceptions.
 */


#undef NDEBUG
#include "AthContainers/exceptions.h"
#include "AthContainers/AuxTypeRegistry.h"
#include <iostream>


void test1()
{
  std::cout << "test1\n";

  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  SG::auxid_t id1 = r.getAuxID<int> ("foo");
  SG::auxid_t id2 = r.getAuxID<float> ("bar", "cl");

  std::cout << SG::ExcNoAuxStore(id1).what() << "\n";
  std::cout << SG::ExcNoAuxStore(id2).what() << "\n";
  std::cout << SG::ExcNoAuxStore(SG::null_auxid).what() << "\n";
  std::cout << SG::ExcNoAuxStore("op").what() << "\n";

  std::cout << SG::ExcBadAuxVar(id1).what() << "\n";

  std::cout << SG::ExcConstAuxData("someop", id1).what() << "\n";
  std::cout << SG::ExcConstAuxData("otherop").what() << "\n";

  std::cout << SG::ExcUntrackedSetStore().what() << "\n";

  std::cout << SG::ExcBadPrivateStore("op").what() << "\n";

  std::cout << SG::ExcAuxTypeMismatch(id1, typeid(int), typeid(float)).what() << "\n";
  std::cout << SG::ExcAuxTypeMismatch(id2, typeid(int), typeid(float)).what() << "\n";

  std::cout << SG::ExcInsertionInBaseClass("op", typeid(int), typeid(float)).what() << "\n";

  std::cout << SG::ExcStoreLocked(id1).what() << "\n";
  std::cout << SG::ExcStoreLocked("op").what() << "\n";
  std::cout << SG::ExcNonowningContainer().what() << "\n";
  std::cout << SG::ExcUnknownAuxItem("foo").what() << "\n";
  std::cout << SG::ExcUnknownAuxItem("foo", "bar").what() << "\n";
  std::cout << SG::ExcUnknownAuxItem("foo", "bar", &typeid(int)).what() << "\n";
  std::cout << SG::ExcDVToELV("why").what() << "\n";
  std::cout << SG::ExcViewVectorNotView().what() << "\n";
  std::cout << SG::ExcMissingViewVectorCLID(typeid(int)).what() << "\n";
  std::cout << SG::ExcInsertMoveOwnershipMismatch().what() << "\n";
  std::cout << SG::ExcAtomicMismatch(id2, typeid(int)).what() << "\n";
  std::cout << SG::ExcInvalidThinningTarget(1234, "asd").what() << "\n";
  std::cout << SG::ExcBadIterSwap().what() << "\n";
  std::cout << SG::ExcAllocOwnership().what() << "\n";
}


int main()
{
  test1();
  return 0;
}
