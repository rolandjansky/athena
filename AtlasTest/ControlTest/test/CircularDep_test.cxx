/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include "TestTools/SGassert.h"
#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLink.h"
#include "SGTools/BaseInfo.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "TestTools/initGaudi.h"
#include "SGTools/CLASS_DEF.h"

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;



struct Circular {
  typedef DataVector<Circular> CircularVector;
  ElementLink<CircularVector> m_link;
};

typedef DataVector<Circular> CircularVector;

class DerivedCircularVector : public DataVector<Circular> {};

CLASS_DEF( CircularVector , 218202856 , 1 )
CLASS_DEF( DerivedCircularVector , 254054940 , 1 )
SG_BASE (DerivedCircularVector, CircularVector);

using namespace Athena_test;

int main() {

  cout << "CircularDep_test - testing Circular Deps in ElementLink" << endl;
    ISvcLocator* pSvcLoc;
  if (!initGaudi("DataLink_test.txt", pSvcLoc)) {
    cerr << "This test can not be run" << endl;
    return 0;
  }  

  assert(pSvcLoc);
  StoreGateSvc* pSG;
  
  const bool CREATEIF(true);
  assert((pSvcLoc->service("StoreGateSvc", pSG, CREATEIF)).isSuccess());
  assert(pSG);

  Circular *circ0(new Circular), *circ1(new Circular);

  CircularVector* circVector = new CircularVector;
  assert (pSG->record (circVector, "circVector").isSuccess());
  circVector->push_back(circ0);
  circ1->m_link.toIndexedElement(*circVector, 0); //circ1 link->circ0

  assert(*(circ1->m_link) == circ0);

  Circular *circ2(new Circular), *circ3(new Circular);
  DerivedCircularVector* derCircVector = new DerivedCircularVector;
  assert (pSG->record (derCircVector, "derCircVector").isSuccess());
  derCircVector->push_back(circ2);
  circ3->m_link.toIndexedElement(*derCircVector, 0); //circ3 link->circ2

  assert(*(circ3->m_link) == circ2);

  cout << "*** CircularDep_test OK ***" <<endl;
  return 0;
}
