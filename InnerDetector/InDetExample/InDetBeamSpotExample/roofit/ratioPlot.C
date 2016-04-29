/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ratioPlot.C 684446 2015-07-20 21:20:54Z beringer $
//
// Utility function to calculate ratio of data histogram over fit curve
//
// J. Beringer (LBNL), July 2015

#include <iostream>

#include "TAxis.h"
#include "RooHist.h"
#include "RooCurve.h"


RooHist* ratioPlot(const RooHist& data, const RooCurve& fit) {
  // std::cout << "running ratioPlot ..." << std::endl;
  RooHist* ratio = new RooHist(data.getNominalBinWidth());
  ratio->SetName("ratio_data_fit");
  ratio->SetTitle("Ratio data/fit");
  ratio->GetXaxis()->SetTitle(data.GetXaxis()->GetTitle());

  // Determine range of curve
  Double_t xstart,xstop,y;
  fit.GetPoint(0,xstart,y) ;
  fit.GetPoint(fit.GetN()-1,xstop,y) ;

  for(Int_t i=0; i<data.GetN(); i++) {
    Double_t x, d;
    data.GetPoint(i,x,d);

    // Omit points outside curve range and points with zero data entries
    if (x<xstart || x>xstop) continue;
    if (d==0.) continue;

    // Ratio and errors
    Double_t f = fit.interpolate(x);
    if (f==0.) continue;  // avoid division by zero
    Double_t r = d/f;
    Double_t rl = data.GetErrorYlow(i)/f;
    Double_t rh = data.GetErrorYhigh(i)/f;
    ratio->addBinWithError(x,r,rl,rh);
  }
  return ratio;
}
