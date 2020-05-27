/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_ROOTHELPERS_H
#define JETUNCERTAINTIES_ROOTHELPERS_H


#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "TString.h"
#include "TObjString.h"
#include "TObjArray.h"
#include "TFile.h"


class TH1;

class TAxis;

namespace RootHelpers 
{

        // RootHelper to have a const method for interpolation (why is there not a const version in ROOT???)

        double Interpolate(const TH1* histo, const double x);
        double Interpolate(const TH1* histo, const double x, const double y);
        double Interpolate(const TH1* histo, const double x, const double y, const double z);
        Int_t FindBin(const TAxis* axis, const double x);
        
        double Interpolate2D(const TH1* histo, const double x, const double y, const int xAxis=1, const int yAxis=2, const int otherDimBin=-1);

	Int_t test();



}





#endif

