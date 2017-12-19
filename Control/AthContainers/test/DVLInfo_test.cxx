/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DVLInfo_test.cxx 508150 2012-06-29 10:25:42Z ssnyder $
/**
 * @file AthContainers/test/DVLInfo_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2008
 * @brief Regression tests for DVLInfo.
 */

// Disable this test in standalone mode:
#ifndef XAOD_STANDALONE

#undef NDEBUG
#include "AthContainers/tools/DVLInfo.h"
#include "SGTools/BaseInfo.h"
#include "SGTools/CLASS_DEF.h"
#include "boost/type_traits/remove_pointer.hpp"
#include <vector>
#include <list>
#include <cassert>


class Foo{};
CLASS_DEF(Foo, 1234, 1)


class DVLInfoTest
  : public DataModel_detail::DVLInfoBase
{
public:
  DVLInfoTest() : DVLInfoBase (typeid(Foo), typeid(double)) {}
  virtual void* make (size_t nreserve) const;
  virtual void  push (void* cont_p, void* elt_p) const;
  virtual size_t size (void* cont_p) const;
  virtual void clear (void* cont_p) const;
  virtual void  del (void* cont_p) const;
  virtual void* clone (void* cont_p) const;
  virtual DataModel_detail::DVLIteratorBase*
  iterator (const void* cont_p) const;
  SG::AuxVectorBase* base (void* /*cont_p*/) const { return 0; }
};


void* DVLInfoTest::make (size_t nreserve) const
{
  std::vector<int*>* vec = new std::vector<int*>;
  vec->reserve (nreserve);
  return vec;
}


void DVLInfoTest::push (void* cont_p, void* elt_p) const
{
  reinterpret_cast<std::vector<int*>*>(cont_p) -> push_back ((int*)elt_p);
}


size_t DVLInfoTest::size (void* cont_p) const
{
  return reinterpret_cast<std::vector<int*>*>(cont_p) -> size();
}


void DVLInfoTest::clear (void* cont_p) const
{
  reinterpret_cast<std::vector<int*>*>(cont_p) -> clear();
}


void DVLInfoTest::del (void* cont_p) const
{
  delete reinterpret_cast<std::vector<int*>*>(cont_p);
}


void* DVLInfoTest::clone (void* cont_p)  const
{
  return new std::vector<int*> (*reinterpret_cast<std::vector<int*>*>(cont_p));
}


DataModel_detail::DVLIteratorBase*
DVLInfoTest::iterator (const void* /*cont_p*/) const
{
  return 0;
}


void test_DVLInfoBase()
{
  static DVLInfoTest info;
  assert (DataModel_detail::DVLInfoBase::find (typeid (float)) == 0);
  DataModel_detail::DVLInfoBase* info2 =
    DataModel_detail::DVLInfoBase::find (typeid (Foo));
  assert (info2 == &info);
  assert (info2->tinfo() == typeid (Foo));
  assert (info2->clid() == 1234);
  assert (info2->elt_tinfo() == typeid (double));
  info2 = DataModel_detail::DVLInfoBase::find (1234);
  assert (info2 == &info);

  void* cont_p = info2->make (5);
  info2->push (cont_p, new int(1));
  info2->push (cont_p, new int(2));  
  assert (info2->base(cont_p) == 0);

  std::vector<int*>* vec = reinterpret_cast<std::vector<int*>*> (cont_p);
  assert (vec->capacity() == 5);
  assert (vec->size() == 2);
  assert (*(*vec)[0] == 1);
  assert (*(*vec)[1] == 2);
  assert (info2->size (cont_p) == 2);

  std::vector<int*>* vec2 =
    reinterpret_cast<std::vector<int*>*> (info2->clone (cont_p));
  assert (vec2 != vec);
  assert (vec2->size() == 2);
  assert (*(*vec2)[0] == 1);
  assert (*(*vec2)[1] == 2);
  assert (info2->size (vec2) == 2);
  info2->clear (vec2);
  assert (info2->size (vec2) == 0);
  assert (vec2->size() == 0);

  info2->del (vec);

  assert (info2->iterator (vec2) == 0);
}


template <class T>
class mycont
  : public T
{
public:
  typedef typename boost::remove_pointer<typename T::value_type>::type
    base_value_type;
  mycont (SG::OwnershipPolicy pol) : m_pol (pol) {}
  T& stdcont() { return *this; }

  SG::OwnershipPolicy m_pol;
};


typedef mycont<std::vector<int*> > myvec;
typedef mycont<std::list<int*> > mylist;


class myvec2 
  : public myvec, public SG::AuxVectorBase
{
public:
  myvec2 (SG::OwnershipPolicy pol = SG::VIEW_ELEMENTS) : myvec (pol) {}
  myvec2 (const myvec2&) : myvec (SG::VIEW_ELEMENTS), SG::AuxVectorBase() {}
  myvec2& operator= (const myvec2&) = delete;
  virtual size_t capacity_v() const { return 0; }
  virtual size_t size_v() const { return 0; }
};



template <class T>
void dvl_makecontainer (size_t nreserve, mycont<std::vector<T> >*& cont)
{
  cont = new mycont<std::vector<T> > (SG::VIEW_ELEMENTS);
  cont->reserve (nreserve);
}


template <class T>
void dvl_makecontainer (size_t /*nreserve*/, mycont<std::list<T> >*& cont)
{
  cont = new mycont<std::list<T> > (SG::VIEW_ELEMENTS);
}



CLASS_DEF (myvec, 7428347, 1)
CLASS_DEF (mylist, 7428348, 1)


template <class CONT>
CONT* test_DVLInfo1 ()
{
  static DataModel_detail::DVLInfo<CONT> info;
  assert (DataModel_detail::DVLInfoBase::find (typeid (float)) == 0);
  DataModel_detail::DVLInfoBase* info2 =
    DataModel_detail::DVLInfoBase::find (typeid (CONT));
  assert (info2 == &info);
  info2 = DataModel_detail::DVLInfoBase::find (ClassID_traits<CONT>::ID());
  assert (info2 == &info);

  assert (info2->tinfo() == typeid (CONT));
  assert (info2->clid() == ClassID_traits<CONT>::ID());
  assert (info2->elt_tinfo() == typeid (int));

  void* cont_p = info2->make (5);
  info2->push (cont_p, new int(1));
  info2->push (cont_p, new int(2));

  CONT* cont = (CONT*)cont_p;
  assert (cont->m_pol == SG::VIEW_ELEMENTS);
  assert (cont->size() == 2);
  typename CONT::iterator it = cont->begin();
  assert (**it == 1);
  ++it;
  assert (**it == 2);
  assert (info2->size (cont_p) == 2);

  CONT* cont2 = reinterpret_cast<CONT*> (info2->clone (cont_p));
  assert (cont2 != cont);
  assert (cont2->m_pol == SG::VIEW_ELEMENTS);
  assert (cont2->size() == 2);
  it = cont2->begin();
  assert (**it == 1);
  ++it;
  assert (**it == 2);

  assert (info2->size (cont2) == 2);
  info2->clear(cont2);
  assert (info2->size (cont2) == 0);
  assert (cont2->size() == 0);

  info2->del (cont2);

  CONT cont3 (SG::VIEW_ELEMENTS);
  for (int i = 0; i < 10; i++) {
    cont3.push_back (new int (i));
    cont3.push_back ((int*)0);
  }
  DataModel_detail::DVLIteratorBase* iit = info2->iterator (&cont3);
  int ii = 0;
  while (const void* p = iit->next()) {
    const int* pp = reinterpret_cast<const int*> (p);
    assert (*pp == ii);
    ++ii;
  }

  return cont;
}



void test_DVLInfo()
{
  myvec* vec = test_DVLInfo1<myvec>();
  assert (vec->capacity() == 5);
  test_DVLInfo1<mylist> ();

  static const DataModel_detail::DVLInfo<myvec> info;
  assert (info.base(vec) == 0);

  myvec2 vec2 (SG::VIEW_ELEMENTS);
  static const DataModel_detail::DVLInfo<myvec2> info2;
  assert (info2.base(&vec2) == &vec2);
}


struct B
{
  B(int x) : bb(x) {}
  virtual ~B() {}
  int bb;
};


struct D
  : virtual public B
{
  D(int x) : B(x+10), dd(x) {}
  int dd;
};

SG_BASE (D, SG_VIRTUAL(B));


typedef mycont<std::vector<B*> > myvecb;
typedef mycont<std::vector<D*> > myvecd;
typedef mycont<std::list<B*> > mylistb;
typedef mycont<std::list<D*> > mylistd;

CLASS_DEF (myvecb, 7428349, 1)
CLASS_DEF (myvecd, 7428350, 1)
CLASS_DEF (mylistb, 7428351, 1)
CLASS_DEF (mylistd, 7428352, 1)


template <class CONTB, class CONTD, class CONTI>
void test_dvl_convert1()
{
  static const DataModel_detail::DVLInfo<CONTB> info;

  CONTD contd (SG::VIEW_ELEMENTS);
  for (int i=0; i < 10; i++)
    contd.push_back (new D(i));

  DataModel_detail::DVLInfoBase* info2;
  void* newp = DataModel_detail::dvl_convert (contd, typeid (CONTI), info2);
  assert (newp == 0);
  newp = DataModel_detail::dvl_convert (contd, typeid (CONTB), info2);
  assert (info2->tinfo() == info.tinfo());
  CONTB* contb = reinterpret_cast<CONTB*> (newp);
  assert (contb->size() == 10);
  typename CONTB::iterator it = contb->begin();
  for (int i=0; i < 10; i++) {
    assert ((*it)->bb = i+10);
    ++it;
  }

  newp = DataModel_detail::dvl_convert (contd,
                                        ClassID_traits<CONTB>::ID(), info2);
  assert (info2->tinfo() == info.tinfo());
  contb = reinterpret_cast<CONTB*> (newp);
  assert (contb->size() == 10);
  it = contb->begin();
  for (int i=0; i < 10; i++) {
    assert ((*it)->bb = i+10);
    ++it;
  }

  CONTD contd2 (SG::VIEW_ELEMENTS);
  for (int i=9; i >= 0; i--)
    contd.push_back (new D(i));
  DataModel_detail::dvl_update (contd2, newp, info2);
  it = contb->begin();
  for (int i=9; i >= 0; i--) {
    assert ((*it)->bb = i+10);
    ++it;
  }
}


void test_dvl_convert()
{
  D* d1 = new D(1);
  B* b1 = d1;
  assert ((void*)b1 != (void*)d1);
  test_dvl_convert1<myvecb, myvecd, myvec>();
}


int main()
{
  test_DVLInfoBase();
  test_DVLInfo();
  test_dvl_convert();
  return 0;
}

#else

int main() {
   return 0;
}

#endif // not XAOD_STANDALONE
