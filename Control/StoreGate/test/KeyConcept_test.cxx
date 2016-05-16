/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/constraints/KeyConcept.h"

#include <cassert>
#include <iostream>
#include <string>

using namespace std;


int main () {

  cerr << "*** KeyConcept_test BEGIN ***" << endl;

  KeyClass<string> tryThis;
  classIsKey<string>();
  
  KeyClass<int> tryThat;
  classIsKey<int>();
  
  cerr << "*** KeyConcept_test OK ***" <<endl;
  return 0;

}





