/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <vector>
#include <math.h>

using namespace std;

namespace BTagCalib
{
   std::pair<float, vector<float> > BTagCalibrationFunction(const vector<float>& pt, const vector<float>& eta,
                                                            const vector<float>& SV0, const vector<int>& pdgid, 
                                                            int error, std::string taggerName = "SV0",
                                                            std::string OP = "5_85", float opval = 5.85);
}

