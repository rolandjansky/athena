/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define APReweight_cxx
#include "ReweightUtils/APReweight.h"
#include "ReweightUtils/APWeightEntry.h"
#include "ReweightUtils/MathTools.h"
#include <iostream>
#include "TTree.h"
#include "TH1.h"

using namespace std;

APReweight::APReweight() :
  APReweightBase(),
  m_denominator_hist(0),
  m_numerator_hist(0),
  m_n_bins(0),
  m_axis(0)
{
  m_isQuiet = false;
}

APReweight::APReweight(TTree* denominator, string denominator_branch, TTree* numerator, string numerator_branch, unsigned int n_bins, double x_min, double x_max, bool isTrig) : APReweightBase() {
  m_empty_weight = new APWeightEntry(0, 0, 1.);
  m_denominator_hist = new TH1D("", "denominator_hist", n_bins, x_min, x_max);
  m_numerator_hist = new TH1D("", "numerator_hist", n_bins, x_min, x_max);
  m_axis = (TAxis*) m_denominator_hist->GetXaxis()->Clone("");
  m_n_bins = m_denominator_hist->GetNbinsX();

  //Readout from Trees and fill histograms
  vector<double> *denominator_vec = 0, *numerator_vec = 0;
  TBranch *b_denominator_vec, *b_numerator_vec;
  denominator->SetBranchAddress(denominator_branch.c_str(), &denominator_vec, &b_denominator_vec);
  numerator->SetBranchAddress(numerator_branch.c_str(), &numerator_vec, &b_numerator_vec);
  unsigned long nentries_denominator = denominator->GetEntries(), nentries_numerator = numerator->GetEntries(), counter = 0;
  while (counter < nentries_denominator) {
    denominator->GetEntry(counter);
    for (unsigned int i = 0, I = denominator_vec->size(); i < I; ++i) m_denominator_hist->Fill((*denominator_vec)[i]);
    ++counter;
  }
  counter = 0;
  while (counter < nentries_numerator) {
    numerator->GetEntry(counter);
    for (unsigned int i = 0, I = numerator_vec->size(); i < I; ++i) m_numerator_hist->Fill((*numerator_vec)[i]);
    ++counter;
  }

  //Cleanup
  denominator->ResetBranchAddress(b_denominator_vec);
  numerator->ResetBranchAddress(b_numerator_vec);
  delete b_denominator_vec;
  delete b_numerator_vec;
  denominator_vec->clear();
  delete denominator_vec;
  numerator_vec->clear();
  delete numerator_vec;

  m_scale = (double) nentries_denominator / (double) nentries_numerator;
  m_isTrig = isTrig;
  m_isQuiet = false;
  for (unsigned int i = 0; i < m_n_bins; ++i) {
    APWeightEntry* temp_entry = new APWeightEntry((unsigned int) m_denominator_hist->GetBinContent(i + 1), (unsigned int) m_numerator_hist->GetBinContent(i + 1), m_scale, m_isTrig);
    temp_entry->SetCoordinates(vector<int>(1,i+1),vector<int>(1,m_n_bins));
    temp_entry->SetID(m_ID);
    m_weights.push_back(temp_entry);
  }
}

APReweight::APReweight(vector<double> denominator, vector<double> numerator, unsigned int n_bins, double x_min, double x_max, bool isTrig) : APReweightBase() {
  m_empty_weight = new APWeightEntry(0, 0, 1.);
  m_denominator_hist = new TH1D("", "denominator_hist", n_bins, x_min, x_max);
  m_numerator_hist = new TH1D("", "numerator_hist", n_bins, x_min, x_max);
  m_axis = (TAxis*) m_denominator_hist->GetXaxis()->Clone("");
  m_n_bins = m_denominator_hist->GetNbinsX();

  //Readout from vectors and fill histograms
  for (unsigned long i = 0, I = denominator.size(); i < I; ++i) m_denominator_hist->Fill(denominator[i]);
  for (unsigned long i = 0, I = numerator.size(); i < I; ++i) m_numerator_hist->Fill(numerator[i]);

  m_scale = (double) denominator.size() / (double) numerator.size();
  m_isTrig = isTrig;
  m_isQuiet = false;
  for (unsigned int i = 0; i < m_n_bins; ++i) {
    APWeightEntry* temp_entry = new APWeightEntry((unsigned int) m_denominator_hist->GetBinContent(i + 1), (unsigned int) m_numerator_hist->GetBinContent(i + 1), m_scale, m_isTrig);
    temp_entry->SetCoordinates(vector<int>(1,i+1),vector<int>(1,m_n_bins));
    temp_entry->SetID(m_ID);
    m_weights.push_back(temp_entry);
  }
}

APReweight::APReweight(TH1* denominator_in, TH1* numerator_in, bool isTrig) : APReweightBase() {
  m_empty_weight = new APWeightEntry(0, 0, 1.);
  m_denominator_hist = (TH1D*) denominator_in->Clone("");
  m_numerator_hist = (TH1D*) numerator_in->Clone("");
  m_axis = (TAxis*) m_denominator_hist->GetXaxis()->Clone("");
  m_n_bins = m_denominator_hist->GetNbinsX();
  m_scale = (double) denominator_in->GetEntries() / (double) numerator_in->GetEntries();
  m_isTrig = isTrig;
  m_isQuiet = false;
  for (unsigned int i = 0; i < m_n_bins; ++i) {
    APWeightEntry* temp_entry = new APWeightEntry((unsigned int) m_denominator_hist->GetBinContent(i + 1), (unsigned int) m_numerator_hist->GetBinContent(i + 1), m_scale, m_isTrig);
    temp_entry->SetCoordinates(vector<int>(1,i+1),vector<int>(1,m_n_bins));
    temp_entry->SetID(m_ID);
    m_weights.push_back(temp_entry);
  }
  
  if( m_isTrig ) {
    for( int i = 1; i < m_numerator_hist->GetNbinsX(); ++i ) {
      if( m_numerator_hist->GetBinContent(i) > m_denominator_hist->GetBinContent(i) ) {
        std::cout << "WARNING in APReweight::~APReweight(TH1* denominator_in, TH1* numerator_in, bool isTrig) : Using histograms " << m_numerator_hist->GetName() << " and " << m_denominator_hist->GetName() << " the efficiency is larger than 1 for bin " << i << "! This is inconsistent and can lead to unwanted behaviour (weights > 1, variance < 0 )! Please check your input histograms. In order to avoid negative variances, the efficiency in this bin will be set to 0. " << std::endl;
        m_numerator_hist->SetBinContent(i,0);
      }
    }
  }

}

void APReweight::ReadEfficiency(TH1* efficiency_in, TH1* err_low_in, TH1* err_high_in) {
  if (err_high_in == 0) err_high_in = err_low_in;
  m_empty_weight = new APWeightEntry(0, 0, 1.);
  m_denominator_hist = new TH1D("", "", 1, 0., 1.);
  m_numerator_hist = new TH1D("", "", 1, 0., 1.);
  m_axis = (TAxis*) efficiency_in->GetXaxis()->Clone("");
  m_n_bins = efficiency_in->GetNbinsX();
  m_scale = 1.0;
  m_isTrig = true;
  for (unsigned int i = 0; i < m_n_bins; ++i) {
    APWeightEntry *temp_entry = new APWeightEntry();
    temp_entry->ReadEfficiency(efficiency_in->GetBinContent(i + 1), err_low_in->GetBinContent(i + 1), err_high_in->GetBinContent(i + 1));
    temp_entry->SetCoordinates(vector<int>(1,i+1),vector<int>(1,m_n_bins));
    temp_entry->SetID(m_ID);
    m_weights.push_back(temp_entry);
  }
}

APReweight::~APReweight() {
  delete m_denominator_hist;
  delete m_numerator_hist;
  delete m_empty_weight;
  delete m_axis;
  for (vector<APWeightEntry*>::reverse_iterator it = m_weights.rbegin(); it != m_weights.rend(); ++it) {
    delete *it;
  }
  m_weights.clear();
}

APWeightEntry* APReweight::GetBinWeight(unsigned int bin) const {
  if (bin == 0) return m_empty_weight;
  return m_weights[bin - 1];
}

APWeightEntry* APReweight::GetWeight(double value) const {
  return GetBinWeight(GetBin(value));
}

TH1D* APReweight::GetDenominatorHist() const {
  return m_denominator_hist;
}

TH1D* APReweight::GetNumeratorHist() const {
  return m_numerator_hist;
}

double APReweight::GetSampleScale() const {
  return m_scale;
}

unsigned int APReweight::NBins() const {
  return m_n_bins;
}

unsigned int APReweight::GetBin(double value) const {
  for (unsigned int i = 1; i <= m_n_bins; ++i) {
    if (value >= m_axis->GetBinLowEdge(i) && value <= m_axis->GetBinUpEdge(i)) {
      return i;
    }
  }
  if (!m_isQuiet) cout << "WARNING in APReweight::GetBin: Value out of range! Returning 0." << endl;
  return 0;
}

void APReweight::SetSystUncert(double rel_uncert) {
  for (unsigned int i = 1; i <= m_n_bins; ++i) {
    GetBinWeight(i)->SetSystUncert(rel_uncert);
  }
}

void APReweight::SetQuietMode(bool isQuiet) {
  m_isQuiet = isQuiet;
}
