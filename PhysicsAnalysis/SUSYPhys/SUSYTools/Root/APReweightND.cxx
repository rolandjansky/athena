/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define APReweightND_cxx
#include "SUSYTools/APReweightND.h"
#include "SUSYTools/APWeightEntry.h"
//#include "MathTools.h"
#include <iostream>
#include <cstdlib>
#include "TTree.h"
#include "THnSparse.h"
#include "TAxis.h"

#ifndef SUSY_ATHENA
ClassImp(APReweightND)
#endif

using namespace std;

APReweightND::APReweightND() {
  _isQuiet = false;
  _empty_weight = 0;
  _denominator_hist = 0;
  _numerator_hist = 0;
  _axes.clear();
  _weights.clear();
}

APReweightND::APReweightND(THnSparse* denominator_in, THnSparse* numerator_in, bool isTrig) {  /*!< Constructor which takes the object to be reweighted, the object container to perform the reweighting on and the corresponding efficiency map. The reweighting will be performed in all dimensions contained in the EfficiencyMap. */
  //Prepare simple histograms to store values for current binning
  _empty_weight = new APWeightEntry(0,0,1.);
  _denominator_hist = (THnSparse*)denominator_in->Clone("");
  _numerator_hist = (THnSparse*)numerator_in->Clone("");
  _n_dim = _denominator_hist->GetNdimensions();
  for (unsigned int i = 0; i < _n_dim; ++i) _axes.push_back( (TAxis*)_denominator_hist->GetAxis(i)->Clone("") );
  _scale = (double)denominator_in->GetEntries() / (double)numerator_in->GetEntries();
  _isTrig = isTrig;
  _isQuiet = false;
  _syst_uncert_global = 0.;
}

void APReweightND::ReadEfficiency(THnSparse* efficiency_in, THnSparse* err_low_in, THnSparse* err_high_in) {
  _empty_weight = new APWeightEntry(0,0,1.);
  _denominator_hist = new THnSparseD();
  _numerator_hist = new THnSparseD();
  _n_dim = efficiency_in->GetNdimensions();
  for (unsigned int i = 0; i < _n_dim; ++i) _axes.push_back( (TAxis*)efficiency_in->GetAxis(i)->Clone("") );
  _scale = 1.0;
  _isTrig = true;
  _syst_uncert_global = 0.;
  for (int i = 0, I = efficiency_in->GetNbins(); i < I; ++i) {
    int *coords = new int[_n_dim];
    //int *coords  = (int*) malloc(_n_dim * sizeof(int));
    double efficiency = efficiency_in->GetBinContent( i,coords );
    vector<int> temp_vec(&coords[0],&coords[_n_dim]);
    APWeightEntry *temp_entry = new APWeightEntry();
    temp_entry->ReadEfficiency(efficiency,err_low_in->GetBinContent(err_low_in->GetBin(coords)),err_high_in->GetBinContent(err_high_in->GetBin(coords)));
    _weights[temp_vec] = temp_entry;
  }
}

APReweightND::~APReweightND() {
  delete _denominator_hist;
  delete _numerator_hist;
  delete _empty_weight;
  for (vector<TAxis*>::reverse_iterator it=_axes.rbegin(); it != _axes.rend(); ++it) {
    delete *it;
  }
  _weights.clear();
  _axes.clear();
}

APWeightEntry* APReweightND::GetBinWeight(const int bin[]) {
  vector<int> temp_vec(&bin[0],&bin[_n_dim]);
  map< std::vector<int> , APWeightEntry* >::iterator temp_it = _weights.find(temp_vec);
  if ( temp_it == _weights.end() ) {
      APWeightEntry* temp_weight = new APWeightEntry((unsigned int)_denominator_hist->GetBinContent(_denominator_hist->GetBin(bin)),(unsigned int)_numerator_hist->GetBinContent(_numerator_hist->GetBin(bin)),_scale, _isTrig);
      temp_weight->SetSystUncert(_syst_uncert_global);
      _weights[temp_vec] = temp_weight;
      return temp_weight;
  }
  return (*temp_it).second;
}

APWeightEntry* APReweightND::GetWeight(double value[]) {
  int *temp_bin = new int[_n_dim];
  for (unsigned int i = 0; i < _n_dim; ++i) {
    if ( value[i] < (_axes[i])->GetXmin() || value[i] > (_axes[i])->GetXmax() ) {
      if (!_isQuiet) cout << "WARNING in APReweightND::GetBin: Value out of range! Dim: " << i << ", value: " << value[i] << ", return value: " << (_axes[i])->FindFixBin(value[i]) << endl;
    }
    temp_bin[i] = (_axes[i])->FindFixBin(value[i]);
  }
  return GetBinWeight(temp_bin);
}

THnSparse* APReweightND::GetDenominatorHist() {
  return _denominator_hist;
}

THnSparse* APReweightND::GetNumeratorHist() {
  return _numerator_hist;
}

double APReweightND::GetSampleScale() {
  return _scale;
}

unsigned int APReweightND::NBins() {
  unsigned int ret = 0;
  for (unsigned int i = 0; i < _n_dim; ++i) ret += (_axes[i])->GetNbins();
  return ret;
}

void APReweightND::SetSystUncert(double rel_uncert) {
  _syst_uncert_global = rel_uncert;
  for ( std::map< std::vector<int> , APWeightEntry* >::const_iterator temp_it = _weights.begin(); temp_it != _weights.end(); ++temp_it ) {
    ((*temp_it).second)->SetSystUncert(rel_uncert);
  }
}

void APReweightND::SetQuietMode(bool isQuiet) {
  _isQuiet = isQuiet;
}
