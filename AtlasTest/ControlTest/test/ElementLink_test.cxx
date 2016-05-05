/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TestTools/FLOATassert.h"
#include "TestTools/SGassert.h"
#include "AthContainers/DataVector.h"
#include "AthLinks/tools/MapIndexingPolicy.h"
#include "AthLinks/tools/SetIndexingPolicy.h"
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"
#include "AthLinks/DeclareIndexingPolicy.h"
#include "SGTools/CLASS_DEF.h"
#include "SGTools/StlVectorClids.h"
#include "StoreGate/StoreGateSvc.h"
#include "TestTools/initGaudi.h"

#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <deque>
#include <string>
#include <sys/resource.h>
#include <sys/time.h>

using std::cout;
using std::endl;
using std::deque;
using std::map;
using std::set;
using std::string;
using std::vector;

class Bar {};

class Foo {
public:
  Foo(double d=0.0) : m_d(d) {};
  double d() const { return m_d; }
private:
  double m_d;
};
bool 
operator==(const Foo& lhs, const Foo& rhs) {
  return Athena_test::floatEQ(lhs.d(), rhs.d());
}
bool 
operator!=(const Foo& lhs, const Foo& rhs) {
  return Athena_test::floatNEQ(lhs.d(), rhs.d());
}


class Hell {
public:
  Hell() { };
  ElementLink<DataVector<Foo> > test(DataVector<Foo>& cont, ElementLink<DataVector<Foo> >& link) {
    const Foo* f = *link;
    ElementLink<DataVector<Foo> > ftest;
    ftest.toContainedElement(cont, f);
    return ftest;
  }

  double get( const Foo& f) {return f.d();}
};

CLASS_DEF(DataVector<Foo>, 1234, 1)
CLASS_DEF(std::vector<Foo>, 2233, 1)


float tv_diff (const timeval& tv1, const timeval& tv2)
{
  return static_cast<float>(tv2.tv_sec - tv1.tv_sec + (tv2.tv_usec - tv1.tv_usec) * 1e-6);
}


// Time EL construction and string key initialization.
void timetest1 (const char* arg)
{
  int n = 10000000;
  const char* p = strchr (arg, ':');
  if (p)
    n = atoi (p+1);

  ElementLink<DataVector<Foo> > el;

  rusage ru0, ru1;
  getrusage (RUSAGE_SELF, &ru0);
  for (int i=0; i < n; i++) {
    // Default-construct, then use resetWithKeyAndIndex.
    new (&el) ElementLink<DataVector<Foo> >;
    el.resetWithKeyAndIndex ("fooVec", 0);
  }
  getrusage (RUSAGE_SELF, &ru1);

  std::cout << "t1 " << n << ": " << tv_diff (ru0.ru_utime, ru1.ru_utime)
            << "\n";
}


// Time EL construction and sgkey_t initialization.
void timetest2 (const char* arg)
{
  int n = 100000000;
  const char* p = strchr (arg, ':');
  if (p)
    n = atoi (p+1);

  ElementLink<DataVector<Foo> > el;
  IStringPool::sgkey_t key = 12345;

  rusage ru0, ru1;
  getrusage (RUSAGE_SELF, &ru0);
  for (int i=0; i < n; i++) {
    // Default-construct, then use resetWithKeyAndIndex.
    new (&el) ElementLink<DataVector<Foo> >;
    el.resetWithKeyAndIndex (key, 0);
  }
  getrusage (RUSAGE_SELF, &ru1);

  std::cout << "t2 " << n << ": " << tv_diff (ru0.ru_utime, ru1.ru_utime)
            << "\n";
}


int main(int argc, char** argv) {

  // init:

  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("ElementLink_test.txt", pSvcLoc)) {
    std::cerr << " This test cannot be run without init Gaudi" << endl;
  }
  assert(pSvcLoc);
  StoreGateSvc* pSG;
  const bool CREATEIF(true);
  assert((pSvcLoc->service("StoreGateSvc", pSG, CREATEIF)).isSuccess());
  assert(pSG);

  StatusCode sc;

  std::cout << " ************* Starting Test 1 " << std::endl;  // test 1:
  DataVector<Foo>* fooVec = new DataVector<Foo>();
  Foo* f1 = new Foo(1.0);
  Foo* f2 = new Foo(2.0);
  Foo* f3 = new Foo(3.0);

  fooVec->push_back(f1);
  fooVec->push_back(f2);
  fooVec->push_back(f3);
  sc = pSG->record(fooVec, "fooVec");

  ElementLink< DataVector<Foo> > FooVecLink1;
  FooVecLink1.toContainedElement(*fooVec, f1);

  std::cout << " Test1: test of contained Element " << std::endl;  
  // move to persistent state and check index 
  FooVecLink1.toPersistent();
  assert(FooVecLink1.index() == 0);
  // get the pointer from ElementLink and check
  FLOAT_EQassert ((**FooVecLink1).d(), 1.);

  std::cout << " Test1: test of indexed Element " << std::endl;
  ElementLink< DataVector<Foo> > FooVecLink2(*fooVec, 1);
  // Dereference to make sure we get Foo2 back:
  FLOAT_EQassert((**FooVecLink2).d(), 2.0);
  // move to persistent state and check index
  FooVecLink2.toPersistent();
  assert(FooVecLink2.index() == 1);

  ElementLink< DataVector<Foo> > FooVecLink21;
  assert( FooVecLink21.toIndexedElement(*fooVec, 1) );
  // Dereference to make sure we get Foo2 back:
  FLOAT_EQassert((*FooVecLink21)->d(), 2.0);
  // move to persistent state and check index
  FooVecLink21.toPersistent();
  assert(FooVecLink21.index() == 1);

  std::cout << " Test1: test of Element Link with key " << std::endl;
  ElementLink< DataVector<Foo> > FooVecLink3("fooVec", 2);
  assert (FooVecLink3.index() == 2);
  const Foo* f = *FooVecLink3;  
  FLOAT_EQassert (f->d(), 3.);

  // check resetWithKeyAndIndex
  std::cout << " Test1: test of Element Link resetWithKeyAndIndex " << std::endl;
  // assign el1 to el4
  ElementLink< DataVector<Foo> > FooVecLink4 = FooVecLink1;
  // check index 
  assert(FooVecLink4.index() == 0);
  // get the pointer from ElementLink and check
  FLOAT_EQassert ((**FooVecLink4).d(), 1.);
  // reset EL
  FooVecLink4.resetWithKeyAndIndex("fooVec", 2);
  // Should be in valid state
  assert (FooVecLink4.isValid());
  // Should be in identified state
  //assert (IdentifiedState::valid(&FooVecLink4));
  // move to transient and check that it has the right values
  FooVecLink4.toTransient();
  f = *FooVecLink4;  
  FLOAT_EQassert (f->d(), 3.);


  // create a vector of element links and iterate over it:
  std::cout << " Test1: test of ElementLink in/out of scope" << std::endl;
  ElementLink< DataVector<Foo> > linkTest;
  linkTest.toContainedElement(*fooVec, f);
  Hell h;
  ElementLink<DataVector<Foo> > ftest = h.test(*fooVec, linkTest);
  // make sure that the two pointers are the same.
  assert (f == *ftest);

  std::cout << " Test: test of ElementLink setElement, setStorable" << std::endl;
  ElementLink< DataVector<Foo> > linkset;
  assert( linkset.toPersistent() );

  const Foo* pCF(f);
  linkset.setElement(pCF);  // set only element
  SGASSERTERROR( linkset.toPersistent() );

  linkset.setStorableObject(*fooVec);  // set Collection
  assert (linkset.index() == 2);
  FLOAT_EQassert ((**linkset).d(), 3.);
  assert( linkset.toPersistent() );

  std::cout << " >>>>>>>>> Passed Test 1 <<<<<<<<<<< " << std::endl;
 
  std::cout << " ************* Starting Test 2 " << std::endl;
  // test2:
  std::vector<std::string> strVec;
  sc = pSG->record(&strVec, " key");
  strVec.push_back("foo");
  strVec.push_back("bar");

  std::cout << " Test2: test of contained Element " << std::endl;
  ElementLink< std::vector<std::string> > strLink1;
  std::string x = "foo"; // note that you cannot pass foo directly.
  strLink1.toContainedElement(strVec, x);
  // note that the call to .index() itself calls toPersistent()
  assert (strLink1.index() == 0);
  assert (*strLink1=="foo");

  std::cout << " Test2: test of indexed Element " << std::endl;
  ElementLink< std::vector<std::string> > strLink2(strVec, 1);
  assert(*strLink2 == "bar");
  std::cout << " >>>>>>>>> Passed Test 2 <<<<<<<<<<< " << std::endl;

  std::cout << " Starting Test 3 " << std::endl;
  // test 3: use simple vector<objects> for test.

  std::vector<Foo>* fooVec2 = new std::vector<Foo>();
  Foo foo1(1.0);
  Foo foo2(2.0);
  fooVec2->push_back(foo1);
  fooVec2->push_back(foo2);
  sc = pSG->record(fooVec2, "fooVec2");

  ElementLink< std::vector<Foo> > FooVec2Link1;
  FooVec2Link1.toContainedElement(*fooVec2, foo1);
  assert (foo1 == *FooVec2Link1.cptr());

  std::cout << " Test3: test of contained Element " << std::endl;  
  // move to persistent state
  FooVec2Link1.toPersistent();
  assert(FooVec2Link1.index() == 0);

  std::cout << " Test3: test of indexed Element " << std::endl;
  ElementLink< std::vector<Foo> > FooVec2Link2(*fooVec2, 1);
  // Dereference to make sure we get Foo2 back:
  //  assert(FooVecLink2.d() == 2.0);
  // move to persistent state
  //  assert(FooVec2Link2.index() == 1);
  FooVec2Link2.toPersistent();
  // test of ElementLinkVector:
  std::cout << " Starting Test 4: Testing ElementLinkVector " << std::endl;
  typedef ElementLinkVector<DataVector<Foo> > ELVec;
  ELVec linkVector;
  linkVector.push_back(FooVecLink1);
  linkVector.push_back(FooVecLink2);
  linkVector.push_back(FooVecLink3);
  const Foo* fv = *linkVector.elementCPtr(0);  //!!FIXME
  FLOAT_EQassert (fv->d(), 1.0);

  FLOAT_EQassert ((**linkVector.elementCPtr(1)).d(), 2.0);

  FLOAT_EQassert ((**linkVector.elementCPtr(2)).d(), 3.0);

  // let us check the begin() end() of ElementLinkVector:
  double result(0.00);
  ELVec::iterator iter(linkVector.begin()), iend(linkVector.end());
  for (; iter!=iend; ++iter) {
    result += 1.0;
    FLOAT_EQassert((**iter)->d(), result);
  }

  for (int i=1; i < argc; i++) {
    if (strncmp (argv[i], "-t1", 3) == 0)
      timetest1 (argv[i]);
    else if (strncmp (argv[i], "-t2", 3) == 0)
      timetest2 (argv[i]);
  }

  std::cout << "ElementLink test ok" << std::endl;
  return 0;
}
