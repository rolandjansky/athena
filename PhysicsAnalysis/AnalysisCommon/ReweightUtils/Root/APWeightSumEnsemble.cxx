/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define APWeightSumEnsemble_cxx
#include "ReweightUtils/APWeightSumEnsemble.h"
#include <cmath>
#include <iostream>
#include "TH1F.h"
#include "TVirtualFitter.h"
#include "ReweightUtils/APWeightEntry.h"

APWeightSumEnsemble::APWeightSumEnsemble() {
  _k_evt_orig = 0;
  _ensembleTest_done = false;
  _pdf = 0;
  _ensemble_mode = 0.;
  _ensemble_sigma = 0.;
  _rel_prec = 0.01;
}

APWeightSumEnsemble::~APWeightSumEnsemble() {
  delete _pdf;
}

void APWeightSumEnsemble::SetRelPrecision(double rel_prec) {
  _rel_prec = rel_prec;
  _ensembleTest_done = false;
}

void APWeightSumEnsemble::AddWeightToEvt(APWeightEntry* weight) {
  APWeightSum::AddWeightToEvt(weight);
  _current_evt_pdfs.push_back(weight);
  _ensembleTest_done = false;
}

double APWeightSumEnsemble::GetQuantile(const double prob) {
  if (!_ensembleTest_done) Compute();
  double prob_in[1] = {prob};
  double quant[1];
  _pdf->GetQuantiles(1, quant, prob_in);
  return quant[0];
}

double APWeightSumEnsemble::GetRandom() {
  if (!_ensembleTest_done) Compute();
  return _pdf->GetRandom();
}

double APWeightSumEnsemble::GetEnsemblePDFMode() {
  if (!_ensembleTest_done) Compute();
  return _ensemble_mode;
}

double APWeightSumEnsemble::GetEnsemblePDFStdDev() {
  if (!_ensembleTest_done) Compute();
  return _ensemble_sigma;
}

TH1F* APWeightSumEnsemble::GetPDF() {
  if (!_ensembleTest_done) Compute();
  return _pdf;
}

void APWeightSumEnsemble::FinishEvt(double ext_weight) {
  APWeightSum::FinishEvt(ext_weight);
  _weight_vector.push_back(_current_evt_pdfs);
  _ext_weights.push_back(ext_weight);
  _current_evt_pdfs.clear();
}

void APWeightSumEnsemble::Compute() {
  if ( _current_evt_pdfs.size() > 0 ) std::cout << "ERROR in APWeightSumEnsemble::Compute: Last event was not finished and will not be taken into account for the calculation!" << std::endl;
  //Improve estimation of range
  if (_pdf != 0) delete _pdf;
  _pdf = new TH1F("", "", 1000, _k_evt_weight - 6. * sqrt(_k_evt_weight), _k_evt_weight + 6. * sqrt(_k_evt_weight));

  for (unsigned int i = 0; i < 100000000; ++i) { //variable n_iter
    double sum_weights = 0.0;
    for (unsigned int j = 0; j < _k_evt_orig; ++j) {
      double weight = 1.0;
      for (unsigned int l = 0, L = _weight_vector[j].size(); l < L; ++l) {
        weight *= (1. - (_weight_vector[j][l])->GetRandom());
      }
      sum_weights += _ext_weights[j] * (1. - weight);
    }
    _pdf->Fill(sum_weights);
    if (i % 100 == 0) {
      _pdf->Fit("gaus", "0Q");
      TVirtualFitter *fitter = TVirtualFitter::GetFitter();
      if ( fitter->GetParError(2) / fitter->GetParameter(2) < _rel_prec ) break;
    }
  }

  _pdf->Fit("gaus", "0Q");
  TVirtualFitter *fitter = TVirtualFitter::GetFitter();
  _ensemble_mode = fitter->GetParameter(1);
  _ensemble_sigma = fitter->GetParameter(2);
  _ensembleTest_done = true;
}
