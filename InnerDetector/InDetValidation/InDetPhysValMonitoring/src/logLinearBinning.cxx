/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//
//  logLinearBinning.h
//  
//
//  Created by sroe on 22/11/2019, based on code by Max Goblirsch-Kolb.
//
#include "logLinearBinning.h"
#include <stdexcept>
#include <cmath>
#include <algorithm>
namespace IDPVM{
  std::vector<double>
  logLinearBinning(const unsigned int nBins, const double absXmin, const double absXmax, const bool symmetriseAroundZero){
    // some checks to ensure the user is requesting something sensible 
    if (absXmin <=0 or absXmax <=0){
      throw std::range_error("absXmin or absXmax argument to logLinearBinning is out of range");
    } else if (nBins ==0){
      throw std::range_error("nBins argument to logLinearBinning is zero");
    } 
    // reserve the vector space
    const unsigned int asymVecSize = nBins + 1; 
    std::vector<double> theBinning(asymVecSize,0.); 
    // define our starting bin edge and step size in log space
    const double logStart = std::log(absXmin);
    const double logDist = std::log(absXmax) - logStart;
    const double logStep = logDist / (double) nBins;
    // then populate the bin array
    double thisLog{logStart};
    for (auto & thisBin:theBinning){
      thisBin=std::exp(thisLog);
      thisLog+=logStep;
    }
    //sroe: I think the following is logically incorrect, but for now will reproduce the original functionality
    //pending testing
    //
    // if we want to symmetrise, we need one extra step to add the negative 
    // half axis (and the division at zero). 
    if (symmetriseAroundZero){
      std::vector<double> aux_negative;
      aux_negative.reserve(nBins+1);
      // flip signs (and populate new vec)
      std::transform(theBinning.begin(),theBinning.end(), std::back_inserter(aux_negative), 
                    [](double & val){return -1. * val;});
      // reorder
      std::reverse(aux_negative.begin(),aux_negative.end());
      // and add the split at zero 
      aux_negative.push_back(0.);
      // then put it all together
      theBinning.insert(theBinning.begin(), aux_negative.begin(), aux_negative.end()); 
    }
    return theBinning;
  }
}
