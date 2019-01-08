/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file AthContainers/test/AtomicConstAccessor_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2018
 * @brief Regression tests for AtomicConstAccessor.
 */


#undef NDEBUG
#include "AthContainers/tools/AtomicConstAccessor.h"
#include "AthContainers/tools/AtomicDecorator.h"
#include "AthContainers/AuxElement.h"
#include "AthContainers/AuxStoreInternal.h"
#include "TestTools/expect_exception.h"
#include <iostream>
#include <cassert>


namespace SG {

class AuxVectorBase
  : public SG::AuxVectorData
{
public:
  virtual size_t size_v() const { return 10; }
  virtual size_t capacity_v() const { return 10; }

  using SG::AuxVectorData::setStore;
  void set (SG::AuxElement& b, size_t index)
  {
    b.setIndex (index, this);
  }
  void clear (SG::AuxElement& b)
  {
    b.setIndex (0, 0);
  }

  static
  void clearAux (SG::AuxElement& b)
  {
    b.clearAux();
  }

  static
  void copyAux (SG::AuxElement& a, const SG::AuxElement& b)
  {
    a.copyAux (b);
  }

  static
  void testAuxElementCtor (SG::AuxVectorData* container,
                           size_t index)
  {
    SG::AuxElement bx (container, index);
    assert (bx.index() == index);
    assert (bx.container() == container);
  }
};


} // namespace SG


void test1()
{
  std::cout << "test1\n";
  SG::AuxElement b;
  SG::AuxVectorBase v;
  v.set (b, 5);

  SG::AuxStoreInternal store;
  v.setStore (&store);

  SG::AtomicConstAccessor<int> ityp2 ("anInt2");

  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  SG::auxid_t ityp2_id = r.getAuxID<int> ("anInt2", "",
                                          SG::AuxTypeRegistry::Flags::Atomic);
  assert (ityp2.auxid() == ityp2_id);

  static_assert (std::is_same<decltype(ityp2(b)),
                              int>::value, "test");

  SG::AtomicDecorator<int> ityp2_d ("anInt2");
  ityp2_d(b) = 11;
  assert (11 == ityp2(b));

  SG::AtomicConstAccessor<int> ityp3 ("anInt3");
  SG::AtomicDecorator<int> ityp3_d ("anInt3");

  const SG::AuxElement& cb = b;
  ityp3_d(cb) = 12;
  assert (12 == ityp3(cb));

  assert (*(ityp3.getDataArray (v)+5) == ityp3(cb));
  static_assert (std::is_same<decltype(ityp3.getDataArray(v)),
                              const std::atomic<int>*>::value, "test");

  EXPECT_EXCEPTION (SG::ExcAtomicMismatch,
                    SG::AuxElement::Accessor<int> ityp3a ("anInt3"));
}


int main()
{
  std::cout << "AtomicConstAccessor_test\n";
  test1();
  return 0;
}
