/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_JETHELPERS_H
#define JETUNCERTAINTIES_JETHELPERS_H

#include "TFile.h"


class TH1;

class TAxis;

namespace JetHelpers 
{

        // JetHelpers to have a const method for interpolation. This will be change in future versions as discussed in https://sft.its.cern.ch/jira/browse/ROOT-9994

        double Interpolate(const TH1* histo, const double x);
        double Interpolate(const TH1* histo, const double x, const double y);
        double Interpolate(const TH1* histo, const double x, const double y, const double z);
        Int_t FindBin(const TAxis* axis, const double x);
        
        double Interpolate2D(const TH1* histo, const double x, const double y, const int xAxis=1, const int yAxis=2, const int otherDimBin=-1);


}

#endif
