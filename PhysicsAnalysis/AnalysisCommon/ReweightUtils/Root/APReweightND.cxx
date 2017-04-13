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
    m_n_dim(0),
    m_denominator_hist(0),
    m_numerator_hist(0)
{
  m_isQuiet = false;
}

APReweightND::APReweightND(THnSparse* denominator_in, THnSparse* numerator_in, bool isTrig) : APReweightBase() {
  m_empty_weight = new APWeightEntry(0,0,1.);
  m_denominator_hist = (THnSparse*)denominator_in->Clone("");
  m_numerator_hist = (THnSparse*)numerator_in->Clone("");
  m_n_dim = m_denominator_hist->GetNdimensions();
  for (unsigned int i = 0; i < m_n_dim; ++i) m_axes.push_back( (TAxis*)m_denominator_hist->GetAxis(i)->Clone("") );
  m_scale = (double)denominator_in->GetEntries() / (double)numerator_in->GetEntries();
  m_isTrig = isTrig;
  m_isQuiet = false;
  m_syst_uncert_global = 0.;
  
  if( m_isTrig ) {
    std::vector<int> coords(m_n_dim);
    for( unsigned int i = 0; i < m_n_dim; ++i ) coords[i] = 1;
    bool checkComplete = false;
    while( 1 ) {
      if( m_numerator_hist->GetBinContent(coords.data()) > m_denominator_hist->GetBinContent(coords.data()) ) {
        std::cout << "WARNING in APReweightND::~APReweightND(THnSparse* denominator_in, THnSparse* numerator_in, bool isTrig) : Using histograms " << m_numerator_hist->GetName() << " and " << m_denominator_hist->GetName() << " the efficiency is larger than 1 for coordinates [ "; for( unsigned int j = 0; j < m_n_dim; ++j ) { std::cout << coords[j] << " "; } std::cout << " ]! This is inconsisten and can lead to unwanted behaviour (weights > 1, variance < 0 )! Please check your input histograms! In order to avoid negative variances, the efficiency in this bin will be set to 0." << std::endl;
        m_numerator_hist->SetBinContent(coords.data(), 0);
      }

      checkComplete = true;
      for( unsigned int j = 0; j < m_n_dim; ++j ) {
        if( coords[j] != m_numerator_hist->GetAxis(j)->GetNbins() ) checkComplete = false;
      }
      if( checkComplete ) break;

      int completeDimension = -1;
      for( unsigned int j = 0; j < m_n_dim; ++j ) {
        if( coords[j] == m_numerator_hist->GetAxis(j)->GetNbins() ) {
          bool isComplete = true;
          for( int k = (int)j-1; k >= 0; --k ) {
            if( coords[k] != m_numerator_hist->GetAxis(k)->GetNbins() ) isComplete = false;
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
  m_empty_weight = new APWeightEntry(0,0,1.);
  m_denominator_hist = new THnSparseD();
  m_numerator_hist = new THnSparseD();
  m_n_dim = efficiency_in->GetNdimensions();
  for (unsigned int i = 0; i < m_n_dim; ++i) m_axes.push_back( (TAxis*)efficiency_in->GetAxis(i)->Clone("") );
  m_scale = 1.0;
  m_isTrig = true;
  m_syst_uncert_global = 0.;
  vector<int> temp_vec_axes(m_n_dim,0);
  for (unsigned int j = 0; j < m_n_dim; ++j) temp_vec_axes[j] = efficiency_in->GetAxis(j)->GetNbins();
  for (int i = 0, I = efficiency_in->GetNbins(); i < I; ++i) {
    std::vector<int> coords (m_n_dim);
    double efficiency = efficiency_in->GetBinContent( i,coords.data() );
    APWeightEntry *temp_entry = new APWeightEntry();
    temp_entry->ReadEfficiency(efficiency,err_low_in->GetBinContent(err_low_in->GetBin(coords.data())),err_high_in->GetBinContent(err_high_in->GetBin(coords.data())));
    temp_entry->SetCoordinates(coords,temp_vec_axes);
    temp_entry->SetID(m_ID);
    m_weights[coords] = temp_entry;
  }
}

APReweightND::~APReweightND() {
  delete m_denominator_hist;
  delete m_numerator_hist;
  delete m_empty_weight;
  for (vector<TAxis*>::reverse_iterator it=m_axes.rbegin(); it != m_axes.rend(); ++it) {
    delete *it;
  }
  for (map< vector<int>, APWeightEntry* >::reverse_iterator it=m_weights.rbegin(); it != m_weights.rend(); ++it) {
    delete it->second;
  }
  m_weights.clear();
  m_axes.clear();
}

APWeightEntry* APReweightND::GetBinWeight(const int bin[]) {
  vector<int> temp_vec(&bin[0],&bin[m_n_dim]);
  map< vector<int> , APWeightEntry* >::iterator temp_it = m_weights.find(temp_vec);
  if ( temp_it == m_weights.end() ) {
      APWeightEntry* temp_weight = new APWeightEntry((unsigned int)m_denominator_hist->GetBinContent(m_denominator_hist->GetBin(bin)),(unsigned int)m_numerator_hist->GetBinContent(m_numerator_hist->GetBin(bin)),m_scale, m_isTrig);
      vector<int> temp_vec_axes(m_n_dim,0);
      for (unsigned int i = 0; i < m_n_dim; ++i) temp_vec_axes[i] = m_denominator_hist->GetAxis(i)->GetNbins();
      temp_weight->SetCoordinates(temp_vec,temp_vec_axes);
      temp_weight->SetSystUncert(m_syst_uncert_global);
      temp_weight->SetID(m_ID);
      m_weights[temp_vec] = temp_weight;
      return temp_weight;
  }
  return (*temp_it).second;
}

APWeightEntry* APReweightND::GetWeight(double value[]) {
  vector<int> temp_bin;
  for (unsigned int i = 0; i < m_n_dim; ++i) {
    if ( value[i] < (m_axes[i])->GetXmin() || value[i] > (m_axes[i])->GetXmax() ) {
      if (!m_isQuiet) cout << "WARNING in APReweightND::GetBin: Value out of range! Dim: " << i << ", value: " << value[i] << ", return value: " << (m_axes[i])->FindFixBin(value[i]) << endl;
    }
    temp_bin.push_back((m_axes[i])->FindFixBin(value[i]));
  }
  return GetBinWeight(&temp_bin[0]);
}

THnSparse* APReweightND::GetDenominatorHist() const {
  return m_denominator_hist;
}

THnSparse* APReweightND::GetNumeratorHist() const {
  return m_numerator_hist;
}

double APReweightND::GetSampleScale() const {
  return m_scale;
}

unsigned int APReweightND::NBins() const {
  unsigned int ret = 0;
  for (unsigned int i = 0; i < m_n_dim; ++i) ret += (m_axes[i])->GetNbins();
  return ret;
}

void APReweightND::SetSystUncert(double rel_uncert) {
  m_syst_uncert_global = rel_uncert;
  for ( map< vector<int> , APWeightEntry* >::const_iterator temp_it = m_weights.begin(); temp_it != m_weights.end(); ++temp_it ) {
    ((*temp_it).second)->SetSystUncert(rel_uncert);
  }
}

void APReweightND::SetQuietMode(bool isQuiet) {
  m_isQuiet = isQuiet;
}
