/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file Navigation/test/NavigableIterator_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2008
 * @brief Regression tests for NavigableIterator.
 */

#undef NDEBUG

#include "Navigation/AthenaBarCodeImpl.h"

#include "boost/type_traits/is_same.hpp"
#include "boost/static_assert.hpp"
#include <typeinfo>
#include <iostream>
#include <cassert>

template < class ABCImpl_t >
class MyParticle
  : public IAthenaBarCode
{
  ABCImpl_t m_abc;

public:
  MyParticle() {}

  AthenaBarCode_t getAthenaBarCode() const {
    return m_abc.getAthenaBarCode();
  }

  void setAthenaBarCode(AthenaBarCode_t id) {
    m_abc.setAthenaBarCode(id);
  }

  //Comparing & Versioning Control
  bool hasSameAthenaBarCode(const IAthenaBarCode &obj) const {
    return m_abc.hasSameAthenaBarCode(obj);
  }

  bool hasSameAthenaBarCodeExceptVersion(const IAthenaBarCode &obj) const {
    return m_abc.hasSameAthenaBarCodeExceptVersion(obj);
  }

  AthenaBarCodeVersion_t getVersion() const {
    return m_abc.getVersion();
  }

  void newVersion() {
    m_abc.newVersion();
  }

  void setVersion(AthenaBarCodeVersion_t newversion) {
    m_abc.setVersion(newversion);
  }
  
};


void test0() {
  //test creating lots of Particles to reach the limit of the counter
  MyParticle<AthenaBarCodeImpl> *p;

  static const AthenaBarCode_t tmp2=0;
  static const AthenaBarCode_t tmp=((~tmp2) >> (AthenaBarCodeImpl::TotalBits - AthenaBarCodeImpl::CounterBits));

  for(int i=0;i<tmp*1.1;i++) {
    p=new MyParticle<AthenaBarCodeImpl>();
    if(i%10000==0)
      std::cout<<std::hex<<p->getAthenaBarCode()<<std::endl;
    delete p;
  }
}

void test1() {
  //test of set/compare
  MyParticle<AthenaBarCodeImpl> *p1,*p2;
  p1=new MyParticle<AthenaBarCodeImpl>();
  p2=new MyParticle<AthenaBarCodeImpl>();

  p2->setAthenaBarCode(p1->getAthenaBarCode());

  assert(p1->getAthenaBarCode()==p2->getAthenaBarCode());

  assert(p1->hasSameAthenaBarCode(*p1));
  assert(p2->hasSameAthenaBarCode(*p2));
  assert(p1->hasSameAthenaBarCode(*p2));
  assert(p2->hasSameAthenaBarCode(*p1));

  //version
  p1->newVersion();
  assert(p1->getVersion()!=p2->getVersion());
  assert(p1->hasSameAthenaBarCodeExceptVersion(*p2));
  assert(p2->hasSameAthenaBarCodeExceptVersion(*p1));
  assert(p1->hasSameAthenaBarCodeExceptVersion(*p1));
  assert(p2->hasSameAthenaBarCodeExceptVersion(*p2));

  assert(!p1->hasSameAthenaBarCode(*p2));
  assert(!p2->hasSameAthenaBarCode(*p1));

  p2->setVersion(p1->getVersion());
  assert(p1->getVersion()==p2->getVersion());
  assert(p1->hasSameAthenaBarCodeExceptVersion(*p2));
  assert(p2->hasSameAthenaBarCodeExceptVersion(*p1));
  assert(p1->hasSameAthenaBarCodeExceptVersion(*p1));
  assert(p2->hasSameAthenaBarCodeExceptVersion(*p2));

  assert(p1->hasSameAthenaBarCode(*p2));
  assert(p2->hasSameAthenaBarCode(*p1));

}

int main()
{
  //  test0();
  test1();
  return 0;
}
