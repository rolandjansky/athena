/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloWeightInterpolator.cxx,v 1.9 2009-01-27 09:09:15 gunal Exp $
//
// Description: see CaloWeightInterpolator.h
// 
// Environment:
//      Software developed for the ATLAS Detector at CERN LHC
//
// Author List:
//      Sven Menke
//
//-----------------------------------------------------------------------

//-----------------------
// This Struct's Header --
//-----------------------
#include "CaloUtils/CaloWeightInterpolator.h"
//#include "CaloWeightInterpolator.h"
#include <iostream>

double CaloWeightInterpolator::getWeight(std::vector<double> & w, 
					 std::vector<double> & x) {
  // check input
  if ( w.size() > 1 ) {
    if ( w.size() != (unsigned long)(1<<x.size()) ) {
      std::cerr << "CaloWeightInterpolator::getWeight(): vectors have wrong dimension"
		<< std::endl;
      return 0;
    }
    if ( x[0] < 0 || x[0] > 1 ) {
      std::cerr << "CaloWeightInterpolator::getWeight(): coordinate " << x[0] 
		<< " out of allowed range [0,1]"
		<< std::endl;
      return 0;
    }
    std::vector<double> w1(w.size()>>1);
    std::vector<double> x1(x.size()-1);
    std::copy(x.begin()+1,x.end(),x1.begin());
    for(unsigned int i=0;i<w1.size();i++) {
      w1[i] = w[2*i]+x[0]*(w[2*i+1]-w[2*i]);
    }
    return getWeight(w1,x1);
  }
  else return w[0];
}
