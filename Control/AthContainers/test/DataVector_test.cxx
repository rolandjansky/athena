/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Disable this test in standalone mode:
#ifndef XAOD_STANDALONE

#undef NDEBUG
#include <cassert>
#include <stdexcept>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include <math.h>
#include <atomic>
#include <boost/iterator_adaptors.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/assign/list_of.hpp>
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/ClassName.h"


#include "DataVector_test.icc"

//*************************************************************************


using boost::assign::list_of;

#include "AthContainers/DataVector.h"
#include "AthContainers/ConstDataVector.h"
#include "AthContainers/AuxStoreInternal.h"
#include "AthContainers/AuxElementComplete.h"


//************************************************************************

struct AbsFluff {
  static std::atomic<int> s_alive;
  AbsFluff() { ++s_alive; }
  virtual ~AbsFluff() {
    std::cout << " ----> Destructor of AbsFluff called for " << this 
              << ". After return, left alive " << --s_alive << std::endl;
  } 
  virtual void foo() = 0;
  virtual void cfoo() const = 0;
};

std::atomic<int> AbsFluff::s_alive;

struct DerivedFluff : public AbsFluff {
  int m_int;
  float m_float;
  std::string m_string;
  virtual ~DerivedFluff() 
  {
    //    cout << " ----> Destructor of DerivedFluff called for " << this << std::endl;
  } 
  std::string m_anotherString;
  DerivedFluff() : AbsFluff(), m_int(1), m_float(-379.456f),  
		   m_string("this is the Fluff struct"),
		   m_anotherString("this is the DerivedFluff struct") {  }
  DerivedFluff(const DerivedFluff& rhs) :   
    AbsFluff(), 
    m_int(rhs.m_int), m_float(-379.456f), 
    m_string("this is the Fluff struct") { }
  DerivedFluff& operator= (const DerivedFluff&) = delete;

  virtual void foo() { /* cout << "foo called" << std::endl; */ }
  virtual void cfoo() const { /* cout << "foo called" << std::endl; */ }
};

class FluffContainer : public DataVector<DerivedFluff>
{
public:
  FluffContainer() : DataVector<DerivedFluff>() { };
  virtual ~FluffContainer() {}
};


void test1 ()
{
  std::cout << "*** DataVector_test test1 BEGIN ***" << std::endl;
  typedef DataVector<int> IntVector;
  IntVector intV;
  myassert(intV.empty());
  intV.reserve(10);
  myassert(intV.capacity() == 10);
  myassert(intV.capacity_v() == 10);

  unsigned int i;
  const unsigned int IFILL(3);
  for (i=0; i<IFILL; ++i)  intV.push_back(new int(i));
  myassert (IFILL == intV.size());
  myassert (IFILL == intV.size_v());
  myassert(std::distance(intV.begin(), intV.end()) == (int)IFILL);
  myassert(*(intV.front()) == 0);
  myassert(2 == *(intV.back()));
  intV.front() = new int(7);
  myassert(7 == *(intV.front()));
  *(intV.back()) = 3;
  myassert(3 == *(intV.back()));
  {
    std::ostream_iterator<int*> os(std::cout, " ");
    std::cout << "intV: " << std::hex;
    std::copy(intV.begin(), intV.end(), os);
    std::cout << std::dec <<std::endl;
    std::ostream_iterator<int> intos(std::cout, " ");
    /*1.30    boost::indirect_iterator_generator<DataVector<int>::iterator, int>::type
      intVF(intV.begin()), intVL(intV.end());   */
    boost::indirect_iterator<DataVector<int>::iterator, int> intVF(intV.begin());
    boost::indirect_iterator<DataVector<int>::iterator, int> intVL(intV.end());
    std::copy(intVF, intVL, intos);
    std::cout <<std::endl;
  }

  //FIXME does not work  DataVector<int> intV2(5, new int(5));
  DataVector<int> intV2(5);
  for (unsigned int i=0; i<5; ++i) intV2[i]=new int(5);
  myassert (5 == intV2.size());
  myassert (5 == *intV2[3]);

  intV2[4] = new int(6);
  myassert (6 == *intV2[4]);
  {
    std::ostream_iterator<int*> os(std::cout, " ");
    std::cout << "intV2: " << std::hex;
    std::copy(intV2.begin(), intV2.end(), os);
    std::cout << std::dec <<std::endl;
    std::ostream_iterator<int> intos(std::cout, " ");
    /*1.30    boost::indirect_iterator_generator<DataVector<int>::iterator, int>::type
      intV2F(intV2.begin()), intV2L(intV2.end()); */
    boost::indirect_iterator<DataVector<int>::iterator, int> intV2F(intV2.begin());
    boost::indirect_iterator<DataVector<int>::iterator, int> intV2L(intV2.end());
    std::copy(intV2F, intV2L, intos);
    std::cout <<std::endl;
  }
  

  DataVector<int>* pintVZeros(new DataVector<int>(10));
  delete pintVZeros; pintVZeros=0;

  DataVector<int> testConst;
  testConst.push_back(new int(-1));
  testConst.push_back(new int(-2));
  DataVector<int>::const_iterator ci(testConst.begin()), ce(testConst.end());
  while (ci != ce) {
    const int* UNUSED(cp) = *ci;
#if 0
    //this is bad but unfortunately correct: a DataVector<int> is like a
    //vector<int*>. vector<int*>::const_iterator has type (int* const)*
    //and not (const int*)*. It is hence legal to do
    int* p = *ci;  //assign a int* const to a int*
    *p = 77; //OUCH!
#endif
    ++ci;
  }
  

  DataVector<int> intV3(intV2.begin(), intV2.end());
  myassert (5 == intV3.size());
  myassert (5 == *intV3[3]);
  myassert (6 == *intV3[4]);

  int *oldInt(0), *newInt(new int(345));
  intV3.swapElement(3, newInt, oldInt);

  myassert(*oldInt == 5);
  myassert(345 == *intV3[3]);
  // can't do this --- we don't own it, since intV3 is a view container.
  // oldInt is still owned by intV2.
  //delete oldInt;


  DataVector<DerivedFluff>* dfluff = new DataVector<DerivedFluff>();
  dfluff->push_back(new DerivedFluff());
  dfluff->push_back(new DerivedFluff());
  int count(AbsFluff::s_alive);
  {
    std::ostream_iterator<DerivedFluff*> os(std::cout, " ");
    std::cout << "dfluff: " << std::hex;
    std::copy(dfluff->begin(), dfluff->end(), os);
    std::cout << std::dec <<std::endl;
  }

  myassert(2 == dfluff->size()); 


  DataVector<DerivedFluff>::iterator iter = dfluff->begin();
  int n = 0;
  for (; iter != dfluff->end(); iter++) {
    (*iter)->foo();
    n++;
  }
  myassert (2 == n);

  ((*dfluff)[0])->cfoo();

  (*dfluff->back()).foo();
  //FIXME dfluff->back()->foo(); DNC: side effect of ElementProxy

  // copy:

  DataVector<DerivedFluff>* fluff2 = new DataVector<DerivedFluff>(*dfluff);
  myassert(2 == fluff2->size());
  myassert(count == AbsFluff::s_alive);

  // copy using iterators:
  DataVector<DerivedFluff>* fluff3 = new DataVector<DerivedFluff>(dfluff->begin(), dfluff->end());
  myassert(2 == fluff3->size());
  myassert(count == AbsFluff::s_alive);

  // delete copy:
  std::cout << " --> Now deleting copied vector, but it should not delete elements as it does not own them. You should not see message of Element Destructor" << std::endl;
  delete fluff2; fluff2=0;
  std::cout << " <-- delete done" << std::endl;
  myassert(count == AbsFluff::s_alive);

  std::cout << " --> Now deleting vector copied via iterator. You should NOT see the elements being deleted" << std::endl;
  count = AbsFluff::s_alive;
  delete fluff3; fluff3=0;
  std::cout << " <-- delete done" << std::endl;
  myassert(count == AbsFluff::s_alive);

  std::cout << " --> Now resizing DataVector<DerivedFluff>" << std::endl;
  std::cout << " --> You should see one DerivedFluff object being deleted" << std::endl;
  count = AbsFluff::s_alive - 1;
  dfluff->resize(dfluff->size()-1); //reduce dfluff by one
  std::cout << " <-- resize done" << std::endl;
  myassert(count == AbsFluff::s_alive);

  std::cout << " --> Now deleting DataVector<DerivedFluff>. You should see all remaining DerivedFluff objects being deleted" << std::endl;
  count = AbsFluff::s_alive - dfluff->size();
  delete dfluff; dfluff=0;
  std::cout << " <-- delete done" << std::endl;
  myassert(count == AbsFluff::s_alive);

  //  std::cout << " Working with a subclass of DataVector " << std::endl;
  FluffContainer fc;
  DerivedFluff* f1 = new DerivedFluff();
  DerivedFluff* f2 = new DerivedFluff();
  DerivedFluff* f3 = new DerivedFluff();
  DerivedFluff* f4 = new DerivedFluff();
  fc.push_back(f1);
  fc.push_back(f2);
  fc.push_back(f3);
  fc.push_back(f4);

  count = AbsFluff::s_alive;
  std::cout << " --> Now erasing one element of the DerivedFluff container. You should see one instance being deleted" << std::endl;
  fc.erase(fc.begin(), fc.begin()+1);
  std::cout << " <-- erase done" << std::endl;
  myassert(fc.size() == 3);
  myassert(count - 1  == AbsFluff::s_alive);

  FluffContainer copyFluff, *pcopy;
  count = AbsFluff::s_alive;
  copyFluff = fc;
  myassert(count == AbsFluff::s_alive); //op = makes pcopy a view
  count = AbsFluff::s_alive;
  pcopy = new FluffContainer(fc);
  myassert(count == AbsFluff::s_alive); //copy constr makes pcopy a view
  delete pcopy; pcopy=0;
  myassert(count == AbsFluff::s_alive); //copy constr makes pcopy a view
  
  // DataVector with ownership:
  DataVector<DerivedFluff> dfluff5;
  DerivedFluff *pdf( new DerivedFluff());
  count = AbsFluff::s_alive + 9;
  //  dfluff5.assign(10, pdf);
  dfluff5.push_back(pdf); //take ownership
  for(unsigned int i=1; i<10; ++i) dfluff5.push_back(new DerivedFluff(*pdf));
  
  myassert(dfluff5.size() == 10);
  myassert(count == AbsFluff::s_alive);
  //  dfluff5.insert(dfluff5.begin(), 10, (DerivedFluff*)0);
  DataVector<DerivedFluff>::iterator it5(dfluff5.begin());
  for(unsigned int i=0; i<10; ++i) it5 = dfluff5.insert(it5, 0);
  myassert(count == AbsFluff::s_alive);
  myassert(dfluff5.size() == 20);

  // DataVector without ownership:
  DataVector<DerivedFluff>* dfluff4 =
    new DataVector<DerivedFluff>(SG::VIEW_ELEMENTS);
  DerivedFluff *p1(new DerivedFluff()), *p2(new DerivedFluff());
  dfluff4->push_back(p1);
  dfluff4->push_back(p2);
  count = AbsFluff::s_alive;
  std::cout << " --> Now resizing view container. You should NOT see the elements being deleted" << std::endl;
  dfluff4->resize(0); //empties dfluff4;
  std::cout << " <-- resize done" << std::endl;
  myassert(0 == dfluff4->size());
  myassert(count == AbsFluff::s_alive);

  std::cout << " --> Now deleting two DerivedFluff instances" << std::endl;
  delete p1;
  delete p2;
  std::cout << " <-- delete done" << std::endl;
  

  count = AbsFluff::s_alive;
  //  dfluff4->assign(10, pdf);
  //whatever this means...
  for(unsigned int i=0; i<10; ++i) dfluff4->push_back(pdf);
  myassert(dfluff4->size() == 10);
  myassert(count == AbsFluff::s_alive);
  //  dfluff4->insert(dfluff4->begin(), 10, (DerivedFluff*)0);
  DataVector<DerivedFluff>::iterator it(dfluff4->begin());
  for(unsigned int i=0; i<10; ++i) it = dfluff4->insert(it, 0);
  myassert(count == AbsFluff::s_alive);
  myassert(dfluff4->size() == 20);

  delete dfluff4; dfluff4=0;

  //FIXME what is the right thing to do? Put in 5 NULLs or give a compilation
  // error?
  count = AbsFluff::s_alive;
  DataVector<AbsFluff> absFluff(5);
  myassert(count == AbsFluff::s_alive);

  absFluff.resize(10);
  myassert(count == AbsFluff::s_alive);

  count = AbsFluff::s_alive + 5;
  //  absFluff.resize(15, new DerivedFluff);
  absFluff.resize(15);
  for(unsigned int i=10; i<15; ++i) absFluff[i]=new DerivedFluff;
  myassert(count == AbsFluff::s_alive);

  count = AbsFluff::s_alive - 2;
  size_t dsize(absFluff.size() - 2);
  absFluff.pop_back();
  absFluff.pop_back();
  myassert(count == AbsFluff::s_alive);
  myassert(absFluff.size() == dsize);

  std::cout << "*** DataVector_test OK ***" <<std::endl;
}


//************************************************************************



// Make sure myassert is working.
void test2_myassert()
{
  bool caught = false;
  try {
    myassert (false);
  }
  catch (dv_test_err&) {
    caught = true;
  }
  if (!caught) abort();
}


// Test check_indices
void test2_check_indices()
{
  DataVector<BB> vbb;
  typename DataVector<BB>::PtrVector& pbb ATLAS_THREAD_SAFE =
    const_cast<typename DataVector<BB>::PtrVector&> (vbb.stdcont());
  pbb.push_back (new BB(1));
  pbb.push_back (new BB(2));
  CHECK_INDICES(vbb);

  DataVector<BAux> vba;
  typename DataVector<BAux>::PtrVector& pba ATLAS_THREAD_SAFE =
    const_cast<typename DataVector<BAux>::PtrVector&> (vba.stdcont());
  pba.push_back (new BAux(1));
  pba.push_back (new BAux(2));
  bool caught = false;
  try {
    CHECK_INDICES(vba);
  }
  catch (const dv_test_err&) {
    caught = true;
  }
  myassert (caught);
  vba.clear();
  vba.push_back (new BAux(1));
  vba.push_back (new BAux(2));
  CHECK_INDICES(vba);

  BB* b1 = new BB(1);
  BAux* b2 = new BAux(2);
  CHECK_INDEX_CLEAR(b1);
  CHECK_INDEX_CLEAR(b2);
  vbb.push_back (b1);
  vba.push_back (b2);
  CHECK_INDEX_CLEAR(b1);
  caught = false;
  try {
    CHECK_INDEX_CLEAR(b2);
  }
  catch (const dv_test_err&) {
    caught = true;
  }
  //myassert (caught); //xxx

  vba.clear (SG::VIEW_ELEMENTS);
  vbb.clear (SG::VIEW_ELEMENTS);
  b1 = new BB(1);
  b2 = new BAux(2);
  CHECK_INDEX_CLEAR(b1);
  CHECK_INDEX_CLEAR(b2);
  vbb.push_back (b1);
  vba.push_back (b2);
  CHECK_INDEX_CLEAR(b1);
  CHECK_INDEX_CLEAR(b2);
  delete b1;
  delete b2;
}


void test2()
{
  std::cout << "test2\n";
  test2_myassert();
  test2_check_indices();
  // Calls to do_test2 for specific classes are split out into separate
  // unit tests DataVector_X_test, in order to keep the amount of memory
  // needed for compilation down to something reasonable.
  myassert (ClassName<DataVector<AA> >::name() == "DataVector<AA>");
  myassert (ClassName<DataVector<BB> >::name() == "DataVector<BB>");
  myassert (ClassName<DataVector<CC> >::name() == "DataVector<CC>");
  myassert (ClassName<DataVector<M> >::name() == "DataVector<M>");
  myassert (ClassName<DataVector<P> >::name() == "DataVector<P>");
  myassert (ClassName<DataVector<R> >::name() == "DataVector<R>");
  dump_classid<AA> ();
  dump_classid<BB> ();
  dump_classid<CC> ();
#if 0
  std::ostringstream os;
  os << ClassID_traits<DataVector<AA> >::packageInfo();
  myassert (os.str().substr(0,7) == "$Name: " || os.str() == "Package-00-00-00");
  os.str ("");
  os << ClassID_traits<DataVector<CC> >::packageInfo();
  myassert (os.str() == "Package-00-00-00");
#endif
}


void test_baseinfo()
{
  std::vector<CLID> clids;
  clids = SG::BaseInfo<DataVector<CC> >::get_bases();
  std::vector<CLID> exp1 = list_of
    (ClassID_traits<DataVector<AA> >::ID())
    (ClassID_traits<DataVector<BB> >::ID())
    (ClassID_traits<DataVector<CC> >::ID());
  std::sort (clids.begin(), clids.end());
  std::sort (exp1.begin(), exp1.end());
  myassert (clids == exp1);

  clids = SG::BaseInfo<DataVector<R> >::get_bases();
  std::vector<CLID> exp2 = list_of
    (ClassID_traits<DataVector<R> >::ID())
    (ClassID_traits<DataVector<N> >::ID())
    (ClassID_traits<DataVector<O> >::ID())
    (ClassID_traits<DataVector<Q> >::ID())
    (ClassID_traits<DataVector<M> >::ID());
  std::sort (clids.begin(), clids.end());
  std::sort (exp2.begin(), exp2.end());
  myassert (clids == exp2);

  myassert (SG::BaseInfo<DataVector<CC> >::is_base
	    (ClassID_traits<DataVector<BB> >::ID()));
  myassert (! SG::BaseInfo<DataVector<CC> >::is_virtual
	    (ClassID_traits<DataVector<BB> >::ID()));
  myassert (SG::BaseInfo<DataVector<R> >::is_base
	    (ClassID_traits<DataVector<M> >::ID()));
  myassert (SG::BaseInfo<DataVector<R> >::is_virtual
	    (ClassID_traits<DataVector<M> >::ID()));
}


void test_eltbaseinfo()
{
  std::vector<const std::type_info*> tinfos;
  tinfos = SG::BaseInfo<CC>::get_ti_bases();
  std::vector<const std::type_info*> exp1 = list_of
    (&typeid(AA))
    (&typeid(BB))
    (&typeid(CC));
  std::sort (tinfos.begin(), tinfos.end());
  std::sort (exp1.begin(), exp1.end());
  myassert (tinfos == exp1);

  tinfos = SG::BaseInfo<R>::get_ti_bases();
  std::vector<const std::type_info*> exp2 = list_of
    (&typeid(R))
    (&typeid(N))
    (&typeid(O))
    (&typeid(Q))
    (&typeid(M))
    (&typeid(MM));
  std::sort (tinfos.begin(), tinfos.end());
  std::sort (exp2.begin(), exp2.end());
  myassert (tinfos == exp2);

  myassert (SG::BaseInfo<CC>::is_base (typeid(BB)));
  myassert (! SG::BaseInfo<CC>::is_virtual (typeid(BB)));
  myassert (SG::BaseInfo<R>::is_base (typeid(M)));
  myassert (SG::BaseInfo<R>::is_virtual (typeid(M)));
}


void test_copyconvert()
{
  DataVector<P> vec;
  for (int i=0; i<10; i++)
    vec.push_back (new P(i));

  myassert (SG::BaseInfo<DataVector<P> >::is_base (typeid (DataVector<M>)));
  myassert (! SG::BaseInfo<DataVector<P> >::is_base (typeid (DataVector<MM>)));

  DataModel_detail::DVLInfoBase* info;
  void* contp = DataModel_detail::dvl_convert (vec,
                                               typeid (DataVector<AA>), info);
  myassert (contp == 0);
  DataVector<MM>::dvlinfo();
  contp = DataModel_detail::dvl_convert (vec, typeid (DataVector<MM>), info);
  DataVector<MM>& mmvec = *(DataVector<MM>*)contp;
  myassert (mmvec.ownPolicy() == SG::VIEW_ELEMENTS);
  myassert (mmvec.size() == 10);
  for (int i=0; i<10; i++) {
    myassert (mmvec[i]->mm == i + 100);
    MM* mm = mmvec[i];
    P* pp = dynamic_cast<P*> (mm);
    if (!pp) std::abort();
    myassert (pp->x == i);
  }

  for (int i=10; i<20; i++)
    vec.push_back (new P(i));
  DataModel_detail::dvl_update (vec, contp, info);
  myassert (mmvec.ownPolicy() == SG::VIEW_ELEMENTS);
  myassert (mmvec.size() == 20);
  for (int i=0; i<20; i++) {
    myassert (mmvec[i]->mm == i + 100);
    MM* mm = mmvec[i];
    P* pp = dynamic_cast<P*> (mm);
    myassert (pp != nullptr);
    myassert (pp->x == i);
  }
}


void test_iterate()
{
  DataVector<P> vp;
  for (int i=0; i < 10; i++) {
    vp.push_back (new P(i));
    vp.push_back (0);
  }

  const DataModel_detail::DVLInfoBase& info1 = DataVector<P>::dvlinfo();
  DataModel_detail::DVLIteratorBase* iterator = info1.iterator (&vp);
  int ii = 0;
  while (const void* p = iterator->next()) {
    const P* pp = reinterpret_cast<const P*> (p);
    myassert (pp->x == ii);
    ++ii;
  }
  delete iterator;

  DataVector<M>& vm = vp;
  const DataModel_detail::DVLInfoBase& info2 = DataVector<M>::dvlinfo();
  iterator = info2.iterator (&vm);
  ii = 0;
  while (const void* p = iterator->next()) {
    const M* pp = reinterpret_cast<const M*> (p);
    myassert (pp->x == ii++);
  }
  delete iterator;
}


bool cmpfloat (float x1, float x2)
{
  float den = std::abs(x1) + std::abs(x2);
  if (den == 0) den = 1;
  return std::abs(x1-x2)/den < 1e-6;
}


struct AuxR
{
  float x;
  float y;
  float r() const { return hypot (x, y); }
};


template <class T>
void test_auxdata1()
{
  DataVector<T> v;
  SG::AuxStoreInternal store;
  v.setStore (&store);
  
  typename T::template Accessor<AuxR> auxr ("auxr");
  typename T::template Accessor<bool> auxb ("auxb");

  v.push_back (new T(1));
  v.push_back (new T(2));
  v.push_back (new T(3));
  v[0]->setaux();
  v[1]->setaux();
  v[2]->setaux();

  checkaux (v);

  auxr(*v[2]).x = 3;
  auxr(*v[2]).y = 4;
  assert (cmpfloat (auxr(*v[2]).r(), 5));

  auxb(*v[0]) = true;
  auxb(*v[2]) = false;
  assert (auxb(*v[0]) == true);
  assert (auxb(*v[1]) == false);
  assert (auxb(*v[2]) == false);

  typedef SG::AuxElementComplete<T> Tcomp;
  Tcomp* t4 = new Tcomp(4);
  t4->setaux();
  auxr(*t4).x = 3;
  auxr(*t4).y = 4;

  v.push_back (t4);

  v[3]->checkaux();
  assert (cmpfloat (auxr(*v[3]).r(), 5));

  t4->releasePrivateStore();
  v[3]->checkaux();
  assert (cmpfloat (auxr(*v[3]).r(), 5));

  v.erase (v.begin() + 1);
  assert (v.size() == 3);
  checkaux (v);
  assert (v[0]->x == 1);
  assert (v[1]->x == 3);
  assert (v[2]->x == 4);
  v.push_back (new T(5));
  v[3]->setaux();
  v.push_back (new T(6));
  v[4]->setaux();
  // 1 3 4 5 6
  v.erase (v.begin()+2, v.begin()+4);
  assert (v.size() == 3);
  checkaux (v);
  assert (v[0]->x == 1);
  assert (v[1]->x == 3);
  assert (v[2]->x == 6);

  DataVector<T> v2;
  SG::AuxStoreInternal store2;
  v2.setStore (&store2);
  v2.push_back (new T(7));
  v2[0]->setaux();

  v2.swap (v);
  assert (v2.size() == 3);
  assert (v.size() == 1);
  checkaux (v2);
  assert (v2[0]->x == 1);
  assert (v2[1]->x == 3);
  assert (v2[2]->x == 6);
  checkaux (v);
  assert (v[0]->x == 7);

  Tcomp* t8 = new Tcomp(8);
  t8->setaux();
  v2.insert (v2.begin()+1, t8);
  t8->releasePrivateStore();
  checkaux (v2);

  Tcomp* tmp[2] = { new Tcomp(9), new Tcomp(10) };
  tmp[0]->setaux();
  tmp[1]->setaux();
  v2.insert (v2.begin()+1, tmp, tmp+2);
  tmp[0]->releasePrivateStore();
  tmp[1]->releasePrivateStore();
  assert (v2.size() == 6);
  checkaux (v2);
  assert (v2[0]->x == 1);
  assert (v2[1]->x == 9);
  assert (v2[2]->x == 10);
  assert (v2[3]->x == 8);
  assert (v2[4]->x == 3);
  assert (v2[5]->x == 6);

  v.reserve(6);
  v.resize(6);
  assert (v.size() == 6);
  v.pop_back();
  assert (v.size() == 5);
  v.clear();
  assert (v.size() == 0);

  //**************************

  v.push_back (new T(1));
  v.push_back (new T(2));
  v.push_back (new T(3));
  v[0]->setaux();
  v[1]->setaux();
  v[2]->setaux();
  checkaux(v);

  std::vector<Tcomp> vc;
  for (int i=0; i < 3; i++)
    vc.push_back (Tcomp (*v[i]));
  std::reverse (vc.begin(), vc.end());
  for (int i=0; i < 3; i++)
    *v[i] = vc[i];
  assert (v[0]->x == 3);
  assert (v[1]->x == 2);
  assert (v[2]->x == 1);
  checkaux(v);
  CHECK_INDICES(v);

  Tcomp* t9 = new Tcomp (9);
  t9->setaux();
  checkaux(t9);
  v.push_back(t9);
  checkaux(t9);
  checkaux(v);
  assert (!t9->usingPrivateStore());
  T* t10 = 0;
  v.swapElement (3, 0, t10);
  assert (t9 == t10);
  assert (t10->usingPrivateStore());
  checkaux(t10);
}


void test_auxdata()
{
  std::cout << "test_auxdata\n";
  test_auxdata1<AAux>();
  test_auxdata1<BAux>();
  test_auxdata1<CAux>();
}


// If the xAOD base classes are used, they will always report that the static
// auxids are present.  But if the container is empty, they are not actually
// retrievable.  We need to work around that.
class TestEmptyStore
  : public SG::IAuxStore
{
public:
  virtual const void* getData (SG::auxid_t) const { return 0;}
  virtual void* getDecoration (SG::auxid_t, size_t, size_t) { return 0; }
  virtual const SG::auxid_set_t& getAuxIDs() const { return m_auxids; }
  virtual void lock() { }
  virtual void lockDecoration (SG::auxid_t) { }
  virtual bool clearDecorations() { return false; }
  virtual size_t size() const { return 0; }
  virtual void* getData (SG::auxid_t, size_t, size_t) { return 0; }
  virtual const SG::auxid_set_t& getWritableAuxIDs() const { return m_auxids; }
  virtual bool resize (size_t) { return false; }
  virtual void reserve (size_t) { }
  virtual void shift (size_t, ptrdiff_t) { }
  virtual bool insertMove (size_t, IAuxStore&, const SG::auxid_set_t&) { return false; }

  SG::auxid_set_t m_auxids;
};
void test_emptysort()
{
  std::cout << "test_emptysort\n";
  DataVector<AAux> v;
  TestEmptyStore store;
  v.setStore (&store);
  SG::auxid_t ityp = SG::AuxTypeRegistry::instance().getAuxID<int> ("xint");
  store.m_auxids.insert (ityp);
  v.sort();
}


struct MoveTest
{
  MoveTest(int x=0) : m_v(x) {}
  MoveTest(const MoveTest& other): m_v (other.m_v) {}
  MoveTest(MoveTest&& other): m_v (std::move(other.m_v)) {}
  MoveTest& operator= (const MoveTest& other) {
    if (this != &other) m_v = other.m_v;
    return *this;
  }
  MoveTest& operator= (MoveTest&& other) {
    if (this != &other) m_v = std::move(other.m_v);
    return *this;
  }
  std::vector<int> m_v;
  bool operator== (const MoveTest& other) const { return m_v.size() == other.m_v.size(); }
};


template <class T>
bool wasMoved (const T&) { return true; }

bool wasMoved (const MoveTest& x) { return x.m_v.empty(); }


template <class T>
void test_insertmove1()
{
  typename T::template Accessor<MoveTest> auxm ("auxm");

  DataVector<T> v;
  SG::AuxStoreInternal store;
  v.setStore (&store);

  for (int i=0; i < 5; i++)
    v.push_back (new T(i));
  for (int i=0; i < 5; i++) {
    v[i]->setaux();
    auxm(*v[i]) = MoveTest(i);
  }

  DataVector<T> v2;
  SG::AuxStoreInternal store2;
  v2.setStore (&store2);

  for (int i=0; i < 5; i++)
    v2.push_back (new T(i+10));
  for (int i=0; i < 5; i++) {
    v2[i]->setaux();
    auxm(*v2[i]) = MoveTest(i+10);
  }

  v.insertMove (v.begin()+3, v2);
  assert (v.size() == 10);
  checkaux (v);
  CHECK_INDICES (v);
  for (int i=0; i < 3; i++) {
    assert (v[i]->x == i);
    assert (auxm(*v[i]) == MoveTest(i));
  }
  for (int i=0; i < 5; i++) {
    assert (v[3+i]->x == i+10);
    assert (auxm(*v[3+i]) == MoveTest(i+10));
  }
  for (int i=3; i < 5; i++) {
    assert (v[5+i]->x == i);
    assert (auxm(*v[5+i]) == MoveTest(i));
  }
  assert (v2.ownPolicy() == SG::VIEW_ELEMENTS);
  assert (v2.size() == 5);
  for (int i=0; i < 5; i++) {
    assert (v2[i]->x == i+10);
  }
}


void test_insertmove()
{
  std::cout << "test_insertmove\n";
  test_insertmove1<AAux>();
  test_insertmove1<BAux>();
  test_insertmove1<CAux>();
}


class VBase
  : public DataVector<int>
{
public:
  VBase (SG::OwnershipPolicy ownPolicy, float f)
    : DataVector<int> (ownPolicy), m_f(f)
  {
  }

  float m_f;
};


// Test forwarding ctor of ConstDataVector.
void test_constctor()
{
  std::cout << "test_constctor\n";
  ConstDataVector<VBase> vb (SG::OWN_ELEMENTS, 1.5);
  assert (vb.asDataVector()->m_f == 1.5);
}


int main()
{
  test1();
  test2();
  test_baseinfo();
  test_eltbaseinfo();
  test_copyconvert();
  test_iterate();
  test_auxdata();
  test_emptysort();
  test_insertmove();
  test_constctor();
  return 0;
}

#else

int main() {
   return 0;
}

#endif // not XAOD_STANDALONE

// xxx
// per-object flagging of aux data presence?
