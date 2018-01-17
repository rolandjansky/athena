/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElementProxy_test.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file ElementProxy_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2010
 * @brief Regression tests for ElementProxy.
 */


#undef NDEBUG


//***************************************************************************
// Change assertions failures to exceptions, so we can catch them.
//

#include "CxxUtils/make_unique.h"
#include <string>
#include <stdexcept>
#include <sstream>
#include <memory>

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



#include "AthContainers/tools/ElementProxy.h"
#include "AthContainers/exceptions.h"
#include "TestTools/expect_exception.h"
#include "CxxUtils/checker_macros.h"
#include <vector>
#include <cassert>


using CxxUtils::make_unique;


std::vector<int> dtor_log ATLAS_THREAD_SAFE;


struct A
{
  virtual ~A() {}
};
struct B : public A
{
  B(int the_x) : x(the_x) {}
  ~B() { dtor_log.push_back(x); }
  int x;
};


void check_dtor (int x = -1)
{
  if (x == -1)
    assert (dtor_log.size() == 0);
  else {
    assert (dtor_log.size() == 1);
    assert (dtor_log[0] == x);
    dtor_log.clear();
  }
}


struct Cont
  : public std::vector<A*>
{
  typedef std::vector<A*> BaseContainer;
  typedef B* value_type;
  typedef B base_value_type;
  typedef value_type& reference;
  static const bool has_virtual = false;

  Cont (SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS,
        bool can_insert_p = false)
    : m_ownPolicy (ownPolicy),
      m_can_insert (can_insert_p)
  {
  }

  void swapBaseElement(BaseContainer::iterator pos, A* newElem, A*& oldElem)
  {
    oldElem = dynamic_cast<B*> (*pos);
    *pos = newElem;
  }

  void assignElement (BaseContainer::iterator pos,
                      value_type newElem)
  {
    ATHCONTAINERS_ASSERT (m_can_insert);
    BaseContainer::value_type old = *pos;
    if (this->m_ownPolicy == SG::OWN_ELEMENTS)
      delete old;
    *pos = newElem;
  }

  void assignElement (BaseContainer::iterator pos,
                      std::unique_ptr<base_value_type> newElem)
  {
    ATHCONTAINERS_ASSERT (m_can_insert);
    ATHCONTAINERS_ASSERT (this->m_ownPolicy == SG::OWN_ELEMENTS);
    BaseContainer::value_type old = *pos;
    delete old;
    *pos = newElem.release();
  }

  void assignBaseElement (BaseContainer::iterator pos,
                          BaseContainer::value_type newElem)
  {
    ATHCONTAINERS_ASSERT (m_can_insert);
    BaseContainer::value_type old = *pos;
    if (this->m_ownPolicy == SG::OWN_ELEMENTS)
      delete old;
    *pos = newElem;
  }

  SG::OwnershipPolicy ownPolicy() const { return m_ownPolicy; }
  bool can_insert() const { return m_can_insert; }

  SG::OwnershipPolicy m_ownPolicy;
  bool m_can_insert;
};


// own/cannot insert
void test1()
{
  Cont v;
  B* b1 = new B(1);
  v.push_back (b1);
  B* b2 = new B(2);
  v.push_back (b2);

  typedef DataModel_detail::ElementProxy<Cont> EP;
  EP ep1 (v.begin(), &v);
  assert ((B*)ep1 == b1);
  assert (ep1->x == 1);
  ep1 = b1;
  assert (ep1->x == 1);
  assert (ep1.container() == &v);

  EP ep1a (v.begin(), &v);
  ep1 = ep1a;
  assert (ep1->x == 1);

  bool caught = false;
  try {
    ep1 = b2;
  }
  catch (test_err&) {
    caught = true;
  }
  assert (caught);

  EP ep2 (v.begin()+1, &v);
  caught = false;
  try {
    ep1 = ep2;
  }
  catch (test_err&) {
    caught = true;
  }
  assert (caught);

  check_dtor();
}


// own/can insert
void test2()
{
  Cont v (SG::OWN_ELEMENTS, true);
  B* b1 = new B(1);
  v.push_back (b1);
  B* b2 = new B(2);
  v.push_back (b2);
  B* b3 = new B(3);
  v.push_back (b3);

  typedef DataModel_detail::ElementProxy<Cont> EP;
  EP ep1 (v.begin(), &v);
  assert (ep1->x == 1);
  check_dtor();
  ep1 = b1;
  assert (ep1->x == 1);
  check_dtor();
  ep1 = b2;
  assert (ep1->x == 2);
  check_dtor(1);

  EP ep2 (v.begin()+1, &v);
  ep1 = ep2;
  assert (ep1->x == 2);
  check_dtor();

  EP ep3 (v.begin()+2, &v);
  bool caught = false;
  try {
    ep1 = ep3;
  }
  catch (test_err&) {
    caught = true;
  }
  assert (caught);

  Cont v2 (SG::VIEW_ELEMENTS, true);
  v2.push_back (new B(1));
  v2.push_back (new B(2));
  v2.push_back (new B(3));
  EP ep3a (v2.begin()+2, &v2);
  ep1 = ep3a;
  assert (ep1->x == 3);
  check_dtor(2);

  ep1 = make_unique<B> (4);
  assert (ep1->x == 4);
  check_dtor(3);
  EP ep3b (v2.begin()+1, &v2);
  EXPECT_EXCEPTION (test_err,
                    ep3b = make_unique<B> (10));
  check_dtor(10);
}


// unowned/can insert
void test3()
{
  Cont v (SG::VIEW_ELEMENTS, true);
  B* b1 = new B(1);
  v.push_back (b1);
  B* b2 = new B(2);
  v.push_back (b2);
  B* b3 = new B(3);
  v.push_back (b3);

  typedef DataModel_detail::ElementProxy<Cont> EP;
  EP ep1 (v.begin(), &v);
  assert (ep1->x == 1);

  ep1 = b2;
  assert (ep1->x == 2);

  EP ep3 (v.begin()+2, &v);
  ep1 = ep3;
  assert (ep1->x == 3);

  Cont v2 (SG::OWN_ELEMENTS, true);
  v2.push_back (new B(1));
  v2.push_back (new B(2));
  v2.push_back (new B(3));
  EP ep2 (v2.begin()+1, &v2);
  ep1 = ep2;
  assert (ep1->x == 2);

  check_dtor();
}


int main()
{
  test1();
  test2();
  test3();
  return 0;
}
