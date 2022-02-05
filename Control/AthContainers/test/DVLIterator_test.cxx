/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file DVLIterator_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2010
 * @brief Regression tests for DVLIterator.
 */


#undef NDEBUG


#include "AthContainers/tools/DVLIterator.h"
#include "AthContainers/exceptions.h"
#include <vector>
#include <list>
#include <cassert>


struct X1
{
  X1(int the_x) : x(the_x) {}
  virtual ~X1() {}
  int x;
};
struct X2 : virtual public X1
{
  X2(int the_x) : X1(the_x) {}
};

struct Cont1
{
  typedef Cont1 Cont;
  typedef X1 base_value_type;
  typedef std::vector<X1*> BaseContainer;
  static constexpr bool has_virtual = false;
  static constexpr bool must_own = false;

  typedef base_value_type* value_type;
  typedef const base_value_type* const_value_type;
  typedef base_value_type** pointer;
  typedef std::unique_ptr<X1> unique_type;
  typedef DataModel_detail::ElementProxy<Cont> ElementProxy;
  typedef DataModel_detail::const_iterator<Cont> const_iterator;

  Cont1 (SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS,
         bool can_insert = true)
    : m_ownPolicy (ownPolicy),
      m_can_insert (can_insert)
  {}

  SG::OwnershipPolicy ownPolicy() const { return m_ownPolicy; }
  void testInsert (const char* op)
  {
    if (!m_can_insert)
      throw SG::ExcInsertionInBaseClass (op, typeid(int), typeid(int));
  }

  SG::OwnershipPolicy m_ownPolicy;
  bool m_can_insert;
};


struct Cont2
{
  typedef Cont2 Cont;
  typedef X2 base_value_type;
  typedef std::vector<X1*> BaseContainer;
  static constexpr bool has_virtual = true;
  static constexpr bool must_own = false;

  typedef base_value_type* value_type;
  typedef const base_value_type* const_value_type;
  typedef base_value_type** pointer;
  typedef std::unique_ptr<X1> unique_type;
  typedef DataModel_detail::ElementProxy<Cont> ElementProxy;
  typedef DataModel_detail::const_iterator<Cont> const_iterator;

  Cont2 (SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS,
         bool can_insert = true)
    : m_ownPolicy (ownPolicy),
      m_can_insert (can_insert)
  {}

  SG::OwnershipPolicy ownPolicy() const { return m_ownPolicy; }
  void testInsert (const char* op)
  {
    if (!m_can_insert)
      throw SG::ExcInsertionInBaseClass (op, typeid(int), typeid(int));
  }

  SG::OwnershipPolicy m_ownPolicy;
  bool m_can_insert;
};


struct Cont3
{
  typedef Cont3 Cont;
  typedef X1 base_value_type;
  typedef std::list<X1*> BaseContainer;
  static constexpr bool has_virtual = false;
  static constexpr bool must_own = false;

  typedef base_value_type* value_type;
  typedef const base_value_type* const_value_type;
  typedef base_value_type** pointer;
  typedef std::unique_ptr<X1> unique_type;
  typedef DataModel_detail::ElementProxy<Cont> ElementProxy;
  typedef DataModel_detail::const_iterator<Cont> const_iterator;

  Cont3 (SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS,
         bool can_insert = true)
    : m_ownPolicy (ownPolicy),
      m_can_insert (can_insert)
  {}

  SG::OwnershipPolicy ownPolicy() const { return m_ownPolicy; }
  void testInsert (const char* op)
  {
    if (!m_can_insert)
      throw SG::ExcInsertionInBaseClass (op, typeid(int), typeid(int));
  }

  SG::OwnershipPolicy m_ownPolicy;
  bool m_can_insert;
};


struct Cont4
{
  typedef Cont4 Cont;
  typedef X2 base_value_type;
  typedef std::list<X1*> BaseContainer;
  static constexpr bool has_virtual = true;
  static constexpr bool must_own = false;

  typedef base_value_type* value_type;
  typedef const base_value_type* const_value_type;
  typedef base_value_type** pointer;
  typedef std::unique_ptr<X1> unique_type;
  typedef DataModel_detail::ElementProxy<Cont> ElementProxy;
  typedef DataModel_detail::const_iterator<Cont> const_iterator;

  Cont4 (SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS,
         bool can_insert = true)
    : m_ownPolicy (ownPolicy),
      m_can_insert (can_insert)
  {}

  SG::OwnershipPolicy ownPolicy() const { return m_ownPolicy; }
  void testInsert (const char* op)
  {
    if (!m_can_insert)
      throw SG::ExcInsertionInBaseClass (op, typeid(int), typeid(int));
  }

  SG::OwnershipPolicy m_ownPolicy;
  bool m_can_insert;
};


template <class CONT, class CAT>
void test1_extra (CAT /*cat*/)
{
}
template <class CONT>
void test1_extra (std::random_access_iterator_tag /*cat*/)
{
  typedef DataModel_detail::const_iterator<CONT> const_iterator;
  typedef DataModel_detail::iterator<CONT> iterator;

  typename CONT::BaseContainer v;
  typedef typename CONT::base_value_type T;
  CONT cont;
  T* t1 = new T(1);
  T* t2 = new T(2);
  T* t3 = new T(3);
  v.push_back (t1);
  v.push_back (t2);
  v.push_back (t3);

  const_iterator c1 (v.begin());
  const_iterator c2 (v.begin()+2);

  iterator i1 (v.begin(), &cont);
  iterator i2 (v.begin()+2, &cont);

  assert (c1 < c2);
  assert (!(c2 < c1));
  assert (!(c1 < c1));
  assert (c1 <= c2);
  assert (c1 <= c1);
  assert (!(c2 <= c1));
  assert (c2 > c1);
  assert (!(c1 > c2));
  assert (!(c1 > c1));
  assert (c2 >= c1);
  assert (!(c1 >= c2));
  assert (c1 >= c1);

  assert (i1 < i2);
  assert (!(i2 < i1));
  assert (!(i1 < i1));
  assert (i1 <= i2);
  assert (i1 <= i1);
  assert (!(i2 <= i1));
  assert (i2 > i1);
  assert (!(i1 > i2));
  assert (!(i1 > i1));
  assert (i2 >= i1);
  assert (!(i1 >= i2));
  assert (i1 >= i1);

  assert (c1 < i2);
  assert (!(i2 < c1));
  assert (!(c1 < c1));
  assert (c1 <= i2);
  assert (c1 <= c1);
  assert (!(i2 <= c1));
  assert (i2 > c1);
  assert (!(c1 > i2));
  assert (!(c1 > c1));
  assert (i2 >= c1);
  assert (!(c1 >= i2));
  assert (c1 >= c1);

  assert (i1 < c2);
  assert (!(c2 < i1));
  assert (!(i1 < i1));
  assert (i1 <= c2);
  assert (i1 <= i1);
  assert (!(c2 <= i1));
  assert (c2 > i1);
  assert (!(i1 > c2));
  assert (!(i1 > i1));
  assert (c2 >= i1);
  assert (!(i1 >= c2));
  assert (i1 >= i1);

  assert (c2 - c1 == 2);
  assert (i2 - i1 == 2);
  assert (c2 - i1 == 2);
  assert (i2 - c1 == 2);

  assert (i1[0]->x == 1);
  assert (i1[1]->x == 2);
  assert (i1[2]->x == 3);
}
template <class CONT>
void test1_cont()
{
  typedef DataModel_detail::const_iterator<CONT> const_iterator;
  typedef DataModel_detail::iterator<CONT> iterator;

  typename CONT::BaseContainer v;
  typedef typename CONT::base_value_type T;
  CONT cont;
  T* t1 = new T(1);
  T* t2 = new T(2);
  T* t3 = new T(3);
  v.push_back (t1);
  v.push_back (t2);
  v.push_back (t3);

  const_iterator c1;
  const_iterator c2 (v.begin());
  assert (*c2 == t1);

  CONT cont3 (SG::OWN_ELEMENTS, false);
  iterator i1;
  iterator i2 (v.begin(), &cont3);
  assert (i2.ownPolicy() == SG::OWN_ELEMENTS);
  bool caught = false;
  try {
    i2.testInsert("foo");
  }
  catch (SG::ExcInsertionInBaseClass&) {
    caught = true;
  }
  assert (caught);

  assert (*i2 == t1);
  assert (i1.container() == 0);
  assert (i2.container() == &cont3);

  CONT cont2 (SG::VIEW_ELEMENTS);
  iterator i3 (v.begin(), &cont2);
  assert (i3.ownPolicy() == SG::VIEW_ELEMENTS);
  i3.testInsert("foo");

  assert (c2 == c2);
  assert (c1 != c2);
  assert (i2 == i2);
  assert (i1 != i2);

  assert (i2 == c2);
  assert (c2 == i2);
  assert (i1 != c2);
  assert (c1 != i2);

  test1_extra<CONT> (typename iterator::iterator_category());
}
void test1()
{
  test1_cont<Cont1>();
  test1_cont<Cont2>();
  test1_cont<Cont3>();
  test1_cont<Cont4>();
}


int main()
{
  test1();
  return 0;
}
