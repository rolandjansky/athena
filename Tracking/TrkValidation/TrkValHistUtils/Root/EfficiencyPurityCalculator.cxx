/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkValHistUtils/EfficiencyPurityCalculator.h"

void
EfficiencyPurityCalculator::inverseSum(TH1 *in, TH1 *out, int axis) {
  if (in->GetEntries() == 0) {
    return;
  }
  int nbinsx = in->GetXaxis()->GetNbins();
  int nbinsy = in->GetYaxis()->GetNbins();
  if (axis == kX) {
    for (int j = 0; j < nbinsy; ++j) {
      float ntotal = 0;
      for (int i = nbinsx; i >= 0; --i) {
        ntotal += in->GetBinContent(i, j);
        out->SetBinContent(i, j, ntotal);
      }
    }
  }else {
    for (int i = 0; i < nbinsx; ++i) {
      float ntotal = 0;
      for (int j = nbinsy; j >= 0; --j) {
        ntotal += in->GetBinContent(i, j);
        out->SetBinContent(i, j, ntotal);
      }
    }
  }
}

void
EfficiencyPurityCalculator::calculateIntegrated(TH1 *all, TH1 *matched, TH1 *puri, int axis, int /*dim*/) {
  inverseSum(matched, puri, axis);
  int nbinsx = matched->GetXaxis()->GetNbins();
  double xmin = matched->GetXaxis()->GetXmin();
  double xmax = matched->GetXaxis()->GetXmax();
  // int nbinsy = matched->GetYaxis()->GetNbins();
  // double ymin = matched->GetYaxis()->GetXmin();
  // double ymax = matched->GetYaxis()->GetXmax();
//  TH1* alli = 0;
  TH1 *alli = new TH1F("tmp", "tmp", nbinsx, xmin, xmax);
//  if (dim == 1) alli = new TH1F("tmp","tmp",nbinsx,xmin,xmax);
//  if (dim == 2) alli = new TH2F("tmp","tmp",nbinsx,xmin,xmax, nbinsy, ymin, ymax);
  inverseSum(all, alli, axis);
  puri->Sumw2();
  puri->Divide(puri, alli, 1, 1, "B");
  delete alli;
}

void
EfficiencyPurityCalculator::calculateEfficiency(TH1 *reference, TH1 *matched, TH1 *effPur) {
  effPur->Add(matched);
  effPur->Sumw2();
  effPur->Divide(effPur, reference, 1, 1, "B");
}
