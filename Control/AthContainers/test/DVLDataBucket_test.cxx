/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DVLDataBucket_test.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file AthContainers/test/DVLDataBucket_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2008
 * @brief Regression tests for DVLDataBucket.
 */

// Disable this test in standalone mode:
#ifndef XAOD_STANDALONE

#undef NDEBUG
#include "AthContainers/DataVector.h"
#include "AthContainers/tools/DVLDataBucket.h"
#include "AthContainers/tools/DVLInfo.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/ConstDataVector.h"
#include "AthenaKernel/BaseInfo.h"
#include "AthenaKernel/IRegisterTransient.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "TestTools/expect_exception.h"
#include "CxxUtils/checker_macros.h"
#include <vector>
#include <cassert>


struct Reg
  : public SG::IRegisterTransient
{
  virtual void registerTransient (void* p);
  std::vector<void*> m_log;
};


void Reg::registerTransient (void* p)
{
  m_log.push_back (p);
}


struct A
{
  A(int x) : aa(x) {}
  virtual ~A() { log.push_back (aa); }
  int aa;
  static std::vector<int> log ATLAS_THREAD_SAFE;
};
std::vector<int> A::log;


struct B
{
  B(int x) : bb(x) {}
  virtual ~B() {}
  int bb;
};


struct D
  : virtual public B, public A
{
  D(int x) : B(x+10), A(x+20), dd(x) {}
  int dd;
};

SG_BASES2 (D, SG_VIRTUAL(B), A);
DATAVECTOR_VIRTBASES1(D, B);

CLASS_DEF( DataVector<A> , 178538882, 1 )
CLASS_DEF( DataVector<B> , 178538883, 1 )
CLASS_DEF( DataVector<D> , 178538884, 1 )


template <class CONTA, class CONTB, class CONTD, class CONSTCONT>
void test1t()
{
  static DataModel_detail::DVLInfo<CONTA> xinfoa;
  static DataModel_detail::DVLInfo<CONTB> xinfob;
  static DataModel_detail::DVLInfo<CONTD> xinfod;

  Reg reg;

  SG::DVLDataBucket<CONTD>* buck = new SG::DVLDataBucket<CONTD>;
  assert (buck->object() == 0);
  delete buck;

  CONTD* contd = new CONTD;
  for (int i=0; i < 10; i++)
    contd->push_back (new D (i));

  buck = new SG::DVLDataBucket<CONTD> (contd);

  assert (buck->clID() == ClassID_traits<CONTD>::ID());
  assert (buck->tinfo() == typeid(CONTD));

  void* cont_p = buck->cast (typeid (CONTB), &reg);
  CONTB* contb = reinterpret_cast<CONTB*> (cont_p);
  assert (contb == contd);
  assert (reg.m_log.empty());

  cont_p = buck->cast (typeid (CONTA), &reg);
  CONTA* conta = reinterpret_cast<CONTA*> (cont_p);
  assert (reg.m_log.size() == 1);
  assert (reg.m_log[0] == conta);
  assert (conta->size() == 10);
  typename CONTA::iterator it = conta->begin();
  for (int i=0; i < 10; i++) {
    assert ((*it)->aa == i+20);
    ++it;
  }

  assert (cont_p == buck->cast (typeid (CONTA), &reg));

  cont_p = buck->cast (ClassID_traits<CONTA>::ID(), &reg);
  conta = reinterpret_cast<CONTA*> (cont_p);
  assert (reg.m_log.size() == 1);
  assert (reg.m_log[0] == conta);
  assert (conta->size() == 10);
  it = conta->begin();
  for (int i=0; i < 10; i++) {
    assert ((*it)->aa == i+20);
    ++it;
  }

  for (int i=10; i < 20; i++)
    contd->push_back (new D (i));

  assert (cont_p == buck->cast (ClassID_traits<CONTA>::ID(), &reg));

  cont_p = buck->cast (ClassID_traits<CONTA>::ID(), &reg);
  conta = reinterpret_cast<CONTA*> (cont_p);
  assert (reg.m_log.size() == 1);
  assert (reg.m_log[0] == conta);
  assert (conta->size() == 20);
  it = conta->begin();
  for (int i=0; i < 20; i++) {
    assert ((*it)->aa == i+20);
    ++it;
  }

  for (int i=20; i < 30; i++)
    contd->push_back (new D (i));

  assert (cont_p == buck->cast (typeid (CONTA), &reg));

  cont_p = buck->cast (typeid (CONTA), &reg);
  conta = reinterpret_cast<CONTA*> (cont_p);
  assert (reg.m_log.size() == 1);
  assert (reg.m_log[0] == conta);
  assert (conta->size() == 30);
  it = conta->begin();
  for (int i=0; i < 30; i++) {
    assert ((*it)->aa == i+20);
    ++it;
  }

  SG::DVLDataBucket<CONTD>* buck2 = buck->clone();
  delete buck;
  delete buck2;

  {
    A::log.clear();
    CONSTCONT* pp = new CONSTCONT;
    pp->push_back (new A (21));
    typedef typename SG::DataBucketTrait<CONSTCONT>::type BUCKET;
    BUCKET* buck3 = new BUCKET (pp);
    assert (A::log.empty());
    delete buck3;
    std::vector<int> exp21;
    exp21.push_back(21);
    assert (A::log == exp21);
  }

  {
    A::log.clear();
    std::unique_ptr<CONTA> pp = std::make_unique<CONTA>();
    pp->push_back (new A(11));
    typedef typename SG::DataBucketTrait<CONTA>::type BUCKET;
    {
      std::unique_ptr<BUCKET> buck3 =
        std::make_unique<BUCKET>(std::move(pp));
      assert (A::log.empty());
    }
    assert (A::log == std::vector<int>{11});
  }

  {
    A::log.clear();
    std::unique_ptr<CONSTCONT> pp (new CONSTCONT);
    pp->push_back (new A (31));
    typedef typename SG::DataBucketTrait<CONSTCONT>::type BUCKET;
    {
      std::unique_ptr<BUCKET> buck3 =
        std::make_unique<BUCKET>(std::move(pp));
      assert (A::log.empty());
    }
    assert (A::log == std::vector<int>{31});
  }
}


void test1()
{
  test1t<DataVector<A>, DataVector<B>, DataVector<D>, ConstDataVector<DataVector<A> > >();
}


typedef ViewVector<DataVector<A> > AView;
VIEWVECTOR_CLASS_DEF (AView, 491783424)

typedef ViewVector<DataVector<B> > BView;


void test2()
{
  Reg reg;

  auto a1 = std::make_unique<AView>();
  void* vp = a1.get();
  auto buck = std::make_unique<SG::DVLDataBucket<DataVector<A> > > (a1.release());

  assert (buck->clID() == 491783424);
  assert (buck->tinfo() == typeid(AView));

  assert (buck->cast(491783424, &reg) == vp);
  assert (buck->cast(178538882, &reg) == vp);
  assert (buck->cast(2987492385, &reg) == nullptr);

  assert (buck->cast(typeid(AView), &reg) == vp);
  assert (buck->cast(typeid(DataVector<A>), &reg) == vp);
  assert (buck->cast(typeid(int), &reg) == nullptr);

  a1 = std::make_unique<AView>();
  vp = a1.get();
  buck = std::make_unique<SG::DVLDataBucket<DataVector<A> > > (std::move(a1));

  assert (buck->clID() == 491783424);
  assert (buck->tinfo() == typeid(AView));

  assert (buck->cast(491783424, &reg) == vp);
  assert (buck->cast(178538882, &reg) == vp);
  assert (buck->cast(2987492385, &reg) == nullptr);

  assert (buck->cast(typeid(AView), &reg) == vp);
  assert (buck->cast(typeid(DataVector<A>), &reg) == vp);
  assert (buck->cast(typeid(int), &reg) == nullptr);

  auto b1 = std::make_unique<BView>();
  EXPECT_EXCEPTION (SG::ExcMissingViewVectorCLID,
                    std::make_unique<SG::DVLDataBucket<DataVector<B> > > (b1.release()));
}


#else

void test1() {}
void test2() {}

#endif // not XAOD_STANDALONE

int main()
{
  test1();
  test2();
  return 0;
}
