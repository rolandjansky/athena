/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/IndexRange.h"

#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;


bool IndexRange::checkRange(int& l, int& h) const
{  
  if (l < 0) 
    l = lwb();
  else if (!isInRange(l)) {
    cout << "IndexRange::checkRange : lower bound " << l << " is out of bounds" << endl;
    return false;
  }

  if (h < 0) 
    h = upb();
  else if (!isInRange(h)) {
    cout << "IndexRange::checkRange : upper bound " << h << " is out of bounds" << endl;
    return false;
  }

  return true;
}
