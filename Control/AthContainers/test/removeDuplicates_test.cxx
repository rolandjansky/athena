/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Disable this test in standalone compilation:
#ifndef XAOD_STANDALONE

#define DO_REMOVE_DUPLICATES 1
#include <cassert>
#include "AthContainers/DataVector.h"
#include "AthContainers/DataList.h"
using namespace std;
int main() {
  DataVector<int> *pDV = new DataVector<int>;
  int *pDuplicated = new int(0);
  pDV->push_back(pDuplicated);
  pDV->push_back(new int(1));
  pDV->push_back(pDuplicated); //this is an error: duplicates pointer
  {
    ostream_iterator<int*> os(cout, " ");
    cout << "--> We have purposely duplicated an element of pDV: " << hex;
    std::copy(pDV->begin(), pDV->end(), os);
    cout << dec <<endl;
  }
  cout << "---> now we should see a warning message from ~DataVector" <<endl;
  delete pDV; pDV=0;
  cout << "<--- ~DataVector returns" <<endl;
  DataList<int> *pDL = new DataList<int>;
  pDuplicated = new int(0);
  pDL->push_back(pDuplicated);
  pDL->push_back(new int(1));
  pDL->push_back(pDuplicated); //this is an error: duplicates pointer
  {
    ostream_iterator<int*> os(cout, " ");
    cout << "--> We have purposely duplicated an element of pDL: " << hex;
    std::copy(pDL->begin(), pDL->end(), os);
    cout << dec <<endl;
  }
  cout << "---> now we should see a warning message from ~DataList" <<endl;
  delete pDL; pDL=0;
  cout << "<--- ~DataList returns" <<endl;
  return 0;
}

#else

int main() {
   return 0;
}

#endif // not XAOD_STANDALONE
