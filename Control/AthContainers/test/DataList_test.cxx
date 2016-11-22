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
#include <boost/iterator_adaptors.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/concept_check.hpp>
#include "SGTools/CLASS_DEF.h"
#include "CxxUtils/unused.h"


class dl_test_err
  : public std::exception
{
public:
  explicit dl_test_err (const char* file,
                        int line,
                        const char* what);
  virtual ~dl_test_err() throw() {}
  virtual const char* what() const throw() { return m_what.c_str(); }
private:
  std::string m_what;
};


dl_test_err::dl_test_err (const char* file,
                          int line,
                          const char* what)
{
  std::ostringstream os;
  os << file << ":" << line << " myassertion failure: " << what;
  m_what = os.str();
}


void throw_dl_test_err (const char* file, int line, const char* what)
{
  throw (dl_test_err (file, line, what));
}

#define myassert(X) do {     \
    if (!(X)) { \
      throw_dl_test_err (__FILE__, __LINE__, #X); \
    } \
  } while (0)


#define ATHCONTAINERS_ASSERT(X) myassert(X)

#include "AthContainers/DataList.h"
#include "AthContainers/ConstDataList.h"
#include "AthContainers/exceptions.h"
#include "TestTools/random.h"


Athena_test::RNG stlrand;


using boost::assign::list_of;


//************************************************************************

struct AbsFluff {
  static int s_alive;
  AbsFluff() { ++s_alive; }
  virtual ~AbsFluff() {
    std::cout << " ----> Destructor of AbsFluff called for " << this 
              << ". After return, left alive " << --s_alive << std::endl;
  } 
  virtual void foo() = 0;
  virtual void cfoo() const = 0;
};

int AbsFluff::s_alive = 0;

struct DerivedFluff : public AbsFluff {
  int m_int;
  float m_float;
  std::string m_string;
  virtual ~DerivedFluff() 
  {
    //    std::cout << " ----> Destructor of DerivedFluff called for " << this << std::endl;
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

  virtual void foo() { /* std::cout << "foo called" << std::endl; */ }
  virtual void cfoo() const { /* std::cout << "foo called" << std::endl; */ }
};

class FluffList : public DataList<DerivedFluff>
{
public:
  FluffList() : DataList<DerivedFluff>() { };
  virtual ~FluffList() {}
};


void test1 ()
{
  std::cout << "*** DataList_test test1 BEGIN ***" << std::endl;
  typedef DataList<int> IntList;
  IntList intL;
  myassert(intL.empty());

  unsigned int i;
  const unsigned int IFILL(3);
  for (i=0; i<IFILL; ++i)  intL.push_back(new int(i));
  myassert (IFILL == intL.size());
  myassert(std::distance(intL.begin(), intL.end()) == (int)IFILL);
  myassert( *(intL.front()) == 0);
  myassert(2 == *(intL.back()));
  intL.front() = new int(7);
  myassert(7 == *(intL.front()));
  *(intL.back()) = 3;
  myassert(3 == *(intL.back()));
  {
    std::ostream_iterator<int*> os(std::cout, " ");
    std::cout << "intL: " << std::hex;
    std::copy(intL.begin(), intL.end(), os);
    std::cout << std::dec <<std::endl;
    std::ostream_iterator<int> intos(std::cout, " ");
    /*1.30    boost::indirect_iterator_generator<DataList<int>::iterator, int>::type
      intLF(intL.begin()), intLL(intL.end());   */
    boost::indirect_iterator<DataList<int>::iterator, int> intLF(intL.begin());
    boost::indirect_iterator<DataList<int>::iterator, int> intLL(intL.end());
    copy(intLF, intLL, intos);
    std::cout <<std::endl;
  }

  //FIXME does not work  DataList<int> intL2(5, new int(5));
  DataList<int> intL2;
  for (unsigned int i=0; i<5; ++i) intL2.push_back( new int(5) );
  myassert (5 == intL2.size());
  {
   DataList<int>::const_iterator itrList = intL2.begin();
   myassert (5 == **itrList);
   ++itrList; myassert(5 == **itrList);
   ++itrList; myassert(5 == **itrList);
   ++itrList; myassert(5 == **itrList);
   ++itrList; myassert(5 == **itrList);
  }

  {
   DataList<int>::iterator itrList = intL2.begin();
   std::advance( itrList, 4 );
   (*itrList) = new int(6);
   itrList = intL2.begin();
   std::advance( itrList, 4 );
   myassert (6 == **itrList);
  }
  {
    std::ostream_iterator<int*> os(std::cout, " ");
    std::cout << "intL2: " << std::hex;
    std::copy(intL2.begin(), intL2.end(), os);
    std::cout << std::dec <<std::endl;
    std::ostream_iterator<int> intos(std::cout, " ");
    /*1.30    boost::indirect_iterator_generator<DataList<int>::iterator, int>::type
      intL2F(intL2.begin()), intL2L(intL2.end()); */
    boost::indirect_iterator<DataList<int>::iterator, int> intL2F(intL2.begin());
    boost::indirect_iterator<DataList<int>::iterator, int> intL2L(intL2.end());
    copy(intL2F, intL2L, intos);
    std::cout <<std::endl;
  }
  

  DataList<int>* pintLZeros(new DataList<int>(10));
  delete pintLZeros; pintLZeros=0;

  DataList<int> testConst;
  testConst.push_back(new int(-1));
  testConst.push_back(new int(-2));
  DataList<int>::const_iterator ci(testConst.begin()), ce(testConst.end());
  while (ci != ce) {
    const int* UNUSED(cp) = *ci;
    cp = 0; //remove warning
    //this is bad but unfortunately correct: a DataList<int> is like a
    //list<int*>. list<int*>::const_iterator has type (int* const)*
    //and not (const int*)*. It is hence legal to do
    int* p = *ci;  //assign a int* const to a int*
    *p = 77; //OUCH!
    ++ci;
  }
  

  DataList<int> intL3(intL2.begin(), intL2.end());
  myassert (5 == intL3.size());
  {
    DataList<int>::const_iterator itrList = intL3.begin();
    std::advance( itrList, 3 );
    myassert (5 == **itrList );
    std::advance( itrList, 1 );
    myassert (6 == **itrList );
  }
  

  DataList<DerivedFluff>* dfluff = new DataList<DerivedFluff>();
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


  DataList<DerivedFluff>::iterator iter = dfluff->begin();
  int n = 0;
  for (; iter != dfluff->end(); iter++) {
    (*iter)->foo();
    n++;
  }
  myassert (2 == n);

  (dfluff->front())->cfoo();

  (*(dfluff->back())).foo();
  //FIXME dfluff->back()->foo(); DNC: side effect of ElementProxy

  // copy:

  DataList<DerivedFluff>* fluff2 = new DataList<DerivedFluff>(*dfluff);
  myassert(2 == fluff2->size());
  myassert(count == AbsFluff::s_alive);

  // copy using iterators:
  DataList<DerivedFluff>* fluff3 = new DataList<DerivedFluff>(dfluff->begin(), dfluff->end());
  myassert(2 == fluff3->size());
  myassert(count == AbsFluff::s_alive);

  // delete copy:
  std::cout << " --> Now deleting copied list, but it should not delete elements as it does not own them. You should not see message of Element Destructor" << std::endl;
  delete fluff2; fluff2=0;
  std::cout << " <-- delete done" << std::endl;
  myassert(count == AbsFluff::s_alive);

  std::cout << " --> Now deleting list copied via iterator. You should NOT see the elements being deleted" << std::endl;
  count = AbsFluff::s_alive;
  delete fluff3; fluff3=0;
  std::cout << " <-- delete done" << std::endl;
  myassert(count == AbsFluff::s_alive);

  std::cout << " --> Now resizing DataList<DerivedFluff>" << std::endl;
  std::cout << " --> You should see one DerivedFluff object being deleted" << std::endl;
  count = AbsFluff::s_alive - 1;
  const int dfluffNewSize = dfluff->size()-1;
  dfluff->resize(dfluffNewSize); //reduce dfluff by one
  std::cout << " <-- resize done" << std::endl;
  myassert(count == AbsFluff::s_alive);

  std::cout << " --> Now deleting DataList<DerivedFluff>. You should see all remaining DerivedFluff objects being deleted" << std::endl;
  count = AbsFluff::s_alive - dfluff->size();
  delete dfluff; dfluff=0;
  std::cout << " <-- delete done" << std::endl;
  myassert(count == AbsFluff::s_alive);

  //  std::cout << " Working with a subclass of DataList " << std::endl;
  FluffList fc;
  {
  DerivedFluff* f1 = new DerivedFluff();
  DerivedFluff* f2 = new DerivedFluff();
  DerivedFluff* f3 = new DerivedFluff();
  DerivedFluff* f4 = new DerivedFluff();
  
  fc.push_back(f1);
  fc.push_back(f2);
  fc.push_back(f3);
  fc.push_back(f4);
  }
  count = AbsFluff::s_alive;
  std::cout << " --> Now erasing one element of the DerivedFluff container. You should see one instance being deleted" << std::endl;
  fc.erase(fc.begin(), ++fc.begin());
  std::cout << " <-- erase done" << std::endl;
  myassert(fc.size() == 3);
  myassert(count - 1  == AbsFluff::s_alive);

  FluffList copyFluff, *pcopy;
  count = AbsFluff::s_alive;
  copyFluff = fc;
  myassert(count == AbsFluff::s_alive); //op = makes pcopy a view
  count = AbsFluff::s_alive;
  pcopy = new FluffList(fc);
  myassert(count == AbsFluff::s_alive); //copy constr makes pcopy a view
  delete pcopy; pcopy=0;
  myassert(count == AbsFluff::s_alive); //copy constr makes pcopy a view
  
  // DataList with ownership:
  DataList<DerivedFluff> dfluff5;
  DerivedFluff *pdf( new DerivedFluff());
  count = AbsFluff::s_alive + 9;
  //  dfluff5.assign(10, pdf);
  dfluff5.push_back(pdf); //take ownership
  for(unsigned int i=1; i<10; ++i) dfluff5.push_back(new DerivedFluff(*pdf));
  
  myassert(dfluff5.size() == 10);
  myassert(count == AbsFluff::s_alive);
  //  dfluff5.insert(dfluff5.begin(), 10, (DerivedFluff*)0);
  DataList<DerivedFluff>::iterator it5(dfluff5.begin());
  for(unsigned int i=0; i<10; ++i) it5 = dfluff5.insert(it5, 0);
  myassert(count == AbsFluff::s_alive);
  myassert(dfluff5.size() == 20);

  // DataList without ownership:
  DataList<DerivedFluff>* dfluff4 =
    new DataList<DerivedFluff>(SG::VIEW_ELEMENTS);
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
  DataList<DerivedFluff>::iterator it(dfluff4->begin());
  for(unsigned int i=0; i<10; ++i) it = dfluff4->insert(it, 0);
  myassert(count == AbsFluff::s_alive);
  myassert(dfluff4->size() == 20);

  delete dfluff4; dfluff4=0;

  //FIXME what is the right thing to do? Put in 5 NULLs or give a compilation
  // error?
  count = AbsFluff::s_alive;
  DataList<AbsFluff> absFluff(5);
  myassert(count == AbsFluff::s_alive);

  absFluff.resize(10);
  myassert(count == AbsFluff::s_alive);

  count = AbsFluff::s_alive + 5;
  //  absFluff.resize(15, new DerivedFluff);
  absFluff.resize(15);
  for(unsigned int i=10; i<15; ++i) absFluff.push_back( new DerivedFluff );
  myassert(count == AbsFluff::s_alive);

  count = AbsFluff::s_alive - 2;
  size_t dsize(absFluff.size() - 2);
  absFluff.pop_back();
  absFluff.pop_back();
  myassert(count == AbsFluff::s_alive);
  myassert(absFluff.size() == dsize);

  std::cout << "*** DataList_test test1 OK ***" <<std::endl;
}


//************************************************************************

std::list<int> dtor_log;
std::list<int> get_dtor_log()
{
  std::list<int> out;
  out.swap (dtor_log);
  // the order of element deletion within a container is unpredictable,
  // due to the sort() in remove_duplicates().
  out.sort();
  return out;
}
void clear_dtor_log()
{
  dtor_log.clear();
}
void check_dtor_log(int a=-1,int b=-1,int c=-1,int d=-1, int e=-1, int f=-1)
{
  std::list<int> v;
  if (a != -1) v.push_back(a);
  if (b != -1) v.push_back(b);
  if (c != -1) v.push_back(c);
  if (d != -1) v.push_back(d);
  if (e != -1) v.push_back(e);
  if (f != -1) v.push_back(f);
  v.sort();
  myassert (get_dtor_log() == v);
}

template <class ITERATOR>
ITERATOR next (ITERATOR it, size_t n=1)
{
  std::advance (it, n);
  return it;
}
template <class ITERATOR>
ITERATOR prev (ITERATOR it, size_t n=1)
{
  std::advance (it, -n);
  return it;
}
template <class CONT>
typename CONT::const_value_type nth (const CONT& cont, size_t n)
{
  return *::next(cont.begin(), n);
}

struct AA
{
  AA (int the_x=0): x(the_x) {}
  ~AA() { dtor_log.push_back (x); }
  int x;
  bool operator< (const AA& other) const { return x < other.x; }
};
struct BB : public AA
{
  BB (int the_x=0) : AA(the_x) {}
};
struct CC : public BB
{
  CC (int the_x=0) : BB(the_x) {}
};

DATALIST_BASE(BB, AA);
DATALIST_BASE(CC, BB);

CLASS_DEF( DataList<AA> , 9981 , 1 )
CLASS_DEF( DataList<BB> , 9982 , 1 )
CLASS_DEF( DataList<CC> , 139782528 , 2 )

struct MM
{
  MM (int the_x=0) : mm(the_x) {}
  virtual ~MM() {}
  int mm;
};
struct M
{
  M (int the_x=0) : x(the_x) {}
  virtual ~M() { dtor_log.push_back (x); }
  int x;
  bool operator< (const M& other) const { return x < other.x; }
};
struct N : virtual public M, public MM
{
  N (int the_x=0) : M(the_x), MM(the_x+100) {}
};
struct O : virtual public M
{
  O (int the_x=0) : M(the_x) {}
};
struct P : virtual public N, virtual public O
{
  P (int the_x=0) : M(the_x), N(the_x) {}
};

DATALIST_VIRTBASES1 (N, M);
DATALIST_VIRTBASES1 (O, M);
DATALIST_VIRTBASES2 (P, N, O);

SG_BASES2 (N, SG_VIRTUAL(M), MM);
SG_BASE (O, SG_VIRTUAL(M));
SG_BASES2 (P, SG_VIRTUAL(N), SG_VIRTUAL(O));

struct Q : virtual public M
{
  Q (int the_x=0) : M(the_x) {}
};
struct R : virtual public N, virtual public O, virtual public Q
{
  R (int the_x=0) : M(the_x) {}
};

DATALIST_VIRTBASES1 (Q, M);
DATALIST_VIRTBASES3 (R, N, O, Q);

CLASS_DEF( DataList<R> , 9983 , 1 )
CLASS_DEF( DataList<N> , 9984 , 1 )
CLASS_DEF( DataList<O> , 9985 , 1 )
CLASS_DEF( DataList<Q> , 9986 , 1 )
CLASS_DEF( DataList<M> , 9987 , 1 )


// Make sure myassert is working.
void test2_myassert()
{
  bool caught = false;
  try {
    myassert (false);
  }
  catch (dl_test_err&) {
    caught = true;
  }
  if (!caught) abort();
}

// Initial tests.
template <class B, class D>
void test2_initial()
{
  DataList<D> vd;
  vd.push_back (new D(1));
  DataList<B>& vb = vd;
  const B* b = vb.front();
  const D* d = vd.front();
  myassert (b->x == 1);
  myassert (d->x == 1);

  DataList<D> d2 (10);
  myassert (d2.size() == 10);
  DataList<D> d3 (d2);
  myassert (d3.size() == 10);
}


// Test default ctor
template <class DL>
void test2_default_ctor1()
{
  typedef typename DL::base_value_type T;
  clear_dtor_log();
  {
    DL v1;
    v1.push_back (new T(1));
    myassert (v1.ownPolicy() == SG::OWN_ELEMENTS);
    DL v2 (SG::VIEW_ELEMENTS);
    v2.push_back (new T(2));
    myassert (v2.ownPolicy() == SG::VIEW_ELEMENTS);
  }
  check_dtor_log (1);
}
template <class B, class D>
void test2_default_ctor()
{
  test2_default_ctor1<DataList<B> > ();
  test2_default_ctor1<DataList<D> > ();
}


// Test sized ctor
template <class DL>
void test2_sized_ctor1()
{
  typedef typename DL::base_value_type T;
  clear_dtor_log();
  {
    DL v1 (10);
    myassert (v1.size() == 10);
    myassert (v1.front() == 0);
    v1.push_back (new T(1));
    DL v2 (10, SG::VIEW_ELEMENTS);
    myassert (v2.size() == 10);
    myassert (v2.front() == 0);
    v2.push_back (new T(2));
  }
  check_dtor_log (1);
}
template <class B, class D>
void test2_sized_ctor()
{
  test2_sized_ctor1<DataList<B> > ();
  test2_sized_ctor1<DataList<D> > ();
  test2_sized_ctor1<ConstDataList<DataList<D> > > ();
}


// Test insertion ctor
template <class DL> 
void test2_insertion_ctor1()
{
  typedef typename DL::base_value_type T;
  clear_dtor_log();
  {
    std::list<T*> tt;
    tt.push_back (new T(1));
    tt.push_back (new T(2));
    DL v1 (tt.begin(), tt.end());
    DL v2 (tt.begin(), tt.end(), SG::OWN_ELEMENTS);
    myassert (v1.size() == 2);
    myassert (v2.size() == 2);
    {
      typename DL::const_iterator itrV1 = v1.begin();
      typename std::list<T*>::const_iterator itrTT = tt.begin();
      myassert( *itrV1 == *itrTT );
      ++itrV1;
      ++itrTT;
      myassert( *itrV1 == *itrTT );
    }
    {
      typename DL::const_iterator itrV2 = v2.begin();
      typename std::list<T*>::const_iterator itrTT = tt.begin();
      myassert( *itrV2 == *itrTT );
      ++itrV2;
      ++itrTT;
      myassert( *itrV2 == *itrTT );
    }
  }
  check_dtor_log (1, 2);
}
template <class B, class D>
void test2_insertion_ctor()
{
  test2_insertion_ctor1<DataList<B> > ();
  test2_insertion_ctor1<DataList<D> > ();
  test2_insertion_ctor1<ConstDataList<DataList<D> > > ();
  {
    // We can put D pointers into a B container.
    std::list<D*> dd;
    dd.push_back (new D(1));
    dd.push_back (new D(2));
    DataList<B> v1 (dd.begin(), dd.end());
    myassert (v1.size() == 2);
    {
      typename DataList<B>::const_iterator itrV1 = v1.begin();
      typename std::list<D*>::const_iterator itrDD = dd.begin();
      myassert( *itrV1 == *itrDD );
      ++itrV1;
      ++itrDD;
      myassert( *itrV1 == *itrDD );
     }
  }
#ifdef COMPILE_ERROR
  {
    // But not the other way 'round.
    std::list<B*> dd;
    dd.push_back (new B(1));
    dd.push_back (new B(2));
    DataList<D> v1 (dd.begin(), dd.end());
    {
      typename std::list<B>::const_iterator itrV1 = v1.begin();
      typename DataList<D*>::const_iterator itrDD = dd.begin();
      myassert( *itrV1 == *itrDD );
      ++itrV1;
      ++itrDD;
      myassert( *itrV1 == *itrDD );
     }
  }
#endif
}


// Test copy ctor
template <class DL>
void test2_copy_ctor1()
{
  typedef typename DL::base_value_type T;
  clear_dtor_log();
  {
    DL v1;
    v1.push_back (new T(1));
    v1.push_back (new T(2));
    DL v2 (v1);
    myassert (v2.size() == 2);
    {
      typename DL::const_iterator itrV1 = v1.begin();
      typename DL::const_iterator itrV2 = v2.begin();
      myassert( *itrV1 == *itrV2 );
      ++itrV1;
      ++itrV2;
      myassert( *itrV1 == *itrV2 );
    }
  }
  check_dtor_log (1, 2);
}
template <class B, class D>
void test2_copy_ctor()
{
  test2_copy_ctor1<DataList<B> >();
  test2_copy_ctor1<DataList<D> >();
  test2_copy_ctor1<ConstDataList<DataList<D> > >();
  {
    // Can copy derived list to base list.
    DataList<D> v1;
    v1.push_back (new D(1));
    v1.push_back (new D(2));
    DataList<B> v2 (v1);
    myassert (v2.size() == 2);
    {
      typename DataList<D>::const_iterator itrV1 = v1.begin();
      typename DataList<B>::const_iterator itrV2 = v2.begin();
      myassert( *itrV1 == *itrV2 );
      ++itrV1;
      ++itrV2;
      myassert( *itrV1 == *itrV2 );
    }
  }
  check_dtor_log (1, 2);
#ifdef COMPILE_ERROR
  {
    // But not the other way 'round.
    DataList<B> v1;
    v1.push_back (new B(1));
    v1.push_back (new B(2));
    DataList<D> v2 (v1);
    myassert (v2.size() == 2);
    {
      typename DataList<B>::const_iterator itrV1 = v1.begin();
      typename DataList<D>::const_iterator itrV2 = v2.begin();
      myassert( *itrV1 == *itrV2 );
      ++itrV1;
      ++itrV2;
      myassert( *itrV1 == *itrV2 );
    }
  }
  check_dtor_log (1, 2);
#endif
}



// Test destructor.
template <class DL>
void test2_dtor1()
{
  typedef typename DL::base_value_type T;
  clear_dtor_log();
  {
    DL v1;
    v1.push_back (new T(1));
    v1.push_back (new T(2));
  }
  check_dtor_log (1, 2);
  {
    DL v1 (SG::VIEW_ELEMENTS);
    v1.push_back (new T(1));
    v1.push_back (new T(2));
  }
  check_dtor_log ();
#ifdef DO_REMOVE_DUPLICATES
  {
    DL v1;
    T* t = new T(1);
    v1.push_back (t);
    v1.push_back (t);
  }
  check_dtor_log (1);
#endif
}
template <class B, class D>
void test2_dtor()
{
  test2_dtor1<DataList<B> > ();
  test2_dtor1<DataList<D> > ();
  test2_dtor1<ConstDataList<DataList<D> > > ();
}


// Test push_front
template <class B, class D>
void test2_push_front()
{
  DataList<B> vb;
  vb.push_front (new B(1));
  vb.push_front (new D(2));
  myassert (vb.size() == 2);
  {
    typename DataList<B>::const_iterator itrVB = vb.begin();
    myassert( (*itrVB)->x == 2 );
    ++itrVB;
    myassert( (*itrVB)->x == 1 );
  }

  DataList<D> vd;
  vd.push_front (new D(3));
  myassert (vd.size() == 1);
  myassert (vd.front()->x == 3);

  ConstDataList<DataList<D> > cvd;
  cvd.push_front (new D(3));
  cvd.push_front (new D(4));
  myassert (cvd.size() == 2);
  myassert (cvd.front()->x == 4);
  myassert (cvd.back()->x == 3);

  // This isn't allowed.
  DataList<B>& vd2 = vd;
  bool caught = false;
  try {
    vd2.push_front (new B(4));
  }
  catch (dl_test_err&) {
    caught = true;
  }
  catch (SG::ExcInsertionInBaseClass&) {
    caught = true;
  }
  myassert (caught);
}

 
// Test push_back
template <class B, class D>
void test2_push_back()
{
  DataList<B> vb;
  vb.push_back (new B(1));
  vb.push_back (new D(2));
  myassert (vb.size() == 2);
  {
    typename DataList<B>::const_iterator itrVB = vb.begin();
    myassert( (*itrVB)->x == 1 );
    ++itrVB;
    myassert( (*itrVB)->x == 2 );
  }

  DataList<D> vd;
  vd.push_back (new D(3));
  myassert (vd.size() == 1);
  myassert (vd.front()->x == 3);

  ConstDataList<DataList<D> > cvd;
  cvd.push_back (new D(3));
  cvd.push_back (new D(4));
  myassert (cvd.size() == 2);
  myassert (cvd.front()->x == 3);
  myassert (cvd.back()->x == 4);

  // This isn't allowed.
  DataList<B>& vd2 = vd;
  bool caught = false;
  try {
    vd2.push_back (new B(4));
  }
  catch (dl_test_err&) {
    caught = true;
  }
  catch (SG::ExcInsertionInBaseClass&) {
    caught = true;
  }
  myassert (caught);
}

 
// Test size
template <class DL, class B, class D>
void test2_size1()
{
  DL vb;
  myassert (vb.size() == 0);
  vb.push_back (new B(1));
  vb.push_back (new D(2));
  myassert (vb.size() == 2);
}
template <class B, class D>
void test2_size()
{
  test2_size1<DataList<B>, B, D>();
  test2_size1<ConstDataList<DataList<B> >, B, D>();
}


// Test stdcont
template <class B, class D>
void test2_stdcont()
{
  DataList<B> vb;
  vb.push_back (new B(1));
  vb.push_back (new D(2));
  const typename DataList<B>::PtrList& vv = vb.stdcont();
  myassert (vv.size() == 2);
  myassert (vv.front()->x == 1);
}


// Test empty
template <class DL>
void test2_empty1()
{
  typedef typename DL::base_value_type T;
  DL vb;
  myassert (vb.empty());
  vb.push_back (new T(1));
  myassert (!vb.empty());
}
template <class B, class D>
void test2_empty()
{
  test2_empty1<DataList<B> >();
  test2_empty1<ConstDataList<DataList<B> > >();
}


// Test max_size
template <class B, class D>
void test2_max_size()
{
  DataList<B> vb;
  myassert (vb.max_size() == std::list<void*>().max_size());
  ConstDataList<DataList<B> > cvb;
  myassert (cvb.max_size() == std::list<void*>().max_size());
}


// Test front const, back const
template <class DL, class B, class D>
void test2_front_back_const1()
{
  DL vd;
  vd.push_back (new B(1));
  vd.push_back (new D(2));
  const DL& cvd = vd;
  const B* dd = cvd.front();
  myassert (dd->x == 1);
  dd = cvd.back();
  myassert (dd->x == 2);
}
template <class B, class D>
void test2_front_back_const()
{
  test2_front_back_const1<DataList<B>, B, D>();
  test2_front_back_const1<DataList<D>, D, D>();
  test2_front_back_const1<ConstDataList<DataList<D> >, D, D>();
}


// Test begin const, end const
template <class DL>
void test2_begin_end_const1()
{
  typedef typename DL::base_value_type T;
  DL v;
  v.push_back (new T(1));
  v.push_back (new T(2));
  const DL& cv = v;
  myassert ( std::distance( cv.begin(), cv.end() ) == 2);
  typename DL::const_iterator i = cv.begin();
  myassert (i != cv.end());
  const T* tt = *i;
  myassert (tt->x == 1);
  ++i;
  myassert (i != cv.end());
  myassert ((*i)->x == 2);
  i++;
  myassert (i == cv.end());
  i--;
  myassert ((*i)->x == 2);
  --i;
  myassert ((*i)->x == 1);
  myassert (i == cv.begin());
}
template <class B, class D>
void test2_begin_end_const()
{
  test2_begin_end_const1<DataList<B> > ();
  test2_begin_end_const1<DataList<D> > ();
  test2_begin_end_const1<ConstDataList<DataList<D> > > ();
}


// Test rbegin const, rend const
template <class DL>
void test2_rbegin_rend_const1()
{
  typedef typename DL::base_value_type T;
  DL v;
  v.push_back (new T(1));
  v.push_back (new T(2));
  const DL& cv = v;
  myassert ( std::distance( cv.rbegin(), cv.rend() ) == 2);
  typename DL::const_reverse_iterator i = cv.rbegin();
  myassert (i != cv.rend());
  const T* tt = *i;
  myassert (tt->x == 2);
  ++i;
  myassert (i != cv.rend());
  myassert ((*i)->x == 1);
  i++;
  myassert (i == cv.rend());
  i--;
  myassert ((*i)->x == 1);
  --i;
  myassert ((*i)->x == 2);
  myassert (i == cv.rbegin());
}
template <class B, class D>
void test2_rbegin_rend_const()
{
  test2_rbegin_rend_const1<DataList<B> > ();
  test2_rbegin_rend_const1<DataList<D> > ();
  test2_rbegin_rend_const1<ConstDataList<DataList<D> > > ();
}


// Test ElementProxy operations.
template <class B, class D>
void test2_elementproxy()
{
  clear_dtor_log();

  {
    DataList<B> vb1;
    vb1.push_back (new B(1));
    vb1.push_back (new D(2));

    DataList<B> vb2 (SG::VIEW_ELEMENTS);
    vb2.push_back (new B(3));
    vb2.push_back (new D(4));

    DataList<D> vd1;
    vd1.push_back (new D(5));
    vd1.push_back (new D(6));

    DataList<D> vd2 (SG::VIEW_ELEMENTS);
    vd2.push_back (new D(7));
    vd2.push_back (new D(8));

    {
      typename DataList<B>::const_iterator itrVB1 = vb1.begin();
      myassert( (*itrVB1)->x == 1 );
      ++itrVB1;
      myassert( (**itrVB1).x == 2 );
    }

    D* dd = vd1.front();
    myassert (dd->x == 5);
    {
      typename DataList<D>::const_iterator itrVD1 = vd1.begin();
      ++itrVD1;
      myassert( (*itrVD1)->x == 6);
    }

    // vb1: (1,2)  vb2: (3,4)  vd1: (5,6)  vd2: (7,8)
    // Test ownership transfer.
    // VIEW->VIEW doesn't delete anything.
    vb2.front() = *(++vb2.begin());
    vd2.front() = *(++vd2.begin());
    check_dtor_log();

    // vb1: (1,2)  vb2: (4,4)  vd1: (5,6)  vd2: (8,8)
    // OWN->VIEW doesn't delete anything.
    vb2.front() = vb1.front();
    vd2.front() = vd1.front();
    check_dtor_log();

    // vb1: (1,2)  vb2: (1,4)  vd1: (5,6)  vd2: (5,8)
    // VIEW->OWN deletes old val, takes ownership.
    *(++vb1.begin()) = *(++vb2.begin());
    *(++vd1.begin()) = *(++vd2.begin());
    check_dtor_log (2, 6);

    // vb1: (1,4)  vb2: (1,4)  vd1: (5,8)  vd2: (5,8)
    // OWN->OWN not allowed.
    bool caught = false;
    try {
      vb1.front() = *(++vb1.begin());
    }
    catch (dl_test_err&) {
      caught = true;
    }
    catch (SG::ExcInsertionInBaseClass&) {
      caught = true;
    }
    myassert (caught);

    caught = false;
    try {
      vd1.front() = *(++vd1.begin());
    }
    catch (dl_test_err&) {
      caught = true;
    }
    catch (SG::ExcInsertionInBaseClass&) {
      caught = true;
    }
    myassert (caught);

    // Unless it's the same element.
    vb1.front() = vb1.front();
    B* bb = vb1.front();
    vb1.front() = bb;

    // Check that we can't manage to put a B into a D container
    // using ElementProxy.
    DataList<B>& vd2_b = vd2;
    caught = false;
    try {
      vd2_b.front() = vb2.front();
    }
    catch (dl_test_err&) {
      caught = true;
    }
    catch (SG::ExcInsertionInBaseClass&) {
      caught = true;
    }
    myassert (caught);

    // On the other hand, we can put a D in a B container.
    vb2.front() = vd2_b.front();

    // vb1: (1,4)  vb2: (5,4)  vd1: (5,8)  vd2: (5,8)

    // Test assignment from explicit pointers.
    vb1.front() = new B(9);
    check_dtor_log (1);
    vb2.front() = new B(10);
    check_dtor_log ();
    // vb1: (9,4)  vb2: (10,4)  vd1: (5,8)  vd2: (5,8)
    vd2.front() = new D(11);
    check_dtor_log ();
    vd1.front() = new D(12);
    check_dtor_log (5);
    // vb1: (9,4)  vb2: (10,4)  vd1: (12,8)  vd2: (11,8)
  }
  check_dtor_log (12, 8, 9, 4);
}


// Test front, back
template <class DL>
void test2_front_back1()
{
  typedef typename DL::base_value_type T;
  clear_dtor_log();
  DL v;
  v.push_back (new T(1));
  v.push_back (new T(2));
  myassert (v.front()->x == 1);
  myassert (v.back()->x == 2);
  v.front() = new T(3);
  check_dtor_log (1);
  myassert (v.front()->x == 3);
}
// Test front, back
template <class B, class D>
void test2_front_back()
{
  test2_front_back1<DataList<B> > ();
  test2_front_back1<DataList<D> > ();
  test2_front_back1<ConstDataList<DataList<D> > > ();
}


// Test swapElement with iterator
template <class B, class D>
void test2_swapelement_iter()
{
  clear_dtor_log();
  {
    DataList<B> vb;
    vb.push_back (new B(1));
    vb.push_back (new B(2));

    B* bold;
    vb.swapElement (::next(vb.begin()), new B(3), bold);
    myassert (nth(vb, 1)->x == 3);
    myassert (bold->x == 2);
    delete bold;
    check_dtor_log (2);

    DataList<D> vd;
    vd.push_back (new D(4));
    vd.push_back (new D(5));

    D* dold;
    vd.swapElement (::next(vd.begin()), new D(6), dold);
    myassert (nth(vd,1)->x == 6);
    myassert (dold->x == 5);
    delete dold;
    check_dtor_log (5);

    DataList<B>& vd_b = vd;
    bool caught = false;
    try {
      vd_b.swapElement (::next(vd_b.begin()), new B(10), bold);
    }
    catch (dl_test_err&) {
      caught = true;
    }
    catch (SG::ExcInsertionInBaseClass&) {
      caught = true;
    }
    myassert (caught);

    ConstDataList<DataList<D> > cvd;
    cvd.push_back (new D(7));
    cvd.push_back (new D(8));

    const D* cdold;
    cvd.swapElement (::next(cvd.begin()), new D(9), cdold);
    myassert (nth(cvd,1)->x == 9);
    myassert (cdold->x == 8);
    delete cdold;
    check_dtor_log (8);
  }

  check_dtor_log (4, 6, 1, 3, 7, 9);
}


// Test resize
template <class DL>
void test2_resize1()
{
  typedef typename DL::base_value_type T;
  clear_dtor_log();
  DL v1;
  v1.push_back (new T(1));
  v1.push_back (new T(2));
  DL v2 (v1);
  myassert (v2.size() == 2);
  v2.resize (1);
  check_dtor_log();
  myassert (v2.size() == 1);
  myassert (v2.front()->x == 1);
  myassert (v1.size() == 2);
  v1.resize (1);
  check_dtor_log(2);
  myassert (v1.size() == 1);
  myassert (v1.front()->x == 1);
  v1.resize (3);
  myassert (v1.size() == 3);
  myassert (v1.front()->x == 1);
  {
    typename DL::const_iterator itrV1 = v1.begin();
    ++itrV1;
    myassert( *itrV1 == 0 );
    ++itrV1;
    myassert( *itrV1 == 0 );
  }
}
template <class B, class D>
void test2_resize()
{
  test2_resize1<DataList<B> >();
  test2_resize1<DataList<D> >();
  test2_resize1<ConstDataList<DataList<D> > >();
}


// Test pop_front
template <class DL>
void test2_pop_front1()
{
  typedef typename DL::base_value_type T;
  clear_dtor_log();
  DL v1;
  v1.push_back (new T(1));
  v1.push_back (new T(2));
  DL v2 (v1);
  myassert (v2.size() == 2);
  v2.pop_front();
  myassert (v2.size() == 1);
  check_dtor_log();
  myassert (v1.size() == 2);
  v1.pop_front();
  myassert (v1.size() == 1);
  myassert( v1.front()->x == 2 );
  check_dtor_log(1);
}
template <class B, class D>
void test2_pop_front()
{
  test2_pop_front1<DataList<B> > ();
  test2_pop_front1<DataList<D> > ();
  test2_pop_front1<ConstDataList<DataList<D> > > ();
}

// Test pop_back
template <class DL>
void test2_pop_back1()
{
  typedef typename DL::base_value_type T;
  clear_dtor_log();
  DL v1;
  v1.push_back (new T(1));
  v1.push_back (new T(2));
  DL v2 (v1);
  myassert (v2.size() == 2);
  v2.pop_back();
  myassert (v2.size() == 1);
  check_dtor_log();
  myassert (v1.size() == 2);
  v1.pop_back();
  myassert (v1.size() == 1);
  check_dtor_log(2);
}
template <class B, class D>
void test2_pop_back()
{
  test2_pop_back1<DataList<B> > ();
  test2_pop_back1<DataList<D> > ();
  test2_pop_back1<ConstDataList<DataList<D> > > ();
}


// Test begin, end
template <class DL>
void test2_begin_end1()
{
  typedef typename DL::base_value_type T;
  clear_dtor_log();
  DL v1;
  v1.push_back (new T(1));
  v1.push_back (new T(2));
  myassert ( std::distance( v1.begin(), v1.end() ) == 2);
  typename DL::value_type tt = *v1.begin();
  myassert (tt->x == 1);
  {
    typename DL::const_iterator itrV1 = v1.begin();
    ++itrV1;
    myassert( (*itrV1)->x == 2);
  }
  *v1.begin() = new T(3);
  check_dtor_log(1);
  myassert (v1.front()->x == 3);

  DL v2 (SG::VIEW_ELEMENTS);
  v2.push_back (new T(4));
  v2.push_back (new T(5));
  *v2.begin() = *v1.begin();
  check_dtor_log();
  myassert (v2.front()->x == 3);
  
  // (3,2)  (3,5)
  *(--v1.end()) = *(--v2.end());
  check_dtor_log(2);
  {
    typename DL::const_iterator itrV1 = v1.begin();
    ++itrV1;
    myassert( (*itrV1)->x == 5);
  }
  
  // (3,5)  (3,5)
  *v2.begin() = *(++v2.begin());
  check_dtor_log();
  myassert (v2.front()->x == 5);

  bool caught = false;
  try {
    *v1.begin() = *(++v1.begin());
  }
  catch (dl_test_err&) {
    caught = true;
  }
  catch (SG::ExcInsertionInBaseClass&) {
    caught = true;
  }
  myassert (caught);

  // Check creating a const_iterator from an iterator.
  typename DL::const_iterator ci = v2.begin();
  myassert ((*ci)->x == 5);

  // Check comparing iterators and const_iterators.
  typename DL::iterator i = v2.begin();
  myassert (i == ci);
  myassert (ci == i);
  myassert (! (i != ci));
  myassert (! (ci != i));

  v2.sort();
}
template <class B, class D>
void test2_begin_end()
{
  test2_begin_end1<DataList<B> > ();
  test2_begin_end1<DataList<D> > ();
  test2_begin_end1<ConstDataList<DataList<D> > > ();

  DataList<D> vd (1);
  DataList<B>& vb = vd;
  bool caught = false;
  try {
    *vb.begin() = new B(1);
  }
  catch (dl_test_err&) {
    caught = true;
  }
  catch (SG::ExcInsertionInBaseClass&) {
    caught = true;
  }
  myassert (caught);
}


// Test rbegin, rend
template <class DL>
void test2_rbegin_rend1()
{
  typedef typename DL::base_value_type T;
  clear_dtor_log();
  DL v1;
  v1.push_back (new T(1));
  v1.push_back (new T(2));
  myassert ( std::distance( v1.rbegin(), v1.rend() ) == 2);
  typename DL::value_type tt = *v1.rbegin();
  myassert (tt->x == 2);
  {
    typename DL::const_reverse_iterator ritrV1 = v1.rbegin();
    ++ritrV1;
    myassert( (*ritrV1)->x == 1 );
  }
  *v1.rbegin() = new T(3);
  check_dtor_log(2);
  {
    typename DL::const_iterator itrV1 = v1.begin();
    ++itrV1;
    myassert( (*itrV1)->x == 3 );
   }

  DL v2 (SG::VIEW_ELEMENTS);
  v2.push_back (new T(4));
  v2.push_back (new T(5));
  // (1,3) (4,5)
  *v2.rbegin() = *v1.rbegin();
  check_dtor_log();
  {
    typename DL::const_iterator itrV2 = v2.begin();
    ++itrV2;
    myassert( (*itrV2)->x == 3 );
  }
  
  // (1,3)  (4,3)
  *(--v1.rend()) = *(--v2.rend());
  check_dtor_log(1);
  myassert (v1.front()->x == 4);
  
  // (4,3)  (4,3)
  (*v2.rbegin()) = *(++v2.rbegin());
  check_dtor_log();
  {
    typename DL::const_iterator itrV2 = v2.begin();
    ++itrV2;
    myassert( (*itrV2)->x == 4 );
  }
  // (4,3)  (4,4)

  bool caught = false;
  try {
    *v1.rbegin() = *(++v1.rbegin());
  }
  catch (dl_test_err&) {
    caught = true;
  }
  catch (SG::ExcInsertionInBaseClass&) {
    caught = true;
  }
  myassert (caught);

  // Check creating a const_iterator from an iterator.
  typename DL::const_reverse_iterator ci = v2.rbegin();
  myassert ((*ci)->x == 4);

  v2.sort();
}
template <class B, class D>
void test2_rbegin_rend()
{
  test2_rbegin_rend1<DataList<B> > ();
  test2_rbegin_rend1<DataList<D> > ();
  test2_rbegin_rend1<ConstDataList<DataList<D> > > ();

  DataList<D> vd (1);
  DataList<B>& vb = vd;
  bool caught = false;
  try {
    *vb.rbegin() = new B(1);
  }
  catch (dl_test_err&) {
    caught = true;
  }
  catch (SG::ExcInsertionInBaseClass&) {
    caught = true;
  }
  myassert (caught);
}


// Test erase(it, it)
template <class DL>
void test2_erase_range1()
{
  typedef typename DL::base_value_type T;
  clear_dtor_log();
  DL v;
  for (int i=0; i < 10; i++)
    v.push_back (new T(i));
  myassert (v.size() == 10);
  
  typename DL::iterator i1;
  {
    typename DL::iterator vItrAtPlus3 = v.begin();
    std::advance( vItrAtPlus3, 3 );
    typename DL::iterator vItrAtMinus3 = v.end();
    std::advance( vItrAtMinus3, -3 );
    i1 = v.erase( vItrAtPlus3, vItrAtMinus3 );
  }
  myassert ((*i1)->x == 7);
  myassert (v.size() == 6);
  check_dtor_log (3, 4, 5, 6);
  {
    typename DL::const_iterator itrV = v.begin();
    std::advance( itrV, 2 );
    myassert( (*itrV)->x == 2 );
    ++itrV;
    myassert( (*itrV)->x == 7 );
  }

  DL v2 (v);
  myassert (v2.size() == 6);
  typename DL::iterator i2;
  {
    typename DL::iterator itrV2AtPlus2 = v2.begin();
    std::advance( itrV2AtPlus2, 2 );
    typename DL::iterator itrV2AtMinus2 = v2.end();
    std::advance( itrV2AtMinus2, -2 );
    i2 = v2.erase( itrV2AtPlus2, itrV2AtMinus2 );
  }
  myassert ((*i2)->x == 8);
  myassert (v2.size() == 4);
  check_dtor_log ();
  {
    typename DL::const_iterator itrV2 = v2.begin();
    ++itrV2;
    myassert( (*itrV2)->x == 1 );
    ++itrV2;
    myassert( (*itrV2)->x == 8 );
  }

#ifdef DO_REMOVE_DUPLICATES
  {
    DL v3;
    T* t = new T(1);
    v3.push_back (t);
    v3.push_back (t);
    v3.push_back (new T(2));
    v3.erase( v3.begin(), ++(++v3.begin()) );
    check_dtor_log (1);
  }
  check_dtor_log(2);
#endif
}
template <class B, class D>
void test2_erase_range()
{
  test2_erase_range1<DataList<B> > ();
  test2_erase_range1<DataList<D> > ();
  test2_erase_range1<ConstDataList<DataList<D> > > ();
}

// Test erase(it)
template <class DL>
void test2_erase_single1()
{
  typedef typename DL::base_value_type T;
  clear_dtor_log();
  DL v1;
  v1.push_back (new T(1));
  v1.push_back (new T(2));
  v1.erase (v1.begin());
  myassert (v1.size() == 1);
  myassert (v1.front()->x == 2);
  check_dtor_log (1);

  DL v2 (SG::VIEW_ELEMENTS);
  v2.push_back (new T(1));
  v2.push_back (new T(2));
  v2.erase (v2.begin());
  myassert (v2.size() == 1);
  myassert (v2.front()->x == 2);
  check_dtor_log ();
}
template <class B, class D>
void test2_erase_single()
{
  test2_erase_single1<DataList<B> > ();
  test2_erase_single1<DataList<D> > ();
  test2_erase_single1<ConstDataList<DataList<D> > > ();
}


// Test operator=
template <class DL>
void test2_operator_assign1()
{
  typedef typename DL::base_value_type T;
  clear_dtor_log();
  {
    DL v1;
    v1.push_back (new T(1));
    v1.push_back (new T(2));
    {
      DL v2;
      v2 = v1;
      myassert (v2.size() == 2);
      myassert (v2.front()->x == 1);
      {
        typename DL::const_iterator itrV2 = v2.begin();
        ++itrV2;
        myassert( (*itrV2)->x == 2 );
      }
    }
    check_dtor_log();

    v1 = v1;
    check_dtor_log();

    {
      DL v2;
      v1 = v2;
      myassert (v1.size() == 0);
    }
    check_dtor_log (1, 2);
  }
  check_dtor_log ();
}
template <class B, class D>
void test2_operator_assign()
{
  test2_operator_assign1<DataList<B> > ();
  test2_operator_assign1<DataList<D> > ();
  test2_operator_assign1<ConstDataList<DataList<D> > > ();

  DataList<D> vd1;
  vd1.push_back (new D(1));
  DataList<B> vb1;
  vb1 = vd1;
  myassert (vb1.size() == 1);
  myassert (vb1.front()->x == 1);

  DataList<B>& vd1_b = vd1;
  bool caught = false;
  try {
    vd1_b = vb1;
  }
  catch (dl_test_err&) {
    caught = true;
  }
  catch (SG::ExcInsertionInBaseClass&) {
    caught = true;
  }
  myassert (caught);
}


// Test insert(it, val)
template <class DL>
void test2_insert_value1()
{
  typedef typename DL::base_value_type T;
  clear_dtor_log();
  {
    DL v1;
    v1.push_back (new T(1));
    v1.push_back (new T(2));
    typename DL::iterator i1 = v1.insert( ++v1.begin(), new T(3));
    myassert (v1.size() == 3);
    myassert ((*i1)->x == 3);
    {
      typename DL::const_iterator itrV1 = v1.begin();
      myassert( (*itrV1)->x == 1 );
      ++itrV1;
      myassert( (*itrV1)->x == 3 );
      ++itrV1;
      myassert( (*itrV1)->x == 2 );
    }

    DL v2 (v1);
    typename DL::iterator i2 = v2.insert( ++v2.begin(), new T(4));
    myassert (v2.size() == 4);
    myassert ((*i2)->x == 4);
  }
  check_dtor_log (1, 3, 2);
}
template <class B, class D>
void test2_insert_value()
{
  test2_insert_value1<DataList<B> > ();
  test2_insert_value1<DataList<D> > ();
  test2_insert_value1<ConstDataList<DataList<D> > > ();

  DataList<D> vd;
  DataList<B>& vb = vd;
  bool caught = false;
  try {
    vb.insert (vb.begin(), new D(1));
  }
  catch (dl_test_err&) {
    caught = true;
  }
  catch (SG::ExcInsertionInBaseClass&) {
    caught = true;
  }
  myassert (caught);
  vd.insert (vd.begin(), new D(1));
}


// Test insert(it, It, It)
template <class DL>
void test2_insert_range1()
{
  typedef typename DL::base_value_type T;
  clear_dtor_log();
  {
    T* tt[] = {new T(1), new T(2)};
    DL v;
    v.insert (v.begin(), tt, tt+2);
    myassert (v.size() == 2);
    {
      typename DL::const_iterator itrV = v.begin();
      myassert( (*itrV)->x == 1 );
      ++itrV;
      myassert( (*itrV)->x == 2 );
    }
  }
  check_dtor_log (1, 2);

  {
    T* tt[] = {new T(1), new T(2)};
    DL v (SG::VIEW_ELEMENTS);
    v.insert (v.begin(), tt, tt+2);
    myassert (v.size() == 2);
    {
      typename DL::const_iterator itrV = v.begin();
      myassert( (*itrV)->x == 1 );
      ++itrV;
      myassert( (*itrV)->x == 2 );
    }
  }
  check_dtor_log ();
}
template <class B, class D>
void test2_insert_range()
{
  test2_insert_range1<DataList<B> > ();
  test2_insert_range1<DataList<D> > ();
  test2_insert_range1<ConstDataList<DataList<D> > > ();

  DataList<D> vd;
  DataList<B>& vb = vd;
  bool caught = false;
  D* tt[] = {new D(1), new D(2)};
  try {
    vb.insert (vb.begin(), tt, tt+2);
  }
  catch (dl_test_err&) {
    caught = true;
  }
  catch (SG::ExcInsertionInBaseClass&) {
    caught = true;
  }
  myassert (caught);
  vd.insert (vd.begin(), tt, tt+2);
}


// Test clear()
template <class DL>
void test2_clear1()
{
  typedef typename DL::base_value_type T;
  clear_dtor_log();
  DL v1;
  v1.push_back (new T(1));
  v1.push_back (new T(2));
  v1.clear();
  check_dtor_log (1, 2);
  DL v2 (SG::VIEW_ELEMENTS);
  v2.push_back (new T(1));
  v2.push_back (new T(2));
  v2.clear();
  check_dtor_log ();
  myassert (v2.ownPolicy() == SG::VIEW_ELEMENTS);
  v2.clear(SG::OWN_ELEMENTS);
  myassert (v2.ownPolicy() == SG::OWN_ELEMENTS);
  v2.push_back (new T(1));
  v2.push_back (new T(2));
  v2.clear(SG::VIEW_ELEMENTS);
  myassert (v2.ownPolicy() == SG::VIEW_ELEMENTS);
  check_dtor_log (1, 2);
}
template <class B, class D>
void test2_clear()
{
  test2_clear1<DataList<B> > ();
  test2_clear1<DataList<D> > ();
  test2_clear1<ConstDataList<DataList<D> > > ();
}


// Test swap()
template <class DL>
void test2_swap1()
{
  typedef typename DL::base_value_type T;
  clear_dtor_log();
  {
    DL v1;
    v1.push_back (new T(1));
    v1.push_back (new T(2));
    DL v2;
    v1.swap (v2);
    myassert (v1.size() == 0);
    myassert (v2.size() == 2);
    {
      typename DL::const_iterator itrV2 = v2.begin();
      myassert( (*itrV2)->x == 1 );
      ++itrV2;
      myassert( (*itrV2)->x == 2 );
    }
    check_dtor_log();
  }
  check_dtor_log(1,2);
  {
    DL v1;
    v1.push_back (new T(1));
    v1.push_back (new T(2));
    DL v2;
    swap (v1, v2);
    myassert (v1.size() == 0);
    myassert (v2.size() == 2);
    {
      typename DL::const_iterator itrV2 = v2.begin();
      myassert( (*itrV2)->x == 1 );
      ++itrV2;
      myassert( (*itrV2)->x == 2 );
    }
    check_dtor_log();
  }
  check_dtor_log(1,2);
}
template <class B, class D>
void test2_swap()
{
  test2_swap1<DataList<B> > ();
  test2_swap1<DataList<D> > ();
  test2_swap1<ConstDataList<DataList<D> > > ();

  DataList<D> vd;
  DataList<B>& vb = vd;
  DataList<B> vb2;

  bool caught = false;
  try {
    vb.swap(vb2);
  }
  catch (dl_test_err&) {
    caught = true;
  }
  catch (SG::ExcInsertionInBaseClass&) {
    caught = true;
  }
  myassert (caught);

  caught = false;
  try {
    vb2.swap(vb);
  }
  catch (dl_test_err&) {
    caught = true;
  }
  catch (SG::ExcInsertionInBaseClass&) {
    caught = true;
  }
  myassert (caught);

  caught = false;
  try {
    swap (vb, vb2);
  }
  catch (dl_test_err&) {
    caught = true;
  }
  catch (SG::ExcInsertionInBaseClass&) {
    caught = true;
  }
  myassert (caught);

  caught = false;
  try {
    swap (vb2, vb);
  }
  catch (dl_test_err&) {
    caught = true;
  }
  catch (SG::ExcInsertionInBaseClass&) {
    caught = true;
  }
  myassert (caught);
}


template <class DL>
void test2_assign1()
{
  typedef typename DL::base_value_type T;
  clear_dtor_log();
  {
    DL v;
    v.push_back (new T(0));
    T* t[] = {new T(1), new T(2)};
    v.assign (t, t+2);
    check_dtor_log(0);
  }
  check_dtor_log(1,2);

  {
    DL v (SG::VIEW_ELEMENTS);
    v.push_back (new T(0));
    T* t[] = {new T(1), new T(2)};
    v.assign (t, t+2);
  }
  check_dtor_log();
}
template <class B, class D>
void test2_assign()
{
  test2_assign1<DataList<B> > ();
  test2_assign1<DataList<D> > ();
  test2_assign1<ConstDataList<DataList<D> > > ();

  DataList<D> vd;
  DataList<B>& vb = vd;
  B* t[] = {new B(1), new B(2)};
  bool caught = false;
  try {
    vb.assign (t, t+2);
  }
  catch (dl_test_err&) {
    caught = true;
  }
  catch (SG::ExcInsertionInBaseClass&) {
    caught = true;
  }
  myassert (caught);
}


template <class DL>
void test2_relops1()
{
  typedef typename DL::base_value_type T;
  DL v1;
  v1.push_back (new T(1));
  v1.push_back (new T(2));
  DL v2 (v1);
  DL v3 (v1);
  v3.push_back (new T(3));

  myassert (v1 == v2);
  myassert (!(v1 == v3));
  myassert (v1 != v3);
  myassert (!(v1 != v2));

  myassert (v1 < v3);
  myassert (!(v1 < v2));
  myassert (v3 > v1);
  myassert (!(v2 > v1));

  myassert (v1 <= v3);
  myassert (v1 <= v2);
  myassert (!(v3 <= v1));

  myassert (v3 >= v1);
  myassert (v2 >= v1);
  myassert (!(v1 >= v3));
}
template <class B, class D>
void test2_relops()
{
  test2_relops1<DataList<B> > ();
  test2_relops1<DataList<D> > ();
  test2_relops1<ConstDataList<DataList<D> > > ();
}


template <class T>
struct Comp
{
  bool operator() (const T* a, const T* b) const
  { return *b < *a; }
};

template <class T>
struct PredX
{
  bool operator() ( const T* elem ) const
  { return elem->x > m_cut; }
  int m_cut;
};

template <class T>
struct LessX
{
  bool operator() (const T* a, const T* b) const
  {return a->x < b->x; }
};

template <class T>
struct GreatX
{
  bool operator() (const T* a, const T* b) const
  {return a->x > b->x; }
};

template <class DL, int N>
void test2_sort1_prepare1 (DL& v)
{
  typedef typename DL::base_value_type T;
  T* tmp[N];
  for (int i=0; i<N; ++i)
    tmp[i] = new T(i);
  std::sort (tmp, tmp+N);
  for (int i=0; i<N; ++i)
    tmp[i]->x = i;
  for (int i=0; i<N; ++i)
    v.push_back (tmp[N-1-i]);
}
template <class DL>
void test2_sort1()
{
  typedef typename DL::base_value_type T;
  const int N=10;
  {
    DL v;
    test2_sort1_prepare1<DL, N> (v);
    v.sort();
    typename DL::const_iterator itrV = v.begin();
    for (int i=0; i<N; ++i, ++itrV)
      myassert ( (*itrV)->x == i);
  }

  {
    DL v;
    for (int i=0; i<N; ++i )
      v.push_back (new T(i));
    v.sort(Comp<T>());
    typename DL::const_iterator itrV = v.begin();
    for (int i=0; i<N; ++i, ++itrV)
      myassert ( (*itrV)->x == N-1-i);
  }

  clear_dtor_log();
}
template <class B, class D>
void test2_sort()
{
  test2_sort1<DataList<B> > ();
  test2_sort1<DataList<D> > ();
  test2_sort1<ConstDataList<DataList<D> > > ();
}

template <class DL>
void test2_prepare1( const unsigned int N,
                     const unsigned int ratio,
                     DL& l )
{
  typedef typename DL::base_value_type T;
  for ( unsigned int i = 0; i != N; ++i ) {
    l.push_back( new T( ratio * i ) );
  }
}

template <class DL>
void test2_splice1()
{
  //typedef typename DL::base_value_type T;
  const unsigned int N = 6;
  
  // test splice( iterator, DataList )
  {
    DL l1;
    DL l2;
    test2_prepare1( N, 1, l1 );
    test2_prepare1( N, 2, l2 );
    myassert( l1.size() == N );
    myassert( l2.size() == N );

    // Check VIEW_ELEMENTS
    DL view1 = l1;
    DL view2 = l2;
    myassert( view1.size() == N );
    myassert( view2.size() == N );

    typename DL::iterator itrL1 = view1.begin();
    std::advance( itrL1, N/2 );
    view1.splice( itrL1, view2 );
    
    myassert( view1.size() == 2*N );
    myassert( view2.empty() );
    
    // check content of OWN_ELEMENTS containers
    typename DL::const_iterator itr1 = l1.begin();
    typename DL::const_iterator itr2 = l2.begin();
    for ( unsigned int i = 0; i != N; ++i, ++itr1, ++itr2 ) {
      myassert( (*itr1)->x == static_cast<int>(i) );
      myassert( (*itr2)->x == static_cast<int>(i*2) );
    }
    
    // check OWN_ELEMENTS
    itrL1 = l1.begin();
    std::advance( itrL1, N/2 );
    l1.splice( itrL1, l2 );

    myassert( l1.size() == 2*N );
    myassert( l2.empty() );
  }

  // test splice( iterator, DataList, iterator )
  {
    DL l1;
    DL l2;
    test2_prepare1( N, 1, l1 );
    test2_prepare1( N, 2, l2 );

    // Check VIEW_ELEMENTS
    DL view1 = l1;
    DL view2 = l2;
    myassert( view1.size() == N );
    myassert( view2.size() == N );

    typename DL::iterator itrL1 = view1.begin();
    std::advance( itrL1, N/2 );
    view1.splice( itrL1, view2, view2.begin() );
    
    myassert( view1.size() == N+1 );
    myassert( view2.size() == N-1 );
    
    // check content of OWN_ELEMENTS containers
    typename DL::const_iterator itr1 = l1.begin();
    typename DL::const_iterator itr2 = l2.begin();
    for ( unsigned int i = 0; i != N; ++i, ++itr1, ++itr2 ) {
      myassert( (*itr1)->x == static_cast<int>(i) );
      myassert( (*itr2)->x == static_cast<int>(i*2) );
    }
    
    // check OWN_ELEMENTS
    itrL1 = l1.begin();
    std::advance( itrL1, N/2 );
    l1.splice( itrL1, l2, l2.begin() );

    myassert( l1.size() == N+1 );
    myassert( l2.size() == N-1 );

    itrL1 = l1.begin();
    std::advance( itrL1, N/2 );
    myassert( (*itrL1)->x == 0 );
    ++itrL1;
    myassert( (*itrL1)->x == N/2 );
  }
  
  // test splice( position, DataList, first, last )
  {
    DL l1;
    DL l2;
    
    test2_prepare1( N, 1, l1 );
    test2_prepare1( N, 2, l2 );
    
    // Check VIEW_ELEMENTS
    DL view1 = l1;
    DL view2 = l2;
    myassert( view1.size() == N );
    myassert( view2.size() == N );

    typename DL::iterator itrL1 = view1.begin();
    std::advance( itrL1, N/2 );
    typename DL::iterator first2 = view2.begin();
    ++first2;
    typename DL::iterator last2  = view2.end();
    --last2;
    view1.splice( itrL1, view2, first2, last2 );
    
    myassert( view1.size() == N+(N-2) );
    myassert( view2.size() == 2 );
    
    // check content of OWN_ELEMENTS containers
    typename DL::const_iterator itr1 = l1.begin();
    typename DL::const_iterator itr2 = l2.begin();
    for ( unsigned int i = 0; i != N; ++i, ++itr1, ++itr2 ) {
      myassert( (*itr1)->x == static_cast<int>(i) );
      myassert( (*itr2)->x == static_cast<int>(i*2) );
    }
    
    // check OWN_ELEMENTS
    itrL1 = l1.begin();
    std::advance( itrL1, N/2 );
    first2 = l2.begin();
    ++first2;
    last2  = l2.end();
    --last2;
    
    l1.splice( itrL1, l2, first2, last2 );
    
    myassert( l1.size() == N+(N-2) );
    myassert( l2.size() == 2 );
    
    itrL1 = l1.begin();
    std::advance( itrL1, N/2 );
    myassert( (*itrL1)->x == 2 );
    ++itrL1;
    myassert( (*itrL1)->x == 4 );
    itrL1 = l1.begin();
    std::advance( itrL1, N/2 + (N-2) );
    myassert( (*itrL1)->x == N/2 );
  }
  clear_dtor_log();
}

template< class B, class D >
void test2_splice2()
{
  DataList<B> b1;
  DataList<D> d2;
  
  const unsigned int N = 6;
  test2_prepare1( N, 1, b1 );
  test2_prepare1( N, 1, d2 );

  {
    DataList<B>& b2 = d2;
    bool caught = false;
    try {
      // check we can't splice a DataList<D> via a DataList<B>
      typename DataList<B>::iterator itrB = b2.begin();
      std::advance( itrB, N/2 );
      b2.splice( itrB, b1 );

    }
    catch (dl_test_err&) {
      caught = true;
    }
    catch (SG::ExcInsertionInBaseClass&) {
      caught = true;
    }
    myassert( caught );
  }
  {
    DataList<B>& b2 = d2;
    bool caught = false;
    try {
      // check we can't splice a DataList<D> via a DataList<B>
      typename DataList<B>::iterator itrB = b2.begin();
      std::advance( itrB, N/2 );
      b2.splice( itrB, b1, b1.begin() );

    }
    catch (dl_test_err&) {
      caught = true;
    }
    catch (SG::ExcInsertionInBaseClass&) {
      caught = true;
    }
    myassert( caught );
  }
  {
    DataList<B>& b2 = d2;
    bool caught = false;
    try {
      // check we can't splice a DataList<D> via a DataList<B>
      typename DataList<B>::iterator itrB = b2.begin();
      std::advance( itrB, N/2 );
      b2.splice( itrB, b1, b1.begin(), b1.end() );

    }
    catch (dl_test_err&) {
      caught = true;
    }
    catch (SG::ExcInsertionInBaseClass&) {
      caught = true;
    }
    myassert( caught );
  }
}

template< class B, class D >
void test2_splice()
{
  test2_splice1<DataList<B> >();
  test2_splice1<DataList<D> >();
  test2_splice1<ConstDataList<DataList<D> > >();
  test2_splice2<B,D>();
}

template< class DL >
void test2_remove1()
{
  typedef typename DL::base_value_type T;
  const unsigned int N = 6;
  {
    DL l;
    test2_prepare1( N, 1, l );
    myassert( l.size() == N );
  
    typename DL::iterator itr = l.begin();
    std::advance( itr, N/2 );
    l.remove( *itr );
  
    myassert( l.size() == N-1 );
    itr = l.begin();
    std::advance( itr, N/2 );
    myassert( (*itr)->x == static_cast<int>(N/2 + 1) );
  }
  {
    DL l;
    test2_prepare1( N, 1, l );
    myassert( l.size() == N );
  
    PredX<T> predicate;
    predicate.m_cut = N/2;
    l.remove_if( predicate );
    myassert( l.size() == N/2 + 1);
  }
}

template< class B, class D >
void test2_remove()
{
  test2_remove1<DataList<B> >();
  test2_remove1<DataList<D> >();
  test2_remove1<ConstDataList<DataList<D> > >();
}

template <class DL>
void test2_merge1()
{
  typedef typename DL::base_value_type T;
  const unsigned int N = 6;

  {
    DL l1;
    DL l2;
    test2_prepare1( N, 1, l1 );
    test2_prepare1( N, 2, l2 );
    
    myassert( l1.size() == N );
    myassert( l2.size() == N );
    
    // test VIEW_ELEMENTS
    DL view1 = l1;
    DL view2 = l2;
    
    view1.merge( view2 );
    myassert( view1.size() == 2*N );
    myassert( view2.empty() );
    
    // check content of OWN_ELEMENTS containers
    typename DL::const_iterator itr1 = l1.begin();
    typename DL::const_iterator itr2 = l2.begin();
    for ( unsigned int i = 0; i != N; ++i, ++itr1, ++itr2 ) {
      myassert( (*itr1)->x == static_cast<int>(i) );
      myassert( (*itr2)->x == static_cast<int>(i*2) );
    }
    
    // test OWN_ELEMENTS
    l1.merge( l2 );
    myassert( l1.size() == 2*N );
    myassert( l2.empty() );
  }
  {
    DL l1;
    DL l2;
    test2_prepare1( N, 1, l1 );
    test2_prepare1( N, 2, l2 );
    
    myassert( l1.size() == N );
    myassert( l2.size() == N );
    
    // test VIEW_ELEMENTS
    DL view1 = l1;
    DL view2 = l2;
    
    view1.merge( view2, LessX<T>() );
    myassert( view1.size() == 2*N );
    myassert( view2.empty() );
    
    // check content of OWN_ELEMENTS containers
    typename DL::const_iterator itr1 = l1.begin();
    typename DL::const_iterator itr2 = l2.begin();
    for ( unsigned int i = 0; i != N; ++i, ++itr1, ++itr2 ) {
      myassert( (*itr1)->x == static_cast<int>(i) );
      myassert( (*itr2)->x == static_cast<int>(i*2) );
    }
    
    // test OWN_ELEMENTS
    l1.merge( l2, LessX<T>() );
    myassert( l1.size() == 2*N );
    myassert( l2.empty() );
    
    itr1 = l1.begin();
    myassert( (*itr1)->x == 0 );
    ++itr1;
    myassert( (*itr1)->x == 0 );
    itr1 = --l1.end();
    myassert( (*itr1)->x == static_cast<int>( (N-1)*2 ) );
  }
}

template< class B, class D>
void test2_merge2()
{
  DataList<B> b1;
  DataList<D> d2;
  
  const unsigned int N = 6;
  test2_prepare1( N, 1, b1 );
  test2_prepare1( N, 1, d2 );

  {
    DataList<B>& b2 = d2;
    bool caught = false;
    try {
      // check we can't merge a DataList<D> via a DataList<B>
      b2.merge( b1 );

    }
    catch (dl_test_err&) {
      caught = true;
    }
    catch (SG::ExcInsertionInBaseClass&) {
      caught = true;
    }
    myassert( caught );
  }
  {
    DataList<B>& b2 = d2;
    bool caught = false;
    try {
      // check we can't merge a DataList<D> via a DataList<B>
      b2.merge( b1, LessX<B>() );

    }
    catch (dl_test_err&) {
      caught = true;
    }
    catch (SG::ExcInsertionInBaseClass&) {
      caught = true;
    }
    myassert( caught );
  }
}

template< class B, class D >
void test2_merge()
{
  test2_merge1<DataList<B> >();
  test2_merge1<DataList<D> >();
  test2_merge1<ConstDataList<DataList<D> > >();
  test2_merge2<B, D>();
}

template< class DL >
void test2_unique1()
{
  typedef typename DL::base_value_type T;
  const unsigned int N = 6;
  {
    DL l;
    test2_prepare1( N, 1, l );
    const T * elem = l.front();
    l.push_front( const_cast<T*>(elem) );
    myassert( l.size() == N+1 );

    // [0, 0, 1, 2, 3, 4, 5] => [0, 1, 2, 3, 4, 5]
    l.unique();
    myassert( l.size() == N );
    
    typename DL::const_iterator itr = l.begin();
    for ( unsigned int i = 0; i != N; ++i, ++itr ) {
      myassert( (*itr)->x == static_cast<int>(i) );
    }
  }
  {
    DL l;
    test2_prepare1( N, 1, l );
    const T * elem = l.front();
    l.push_front( const_cast<T*>(elem) );
    myassert( l.size() == N+1 );

    // [0, 0, 1, 2, 3, 4, 5] => [0, 0]
    l.unique( LessX<T>() );
    myassert( l.size() == 2 );

    typename DL::const_iterator itr = l.begin();
    myassert( (*itr)->x == static_cast<int>(0) );
    ++itr;
    myassert( (*itr)->x == static_cast<int>(0) );

    // Prevent crash when l is deleted.
    l.unique();
  }
  // testing VIEW_ELEMENTS
  {
    DL l;
    test2_prepare1( N, 1, l );
    const T * elem = l.front();
    l.push_front( const_cast<T*>(elem) );
    myassert( l.size() == N+1 );

    DL view = l;
    // [0, 0, 1, 2, 3, 4, 5] => [0, 1, 2, 3, 4, 5]
    view.unique();
    myassert( view.size() == N );
    myassert(    l.size() == N+1 );
    typename DL::const_iterator itr = l.begin();
    myassert( (*itr)->x == 0 );
    ++itr;
    for ( unsigned int i = 0; i != N; ++i, ++itr ) {
      myassert( (*itr)->x == static_cast<int>(i) );
    }

    // Prevent crash when l is deleted.
    l.unique();
  }
  // testing VIEW_ELEMENTS
  {
    DL l;
    test2_prepare1( N, 1, l );
    const T * elem = l.front();
    l.push_front( const_cast<T*>(elem) );
    myassert( l.size() == N+1 );

    DL view = l;
    // [0, 0, 1, 2, 3, 4, 5] => [0, 0]
    view.unique( LessX<T>() );
    myassert( view.size() == 2 );
    myassert(    l.size() == N+1 );
    typename DL::const_iterator itr = l.begin();
    myassert( (*itr)->x == 0 );
    ++itr;
    for ( unsigned int i = 0; i != N; ++i, ++itr ) {
      myassert( (*itr)->x == static_cast<int>(i) );
    }

    // Prevent crash when l is deleted.
    l.unique();
  }
}

template< class B, class D >
void test2_unique2()
{
  const unsigned int N = 6;
  {
    DataList<D> d1;
    test2_prepare1( N, 1, d1 );
    const D * elem = d1.front();
    d1.push_front( const_cast<D*>(elem) );
    myassert( d1.size() == N+1 );
    
    // test we can @c unique a derived container from its base container
    DataList<B>& b1 = d1;
    // [0, 0, 1, 2, 3, 4, 5] => [0, 1, 2, 3, 4, 5]
    b1.unique();
    myassert( b1.size() == N );
    myassert( d1.size() == N );
    
    {
      typename DataList<B>::const_iterator itr = b1.begin();
      for ( unsigned int i = 0; i != N; ++i, ++itr ) {
        myassert( (*itr)->x == static_cast<int>(i) );
      }
    }
    {
      typename DataList<D>::const_iterator itr = d1.begin();
      for ( unsigned int i = 0; i != N; ++i, ++itr ) {
        myassert( (*itr)->x == static_cast<int>(i) );
      }
    }
  }
}

template< class B, class D >
void test2_unique()
{
  test2_unique1<DataList<B> >();
  test2_unique1<DataList<D> >();
  test2_unique1<ConstDataList<DataList<D> > >();
  test2_unique2<B,D>();
}

template< class DL >
void test2_reverse1()
{
  //typedef typename DL::base_value_type T;
  const unsigned int N = 6;
  {
    DL l;
    test2_prepare1( N, 1, l );
    myassert( l.size() == N );
    
    {
      typename DL::const_iterator itr = l.begin();
      for ( unsigned int i = 0; i != N; ++i, ++itr ) {
        myassert( (*itr)->x == static_cast<int>(i) );
      }
    }
    
    l.reverse();
    {
      typename DL::const_iterator itr = l.begin();
      for ( unsigned int i = 0; i != N; ++i, ++itr ) {
        myassert( (*itr)->x == static_cast<int>(N-1-i) );
      }
    }
    
  }
}

template< class B, class D >
void test2_reverse2()
{
  const unsigned int N = 6;
  {
    DataList<D> d1;
    test2_prepare1( N, 1, d1 );
    myassert( d1.size() == N );
    
    {
      typename DataList<D>::const_iterator itr = d1.begin();
      for ( unsigned int i = 0; i != N; ++i, ++itr ) {
        myassert( (*itr)->x == static_cast<int>(i) );
      }
    }
    
    // Check one can reverse a derived container through its base container
    DataList<B>& b1 = d1;
    b1.reverse();
    {
      typename DataList<B>::const_iterator itr = b1.begin();
      for ( unsigned int i = 0; i != N; ++i, ++itr ) {
        myassert( (*itr)->x == static_cast<int>(N-1-i) );
      }
    }
  }
}

template< class B, class D >
void test2_reverse()
{
  test2_reverse1<DataList<B> >();
  test2_reverse1<DataList<D> >();
  test2_reverse1<ConstDataList<DataList<D> > >();
  test2_reverse2<B, D>();
}

template <class DL, class It>
void test2_iter_swap_memb2 (DL& v, It beg, It end)
{
  int a = v.front()->x;
  int b = v.back()->x;
  myassert (a != b);
  myassert( a == v.front()->x );
  myassert( b == v.back()->x  );
  DL::iter_swap (beg, end);
  myassert (a == v.back()->x);
  myassert (b == v.front()->x);
}
template <class DL>
void test2_iter_swap_memb1()
{
  typedef typename DL::base_value_type T;
  DL v;
  for (int i=0; i<10; ++i )
    v.push_back (new T(i));
  test2_iter_swap_memb2 (v, v.begin(), --v.end());

  DL v2 (v);
  bool caught = false;
  try {
    test2_iter_swap_memb2 (v, v.begin(), --v2.end());
  }
  catch (dl_test_err&) {
    caught = true;
  }
  catch (SG::ExcInsertionInBaseClass&) {
    caught = true;
  }
  myassert (caught);
}
template <class B, class D>
void test2_iter_swap_memb()
{
  test2_iter_swap_memb1<DataList<B> > ();
  test2_iter_swap_memb1<DataList<D> > ();
  test2_iter_swap_memb1<ConstDataList<DataList<D> > > ();

  DataList<D> v;
  for (int i=0; i<10; ++i)
    v.push_back (new D(i));
  DataList<B>& b = v;
  test2_iter_swap_memb2 (v, v.begin(), --v.end() );
  bool caught = false;
  try {
    test2_iter_swap_memb2 (b, b.begin(), --b.end() );
  }
  catch (dl_test_err&) {
    caught = true;
  }
  catch (SG::ExcInsertionInBaseClass&) {
    caught = true;
  }
  myassert (caught);
}


template <class DL, class It>
void test2_iter_swap2 (DL& v, It beg, It end)
{
  int a = v.front()->x;
  int b = v.back()->x;
  myassert (a != b);
  myassert( a == v.front()->x );
  myassert( b == v.back()->x  );
  std::iter_swap (beg, end);
  myassert (a == v.back()->x);
  myassert (b == v.front()->x);
}
template <class DL>
void test2_iter_swap1()
{
  typedef typename DL::base_value_type T;
  DL v;
  for (int i=0; i<10; ++i )
    v.push_back (new T(i));
  test2_iter_swap2 (v, v.begin(), --v.end());
  test2_iter_swap2 (v, v.rbegin(), --v.rend());

  DL v2 (v);
  bool caught = false;
  try {
    test2_iter_swap2 (v, v.begin(), --v2.end());
  }
  catch (dl_test_err&) {
    caught = true;
  }
  catch (SG::ExcInsertionInBaseClass&) {
    caught = true;
  }
  myassert (caught);
  caught = false;
  try {
    test2_iter_swap2 (v, v.rbegin(), --v2.rend());
  }
  catch (dl_test_err&) {
    caught = true;
  }
  catch (SG::ExcInsertionInBaseClass&) {
    caught = true;
  }
  myassert (caught);
}
template <class B, class D>
void test2_iter_swap()
{
  test2_iter_swap1<DataList<B> > ();
  test2_iter_swap1<DataList<D> > ();
  test2_iter_swap1<ConstDataList<DataList<D> > > ();

  DataList<D> v;
  for (int i=0; i<10; ++i)
    v.push_back (new D(i));
  DataList<B>& b = v;
  test2_iter_swap2 (v, v.begin(), --v.end() );
  bool caught = false;
  try {
    test2_iter_swap2 (b, b.begin(), --b.end() );
  }
  catch (dl_test_err&) {
    caught = true;
  }
  catch (SG::ExcInsertionInBaseClass&) {
    caught = true;
  }
  myassert (caught);
  caught = false;
  try {
    test2_iter_swap2 (b, b.rbegin(), --b.rend() );
  }
  catch (dl_test_err&) {
    caught = true;
  }
  catch (SG::ExcInsertionInBaseClass&) {
    caught = true;
  }
  myassert (caught);
}


template <class T>
struct Pred
{
  Pred (int the_v) : v (the_v) {}
  bool operator() (const T* a) const
  { return a->x == v; }
  int v;
};
template <class DL>
void test2_removealg1()
{
  typedef typename DL::base_value_type T;
  const int N=10;

  {
    clear_dtor_log();
    DL v;
    for (int i=0; i<N; i++)
      v.push_back (new T(i));
    typename DL::iterator it = 
      std::remove (v.begin(), v.end(), nth (v, 5));
    myassert (std::distance (v.begin(), it) == N-1);
    int j=0;
    for (int i=0; i<N-1; i++, j++) {
      if (j == 5) ++j;
      myassert (nth(v,i)->x == j);
    }
    myassert (nth (v, N-1) == 0);
    check_dtor_log (5);

    it = std::remove (v.begin(), ::prev(v.end()), nth(v,8));
    myassert (std::distance (v.begin(), it) == N-2);
    j=0;
    for (int i=0; i<N-2; i++, j++) {
      if (j == 5) ++j;
      if (j == 9) ++j;
      myassert (nth(v,i)->x == j);
    }
    myassert (nth(v,N-2) == 0);
    myassert (nth(v,N-1) == 0);
    check_dtor_log (9);
  }

  {
    clear_dtor_log();
    DL v;
    for (int i=0; i<N; i++)
      v.push_back (new T(i));
    typename DL::iterator it = 
      std::remove_if (v.begin(), v.end(), Pred<T>(5));
    myassert (std::distance (v.begin(), it) == N-1);
    int j=0;
    for (int i=0; i<N-1; i++, j++) {
      if (j == 5) ++j;
      myassert (nth(v,i)->x == j);
    }
    myassert (nth(v,N-1) == 0);
    check_dtor_log (5);

    it = std::remove_if (v.begin(), ::prev(v.end()), Pred<T>(9));
    myassert (std::distance (v.begin(), it) == N-2);
    j=0;
    for (int i=0; i<N-2; i++, j++) {
      if (j == 5) ++j;
      if (j == 9) ++j;
      myassert (nth(v,i)->x == j);
    }
    myassert (nth(v,N-2) == 0);
    myassert (nth(v,N-1) == 0);
    check_dtor_log (9);
  }

  {
    clear_dtor_log();
    DL v;
    for (int i=0; i<N; i++)
      v.push_back (new T(i));
    typename DL::reverse_iterator it = 
      std::remove (v.rbegin(), v.rend(), nth(v,5));
    myassert (std::distance (v.rbegin(), it) == N-1);
    int j=0;
    for (int i=1; i<N; i++, j++) {
      if (j == 5) ++j;
      myassert (nth(v,i)->x == j);
    }
    myassert (nth(v,0) == 0);
    check_dtor_log (5);

    it = std::remove (v.rbegin(), ::prev(v.rend()), nth(v,8));
    myassert (std::distance (v.rbegin(), it) == N-2);
    j=0;
    for (int i=2; i<N; i++, j++) {
      if (j == 5) ++j;
      if (j == 8) ++j;
      myassert (nth(v,i)->x == j);
    }
    myassert (nth(v,0) == 0);
    myassert (nth(v,1) == 0);
    check_dtor_log (8);
  }

  {
    clear_dtor_log();
    DL v;
    for (int i=0; i<N; i++)
      v.push_back (new T(i));
    typename DL::reverse_iterator it = 
      std::remove_if (v.rbegin(), v.rend(), Pred<T>(5));
    myassert (std::distance (v.rbegin(), it) == N-1);
    int j=0;
    for (int i=1; i<N; i++, j++) {
      if (j == 5) ++j;
      myassert (nth(v,i)->x == j);
    }
    myassert (nth(v,0) == 0);
    check_dtor_log (5);

    it = std::remove_if (v.rbegin(), ::prev(v.rend()), Pred<T>(8));
    myassert (std::distance (v.rbegin(), it) == N-2);
    j=0;
    for (int i=2; i<N; i++, j++) {
      if (j == 5) ++j;
      if (j == 8) ++j;
      myassert (nth(v,i)->x == j);
    }
    myassert (nth(v,0) == 0);
    myassert (nth(v,1) == 0);
    check_dtor_log (8);
  }

}
template <class B, class D>
void test2_removealg()
{
  test2_removealg1<DataList<B> > ();
  test2_removealg1<DataList<D> > ();

  test2_removealg1<ConstDataList<DataList<D> > > ();
}

template <class T>
struct BPred
{
  bool operator() (const T* a, const T* b) const
  { return a->x == b->x; }
};
template <class DL>
void test2_uniquealg1()
{
  typedef typename DL::base_value_type T;
  {
    DL v (SG::VIEW_ELEMENTS);
    v.push_back (new T(0));
    v.push_back (new T(1));
    v.push_back (v.back());
    v.push_back (new T(2));
    typename DL::iterator it = std::unique (v.begin(), v.end());
    myassert (std::distance (v.begin(), it) == 3);
    for (int i = 0; i < 3; i++)
      myassert (nth(v,i)->x == i);
    myassert (nth(v,3) == 0);
  }

  {
    clear_dtor_log();
    DL v;
    v.push_back (new T(0));
    v.push_back (new T(1));
    v.push_back (new T(1));
    v.push_back (new T(2));
    v.push_back (new T(3));
    v.push_back (new T(3));
    typename DL::iterator it =
      std::unique (v.begin(), v.end(), BPred<T>());
    myassert (std::distance (v.begin(), it) == 4);
    for (int i = 0; i < 4; i++)
      myassert (nth(v,i)->x == i);
    myassert (nth(v,4) == 0);
    myassert (nth(v,5) == 0);
    check_dtor_log (1, 3);
  }

  {
    DL v (SG::VIEW_ELEMENTS);
    v.push_back (new T(0));
    v.push_back (new T(1));
    v.push_back (v.back());
    v.push_back (new T(2));
    typename DL::reverse_iterator it =
      std::unique (v.rbegin(), v.rend());
    myassert (std::distance (v.rbegin(), it) == 3);
    for (int i = 1; i < 3; i++)
      myassert (nth(v,i)->x == i-1);
    myassert (nth(v,0) == 0);
  }

  {
    clear_dtor_log();
    DL v;
    v.push_back (new T(0));
    v.push_back (new T(1));
    v.push_back (new T(1));
    v.push_back (new T(2));
    v.push_back (new T(3));
    v.push_back (new T(3));
    typename DL::reverse_iterator it =
      std::unique (v.rbegin(), v.rend(), BPred<T>());
    myassert (std::distance (v.rbegin(), it) == 4);
    for (int i = 2; i < 6; i++)
      myassert (nth(v,i)->x == i-2);
    myassert (nth(v,0) == 0);
    myassert (nth(v,1) == 0);
    check_dtor_log (1, 3);
  }
}
template <class B, class D>
void test2_uniquealg()
{
  test2_uniquealg1<DataList<B> > ();
  test2_uniquealg1<DataList<D> > ();
  test2_uniquealg1<ConstDataList<DataList<D> > > ();
}

template <class DL>
void test2_reversealg1()
{
  typedef typename DL::base_value_type T;
  int N=5;
  {
    clear_dtor_log();
    DL v;
    for (int i=0; i<N; i++)
      v.push_back (new T(i));
    std::reverse (v.begin(), v.end());
    for (int i=0; i<N; i++)
      myassert (nth(v,i)->x == N-1-i);
    std::reverse (v.rbegin(), v.rend());
    for (int i=0; i<N; i++)
      myassert (nth(v,i)->x == i);
    check_dtor_log ();
  }
}
template <class B, class D>
void test2_reversealg()
{
  test2_reversealg1<DataList<B> > ();
  test2_reversealg1<DataList<D> > ();
  test2_reversealg1<ConstDataList<DataList<D> > > ();
}

template <class DL>
void test2_rotatealg1()
{
  typedef typename DL::base_value_type T;
  const int N=5;
  {
    clear_dtor_log();
    DL v;
    for (int i=0; i<N; i++)
      v.push_back (new T(i));
    std::rotate (v.begin(), ::next(v.begin(),2), v.end());
    for (int i=0; i<N; i++)
      myassert (nth(v,i)->x == (i+2)%5);
    std::rotate (v.rbegin(), ::next(v.rbegin(),2), v.rend());
    for (int i=0; i<N; i++)
      myassert (nth(v,i)->x == i);
    check_dtor_log();
  }
}
template <class B, class D>
void test2_rotatealg()
{
  test2_rotatealg1<DataList<B> > ();
  test2_rotatealg1<DataList<D> > ();
  test2_rotatealg1<ConstDataList<DataList<D> > > ();
}

template <class T>
struct PPred
{
  PPred (int the_v) : v (the_v) {}
  bool operator() (const T* a) const
  { return a->x < v; }
  int v;
};
template <class DL>
void random_list (DL& l, int n)
{
  typedef typename DL::base_value_type T;
  std::vector<T*> v;
  for (int i=0; i < n; i++)
    v.push_back (new T(i));
  std::random_shuffle (v.begin(), v.end(), stlrand);
  l.assign (v.begin(), v.end());
  clear_dtor_log();
}
template <class DL>
void test2_partitionalg1()
{
  typedef typename DL::base_value_type T;
  const int N=10;
  {
    stlrand.seed = 2;
    clear_dtor_log();
    DL v;
    random_list (v, N);
    typename DL::iterator it =
      std::partition (v.begin(), v.end(), PPred<T>(5));
    typename DL::iterator ii = v.begin();
    while (ii != it)
      myassert ((*ii++)->x < 5);
    while (ii != v.end())
      myassert ((*ii++)->x >= 5);
    check_dtor_log();

    random_list (v, N);
    typename DL::reverse_iterator rit =
      std::partition (v.rbegin(), v.rend(), PPred<T>(5));
    typename DL::reverse_iterator jj = v.rbegin();
    while (jj != rit)
      myassert ((*jj++)->x < 5);
    while (jj != v.rend())
      myassert ((*jj++)->x >= 5);
    check_dtor_log();
  }

  {
    stlrand.seed = 3;
    clear_dtor_log();
    DL v;
    random_list (v, N);
    typename DL::iterator it =
      std::stable_partition (v.begin(), v.end(), PPred<T>(5));
    typename DL::iterator ii = v.begin();
    while (ii != it)
      myassert ((*ii++)->x < 5);
    while (ii != v.end())
      myassert ((*ii++)->x >= 5);
    check_dtor_log();

    random_list (v, N);
    typename DL::reverse_iterator rit =
      std::stable_partition (v.rbegin(), v.rend(), PPred<T>(5));
    typename DL::reverse_iterator jj = v.rbegin();
    while (jj != rit)
      myassert ((*jj++)->x < 5);
    while (jj != v.rend())
      myassert ((*jj++)->x >= 5);
    check_dtor_log();
  }
}
template <class B, class D>
void test2_partitionalg()
{
  test2_partitionalg1<DataList<B> > ();
  test2_partitionalg1<DataList<D> > ();
  test2_partitionalg1<ConstDataList<DataList<D> > > ();
}

template <class D>
void test2_asdatalist()
{
  ConstDataList<DataList<D> > cv;
  cv.push_back (new D(1));
  const DataList<D>* dv = cv.asDataList();
  myassert (dv->size() == 1);
}

template <class DL>
void test2_assignelement1()
{
  typedef typename DL::base_value_type T;
  clear_dtor_log();
  DL v;
  v.push_back (new T(1));
  v.push_back (new T(2));
  v.assignElement ((v.begin()).base(), new T(3));
  check_dtor_log (1);
  v.assignElement ((v.begin()).base(), new T(4));
  check_dtor_log (3);
}
template <class B, class D>
void test2_assignelement()
{
  test2_assignelement1<DataList<B> > ();
  test2_assignelement1<DataList<D> > ();
  test2_assignelement1<ConstDataList<DataList<D> > > ();
}


template <class DL>
void test2_swapranges()
{
  clear_dtor_log();

  typedef typename DL::base_value_type T;
  DL l1;
  DL l2;
  for (int i = 0; i < 10; i++) {
    l1.push_back (new T(i));
    l2.push_back (new T(i+10));
  }
  std::swap_ranges (::next(l1.begin(),3), ::next(l1.begin(),6),
                    ::next(l2.begin(),4));
  std::swap_ranges (::next(l1.rbegin(),3), ::next(l1.rbegin(),6),
                    ::next(l2.rbegin(),4));

  check_dtor_log();
}
template <class B, class D>
void test2_swapranges()
{
  test2_swapranges<DataList<B> > ();
  test2_swapranges<DataList<D> > ();
  test2_swapranges<ConstDataList<DataList<D> > > ();
}

template <class B, class D>
void do_test2()
{
  boost::function_requires<boost::ForwardContainerConcept<DataList<B> > >();
  boost::function_requires<boost::ForwardContainerConcept<DataList<D> > >();

  test2_initial<B,D> ();
  test2_default_ctor<B,D> ();
  test2_sized_ctor<B,D> ();
  test2_insertion_ctor<B,D> ();
  test2_copy_ctor<B,D> ();
  test2_dtor<B,D> ();
  test2_push_front<B,D> ();
  test2_push_back<B,D> ();
  test2_size<B,D> ();
  test2_stdcont<B,D> ();
  test2_empty<B,D> ();
  test2_max_size<B,D> ();
  test2_front_back_const<B,D> ();
  test2_front_back<B,D> ();
  test2_begin_end_const<B,D> ();
  test2_rbegin_rend_const<B,D> ();
  test2_elementproxy<B,D> ();
  test2_swapelement_iter<B,D> ();
  test2_resize<B,D> ();
  test2_pop_front<B,D> ();
  test2_pop_back<B,D> ();
  test2_begin_end<B,D> ();
  test2_rbegin_rend<B,D> ();
  test2_erase_range<B,D> ();
  test2_erase_single<B,D> ();
  test2_operator_assign<B,D> ();
  test2_insert_value<B,D> ();
  test2_insert_range<B,D> ();
  test2_clear<B,D> ();
  test2_swap<B,D> ();
  test2_assign<B,D> ();
  test2_relops<B,D> ();
  test2_sort<B,D> ();
  test2_splice<B,D>();
  test2_merge<B,D>();
  test2_remove<B,D>();
  test2_unique<B,D>();
  test2_reverse<B,D>();
  test2_iter_swap_memb<B,D> ();
  test2_iter_swap<B,D> ();
  test2_removealg<B,D> ();
  test2_uniquealg<B,D> ();
  test2_reversealg<B,D> ();
  test2_rotatealg<B,D> ();
  test2_partitionalg<B,D> ();
  test2_asdatalist<D> ();
  test2_assignelement<B,D> ();
  test2_swapranges<B,D> ();
}


template <class T>
void dump_classid()
{
  typedef ClassID_traits<DataList<T> > traits;
  std::cout << "name: " << traits::typeName() << "\n";
  std::cout << " clid, vers, is_do: "
            << std::hex << traits::ID() << std::dec << " "
            << traits::s_version << " "
            << traits::s_isDataObject << "\n";
}


void test2()
{
  std::cout << "*** DataList_test [test2] BEGIN ***" << std::endl;
  test2_myassert();
  do_test2<AA, BB> ();
  do_test2<BB, CC> ();
  do_test2<M, P> ();
  do_test2<M, R> ();
  myassert (ClassName<DataList<AA> >::name() == "DataList<AA>");
  myassert (ClassName<DataList<BB> >::name() == "DataList<BB>");
  myassert (ClassName<DataList<CC> >::name() == "DataList<CC>");
  myassert (ClassName<DataList<M> >::name() == "DataList<M>");
  myassert (ClassName<DataList<P> >::name() == "DataList<P>");
  myassert (ClassName<DataList<R> >::name() == "DataList<R>");
  dump_classid<AA> ();
  dump_classid<BB> ();
  dump_classid<CC> ();
#if 0
  std::ostringstream os;
  os << ClassID_traits<DataList<AA> >::packageInfo();
  myassert ( os.str().substr(0,7) == "$Name: " ||
             os.str() == "Package-00-00-00" );
  os.str ("");
  os << ClassID_traits<DataList<CC> >::packageInfo();
  myassert (os.str() == "Package-00-00-00");
#endif
  
  std::cout << "*** DataList_test [test2] OK ***" << std::endl;
}


void test_baseinfo()
{
  std::vector<CLID> clids;
  clids = SG::BaseInfo<DataList<CC> >::get_bases();
  std::vector<CLID> exp1 = list_of
    (ClassID_traits<DataList<AA> >::ID())
    (ClassID_traits<DataList<BB> >::ID())
    (ClassID_traits<DataList<CC> >::ID());
  std::sort (clids.begin(), clids.end());
  std::sort (exp1.begin(), exp1.end());
  myassert (clids == exp1);

  clids = SG::BaseInfo<DataList<R> >::get_bases();
  std::vector<CLID> exp2 = list_of
    (ClassID_traits<DataList<R> >::ID())
    (ClassID_traits<DataList<N> >::ID())
    (ClassID_traits<DataList<O> >::ID())
    (ClassID_traits<DataList<Q> >::ID())
    (ClassID_traits<DataList<M> >::ID());
  std::sort (clids.begin(), clids.end());
  std::sort (exp2.begin(), exp2.end());
  myassert (clids == exp2);

  myassert (SG::BaseInfo<DataList<CC> >::is_base
	    (ClassID_traits<DataList<BB> >::ID()));
  myassert (! SG::BaseInfo<DataList<CC> >::is_virtual
	    (ClassID_traits<DataList<BB> >::ID()));
  myassert (SG::BaseInfo<DataList<R> >::is_base
	    (ClassID_traits<DataList<M> >::ID()));
  myassert (SG::BaseInfo<DataList<R> >::is_virtual
	    (ClassID_traits<DataList<M> >::ID()));
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
  DataList<P> lst;
  for (int i=0; i<10; i++)
    lst.push_back (new P(i));

  assert (SG::BaseInfo<DataList<P> >::is_base (typeid (DataList<M>)));
  assert (! SG::BaseInfo<DataList<P> >::is_base (typeid (DataList<MM>)));

  DataModel_detail::DVLInfoBase* info;
  void* contp = DataModel_detail::dvl_convert (lst,
                                               typeid (DataList<AA>), info);
  assert (contp == 0);
  DataList<MM>::dvlinfo();
  contp = DataModel_detail::dvl_convert (lst, typeid (DataList<MM>), info);
  DataList<MM>& mmlst = *(DataList<MM>*)contp;
  assert (mmlst.ownPolicy() == SG::VIEW_ELEMENTS);
  assert (mmlst.size() == 10);
  DataList<MM>::iterator it = mmlst.begin();
  for (int i=0; i<10; i++) {
    MM* mm = *it;
    assert (mm->mm == i + 100);
    P* pp = dynamic_cast<P*> (mm);
    if (!pp) std::abort();
    assert (pp->x == i);
    ++it;
  }

  for (int i=10; i<20; i++)
    lst.push_back (new P(i));
  DataModel_detail::dvl_update (lst, contp, info);
  assert (mmlst.ownPolicy() == SG::VIEW_ELEMENTS);
  assert (mmlst.size() == 20);
  it = mmlst.begin();
  for (int i=0; i<20; i++) {
    MM* mm = *it;
    assert (mm->mm == i + 100);
    P* pp = dynamic_cast<P*> (mm);
    if (!pp) std::abort();
    assert (pp->x == i);
    ++it;
  }
}


void test_iterate()
{
  DataList<P> vp;
  for (int i=0; i < 10; i++) {
    vp.push_back (new P(i));
    vp.push_back (0);
  }

  const DataModel_detail::DVLInfoBase& info1 = DataList<P>::dvlinfo();
  DataModel_detail::DVLIteratorBase* iterator = info1.iterator (&vp);
  int ii = 0;
  while (const void* p = iterator->next()) {
    const P* pp = reinterpret_cast<const P*> (p);
    assert (pp->x == ii);
    ++ii;
  }
  delete iterator;

  DataList<M>& vm = vp;
  const DataModel_detail::DVLInfoBase& info2 = DataList<M>::dvlinfo();
  iterator = info2.iterator (&vm);
  ii = 0;
  while (const void* p = iterator->next()) {
    const M* pp = reinterpret_cast<const M*> (p);
    assert (pp->x == ii++);
  }
  delete iterator;
}


int main()
{
  test1();
  test2();
  test_baseinfo();
  test_eltbaseinfo();
  test_copyconvert();
  test_iterate();
  return 0;
}

#else

int main() {
   return 0;
}

#endif // not XAOD_STANDALONE

// asDataList
