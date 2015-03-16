/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// KFLUT.h
// contains LUT values for KF corrections to MET
//  Created by V Sorin on 03/2015
//

#ifndef L1TopoSimulationUtils_KFLUT
#define L1TopoSimulationUtils_KFLUT

#include <iostream>
#include <vector>

namespace TCS {

using namespace std;

 class KFLUT {

   public:
      KFLUT();
      ~KFLUT();


      int getetabin(double eta);
      int getetbin(unsigned int et);
  
      double getcorrKF(int i, int j);
      

   private:

      void fillLUT();
      vector<unsigned int> etlimits;
      vector<double> etalimits;
     
      vector<vector<double>> LUTKF;

 };



} // end namespace

#endif
