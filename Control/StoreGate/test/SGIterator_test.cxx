/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include "StoreGate/SGIterator.h"
#include "TestTools/initGaudi.h"
#include "TestTools/FLOATassert.h"
#include "TestTools/expect_exception.h"
#include "SGTools/TransientAddress.h"
#include "SGTools/DataProxy.h"
#include "AthenaKernel/StorableConversions.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>


struct MyDataObj {
  MyDataObj (int i=3) : m_i(i), m_f(i+0.14) {}
  int m_i;
  double m_f;
  double f() { return m_f; }
};
bool operator==(const MyDataObj& lhs, const MyDataObj& rhs) {
  return ( (lhs.m_i == rhs.m_i) && Athena_test::floatEQ(lhs.m_f,rhs.m_f) );
}

typedef std::list<int> IntList;
/** @file DataHandle_test.cxx  unit test for DataHandle
 * @author ATLAS Collaboration
 * $Id: SGIterator_test.cxx,v 1.1 2008-09-26 23:37:53 calaf Exp $
 ***************************************************************************/

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(IntList, 8001, 3)
CLASS_DEF(MyDataObj, 8000, 1)

namespace Athena_test {
  class MyProxyMap
    : public SG::ProxyMap
  {
  public:
    ~MyProxyMap()
    {
      for (auto& p : *this) {
        p.second->release();
      }
    }

    void insert (std::unique_ptr<SG::DataProxy> p)
    {
      const std::string& name = p->name();
      p->addRef();
      (*this)[name] = p.release();
    }

    void insert (CLID clid,
                 const std::string& name,
                 DataObject* obj,
                 bool isConst = false)
    {
      SG::TransientAddress taddr (clid, name);
      auto dp = std::make_unique<SG::DataProxy> (obj,
                                                 std::move (taddr),
                                                 isConst);
      insert (std::move (dp));
    }

    void checkOwn (const std::vector<std::string>& v)
    {
      for (const auto& p : *this)
      {
        const SG::DataProxy* dp = p.second;
        size_t c = std::count (v.begin(), v.end(), dp->name());
        assert (dp->refCount() == c+1);
      }
    }
  };


  void iteratorTest() {
    //empty handles
    SG::Iterator<MyDataObj> emptyIter;
    assert (emptyIter == emptyIter);

    MyProxyMap m;
    m.insert (8000, "b", SG::asStorable(std::make_unique<MyDataObj> (2)));
    m.insert (8000, "c", nullptr);
    m.insert (8000, "d", SG::asStorable(std::make_unique<MyDataObj> (4)));

    SG::Iterator<MyDataObj> it2 (m.begin(), m.end());
    SG::Iterator<MyDataObj> it2_end (m.end(), m.end());
    m.checkOwn ({"b", "d"});
    assert (it2 != it2_end);
    assert (it2.key() == "b");
    assert (it2->m_i == 2);

    ++it2;
    assert (it2 != it2_end);
    m.checkOwn ({"d"});
    assert (it2.key() == "d");
    assert (it2->m_i == 4);

    ++it2;
    assert (it2 == it2_end);
    m.checkOwn ({});

    m.insert (8000, "a2", nullptr);
    assert (it2.setState(m.begin(), m.end()).isSuccess());
    assert (it2_end.setState(m.end(), m.end()).isFailure());

    m.checkOwn ({"b", "d"});
    assert (it2 != it2_end);
    assert (it2.key() == "b");
    assert (it2->m_i == 2);

    assert (it2.setState(m.end(), m.end()).isFailure());
    m.checkOwn ({});

    m.insert (8000, "c2", SG::asStorable(std::make_unique<MyDataObj> (3)));
    {
      SG::Iterator<MyDataObj> it3 (m.begin(), m.end());
      SG::Iterator<MyDataObj> it3_end (m.end(), m.end());

      m.checkOwn ({"b", "c2", "d"});
      assert (it3 != it3_end);
      assert (it3.key() == "b");
      assert (it3->m_i == 2);

      ++it3;
      m.checkOwn ({"c2", "d"});
      assert (it3 != it3_end);
      assert (it3.key() == "c2");
      assert (it3->m_i == 3);

      m["c2"]->setConst();
      EXPECT_EXCEPTION(SG::ExcConstObject, *it3);
    }
    m.checkOwn ({});

    assert (it2.setState(m.begin(), m.end()).isSuccess());
    assert (it2_end.setState(m.end(), m.end()).isFailure());
    m.checkOwn ({"b", "d"});
    SG::Iterator<MyDataObj> it4 (it2);
    m.checkOwn ({"b", "b", "d", "d"});
    assert (it4 == it2);
    assert (it4->m_i == 2);

    ++it4;
    m.checkOwn ({"b", "d", "d"});
    assert (it4->m_i == 4);

    assert (std::distance (it2, it4) == 1);

    it2 = it4;
    m.checkOwn ({"d", "d"});
    assert (it2->m_i == 4);

    std::cout << "*** SGIterator test OK ***" <<std::endl;
  }


  void constIteratorTest() {
    //empty handles
    SG::ConstIterator<MyDataObj> emptyIter;
    assert (emptyIter == emptyIter);

    MyProxyMap m;
    m.insert (8000, "b", SG::asStorable(std::make_unique<MyDataObj> (2)));
    m.insert (8000, "c", nullptr);
    m.insert (8000, "d", SG::asStorable(std::make_unique<MyDataObj> (4)));

    SG::ConstIterator<MyDataObj> it2 (m.begin(), m.end());
    SG::ConstIterator<MyDataObj> it2_end (m.end(), m.end());
    m.checkOwn ({"b", "d"});
    assert (it2 != it2_end);
    assert (it2.key() == "b");
    assert (it2->m_i == 2);

    ++it2;
    assert (it2 != it2_end);
    m.checkOwn ({"d"});
    assert (it2.key() == "d");
    assert (it2->m_i == 4);

    ++it2;
    assert (it2 == it2_end);
    m.checkOwn ({});

    m.insert (8000, "a2", nullptr);
    assert (it2.setState(m.begin(), m.end()).isSuccess());
    assert (it2_end.setState(m.end(), m.end()).isFailure());

    m.checkOwn ({"b", "d"});
    assert (it2 != it2_end);
    assert (it2.key() == "b");
    assert (it2->m_i == 2);

    assert (it2.setState(m.end(), m.end()).isFailure());
    m.checkOwn ({});

    m.insert (8000, "c2", SG::asStorable(std::make_unique<MyDataObj> (3)),
              true);
    {
      SG::ConstIterator<MyDataObj> it3 (m.begin(), m.end());
      SG::ConstIterator<MyDataObj> it3_end (m.end(), m.end());

      m.checkOwn ({"b", "c2", "d"});
      assert (it3 != it3_end);
      assert (it3.key() == "b");
      assert (it3->m_i == 2);

      ++it3;
      m.checkOwn ({"c2", "d"});
      assert (it3 != it3_end);
      assert (it3.key() == "c2");
      assert (it3->m_i == 3);
    }
    m.checkOwn ({});

    assert (it2.setState(m.begin(), m.end()).isSuccess());
    assert (it2_end.setState(m.end(), m.end()).isFailure());
    m.checkOwn ({"b", "c2", "d"});
    SG::ConstIterator<MyDataObj> it4 (it2);
    m.checkOwn ({"b", "b", "c2", "c2", "d", "d"});
    assert (it4 == it2);
    assert (it4->m_i == 2);

    ++it4;
    m.checkOwn ({"b", "c2", "c2", "d", "d"});
    ++it4;
    m.checkOwn ({"b", "c2", "d", "d"});
    assert (it4->m_i == 4);

    assert (std::distance (it2, it4) == 2);

    it2 = it4;
    m.checkOwn ({"d", "d"});
    assert (it2->m_i == 4);

    m.insert (8000, "a0", SG::asStorable(std::make_unique<MyDataObj> (0)),
              true);
    assert (it2.setState(m.begin(), m.end()).isSuccess());

    std::cout << "*** SGConstIterator test OK ***" <<std::endl;
  }
}


int main() { 
  std::cout << "*** SGIterator_test starts ***" <<std::endl;
  ISvcLocator* pDum;
  Athena_test::initGaudi(pDum); //need MessageSvc
  Athena_test::iteratorTest();
  Athena_test::constIteratorTest();
  std::cout << "*** SGIterator_test OK ***" <<std::endl;
  return 0; 
}
