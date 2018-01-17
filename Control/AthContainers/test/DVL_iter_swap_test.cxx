/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DVL_iter_swap_test.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file DVL_iter_swap_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2010
 * @brief Regression tests for DVL_iter_swap.
 */


#undef NDEBUG


//***************************************************************************
// Change assertions failures to exceptions, so we can catch them.
//

#include <string>
#include <stdexcept>
#include <sstream>

class test_err
  : public std::exception
{
public:
  explicit test_err (const char* file,
                     int line,
                     const char* what);
  virtual ~test_err() throw() {}
  virtual const char* what() const throw() { return m_what.c_str(); }
private:
  std::string m_what;
};


test_err::test_err (const char* file,
                    int line,
                    const char* what)
{
  std::ostringstream os;
  os << file << ":" << line << " assertion failure: " << what;
  m_what = os.str();
}


void throw_test_err (const char* file, int line, const char* what)
{
  throw (test_err (file, line, what));
}

#define ATHCONTAINERS_ASSERT(X) do {     \
    if (!(X)) { \
      throw_test_err (__FILE__, __LINE__, #X); \
    } \
  } while (0)

//***************************************************************************


#include "AthContainers/tools/DVL_iter_swap.h"
#include "AthContainers/tools/DVLIterator.h"
#include "AthContainers/exceptions.h"
#include "CxxUtils/checker_macros.h"
#include <vector>
#include <cassert>


struct X1
{
  X1(int the_x) : x(the_x) {}
  virtual ~X1() {}
  int x;
};


struct Cont1
{
  typedef Cont1 Cont;
  typedef X1 base_value_type;
  typedef std::vector<X1*> BaseContainer;
  static const bool has_virtual = false;

  typedef base_value_type* value_type;
  typedef const base_value_type* const_value_type;
  typedef base_value_type** pointer;
  typedef DataModel_detail::ElementProxy<Cont> ElementProxy;
  typedef DataModel_detail::const_iterator<Cont> const_iterator;
  typedef DataModel_detail::iterator<Cont> iterator;

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

  static void
  iter_swap (iterator a, iterator b)
  {
    ATHCONTAINERS_ASSERT (a.ownPolicy() == b.ownPolicy());
    a.testInsert("iter_swap");
    b.testInsert("iter_swap");
    std::iter_swap (a.base(), b.base());
    m_x1 = *a;
    m_x2 = *b;
  }

  SG::OwnershipPolicy m_ownPolicy;
  bool m_can_insert;
  static X1* m_x1 ATLAS_THREAD_SAFE;
  static X1* m_x2 ATLAS_THREAD_SAFE;
};


X1* Cont1::m_x1 = 0;
X1* Cont1::m_x2 = 0;


void test1()
{
  typedef DataModel_detail::iterator<Cont1> iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  Cont1 cont;

  std::vector<X1*> v;
  v.push_back (new X1(1));
  v.push_back (new X1(2));

  iterator i1 (v.begin(), &cont);
  iterator i2 (v.begin()+1, &cont);
  assert ((*i1)->x == 1);
  assert ((*i2)->x == 2);
  std::iter_swap (i1, i2);
  assert ((*i2)->x == 1);
  assert ((*i1)->x == 2);
  assert (cont.m_x1->x == 2);
  assert (cont.m_x2->x == 1);

  Cont1 cont3 (SG::OWN_ELEMENTS, false);
  iterator i3 (v.begin()+1, &cont3);
  bool caught = false;
  try {
    std::iter_swap (i1, i3);
  }
  catch (SG::ExcInsertionInBaseClass&) {
    caught = true;
  }
  assert (caught);

  caught = false;
  try {
    std::iter_swap (i3, i1);
  }
  catch (SG::ExcInsertionInBaseClass&) {
    caught = true;
  }
  assert (caught);

  Cont1 cont2 (SG::VIEW_ELEMENTS);
  iterator i4 (v.begin()+1, &cont2);
  caught = false;
  try {
    std::iter_swap (i1, i4);
  }
  catch (test_err&) {
    caught = true;
  }
  assert (caught);

  iterator i5 (v.begin(), &cont2);
  std::iter_swap (i4, i5);
  assert ((*i1)->x == 1);
  assert ((*i2)->x == 2);

  //*************************************************

  reverse_iterator ri1 (iterator (v.begin()+1, &cont));
  reverse_iterator ri2 (iterator (v.begin()+2, &cont));
  assert ((*ri1)->x == 1);
  assert ((*ri2)->x == 2);
  std::iter_swap (ri1, ri2);
  assert ((*ri2)->x == 1);
  assert ((*ri1)->x == 2);

  reverse_iterator ri3 (iterator (v.begin()+2, &cont3));
  caught = false;
  try {
    std::iter_swap (ri1, ri3);
  }
  catch (SG::ExcInsertionInBaseClass&) {
    caught = true;
  }
  assert (caught);

  caught = false;
  try {
    std::iter_swap (ri3, ri1);
  }
  catch (SG::ExcInsertionInBaseClass&) {
    caught = true;
  }
  assert (caught);

  reverse_iterator ri4 (iterator (v.begin()+2, &cont2));
  caught = false;
  try {
    std::iter_swap (ri1, ri4);
  }
  catch (test_err&) {
    caught = true;
  }
  assert (caught);

  reverse_iterator ri5 (iterator (v.begin()+1, &cont2));
  std::iter_swap (ri4, ri5);
  assert ((*ri1)->x == 1);
  assert ((*ri2)->x == 2);
}


int main()
{
  test1();
  return 0;
}

