/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define APReweightND_cxx
#include "ReweightUtils/APReweightND.h"
#include "ReweightUtils/APWeightEntry.h"
#include "ReweightUtils/MathTools.h"
#include <iostream>
#include <cstdlib>
#include "TTree.h"
#include "THnSparse.h"
#include "TAxis.h"

using namespace std;

APReweightND::APReweightND()
  : APReweightBase(),
    _n_dim(0),
    _denominator_hist(0),
    _numerator_hist(0)
{
  _isQuiet = false;
}

APReweightND::APReweightND(THnSparse* denominator_in, THnSparse* numerator_in, bool isTrig) : APReweightBase() {
  _empty_weight = new APWeightEntry(0,0,1.);
  _denominator_hist = (THnSparse*)denominator_in->Clone("");
  _numerator_hist = (THnSparse*)numerator_in->Clone("");
  _n_dim = _denominator_hist->GetNdimensions();
  for (unsigned int i = 0; i < _n_dim; ++i) _axes.push_back( (TAxis*)_denominator_hist->GetAxis(i)->Clone("") );
  _scale = (double)denominator_in->GetEntries() / (double)numerator_in->GetEntries();
  _isTrig = isTrig;
  _isQuiet = false;
  _syst_uncert_global = 0.;
  
  if( _isTrig ) {
    std::vector<int> coords(_n_dim);
    for( unsigned int i = 0; i < _n_dim; ++i ) coords[i] = 1;
    bool checkComplete = false;
    while( 1 ) {
      if( _numerator_hist->GetBinContent(coords.data()) > _denominator_hist->GetBinContent(coords.data()) ) {
        std::cout << "WARNING in APReweightND::~APReweightND(THnSparse* denominator_in, THnSparse* numerator_in, bool isTrig) : Using histograms " << _numerator_hist->GetName() << " and " << _denominator_hist->GetName() << " the efficiency is larger than 1 for coordinates [ "; for( unsigned int j = 0; j < _n_dim; ++j ) { std::cout << coords[j] << " "; } std::cout << " ]! This is inconsisten and can lead to unwanted behaviour (weights > 1, variance < 0 )! Please check your input histograms! In order to avoid negative variances, the efficiency in this bin will be set to 0." << std::endl;
        _numerator_hist->SetBinContent(coords.data(), 0);
      }

      checkComplete = true;
      for( unsigned int j = 0; j < _n_dim; ++j ) {
        if( coords[j] != _numerator_hist->GetAxis(j)->GetNbins() ) checkComplete = false;
      }
      if( checkComplete ) break;

      int completeDimension = -1;
      for( unsigned int j = 0; j < _n_dim; ++j ) {
        if( coords[j] == _numerator_hist->GetAxis(j)->GetNbins() ) {
          bool isComplete = true;
          for( int k = (int)j-1; k >= 0; --k ) {
            if( coords[k] != _numerator_hist->GetAxis(k)->GetNbins() ) isComplete = false;
          }
          if( isComplete ) completeDimension = j;
        }
      }
      
      if( completeDimension != -1 ) {
        for( int j = 0; j <= completeDimension; ++j ) coords[j] = 1;
        coords[completeDimension+1] += 1;
      }
      else {
        coords[0] += 1;
      }
      
    }
  }

}

void APReweightND::ReadEfficiency(THnSparse* efficiency_in, THnSparse* err_low_in, THnSparse* err_high_in) {
  if (err_high_in == 0) err_high_in = err_low_in;
  _empty_weight = new APWeightEntry(0,0,1.);
  _denominator_hist = new THnSparseD();
  _numerator_hist = new THnSparseD();
  _n_dim = efficiency_in->GetNdimensions();
  for (unsigned int i = 0; i < _n_dim; ++i) _axes.push_back( (TAxis*)efficiency_in->GetAxis(i)->Clone("") );
  _scale = 1.0;
  _isTrig = true;
  _syst_uncert_global = 0.;
  vector<int> temp_vec_axes(_n_dim,0);
  for (unsigned int j = 0; j < _n_dim; ++j) temp_vec_axes[j] = efficiency_in->GetAxis(j)->GetNbins();
  for (int i = 0, I = efficiency_in->GetNbins(); i < I; ++i) {
    std::vector<int> coords (_n_dim);
    double efficiency = efficiency_in->GetBinContent( i,coords.data() );
    APWeightEntry *temp_entry = new APWeightEntry();
    temp_entry->ReadEfficiency(efficiency,err_low_in->GetBinContent(err_low_in->GetBin(coords.data())),err_high_in->GetBinContent(err_high_in->GetBin(coords.data())));
    temp_entry->SetCoordinates(coords,temp_vec_axes);
    temp_entry->SetID(_ID);
    _weights[coords] = temp_entry;
  }
}

APReweightND::~APReweightND() {
  delete _denominator_hist;
  delete _numerator_hist;
  delete _empty_weight;
  for (vector<TAxis*>::reverse_iterator it=_axes.rbegin(); it != _axes.rend(); ++it) {
    delete *it;
  }
  for (map< vector<int>, APWeightEntry* >::reverse_iterator it=_weights.rbegin(); it != _weights.rend(); ++it) {
    delete it->second;
  }
  _weights.clear();
  _axes.clear();
}

APWeightEntry* APReweightND::GetBinWeight(const int bin[]) {
  vector<int> temp_vec(&bin[0],&bin[_n_dim]);
  map< vector<int> , APWeightEntry* >::iterator temp_it = _weights.find(temp_vec);
  if ( temp_it == _weights.end() ) {
      APWeightEntry* temp_weight = new APWeightEntry((unsigned int)_denominator_hist->GetBinContent(_denominator_hist->GetBin(bin)),(unsigned int)_numerator_hist->GetBinContent(_numerator_hist->GetBin(bin)),_scale, _isTrig);
      vector<int> temp_vec_axes(_n_dim,0);
      for (unsigned int i = 0; i < _n_dim; ++i) temp_vec_axes[i] = _denominator_hist->GetAxis(i)->GetNbins();
      temp_weight->SetCoordinates(temp_vec,temp_vec_axes);
      temp_weight->SetSystUncert(_syst_uncert_global);
      temp_weight->SetID(_ID);
      _weights[temp_vec] = temp_weight;
      return temp_weight;
  }
  return (*temp_it).second;
}

APWeightEntry* APReweightND::GetWeight(double value[]) {
  vector<int> temp_bin;
  for (unsigned int i = 0; i < _n_dim; ++i) {
    if ( value[i] < (_axes[i])->GetXmin() || value[i] > (_axes[i])->GetXmax() ) {
      if (!_isQuiet) cout << "WARNING in APReweightND::GetBin: Value out of range! Dim: " << i << ", value: " << value[i] << ", return value: " << (_axes[i])->FindFixBin(value[i]) << endl;
    }
    temp_bin.push_back((_axes[i])->FindFixBin(value[i]));
  }
  return GetBinWeight(&temp_bin[0]);
}

THnSparse* APReweightND::GetDenominatorHist() const {
  return _denominator_hist;
}

THnSparse* APReweightND::GetNumeratorHist() const {
  return _numerator_hist;
}

double APReweightND::GetSampleScale() const {
  return _scale;
}

unsigned int APReweightND::NBins() const {
  unsigned int ret = 0;
  for (unsigned int i = 0; i < _n_dim; ++i) ret += (_axes[i])->GetNbins();
  return ret;
}

void APReweightND::SetSystUncert(double rel_uncert) {
  _syst_uncert_global = rel_uncert;
  for ( map< vector<int> , APWeightEntry* >::const_iterator temp_it = _weights.begin(); temp_it != _weights.end(); ++temp_it ) {
    ((*temp_it).second)->SetSystUncert(rel_uncert);
  }
}

void APReweightND::SetQuietMode(bool isQuiet) {
  _isQuiet = isQuiet;
}
