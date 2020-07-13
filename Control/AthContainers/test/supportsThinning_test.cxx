/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthContainers/test/supportsThinning_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2019
 * @brief Tests for supportsThinning.
 */


#undef NDEBUG
#include "AthContainers/tools/supportsThinning.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/AuxStoreInternal.h"
#include <cassert>
#include <iostream>


struct A {};
struct B : public DataVector<int> {};
struct B0 : public DataVector<int> { static bool constexpr supportsThinning = false; };
struct C : public SG::AuxElement {};
// cppcheck-suppress duplInheritedMember
struct C0 : public SG::AuxElement { static bool constexpr supportsThinning = false; };
struct D : public SG::AuxStoreInternal {};
struct D0 : public SG::AuxStoreInternal { static bool constexpr supportsThinning = false; };


int main()
{
  std::cout << "AthContainers/supportsThinning_test\n";

  assert (!SG::xAODInterfaceSupportingThinning<A>());
  assert (!SG::xAODStoreSupportingThinning<A>());

  assert ( SG::xAODInterfaceSupportingThinning<B>());
  assert (!SG::xAODStoreSupportingThinning<B>());

  assert (!SG::xAODInterfaceSupportingThinning<B0>());
  assert (!SG::xAODStoreSupportingThinning<B0>());

  assert ( SG::xAODInterfaceSupportingThinning<C>());
  assert (!SG::xAODStoreSupportingThinning<C>());

  assert (!SG::xAODInterfaceSupportingThinning<C0>());
  assert (!SG::xAODStoreSupportingThinning<C0>());

  assert (!SG::xAODInterfaceSupportingThinning<D>());
  assert ( SG::xAODStoreSupportingThinning<D>());

  assert (!SG::xAODInterfaceSupportingThinning<D0>());
  assert (!SG::xAODStoreSupportingThinning<D0>());
  return 0;
}
