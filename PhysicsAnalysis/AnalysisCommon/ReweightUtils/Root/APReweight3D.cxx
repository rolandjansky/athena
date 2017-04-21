/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define APReweight3D_cxx
#include "ReweightUtils/APReweight3D.h"
#include "ReweightUtils/APWeightEntry.h"
#include "ReweightUtils/MathTools.h"
#include <iostream>
#include "TTree.h"
#include "TH3.h"

using namespace std;

APReweight3D::APReweight3D() :
  APReweightBase(),
  m_denominator_hist(0),
  m_numerator_hist(0),
  m_n_bins_x(0),
  m_n_bins_y(0),
  m_n_bins_z(0),
  m_axis_x(0),
  m_axis_y(0),
  m_axis_z(0)
{
  m_isQuiet = false;
}

APReweight3D::APReweight3D(TH3* denominator_in, TH3* numerator_in, bool isTrig) : APReweightBase() {
  m_empty_weight = new APWeightEntry(0, 0, 1.);
  m_denominator_hist = (TH3D*) denominator_in->Clone("");
  m_numerator_hist = (TH3D*) numerator_in->Clone("");
  m_axis_x = (TAxis*) m_denominator_hist->GetXaxis()->Clone("");
  m_axis_y = (TAxis*) m_denominator_hist->GetYaxis()->Clone("");
  m_axis_z = (TAxis*) m_denominator_hist->GetZaxis()->Clone("");
  m_n_bins_x = m_denominator_hist->GetNbinsX();
  m_n_bins_y = m_denominator_hist->GetNbinsY();
  m_n_bins_z = m_denominator_hist->GetNbinsZ();
  m_scale = (double) denominator_in->GetEntries() / (double) numerator_in->GetEntries();
  m_isTrig = isTrig;
  m_isQuiet = false;
  for (unsigned int i = 0; i < m_n_bins_x; ++i) {
    m_weights.push_back(vector< vector<APWeightEntry*> >());
    for (unsigned int j = 0; j < m_n_bins_y; ++j) {
      m_weights[i].push_back(vector<APWeightEntry*>());
      for (unsigned int k = 0; k < m_n_bins_z; ++k) {
        APWeightEntry* temp_entry = new APWeightEntry((unsigned int) m_denominator_hist->GetBinContent(i + 1, j + 1, k + 1), (unsigned int) m_numerator_hist->GetBinContent(i + 1, j + 1, k + 1), m_scale, m_isTrig);
        vector<int> temp_vec(3,0);
        vector<int> temp_vec_axes(3,0);
        temp_vec[0] = i+1;
        temp_vec[1] = j+1;
        temp_vec[2] = k+1;
        temp_vec_axes[0] = m_n_bins_x;
        temp_vec_axes[1] = m_n_bins_y;
        temp_vec_axes[2] = m_n_bins_z;
        temp_entry->SetCoordinates(temp_vec,temp_vec_axes);
        temp_entry->SetID(m_ID);
        m_weights[i][j].push_back(temp_entry);
      }
    }
  }

  if( m_isTrig ) {
    for( int i = 1; i < m_numerator_hist->GetNbinsX()*m_numerator_hist->GetNbinsY()*m_numerator_hist->GetNbinsZ(); ++i ) {
      if( m_numerator_hist->GetBinContent(i) > m_denominator_hist->GetBinContent(i) ) {
        std::cout << "WARNING in APReweight3D::~APReweight3D(TH3* denominator_in, TH3* numerator_in, bool isTrig) : Using histograms " << m_numerator_hist->GetName() << " and " << m_denominator_hist->GetName() << " the efficiency is larger than 1 for bin " << i << "! This is inconsistent and can lead to unwanted behaviour (weights > 1, variance < 0 )! Please check your input histograms. In order to avoid negative variances, the efficiency in this bin will be set to 0." << std::endl;
        m_numerator_hist->SetBinContent(i,0);
      }
    }
  }


}

void APReweight3D::ReadEfficiency(TH3* efficiency_in, TH3* err_low_in, TH3* err_high_in) {
  if (err_high_in == 0) err_high_in = err_low_in;
  m_empty_weight = new APWeightEntry(0, 0, 1.);
  m_denominator_hist = new TH3D("", "", 1, 0., 1., 1, 0., 1., 1, 0., 1.);
  m_numerator_hist = new TH3D("", "", 1, 0., 1., 1, 0., 1., 1, 0., 1.);
  m_axis_x = (TAxis*) efficiency_in->GetXaxis()->Clone("");
  m_axis_y = (TAxis*) efficiency_in->GetYaxis()->Clone("");
  m_axis_z = (TAxis*) efficiency_in->GetZaxis()->Clone("");
  m_n_bins_x = efficiency_in->GetNbinsX();
  m_n_bins_y = efficiency_in->GetNbinsY();
  m_n_bins_z = efficiency_in->GetNbinsZ();
  m_scale = 1.0;
  m_isTrig = true;
  for (unsigned int i = 0; i < m_n_bins_x; ++i) {
    m_weights.push_back(vector< vector<APWeightEntry*> >());
    for (unsigned int j = 0; j < m_n_bins_y; ++j) {
      m_weights[i].push_back(vector<APWeightEntry*>());
      for (unsigned int k = 0; k < m_n_bins_z; ++k) {
        APWeightEntry *temp_entry = new APWeightEntry();
        temp_entry->ReadEfficiency(efficiency_in->GetBinContent(i + 1, j + 1, k + 1), err_low_in->GetBinContent(i + 1, j + 1, k + 1), err_high_in->GetBinContent(i + 1, j + 1, k + 1));
        vector<int> temp_vec(3,0);
        vector<int> temp_vec_axes(3,0);
        temp_vec[0] = i+1;
        temp_vec[1] = j+1;
        temp_vec[2] = k+1;
        temp_vec_axes[0] = m_n_bins_x;
        temp_vec_axes[1] = m_n_bins_y;
        temp_vec_axes[2] = m_n_bins_z;
        temp_entry->SetCoordinates(temp_vec,temp_vec_axes);
        temp_entry->SetID(m_ID);
        m_weights[i][j].push_back(temp_entry);
      }
    }
  }
}

APReweight3D::~APReweight3D() {
  delete m_denominator_hist;
  delete m_numerator_hist;
  delete m_empty_weight;
  delete m_axis_x;
  delete m_axis_y;
  delete m_axis_z;
  for (unsigned int i = 0; i < m_n_bins_x; ++i) {
    for (unsigned int j = 0; j < m_n_bins_y; ++j) {
      for (vector<APWeightEntry*>::reverse_iterator it = m_weights[i][j].rbegin(); it != m_weights[i][j].rend(); ++it) {
        delete *it;
      }
    }
  }
  m_weights.clear();
}

APWeightEntry* APReweight3D::GetBinWeight(unsigned int bin_x, unsigned int bin_y, unsigned int bin_z) const {
  if (bin_x == 0 || bin_y == 0 || bin_z == 0) return m_empty_weight;
  return m_weights[bin_x - 1][bin_y - 1][bin_z - 1];
}

APWeightEntry* APReweight3D::GetWeight(double value_x, double value_y, double value_z) const {
  return GetBinWeight(GetBinX(value_x), GetBinY(value_y), GetBinZ(value_z));
}

TH3D* APReweight3D::GetDenominatorHist() const {
  return m_denominator_hist;
}

TH3D* APReweight3D::GetNumeratorHist() const {
  return m_numerator_hist;
}

double APReweight3D::GetSampleScale() const {
  return m_scale;
}

unsigned int APReweight3D::NBins() const {
  return m_n_bins_x * m_n_bins_y*m_n_bins_z;
}

unsigned int APReweight3D::GetBinX(double value) const {
  for (unsigned int i = 1; i <= m_n_bins_x; ++i) {
    if (value >= m_axis_x->GetBinLowEdge(i) && value < m_axis_x->GetBinUpEdge(i)) {
      return i;
    }
  }
  if (!m_isQuiet) cout << "WARNING in APReweight3D::GetBinX: Value out of range! Returning 0." << endl;
  return 0;
}

unsigned int APReweight3D::GetBinY(double value) const {
  for (unsigned int i = 1; i <= m_n_bins_y; ++i) {
    if (value >= m_axis_y->GetBinLowEdge(i) && value < m_axis_y->GetBinUpEdge(i)) {
      return i;
    }
  }
  if (!m_isQuiet) cout << "WARNING in APReweight3D::GetBinY: Value out of range! Returning 0." << endl;
  return 0;
}

unsigned int APReweight3D::GetBinZ(double value) const {
  for (unsigned int i = 1; i <= m_n_bins_z; ++i) {
    if (value >= m_axis_z->GetBinLowEdge(i) && value < m_axis_z->GetBinUpEdge(i)) {
      return i;
    }
  }
  if (!m_isQuiet) cout << "WARNING in APReweight3D::GetBinZ: Value out of range! Returning 0." << endl;
  return 0;
}

void APReweight3D::SetSystUncert(double rel_uncert) {
  for (unsigned int i = 0; i < m_n_bins_x; ++i) {
    for (unsigned int j = 0; j < m_n_bins_y; ++j) {
      for (unsigned int k = 0; k < m_n_bins_z; ++k) {
        GetBinWeight(i, j, k)->SetSystUncert(rel_uncert);
      }
    }
  }
}

void APReweight3D::SetQuietMode(bool isQuiet) {
  m_isQuiet = isQuiet;
}
