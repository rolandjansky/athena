/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/ScaledErrorData.h"

#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;


const TVectorD ScaledErrorData::offsets(int first, int last) const
{ 
  if (!checkRange(first, last)) {
    cout << "ScaledErrorData::offsets : range error" << endl;
    return TVectorD();
  }
  
  TVectorD offsets = xi();
  
  double t = time() - tbar() /*- Definitions::samplingTime(lwb())*/;
  offsets -= t*xip();
  offsets *= scale();  
  
  return offsets.GetSub(first, last, "I"); // "I" => indexed as [first, last]
}


const CovMatrix ScaledErrorData::errors(int first, int last) const
{ 
  if (!checkRange(first, last)) {
    cout << "ScaledErrorData::errors : range error" << endl;
    return CovMatrix();
  }
  
  CovMatrix errors = xiErr();
  double t = time() - tbar() /*- Definitions::samplingTime(lwb())*/;
  errors += t*t*xipErr();  
  errors *= scale()*scale();
  
  return errors.GetSub(first, last, first, last, "I");  // "I" => indexed as [first, last]
}

