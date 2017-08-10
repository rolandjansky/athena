/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define APReweight_cxx
#include "ReweightUtils/APWeightHist.h"
#include "ReweightUtils/APWeightEntry.h"
#include "ReweightUtils/MathTools.h"
#include <cmath>
#include "TF1.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"

ClassImp(APWeightHist)

using namespace std;

APWeightHist::APWeightHist(const char *name, const char *title, const int n_bins, const float x_min, const float x_max)
  : TH1D(name, title, n_bins, x_min, x_max)
    //m_prec(0)
{
  m_graph_stat = new TGraphAsymmErrors();
  m_graph_stat->GetXaxis()->SetTitle(GetXaxis()->GetTitle());
  m_graph_stat->GetYaxis()->SetTitle(GetYaxis()->GetTitle());
  m_graph_syst = new TGraphErrors();
  m_graph_syst->GetXaxis()->SetTitle(GetXaxis()->GetTitle());
  m_graph_syst->GetYaxis()->SetTitle(GetYaxis()->GetTitle());
  m_binned_weights = vector< vector< APWeightEntry* > >(n_bins);
  Sumw2();
  m_SumSys2 = vector< double >(n_bins, 0.);
  m_computed_entries = 0.;
}

APWeightHist::APWeightHist()
  : m_graph_stat(0),
    m_graph_syst(0)
    //m_prec(0)
{
  m_computed_entries = 0.;
}

APWeightHist::~APWeightHist() {

}

int APWeightHist::Fill(const double value, APWeightEntry* weight) {
  //   if (fBuffer) return BufferFill(value,w);
  double w = weight->GetExpectancy();
  int bin = fXaxis.FindBin(value);
  fEntries++;
  AddBinContent(bin, w);
  if (fSumw2.fN) fSumw2.fArray[bin] += w * w;
  if (bin == 0 || bin > fXaxis.GetNbins()) {
    if (!fgStatOverflows) return -1;
  }
  Double_t z = (w > 0 ? w : -w);
  fTsumw += z;
  fTsumw2 += z*z;
  fTsumwx += z*value;
  fTsumwx2 += z * value*value;
  (m_binned_weights[bin - 1]).push_back(weight);
  fSumw2.fArray[bin] += weight->GetVariance() + z*z;
  m_SumSys2[bin - 1] += weight->GetSysUncert2();
  return bin;
}

void APWeightHist::ComputeGraph(const int prec) {
  for (int i = 0, I = m_graph_stat->GetN(); i < I; ++i) m_graph_stat->RemovePoint(i);
  for (int i = 0, I = m_graph_syst->GetN(); i < I; ++i) m_graph_syst->RemovePoint(i);
  vector< TH1D* > original_dists = vector< TH1D* >(fXaxis.GetNbins());
  m_bin_dists = vector< TH1D* >(fXaxis.GetNbins());
  int point_count = 0;
  for (int j = 0; j < fXaxis.GetNbins(); ++j) {
    //Create Poisson distributions for original histogram.
    double unweighted_content = (double) (m_binned_weights[j]).size();
    double sig_low_o = (max((double) 0.0, (unweighted_content - 5 * sqrt(unweighted_content))));
    double sig_high_o = (unweighted_content + 5 * sqrt(unweighted_content));
    double step = (sig_high_o - sig_low_o) * 1e-3;

    original_dists[j] = new TH1D("", "", 1000, sig_low_o, sig_high_o);

    int bin_no = 1;
    double curr_fact = ln_factorialApp((m_binned_weights[j]).size());
    for (double lambda = sig_low_o; lambda < sig_high_o; lambda += step) {
      double value = exp(-lambda + unweighted_content * log(lambda) - curr_fact);
      if (value != value) value = 0.;
      (original_dists[j])->SetBinContent(bin_no, value);
      ++bin_no;
    }
    (original_dists[j])->Scale(1. / (original_dists[j])->GetSum());
    const double old_mode = (original_dists[j])->GetBinCenter((original_dists[j])->GetMaximumBin());
    const double inv_old_mode = 1. / old_mode;

    //Create different final sum of weights for this bin
    vector< double > bin_sumw(prec, 0.);
    double max_value = 1.e-100;
    double min_value = 1.e100;

    for (vector<double>::iterator it_hist = bin_sumw.begin(), it_histE = bin_sumw.end(); it_hist != it_histE; ++it_hist) {
      for (vector<APWeightEntry*>::iterator it_func = (m_binned_weights[j]).begin(), it_funcE = (m_binned_weights[j]).end(); it_func != it_funcE; ++it_func) {
        *it_hist += (*it_func)->GetRandom();
      }
      double cur_val = *it_hist;
      if (cur_val < min_value) {
        min_value = cur_val;
      } else if (cur_val > max_value) {
        max_value = cur_val;
      }
    }

    //Create final distribution for this bin
    TH1D* scaled_hist_l = (TH1D*) (original_dists[j])->Clone("");
    TH1D* scaled_hist_h = (TH1D*) (original_dists[j])->Clone("");
    double scale = min_value * inv_old_mode;
    scaled_hist_l->GetXaxis()->SetLimits(scale * sig_low_o, scale * sig_high_o);
    scale = max_value * inv_old_mode;
    scaled_hist_h->GetXaxis()->SetLimits(scale * sig_low_o, scale * sig_high_o);

    double quant[1];
    double prob[1] = {0.001};

    scaled_hist_l->GetQuantiles(1, quant, prob);
    double sig_low = quant[0];
    prob[0] = 0.999;
    scaled_hist_h->GetQuantiles(1, quant, prob);
    double sig_high = quant[0];

    m_bin_dists[j] = new TH1D("", "", 1000, sig_low, sig_high);

    //fill final distribution for this bin with scaled/weighted Poisson distributions
    if (unweighted_content > 0.) {
      for (int k = 0; k < prec; ++k) {
        double new_val = bin_sumw[k];
        TH1D* scaled_hist = (TH1D*) (original_dists[j])->Clone("");
        double temp_scale = new_val * inv_old_mode;
        scaled_hist->GetXaxis()->SetLimits(temp_scale * sig_low_o, temp_scale * sig_high_o);
        for (int l = 1; l < 1001; ++l) {
          (m_bin_dists[j])->AddBinContent(l, scaled_hist->GetBinContent(scaled_hist->FindBin((m_bin_dists[j])->GetBinCenter(l))));
        }
        delete scaled_hist;
      }
      double local_value = sig_low + (m_bin_dists[j])->GetMaximumBin() * ((sig_high - sig_low) * 1e-3);
      m_graph_stat->SetPoint(point_count, (GetXaxis()->GetXmin() + j * GetBinWidth(j + 1) + GetBinWidth(j + 1) / 2), local_value);
      m_graph_syst->SetPoint(point_count, (GetXaxis()->GetXmin() + j * GetBinWidth(j + 1) + GetBinWidth(j + 1) / 2), local_value);
      double quantile[ 2 ];
      double conf[ 2 ] = {0.1585, 0.8415};
      (m_bin_dists[j])->GetQuantiles(2, quantile, conf);
      m_graph_stat->SetPointError(point_count, GetBinWidth(j + 1) / 2, GetBinWidth(j + 1) / 2, local_value - quantile[0], quantile[1] - local_value);
      m_graph_syst->SetPointError(point_count, GetBinWidth(j + 1) / 2, local_value * (sqrt(m_SumSys2[j]) / GetBinContent(j + 1)));
      ++point_count;
    }
  }
  m_computed_entries = fEntries;
}

TGraphAsymmErrors* APWeightHist::GetGraphStatUncert(bool autocompute) {
  if (autocompute && fEntries > m_computed_entries) ComputeGraph();
  return m_graph_stat;
}

TGraphErrors* APWeightHist::GetGraphSystUncert(bool simple, bool autocompute) {
  if (simple) {
    for (int i = 0, I = m_graph_syst->GetN(); i < I; ++i) m_graph_syst->RemovePoint(i);
    int point_count = 0;
    for (int j = 0; j < fXaxis.GetNbins(); ++j) {
      if ((m_binned_weights[j]).size() > 0) {
        m_graph_syst->SetPoint(point_count, (GetXaxis()->GetXmin() + j * GetBinWidth(j + 1) + GetBinWidth(j + 1) / 2), GetBinContent(j + 1));
        m_graph_syst->SetPointError(point_count, GetBinWidth(j + 1) / 2, sqrt(m_SumSys2[j]));
        ++point_count;
      }
    }
    return m_graph_syst;
  }
  if (autocompute && fEntries > m_computed_entries) ComputeGraph();
  return m_graph_syst;
}

TH1D* APWeightHist::GetBinPDF(unsigned int bin, bool autocompute) {
  if (autocompute && fEntries > m_computed_entries) ComputeGraph();
  return m_bin_dists[bin - 1];
}
