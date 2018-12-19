/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
// $Id$
/**
 * @file AthenaKernel/test/RecyclableDataObject_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Tests for RecyclableDataObject.
 */


#undef NDEBUG
#include "AthenaKernel/RecyclableDataObject.h"
#include "GaudiKernel/DataObject.h"
#include <atomic>
#include <iostream>
#include <cassert>


class MyDataObj
  : public DataObject
{
public:
  MyDataObj (int x) : m_x (x)
  { ++s_count; }
  ~MyDataObj() { --s_count; }

  void recycle();

  static std::atomic<int> s_count;
  int m_x = 0;
  bool m_recycled = false;
};


std::atomic<int> MyDataObj::s_count (0);


void MyDataObj::recycle()
{
  m_recycled = true;
}


void test1()
{
  std::cout << "test1\n"; 
  {
    Athena::RecyclableDataQueue<MyDataObj> queue;

    MyDataObj* obj1 = queue.get (1);
    obj1->addRef();
    assert (MyDataObj::s_count == 1);
    assert (obj1->m_x == 1);
    assert (!obj1->m_recycled);

    MyDataObj* obj2 = queue.get (2);
    obj2->addRef();
    assert (MyDataObj::s_count == 2);
    assert (obj2->m_x == 2);
    assert (!obj2->m_recycled);

    obj1->release();

    MyDataObj* obj3 = queue.get (3);
    obj3->addRef();
    assert (MyDataObj::s_count == 2);
    assert (obj3->m_x == 1);
    assert (obj3->m_recycled);

    obj2->release();
    obj1->release();
  }
  assert (MyDataObj::s_count == 0);
}


int main()
{
  std::cout << "RecyclableDataObject_test\n";
  test1();
  return 0;
}
