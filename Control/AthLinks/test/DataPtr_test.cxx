/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include "AthLinks/DataPtr.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>


using namespace std;

struct Fluff {
  virtual ~Fluff() {} 
  int m_int;
  float m_float;
  string m_string;
  Fluff() : m_int(1), m_float(-379.456f), 
            m_string("this is the Fluff struct") {}
};

struct DerivedFluff : public Fluff {
  virtual ~DerivedFluff() {} 
  string m_anotherString;
  DerivedFluff() : 
    Fluff(), m_anotherString("this is the DerivedFluff struct") {}
};

int main () {
  typedef vector< DataPtr<int> > IntVector;
  IntVector intV;
  intV.reserve(10);

  int i;
  for (i=0; i<3; ++i)  intV.push_back(new int(i));


  DataPtr<int> vp3(intV[2]);

  assert (2 == vp3.use_count());
  int* pvp3 = vp3;
  const int* cpvp3 = vp3.ptr();
  assert(*cpvp3==*pvp3);
  DataPtr<int> vpvp3(vp3);
  //FIXME  DataPtr<int> vpvp3(pvp3);
  // the above causes the cleanup at the end of the job to dump core
  // what happens (I think) is that vpvp3 is not aware that it is sharing
  // the c++ pointer with vp3, so the ref count is screwed and intV[2] and
  // intV[3] end up trying to delete the same c++ pointer
  assert (3 == vpvp3.use_count());
  DataPtr<int> vpvpvp3 = vpvp3;
  assert(*vpvp3==*intV[2]);
  assert (4 == vp3.use_count());
  assert (4 == vpvp3.use_count());
  assert (4 == vpvpvp3.use_count());
  assert(vpvpvp3==vp3);
  assert(intV[0]!=vp3);
  assert(intV[1]==intV[1]);
  assert(!(intV[2]!=intV[2]));

  intV.push_back(vpvp3);
  assert (5 == vpvp3.use_count());

  IntVector::const_iterator it(intV.begin());
  IntVector::const_iterator iEnd(intV.end());
  while (it!=iEnd) {
    cout << hex << &*it << "->" << it->ptr() << dec 
	 << " use count " << it->use_count()
	 << " content " << **it << endl;
    ++it;
  }

  vector< DataPtr<Fluff> > fluffV;
  fluffV.push_back(new Fluff());
  Fluff* pf = fluffV[0];
  assert(pf->m_string==Fluff().m_string);


  DerivedFluff* pdf(new DerivedFluff());
  DataPtr<Fluff> pDerFluff(pdf);
  DerivedFluff* extracted = dynamic_cast<DerivedFluff*>((Fluff*)pDerFluff);
  assert (0 != extracted);
  assert (extracted->m_anotherString == pdf->m_anotherString);

  
  //FIXME here is an error that gets undetected (except when using valgrind)
  //we fill a DataPtr with a plain ptr extracted from another DataPtr
  //this creates two DataPtrs both with refcnt = 1 pointing to the same obj!
  //DataPtr<Fluff> pCopyDerFluff(extracted);
  //DerivedFluff* reextracted = dynamic_cast<DerivedFluff*>((Fluff*)pCopyDerFluff);
  //assert (0 != reextracted);
  //assert (reextracted->m_anotherString == pdf->m_anotherString);

  DataPtr<Fluff> pBadCopyFluff(pDerFluff);
  DerivedFluff* badextracted = dynamic_cast<DerivedFluff*>((Fluff*)pBadCopyFluff);
  assert (0 != badextracted);
  assert (badextracted->m_anotherString == pdf->m_anotherString);


  cerr << "*** DataPtr_test OK ***" <<endl;
  return 0;

}


