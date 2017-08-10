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
  m_k_evt_orig = 0;
  m_ensembleTest_done = false;
  m_pdf = 0;
  m_ensemble_mode = 0.;
  m_ensemble_sigma = 0.;
  m_rel_prec = 0.01;
}

APWeightSumEnsemble::~APWeightSumEnsemble() {
  delete m_pdf;
}

void APWeightSumEnsemble::SetRelPrecision(double rel_prec) {
  m_rel_prec = rel_prec;
  m_ensembleTest_done = false;
}

void APWeightSumEnsemble::AddWeightToEvt(APWeightEntry* weight) {
  APWeightSum::AddWeightToEvt(weight);
  m_current_evt_pdfs.push_back(weight);
  m_ensembleTest_done = false;
}

double APWeightSumEnsemble::GetQuantile(const double prob) {
  if (!m_ensembleTest_done) Compute();
  double prob_in[1] = {prob};
  double quant[1];
  m_pdf->GetQuantiles(1, quant, prob_in);
  return quant[0];
}

double APWeightSumEnsemble::GetRandom() {
  if (!m_ensembleTest_done) Compute();
  return m_pdf->GetRandom();
}

double APWeightSumEnsemble::GetEnsemblePDFMode() {
  if (!m_ensembleTest_done) Compute();
  return m_ensemble_mode;
}

double APWeightSumEnsemble::GetEnsemblePDFStdDev() {
  if (!m_ensembleTest_done) Compute();
  return m_ensemble_sigma;
}

TH1F* APWeightSumEnsemble::GetPDF() {
  if (!m_ensembleTest_done) Compute();
  return m_pdf;
}

void APWeightSumEnsemble::FinishEvt(double ext_weight) {
  APWeightSum::FinishEvt(ext_weight);
  m_weight_vector.push_back(m_current_evt_pdfs);
  m_ext_weights.push_back(ext_weight);
  m_current_evt_pdfs.clear();
}

void APWeightSumEnsemble::Compute() {
  if ( m_current_evt_pdfs.size() > 0 ) std::cout << "ERROR in APWeightSumEnsemble::Compute: Last event was not finished and will not be taken into account for the calculation!" << std::endl;
  //Improve estimation of range
  if (m_pdf != 0) delete m_pdf;
  m_pdf = new TH1F("", "", 1000, m_k_evt_weight - 6. * sqrt(m_k_evt_weight), m_k_evt_weight + 6. * sqrt(m_k_evt_weight));

  for (unsigned int i = 0; i < 100000000; ++i) { //variable n_iter
    double sum_weights = 0.0;
    for (unsigned int j = 0; j < m_k_evt_orig; ++j) {
      double weight = 1.0;
      for (unsigned int l = 0, L = m_weight_vector[j].size(); l < L; ++l) {
        weight *= (1. - (m_weight_vector[j][l])->GetRandom());
      }
      sum_weights += m_ext_weights[j] * (1. - weight);
    }
    m_pdf->Fill(sum_weights);
    if (i % 100 == 0) {
      m_pdf->Fit("gaus", "0Q");
      TVirtualFitter *fitter = TVirtualFitter::GetFitter();
      if ( fitter->GetParError(2) / fitter->GetParameter(2) < m_rel_prec ) break;
    }
  }

  m_pdf->Fit("gaus", "0Q");
  TVirtualFitter *fitter = TVirtualFitter::GetFitter();
  m_ensemble_mode = fitter->GetParameter(1);
  m_ensemble_sigma = fitter->GetParameter(2);
  m_ensembleTest_done = true;
}
