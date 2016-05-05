/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TestTools/SGassert.h"
#include "AthLinks/ElementLinkVector.h"
#include "AthContainers/DataVector.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/CLASS_DEF.h"

#include "GaudiKernel/ISvcLocator.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <deque>
#include <string>
#include <stdexcept>

#include "TestTools/initGaudi.h"
using namespace Athena_test;
using namespace std;

typedef DataVector<int> IntVector;
CLASS_DEF( IntVector , 7368883 , 1 )

namespace Athena_test 
{
  void linkVector(StoreGateSvc& rSG)
  {
    cout << "*** ElementLinkVector_test vector starts ***\n\n" <<endl;
    //a link to an element
    IntVector* pvint(new IntVector);
    pvint->push_back(new int(10));
    pvint->push_back(new int(20));
    assert((rSG.record(pvint, "a vector")).isSuccess());
    pvint = new IntVector;
    pvint->push_back(new int(-10));
    pvint->push_back(new int(-20));
    pvint->push_back(new int(-30));
    assert((rSG.record(pvint, "another vector")).isSuccess());

    ElementLinkVector<IntVector> linkVector;
    assert(linkVector.empty());
    linkVector.reserve(100);
    assert(linkVector.capacity() == 100);

    linkVector.push_back(ElementLink<IntVector>("a vector", 1));
    linkVector.push_back(ElementLink<IntVector>("a vector", 0));
    linkVector.push_back(ElementLink<IntVector>("another vector", 2));
    assert(distance(linkVector.begin(), linkVector.end()) == 3);
    assert(linkVector.size() == 3);
    
    assert(**linkVector.elementCPtr(0) == 20);
    assert(linkVector.elementIndex(0) == 1);
    assert(linkVector.elementDataID(0) == "a vector");
    assert(**linkVector.elementCPtr(1) == 10);
    assert(linkVector.elementIndex(1) == 0);
    assert(linkVector.elementDataID(1) == "a vector");
    assert(**linkVector.elementCPtr(2) == -30);
    assert(linkVector.elementIndex(2) == 2);
    assert(linkVector.elementDataID(2) == "another vector");

    assert(*linkVector.front().cptr() == *linkVector.elementCPtr(0));
    assert(linkVector.front().index() == linkVector.elementIndex(0));
    assert(linkVector.front().dataID() == linkVector.elementDataID(0));

    const ElementLink<IntVector>& rLink(linkVector[linkVector.size()-1]);
    assert(*linkVector.back() == *rLink);
    assert(linkVector.back().index() == rLink.index());
    assert(linkVector.back().dataID() == rLink.dataID());

    ElementLinkVector<IntVector>::iterator iLV(linkVector.begin());
    assert(iLV != linkVector.end());
    assert(*iLV->cptr() == *linkVector.elementCPtr(0));
    assert(iLV->dataID() == "a vector");
    assert(iLV->index() == 1);
    iLV++;
    assert(***iLV == 10);
    assert(iLV->dataID() == "a vector");
    assert(iLV->index() == 0);

    ElementLinkVector<IntVector>::iterator nLV =
      linkVector.insert(iLV, ElementLink<IntVector>("another vector", 0));
    assert(distance(linkVector.begin(), linkVector.end()) == 4);
    assert(**nLV->cptr() == -10);
    assert(nLV->dataID() == "another vector");
    assert(nLV->index() == 0);
    
    linkVector.erase(iLV);
    assert(distance(linkVector.begin(), linkVector.end()) == 3);

    int count(0);
    ElementLinkVector<IntVector>::const_iterator ciLV(linkVector.begin());
    for (ciLV=linkVector.begin(); ciLV!=linkVector.end(); ++ciLV) ++count;
    assert(count == 3);
    
    linkVector.pop_back();
    assert(linkVector.size() == 2);

    ElementLinkVector<IntVector> copy(linkVector);
    assert(linkVector == copy);

    ElementLinkVector<IntVector> empty;
    assert(empty != copy);

    std::swap(empty, linkVector);
    assert(linkVector.empty());

    assert (linkVector.toPersistent());

    linkVector.push_back (ElementLink<IntVector>());
    assert (linkVector.toPersistent());

    cout << "*** ElementLinkVector_test vector OK ***\n\n" <<endl;
  }

  void linkVectorStreamers() 
  {
    cout << "*** ElementLinkVector_test streamers start ***\n\n" <<endl;
    cout << "*** ElementLinkVector_test streamers OK ***\n\n" <<endl;
  }
} //end namespace


int main() {
  ISvcLocator* pSvcLoc;
  if (!initGaudi("ElementLinkVector_test.txt", pSvcLoc)) {
    cerr << "This test can not be run" << endl;
    return 0;
  }  
  assert(pSvcLoc);
  StoreGateSvc* pSG;
  
  const bool CREATEIF(true);
  assert((pSvcLoc->service("StoreGateSvc", pSG, CREATEIF)).isSuccess());
  assert(pSG);

  linkVector(*pSG);
  linkVectorStreamers();

  cout << "*** ElementLinkVector_test OK ***" <<endl;
  return 0;
}












