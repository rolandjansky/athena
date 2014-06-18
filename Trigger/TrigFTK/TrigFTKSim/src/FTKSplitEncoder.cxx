/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
   A special module to encode eta bounds for split architecture.
   We use cot(theta) as the main variable, but we divide sectors into unequal bins:
   In the barrel, it is divided into equal cot(theta) bins (up to about eta=1.2)
   In the endcap, it is divided into equal eta bins (!) to give wider pseudo-modules
*/
#include "TrigFTKSim/FTKSplitEncoder.h"
#include <iostream>
using namespace std;

//#define CTHETA_DEBUG

FTKSplitEncoder FTKSplitEncoder::m_instance(21);
FTKSplitEncoder::FTKSplitEncoder(int nbins) : ctheta_nbins(nbins) {
#ifdef CTHETA_DEBUG
  cout << "FTKSplitEncoder: initializing with " << ctheta_nbins << " bins" << endl;
#endif
  ctheta_lookup = new double[ctheta_nbins];
  // ctheta sector bin bounds: flat in ctheta in the barrel; extra wide bins in the endcaps
  ctheta_curindex = 0;
  ctheta_lookup[0] = -7.29744; // left edge
  ctheta_lookup[1] = -5.21984;
  ctheta_lookup[2] = -3.71821;
  ctheta_lookup[3] = -2.62683;
  ctheta_lookup[4] = -1.82531;
  ctheta_lookup[5] = -1.38889;  //31
  ctheta_lookup[6] = -1.11111;  //32
  ctheta_lookup[7] = -0.833334; //33
  ctheta_lookup[8] = -0.555556; //34
  ctheta_lookup[9] = -0.277778; //35
  ctheta_lookup[10] = 0;        //36
  ctheta_lookup[11] = 0.277778; //37
  ctheta_lookup[12] = 0.555556; //38
  ctheta_lookup[13] = 0.833334; //39
  ctheta_lookup[14] = 1.11111;  //40
  ctheta_lookup[15] = 1.38889;  //41
  ctheta_lookup[16] = 1.82531;
  ctheta_lookup[17] = 2.62683;
  ctheta_lookup[18] = 3.71821;
  ctheta_lookup[19] = 5.21984;
  ctheta_lookup[20] = 7.29744; // right edge
}

FTKSplitEncoder::~FTKSplitEncoder() {
  delete [] ctheta_lookup;
}

/* 
   Has to be called first to fill the current-index variable
   Not optimized! Could exploit positive-negative symmetry!
*/
int FTKSplitEncoder::cthetaIndex(double v) {
  int i;
  ctheta_curindex=0;
  for(i=0;i<ctheta_nbins-1;i++) {
    if((i==0||v>=ctheta_lookup[i]) && (i==ctheta_nbins-2 || v<ctheta_lookup[i+1])) {
      ctheta_curindex=i;
#ifdef CTHETA_DEBUG
      cout << "FTKSplitEncoder: ctheta = " << v << " corresponds to bin " << ctheta_curindex << endl;
#endif
      return ctheta_curindex;
    }
  }
  ctheta_curindex=ctheta_nbins-1;
#ifdef CTHETA_DEBUG
  cout << "FTKSplitEncoder: ctheta = " << v << " corresponds to bin " << ctheta_curindex << endl;
#endif
  return ctheta_curindex;
}
