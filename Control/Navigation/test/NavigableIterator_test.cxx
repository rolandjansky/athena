/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: NavigableIterator_test.cxx,v 1.2 2008-09-09 18:52:06 ssnyder Exp $
/**
 * @file Navigation/test/NavigableIterator_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2008
 * @brief Regression tests for NavigableIterator.
 */

#undef NDEBUG

#include "Navigation/NavigableIterator.h"
#include "Navigation/Navigable.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthContainers/DataVector.h"
#include "CxxUtils/unused.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/Bootstrap.h"

#include "boost/type_traits/is_same.hpp"
#include "boost/static_assert.hpp"
#include <typeinfo>


struct Payload
  : public NavigableTerminalNode
{
  Payload (int the_x) : x (the_x) {}
  Payload& operator= (const Payload&) = default;
  int x;
};


typedef DataVector<Payload> dv_t;
typedef Navigable<dv_t> nav0_t;
typedef Navigable<dv_t, double> nav1_t;


CLASS_DEF(dv_t, 198325346, 1)


// Test traits.
template <class NAV>
void test0()
{
  typedef typename NAV::object_iter iter;
  BOOST_STATIC_ASSERT(
    (boost::is_same<typename std::iterator_traits<iter>::difference_type,
                    ptrdiff_t>::value));
  BOOST_STATIC_ASSERT(
    (boost::is_same<typename std::iterator_traits<iter>::value_type,
                    const Payload*>::value));
  BOOST_STATIC_ASSERT(
    (boost::is_same<typename std::iterator_traits<iter>::reference,
                    const Payload*>::value));
  typedef const Payload* ptr_t;
  BOOST_STATIC_ASSERT(
    (boost::is_same<typename std::iterator_traits<iter>::pointer,
                    ptr_t*>::value));
  BOOST_STATIC_ASSERT(
    (boost::is_same<typename std::iterator_traits<iter>::iterator_category,
                    std::random_access_iterator_tag>::value));
}


// Test forward iterator requirements.
template <class NAV>
void test1 (NAV& nav)
{
  typedef typename NAV::object_iter iter;
  iter beg = nav.begin();
  iter end = nav.end();
  iter mid (beg);
  assert ((**beg).x == 0);
  ++mid;
  mid++;
  ++mid;
  assert ((**mid).x == 3);
  iter tmp (mid);
  assert ((**tmp).x == 3);
  tmp = beg;
  assert ((**tmp).x == 0);
  assert (tmp == beg);
  assert (beg != end);
  assert ((*mid)->x == 3);
  assert ((*mid++)->x == 3);
  assert ((*mid)->x == 4);
  assert ((*++mid)->x == 5);
  assert ((*mid)->x == 5);
  iter tmp2;
  tmp2 = mid;
  assert (tmp2 == mid);
  iter& rtmp = tmp2;
  rtmp = beg;
  assert (rtmp == beg);
  assert ((*rtmp)->x == 0);
  ++rtmp;
  assert ((*rtmp)->x == 1);
  rtmp++;
  assert ((*rtmp)->x == 2);
  assert ((*rtmp++)->x == 2);
  assert ((*rtmp)->x == 3);
}


// Test bidirectional iterator requirements.
template <class NAV>
void test2 (NAV& nav)
{
  typedef typename NAV::object_iter iter;
  iter UNUSED(beg) = nav.begin();
  iter end = nav.end();

  iter mid = end;
  --mid;
  assert ((*mid)->x == 9);
  mid--;
  assert ((*mid)->x == 8);
  assert ((*mid--)->x == 8);
  assert ((*mid)->x == 7);
  assert ((*--mid)->x == 6);
  assert ((*mid)->x == 6);
}


// Test random access iterator requirements.
template <class NAV>
void test3 (NAV& nav)
{
  typedef typename NAV::object_iter iter;
  iter beg = nav.begin();
  iter end = nav.end();

  iter mid = beg;
  mid += 3;
  assert ((*mid)->x == 3);
  assert ((*(beg+4))->x == 4);
  assert ((*(5+beg))->x == 5);

  mid = end;
  mid -= 3;
  assert ((*mid)->x == 7);
  assert ((*(end-4))->x == 6);
  assert (mid - beg == 7);
  //assert (beg[5]->x == 5);

  assert (mid > beg);
  assert (!(mid > end));
  assert (!(mid > mid));

  assert (mid < end);
  assert (!(mid < beg));
  assert (!(mid < mid));

  assert (mid >= beg);
  assert (!(mid >= end));
  assert (mid >= mid);

  assert (mid <= end);
  assert (!(mid <= beg));
  assert (mid <= mid);
}


// Other NavigableIterator functions.
template <class NAV>
void test4 (NAV& nav)
{
  typedef typename NAV::object_iter iter;
  iter beg = nav.begin();
  iter end = nav.end();

  iter mid = beg + 4;
  assert (mid.begin() == beg);
  assert ((*mid)->x == 0);
  assert (mid.end() == end);
  assert ((*mid.previous())->x == 9);
  assert ((*mid)->x == 9);
  assert ((*mid.previous())->x == 8);
  assert ((*mid)->x == 8);
  assert ((*mid.next())->x == 9);
  assert ((*mid)->x == 9);
}


template <class NAV>
void test4a (NAV& nav)
{
  typedef typename NAV::object_iter iter;
  iter beg = nav.begin();
  iter mid = beg + 4;
  assert ((**mid.getInternalIterator())->x == 4);
  assert (mid.getParameter() == 1);
  assert (mid.getElement().index() == 4);
}


template <class NAV>
void test4b (NAV& nav)
{
  typedef typename NAV::object_iter iter;
  iter beg = nav.begin();
  iter mid = beg + 4;
  assert ((*(*mid.getInternalIterator()).first)->x == 4);
  assert ((*mid.getInternalIterator()).second == 4.5);
  assert (mid.getParameter() == 4.5);
  assert (mid.getElement().index() == 4);
}


int main()
{
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("NavigableIterator_test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }  

  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StoreGateSvc* sg = 0;
  assert ( svcLoc->service("StoreGateSvc", sg).isSuccess() );

  dv_t* dv = new dv_t;
  for (size_t i = 0; i < 10; i++) {
    dv->push_back (new Payload(i));
  }

  assert (sg->record (dv, "dv").isSuccess() );

  nav0_t nav0;
  nav1_t nav1;
  for (size_t i = 0; i < 10; i++) {
    nav0.insertElement (dv, i);
    nav1.insertElement (dv, i, i + 0.5);
  }

  test0<nav0_t>();
  test0<nav1_t>();

  test1<nav0_t>(nav0);
  test1<nav1_t>(nav1);

  test2<nav0_t>(nav0);
  test2<nav1_t>(nav1);

  test3<nav0_t>(nav0);
  test3<nav1_t>(nav1);

  test4<nav0_t>(nav0);
  test4a<nav0_t>(nav0);
  test4<nav1_t>(nav1);
  test4b<nav1_t>(nav1);

  return 0;
}
