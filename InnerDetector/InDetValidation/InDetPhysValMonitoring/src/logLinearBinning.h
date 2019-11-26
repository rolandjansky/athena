/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//
//  logLinearBinning.h
//  
//
//  Created by sroe on 22/11/2019, based on code by Max Goblirsch-Kolb.
//

#ifndef InDetPhysValMonitoring_logLinearBinning_h
#define InDetPhysValMonitoring_logLinearBinning_h
#include <vector>
#include <stdexcept>
namespace IDPVM{
  //in rel 21.9, this function was in the baseclass, InDetPlotBase.
  std::vector<double>
  logLinearBinning(const unsigned int nBins, const double absXmin, const double absXmax, 
                   const bool symmetriseAroundZero);
   

}
#endif