/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief test the TimedHitPtrCollection
 * @author ATLAS Collaboration
 */
// $Id $

/* #define THDEBUG 1 */
#undef NDEBUG
#include <cassert>
#include <cmath>
#include <iostream>
#include <atomic>
#include "HitManagement/TimedHitPtrCollection.h"
#include "HitManagement/AthenaHitsVector.h"

struct TestHit {
  TestHit() : t(0.0), id(0) {++HOWMANY;}
  TestHit(float T, int I) : t(T), id(I) {++HOWMANY;}
  TestHit(const TestHit& rhs) : t(rhs.t), id(rhs.id) {++HOWMANY;}
  ~TestHit() {--HOWMANY;}
  float t;
  int id;
  static std::atomic<unsigned int> HOWMANY;
};
float hitTime(const TestHit& h) { return h.t; }
bool operator < (const TestHit& lhs, const TestHit& rhs) {
  return (lhs.id < rhs.id);
}

std::atomic<unsigned int> TestHit::HOWMANY=0;


#include "TestTools/initGaudi.h"
using namespace Athena_test;

using namespace std;

int main() {
  cout << "*** TimedHitPtrCollection_test starts ***" <<endl;
  ISvcLocator* pSvcLoc;
  if (!initGaudi(pSvcLoc)) {
    cerr << "This test can not be run" << endl;
    return 0;
  }  
  assert(pSvcLoc);
  { // define a local scope so that destructors are called when we leave it
    AthenaHitsVector<TestHit> testVector;
    testVector.Insert(new TestHit(23.0, 1));
    testVector.Insert(new TestHit(-23.0, 31));
    testVector.Insert(new TestHit(-23.0, 41));
    testVector.Insert(new TestHit(-23.0, 21));
    testVector.Insert(new TestHit(23.0, 31));
    testVector.Insert(new TestHit(23.0, 41));
    
    AthenaHitsVector<TestHit> testVector2;
    testVector2.Insert(new TestHit(32.0, 3));
    testVector2.Insert(new TestHit(-32.0, 31));
    testVector2.Insert(new TestHit(-32.0, 43));
    testVector2.Insert(new TestHit(-32.0, 21));
    testVector2.Insert(new TestHit(32.0, 31));
    testVector2.Insert(new TestHit(32.0, 41));
    
    TimedHitPtrCollection<TestHit> thpc;
    thpc.insert(200.0, &testVector);
    thpc.insert(-2000.0, &testVector2);
    TimedHitPtrCollection<TestHit>::const_iterator i, e;
    while( thpc.nextDetectorElement(i, e) ) {
#ifdef THDEBUG
      cout << " next element " << endl;
#endif
      if ((*i)->id == 1) assert(e-i == 1); 
      if ((*i)->id == 31) assert(e-i == 4); 
      
      while (i != e) {
#ifdef THDEBUG
	cout << "id " << (*i)->id
	     << " raw time " << (*i)->t
	     << " corr time " << hitTime(*i)
	     << " event time " << i->eventTime() << endl;
#endif
	float evtTime(i->eventTime());
	assert( evtTime == 200.0 || evtTime == -2000.0 );
	++i;
      }
    }
    //    cout << TestHit::HOWMANY << endl;
    testVector2.resize(1);
    //    cout << TestHit::HOWMANY << endl;
    AthenaHitsVector<TestHit> testVector3(testVector);
    //    cout << TestHit::HOWMANY << endl;
    assert(testVector3.size() == testVector.size());
    assert(hitTime(*testVector3[0]) == hitTime(*testVector[0]));
    //    cout << TestHit::HOWMANY << endl;
    AthenaHitsVector<TestHit> testVector4;
    //    cout << TestHit::HOWMANY << endl;
    testVector4 = testVector;
    //    cout << TestHit::HOWMANY << endl;
    assert(testVector4.size() == testVector.size());
    assert(hitTime(*testVector4[1]) == hitTime(*testVector[1]));
  }  // now the TestHits should all be gone
  //  cout << TestHit::HOWMANY << endl;
  assert (0 == TestHit::HOWMANY);

  cout << "*** TimedHitPtrCollection_test OK ***" <<endl;
  return 0;
}

