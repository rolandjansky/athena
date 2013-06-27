/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <cassert>
#include "rootincludes.h"
#include "comphistpars.h"

template <class T>
bool checkCompat(TH1*h1,TH1*h2) {
  T * hspecific_1 = dynamic_cast<T*>(h1);
  T * hspecific_2 = dynamic_cast<T*>(h2);
  if (!hspecific_1&&!hspecific_2)
    return true;
  if (!hspecific_1||!hspecific_2) {
    REPORT("incompatible types");
    return false;
  }
  return checkParameters<T>(hspecific_1,hspecific_2);
}


bool histsCompatible(TH1*h1,TH1*h2)
{
  assert(h1!=h2);
  assert(h1&&h2);
  return checkCompat<TH1>(h1,h2)
    && checkCompat<TH1F>(h1,h2)
    && checkCompat<TH1D>(h1,h2)
    && checkCompat<TH1S>(h1,h2)
    && checkCompat<TH1I>(h1,h2)
    && checkCompat<TH1C>(h1,h2)
    && checkCompat<TH2>(h1,h2)
    && checkCompat<TH2F>(h1,h2)
    && checkCompat<TH2D>(h1,h2)
    && checkCompat<TH2S>(h1,h2)
    && checkCompat<TH2I>(h1,h2)
    && checkCompat<TH2C>(h1,h2)
    && checkCompat<TProfile>(h1,h2)
    && checkCompat<TProfile2D>(h1,h2)
    ;
  //FIXME: Add more!!
}
