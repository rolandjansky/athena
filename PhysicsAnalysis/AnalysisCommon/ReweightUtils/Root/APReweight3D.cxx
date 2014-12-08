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
  _denominator_hist(0),
  _numerator_hist(0),
  _n_bins_x(0),
  _n_bins_y(0),
  _n_bins_z(0),
  _axis_x(0),
  _axis_y(0),
  _axis_z(0)
{
  _isQuiet = false;
}

APReweight3D::APReweight3D(TH3* denominator_in, TH3* numerator_in, bool isTrig) : APReweightBase() {
  _empty_weight = new APWeightEntry(0, 0, 1.);
  _denominator_hist = (TH3D*) denominator_in->Clone("");
  _numerator_hist = (TH3D*) numerator_in->Clone("");
  _axis_x = (TAxis*) _denominator_hist->GetXaxis()->Clone("");
  _axis_y = (TAxis*) _denominator_hist->GetYaxis()->Clone("");
  _axis_z = (TAxis*) _denominator_hist->GetZaxis()->Clone("");
  _n_bins_x = _denominator_hist->GetNbinsX();
  _n_bins_y = _denominator_hist->GetNbinsY();
  _n_bins_z = _denominator_hist->GetNbinsZ();
  _scale = (double) denominator_in->GetEntries() / (double) numerator_in->GetEntries();
  _isTrig = isTrig;
  _isQuiet = false;
  for (unsigned int i = 0; i < _n_bins_x; ++i) {
    _weights.push_back(vector< vector<APWeightEntry*> >());
    for (unsigned int j = 0; j < _n_bins_y; ++j) {
      _weights[i].push_back(vector<APWeightEntry*>());
      for (unsigned int k = 0; k < _n_bins_z; ++k) {
        APWeightEntry* temp_entry = new APWeightEntry((unsigned int) _denominator_hist->GetBinContent(i + 1, j + 1, k + 1), (unsigned int) _numerator_hist->GetBinContent(i + 1, j + 1, k + 1), _scale, _isTrig);
        vector<int> temp_vec(3,0);
        vector<int> temp_vec_axes(3,0);
        temp_vec[0] = i+1;
        temp_vec[1] = j+1;
        temp_vec[2] = k+1;
        temp_vec_axes[0] = _n_bins_x;
        temp_vec_axes[1] = _n_bins_y;
        temp_vec_axes[2] = _n_bins_z;
        temp_entry->SetCoordinates(temp_vec,temp_vec_axes);
        temp_entry->SetID(_ID);
        _weights[i][j].push_back(temp_entry);
      }
    }
  }

  if( _isTrig ) {
    for( int i = 1; i < _numerator_hist->GetNbinsX()*_numerator_hist->GetNbinsY()*_numerator_hist->GetNbinsZ(); ++i ) {
      if( _numerator_hist->GetBinContent(i) > _denominator_hist->GetBinContent(i) ) {
        std::cout << "WARNING in APReweight3D::~APReweight3D(TH3* denominator_in, TH3* numerator_in, bool isTrig) : Using histograms " << _numerator_hist->GetName() << " and " << _denominator_hist->GetName() << " the efficiency is larger than 1 for bin " << i << "! This is inconsistent and can lead to unwanted behaviour (weights > 1, variance < 0 )! Please check your input histograms. In order to avoid negative variances, the efficiency in this bin will be set to 0." << std::endl;
        _numerator_hist->SetBinContent(i,0);
      }
    }
  }


}

void APReweight3D::ReadEfficiency(TH3* efficiency_in, TH3* err_low_in, TH3* err_high_in) {
  if (err_high_in == 0) err_high_in = err_low_in;
  _empty_weight = new APWeightEntry(0, 0, 1.);
  _denominator_hist = new TH3D("", "", 1, 0., 1., 1, 0., 1., 1, 0., 1.);
  _numerator_hist = new TH3D("", "", 1, 0., 1., 1, 0., 1., 1, 0., 1.);
  _axis_x = (TAxis*) efficiency_in->GetXaxis()->Clone("");
  _axis_y = (TAxis*) efficiency_in->GetYaxis()->Clone("");
  _axis_z = (TAxis*) efficiency_in->GetZaxis()->Clone("");
  _n_bins_x = efficiency_in->GetNbinsX();
  _n_bins_y = efficiency_in->GetNbinsY();
  _n_bins_z = efficiency_in->GetNbinsZ();
  _scale = 1.0;
  _isTrig = true;
  for (unsigned int i = 0; i < _n_bins_x; ++i) {
    _weights.push_back(vector< vector<APWeightEntry*> >());
    for (unsigned int j = 0; j < _n_bins_y; ++j) {
      _weights[i].push_back(vector<APWeightEntry*>());
      for (unsigned int k = 0; k < _n_bins_z; ++k) {
        APWeightEntry *temp_entry = new APWeightEntry();
        temp_entry->ReadEfficiency(efficiency_in->GetBinContent(i + 1, j + 1, k + 1), err_low_in->GetBinContent(i + 1, j + 1, k + 1), err_high_in->GetBinContent(i + 1, j + 1, k + 1));
        vector<int> temp_vec(3,0);
        vector<int> temp_vec_axes(3,0);
        temp_vec[0] = i+1;
        temp_vec[1] = j+1;
        temp_vec[2] = k+1;
        temp_vec_axes[0] = _n_bins_x;
        temp_vec_axes[1] = _n_bins_y;
        temp_vec_axes[2] = _n_bins_z;
        temp_entry->SetCoordinates(temp_vec,temp_vec_axes);
        temp_entry->SetID(_ID);
        _weights[i][j].push_back(temp_entry);
      }
    }
  }
}

APReweight3D::~APReweight3D() {
  delete _denominator_hist;
  delete _numerator_hist;
  delete _empty_weight;
  delete _axis_x;
  delete _axis_y;
  delete _axis_z;
  for (unsigned int i = 0; i < _n_bins_x; ++i) {
    for (unsigned int j = 0; j < _n_bins_y; ++j) {
      for (vector<APWeightEntry*>::reverse_iterator it = _weights[i][j].rbegin(); it != _weights[i][j].rend(); ++it) {
        delete *it;
      }
    }
  }
  _weights.clear();
}

APWeightEntry* APReweight3D::GetBinWeight(unsigned int bin_x, unsigned int bin_y, unsigned int bin_z) const {
  if (bin_x == 0 || bin_y == 0 || bin_z == 0) return _empty_weight;
  return _weights[bin_x - 1][bin_y - 1][bin_z - 1];
}

APWeightEntry* APReweight3D::GetWeight(double value_x, double value_y, double value_z) const {
  return GetBinWeight(GetBinX(value_x), GetBinY(value_y), GetBinZ(value_z));
}

TH3D* APReweight3D::GetDenominatorHist() const {
  return _denominator_hist;
}

TH3D* APReweight3D::GetNumeratorHist() const {
  return _numerator_hist;
}

double APReweight3D::GetSampleScale() const {
  return _scale;
}

unsigned int APReweight3D::NBins() const {
  return _n_bins_x * _n_bins_y*_n_bins_z;
}

unsigned int APReweight3D::GetBinX(double value) const {
  for (unsigned int i = 1; i <= _n_bins_x; ++i) {
    if (value >= _axis_x->GetBinLowEdge(i) && value < _axis_x->GetBinUpEdge(i)) {
      return i;
    }
  }
  if (!_isQuiet) cout << "WARNING in APReweight3D::GetBinX: Value out of range! Returning 0." << endl;
  return 0;
}

unsigned int APReweight3D::GetBinY(double value) const {
  for (unsigned int i = 1; i <= _n_bins_y; ++i) {
    if (value >= _axis_y->GetBinLowEdge(i) && value < _axis_y->GetBinUpEdge(i)) {
      return i;
    }
  }
  if (!_isQuiet) cout << "WARNING in APReweight3D::GetBinY: Value out of range! Returning 0." << endl;
  return 0;
}

unsigned int APReweight3D::GetBinZ(double value) const {
  for (unsigned int i = 1; i <= _n_bins_z; ++i) {
    if (value >= _axis_z->GetBinLowEdge(i) && value < _axis_z->GetBinUpEdge(i)) {
      return i;
    }
  }
  if (!_isQuiet) cout << "WARNING in APReweight3D::GetBinZ: Value out of range! Returning 0." << endl;
  return 0;
}

void APReweight3D::SetSystUncert(double rel_uncert) {
  for (unsigned int i = 0; i < _n_bins_x; ++i) {
    for (unsigned int j = 0; j < _n_bins_y; ++j) {
      for (unsigned int k = 0; k < _n_bins_z; ++k) {
        GetBinWeight(i, j, k)->SetSystUncert(rel_uncert);
      }
    }
  }
}

void APReweight3D::SetQuietMode(bool isQuiet) {
  _isQuiet = isQuiet;
}
