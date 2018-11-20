/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

class TH1;
class TAxis;

namespace RootHelpers
{

double Interpolate(const TH1* histo, const double x);

double Interpolate(const TH1* histo, const double x, const double y);

double Interpolate2D(const TH1* histo, const double x, const double y, const int xAxis, const int yAxis, const int otherDimBin);

double Interpolate(const TH1* histo, const double x, const double y, const double z);

} // end RootHelpers namespace

