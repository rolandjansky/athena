/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define APWeightSum_cxx
#include "ReweightUtils/APWeightSum.h"
#include "ReweightUtils/APWeightEntry.h"
#include "ReweightUtils/APEvtWeight.h"
#include "ReweightUtils/APReweightBase.h"
#include "THnSparse.h"
#include <cmath>
#include <iostream>
#include <limits>

using namespace std;

APWeightSum::APWeightSum() {
  m_k_evt_orig = 0;
  m_k_evt_weight = 0.;
  m_k_evt_weight2 = 0.;
  m_k_evt_weight_external = 0.;
  m_variance = 0.;
  m_variance_nocorr = 0.;
  m_variance_fullcorr = 0.;
  m_variance_sys = 0.;

  m_isComputed = false;
}

APWeightSum::~APWeightSum() {
  for (vector<THnSparse*>::reverse_iterator it = m_linear_uncert.rbegin(); it != m_linear_uncert.rend(); ++it) delete *it;
  m_linear_uncert.clear();
}

void APWeightSum::AddWeightToEvt(APWeightEntry* weight) {
  m_current_evt_weights.push_back(weight);
}

double APWeightSum::GetSumW() const {
  return m_k_evt_weight;
}

double APWeightSum::GetSumW2() const {
  return m_k_evt_weight2;
}

double APWeightSum::GetSumWExternal() const {
  return m_k_evt_weight_external;
}

double APWeightSum::GetStdDev() {
  if ( !m_isComputed ) Compute();
  return sqrt(m_variance);
}

double APWeightSum::GetVariance() {
  if ( !m_isComputed ) Compute();
  return m_variance;
}

double APWeightSum::GetVarianceNoCorr() {
  return m_variance_nocorr;
}

double APWeightSum::GetVarianceFullCorr() {
  if ( !m_isComputed ) Compute();
  return m_variance_fullcorr;
}

double APWeightSum::GetSysUncert() const {
  return sqrt(m_variance_sys);
}

unsigned long APWeightSum::GetKUnweighted() const {
  return m_k_evt_orig;
}

void APWeightSum::FinishEvt(double ext_weight) {
  ++m_k_evt_orig;
  double evt_weight = 1.0;
  double uncert = 0.0;
  double uncert_sys = 0.0;
  for (unsigned int i = 0, I = m_current_evt_weights.size(); i < I; ++i) {
    double uncert_summand = 1.0;
    evt_weight *= (1. - m_current_evt_weights[i]->GetExpectancy());
    for (unsigned int k = 0; k < I; ++k) {
      if (i != k) uncert_summand *= (1. - m_current_evt_weights[i]->GetExpectancy());
    }
    uncert += (uncert_summand * uncert_summand * m_current_evt_weights[i]->GetVariance());
    uncert_sys += (uncert_summand * uncert_summand * m_current_evt_weights[i]->GetSysUncert2());
  }
  m_k_evt_weight += ext_weight * (1. - evt_weight);
  m_k_evt_weight2 += m_k_evt_weight*m_k_evt_weight;
  m_k_evt_weight_external += ext_weight;
  m_variance += fabs(ext_weight) * uncert;
  m_variance_sys += ext_weight * uncert_sys;
  m_current_evt_weights.clear();
}

void APWeightSum::AddEvt(APEvtWeight* evt_weight, double ext_weight) {
  ++m_k_evt_orig;
  m_k_evt_weight += ext_weight * evt_weight->GetWeight();
  m_k_evt_weight2 += m_k_evt_weight*m_k_evt_weight;
  m_k_evt_weight_external += ext_weight;

  vector<APWeightEntry*> temp_vec_mu = evt_weight->GetWeightObjects(APEvtWeight::kMuon);
  vector<APWeightEntry*> temp_vec_dimu = evt_weight->GetWeightObjects(APEvtWeight::kDiMuon);
  vector<APWeightEntry*> temp_vec_mumo = evt_weight->GetWeightObjects(APEvtWeight::kMuonMO);
  vector<APWeightEntry*> temp_vec_tau = evt_weight->GetWeightObjects(APEvtWeight::kTau);
  vector<APWeightEntry*> temp_vec_ditau = evt_weight->GetWeightObjects(APEvtWeight::kDiTau);
  vector<APWeightEntry*> temp_vec_taumo = evt_weight->GetWeightObjects(APEvtWeight::kTauMO);
  vector<APWeightEntry*> temp_vec_el = evt_weight->GetWeightObjects(APEvtWeight::kElectron);
  vector<APWeightEntry*> temp_vec_diel = evt_weight->GetWeightObjects(APEvtWeight::kDiElectron);
  vector<APWeightEntry*> temp_vec_elmo = evt_weight->GetWeightObjects(APEvtWeight::kElectronMO);
  vector<APWeightEntry*> temp_vec_jet = evt_weight->GetWeightObjects(APEvtWeight::kJet);
  vector<APWeightEntry*> temp_vec_dijet = evt_weight->GetWeightObjects(APEvtWeight::kDiJet);
  vector<APWeightEntry*> temp_vec_jetmo = evt_weight->GetWeightObjects(APEvtWeight::kJetMO);
  
  vector< vector<APWeightEntry*> > temp_vec_all;
  
  temp_vec_all.push_back(temp_vec_mu);
  temp_vec_all.push_back(temp_vec_tau);
  temp_vec_all.push_back(temp_vec_el);
  temp_vec_all.push_back(temp_vec_jet);
  temp_vec_all.push_back(temp_vec_mumo);
  temp_vec_all.push_back(temp_vec_taumo);
  temp_vec_all.push_back(temp_vec_elmo);
  temp_vec_all.push_back(temp_vec_jetmo);
  temp_vec_all.push_back(temp_vec_dimu);
  temp_vec_all.push_back(temp_vec_ditau);
  temp_vec_all.push_back(temp_vec_diel);
  temp_vec_all.push_back(temp_vec_dijet);

  /* check if histogram for error propagation is already there; if not, create it */
  for( unsigned int iAll = 0, IAll = temp_vec_all.size(); iAll < IAll; ++iAll ) {
    for( unsigned int i = 0, I = temp_vec_all[iAll].size(); i < I; ++i ) {
      unsigned int ID = temp_vec_all[iAll][i]->GetID();
      if( m_linear_uncert.size() < ID+1 ) m_linear_uncert.resize(ID+1, 0);
      if( m_linear_uncert[ID] == 0 ) {
        vector<int> original_dimensions = temp_vec_all[iAll][i]->GetOriginalDimensions();
        int *bins = new int[original_dimensions.size()];
        double *xmin = new double[original_dimensions.size()];
        double *xmax = new double[original_dimensions.size()];
        for( unsigned int j = 0, J = original_dimensions.size(); j < J; ++j ) {
          bins[j] = original_dimensions[j];
          xmin[j] = 0.;
          xmax[j] = 10.;
        }
        m_linear_uncert[ID] = new THnSparseD("","",original_dimensions.size(), bins, xmin, xmax);
      }
    }
  }
  /* calculate weight and derivatives */
  /* these are the simple weights: kMuon, kTau, kElectron and kJet: single object trigger, OR of all elements */
  if(evt_weight->GetType() <= APEvtWeight::kJet) {
    vector<APWeightEntry*> temp_vec_rel;
    if(evt_weight->GetType() == APEvtWeight::kMuon) temp_vec_rel = temp_vec_mu;
    else if(evt_weight->GetType() == APEvtWeight::kTau) temp_vec_rel = temp_vec_tau;
    else if(evt_weight->GetType() == APEvtWeight::kElectron) temp_vec_rel = temp_vec_el;
    else if(evt_weight->GetType() == APEvtWeight::kJet) temp_vec_rel = temp_vec_jet;
    
    for (unsigned int i = 0, I = temp_vec_rel.size(); i < I; ++i ) {
      vector<int> coord = temp_vec_rel[i]->GetCoords();
      double weight_uncert = sqrt(temp_vec_rel[i]->GetVariance());
      for (unsigned int j = 0; j < I; ++j ) {
        if (j == i) continue;
        weight_uncert *= (1.0 - temp_vec_rel[j]->GetExpectancy());
      }
      m_linear_uncert[temp_vec_rel[i]->GetID()]->SetBinContent(&coord.front(), m_linear_uncert[temp_vec_rel[i]->GetID()]->GetBinContent(&coord.front())+weight_uncert);
      m_variance_nocorr += weight_uncert*weight_uncert;
    }
  }
  
  /* these are the DiMuon, DiTau, DiElectron and DiJet weights */
  else if (evt_weight->GetType() >= APEvtWeight::kDiMuon && evt_weight->GetType() <= APEvtWeight::kDiJet) {
    vector<APWeightEntry*> temp_vec_rel;
    if(evt_weight->GetType() == APEvtWeight::kDiMuon) temp_vec_rel = temp_vec_dimu;
    else if(evt_weight->GetType() == APEvtWeight::kDiTau) temp_vec_rel = temp_vec_ditau;
    else if(evt_weight->GetType() == APEvtWeight::kDiElectron) temp_vec_rel = temp_vec_diel;
    else if(evt_weight->GetType() == APEvtWeight::kDiJet) temp_vec_rel = temp_vec_dijet;
    
    bool isAsymTrig = false;
    vector<unsigned int> temp_vec_IDs;
    temp_vec_IDs.push_back(temp_vec_rel[0]->GetID() );
    for( unsigned int i = 1, I = temp_vec_rel.size(); i < I; ++i ) {
      bool knownID = false;
      for( unsigned int j = 0, J = temp_vec_IDs.size(); j < J; ++j ) {
        if( temp_vec_rel[i]->GetID() == temp_vec_IDs[j] ) { knownID = true; break; }
      }
      if( !knownID ) temp_vec_IDs.push_back( temp_vec_rel[i]->GetID() );
    }
    if( temp_vec_IDs.size() != 1 ) isAsymTrig = true;

    /* this is for symmetric dilepton triggers */
    if( !isAsymTrig ) {
      for (unsigned int i = 0, I = temp_vec_rel.size(); i < I; ++i ) {
        vector<int> coord = temp_vec_rel[i]->GetCoords();
        double weight_uncert = sqrt(temp_vec_rel[i]->GetVariance());
        double weight_derivative = 0.;
        for (unsigned int j = 0; j < I; ++j ) {
          if (j == i) continue;
          double weight_derivative_temp = temp_vec_rel[j]->GetExpectancy();
          for (unsigned int k = 0; k < I; ++k ) {
            if( k == j || k == i ) continue;
            weight_derivative_temp *= (1.0 - temp_vec_rel[k]->GetExpectancy());
          }
          weight_derivative += weight_derivative_temp;
        }
        weight_uncert *= weight_derivative;
        m_linear_uncert[temp_vec_rel[i]->GetID()]->SetBinContent(&coord.front(), m_linear_uncert[temp_vec_rel[i]->GetID()]->GetBinContent(&coord.front())+weight_uncert);
        m_variance_nocorr += weight_uncert*weight_uncert;
      }
    }
    
    /* this is for asymmetric triggers */
    else {
      /* at first the first leg of the trigger */ 
      for( unsigned int k = 0, K = temp_vec_rel.size(); k < K; k += 4 ) {
        vector<int> coord = temp_vec_rel[k]->GetCoords();
        double variance_k = 0.;
        double variance_temp = 1.;
        for( unsigned int i = 0, I = temp_vec_rel.size(); i < I; i += 4 ) {
          if( i == k ) continue;
          variance_temp *= (1. - temp_vec_rel[i]->GetExpectancy());
        }
        variance_k += variance_temp;
        variance_temp = -1.;
        for( unsigned int i = 0, I = temp_vec_rel.size(); i < I; i += 4 ) {
          variance_temp *= (1.0 - temp_vec_rel[i+2]->GetExpectancy());
          if( i == k ) continue;
          variance_temp *= (1.0 - temp_vec_rel[i]->GetExpectancy());
        }
        variance_k += variance_temp;
        variance_temp = -temp_vec_rel[k+3]->GetExpectancy();
        for( unsigned int i = 0, I = temp_vec_rel.size(); i < I; i += 4 ) {
          if( i == k ) continue;
          variance_temp *= (1.0 - temp_vec_rel[i]->GetExpectancy())*(1.0 - temp_vec_rel[i+2]->GetExpectancy());
        }
        variance_k += variance_temp;
        variance_temp = 0.;
        for( unsigned int j = 0, J = temp_vec_rel.size(); j < J; j+= 4 ) {
          if( j == k ) continue;
          double variance_ijk_temp = temp_vec_rel[j]->GetExpectancy()*temp_vec_rel[j+3]->GetExpectancy();
          for( unsigned int i = 0, I = temp_vec_rel.size(); i < I; i += 4 ) {
            if( i == j ) continue;
            variance_ijk_temp *= (1.0 -  temp_vec_rel[i+2]->GetExpectancy());
            if( i == k ) continue;
            variance_ijk_temp *= (1.0 - temp_vec_rel[i]->GetExpectancy());
          }
          variance_temp += variance_ijk_temp;
        }
        variance_k += variance_temp;
        variance_k *= variance_k*temp_vec_rel[k]->GetVariance();
        m_linear_uncert[temp_vec_rel[k]->GetID()]->SetBinContent(&coord.front(), m_linear_uncert[temp_vec_rel[k]->GetID()]->GetBinContent(&coord.front())+sqrt(variance_k));
        m_variance_nocorr += variance_k;
      }
      
      /* second leg */
      for( unsigned int k = 0, K = temp_vec_rel.size(); k < K; k += 4 ) {
        vector<int> coord = temp_vec_rel[k+1]->GetCoords();
        double variance_temp = 1.;
        for( unsigned int i = 0, I = temp_vec_rel.size(); i < I; i += 4 ) {
        if( i == k ) continue;
          variance_temp *= (1. - temp_vec_rel[i+1]->GetExpectancy());
        }
        variance_temp *= variance_temp*temp_vec_rel[k+1]->GetVariance();
        m_linear_uncert[temp_vec_rel[k+1]->GetID()]->SetBinContent(&coord.front(), m_linear_uncert[temp_vec_rel[k+1]->GetID()]->GetBinContent(&coord.front())+sqrt(variance_temp));
        m_variance_nocorr += variance_temp;
      }
      
      /* second leg | condition leg 1 failed */
      for( unsigned int k = 0, K = temp_vec_rel.size(); k < K; k += 4 ) {
        vector<int> coord = temp_vec_rel[k+2]->GetCoords();
        double variance_k = 0.;
        double variance_temp = 1.;
        for( unsigned int i = 0, I = temp_vec_rel.size(); i < I; i += 4 ) {
          variance_temp *= (1. - temp_vec_rel[i]->GetExpectancy());
          if( i == k ) continue;
          variance_temp *= (1. - temp_vec_rel[i+2]->GetExpectancy());
        }
        variance_k += variance_temp;
        variance_temp = 0.;
        for( unsigned int j = 0, J = temp_vec_rel.size(); j < J; j+= 4 ) {
          double variance_ijk_temp = temp_vec_rel[j]->GetExpectancy()*temp_vec_rel[j+3]->GetExpectancy();
          for( unsigned int i = 0, I = temp_vec_rel.size(); i < I; i += 4 ) {
            if( j == i ) continue;
            variance_ijk_temp *= (1.0 - temp_vec_rel[i]->GetExpectancy());
            if( i == k ) continue;
            variance_ijk_temp *= (1.0 -  temp_vec_rel[i+2]->GetExpectancy());
          }
          variance_temp += variance_ijk_temp;
        }
        variance_k += variance_temp;
        variance_k *= variance_k*temp_vec_rel[k+2]->GetVariance();
        m_linear_uncert[temp_vec_rel[k+2]->GetID()]->SetBinContent(&coord.front(), m_linear_uncert[temp_vec_rel[k+2]->GetID()]->GetBinContent(&coord.front())+sqrt(variance_k));
        m_variance_nocorr += variance_k;
      }
      
      /* second leg | condition leg 1 passed */
      for( unsigned int k = 0, K = temp_vec_rel.size(); k < K; k += 4 ) {
        vector<int> coord = temp_vec_rel[k+3]->GetCoords();
        double variance_k = - temp_vec_rel[k]->GetExpectancy();
        for( unsigned int i = 0, I = temp_vec_rel.size(); i < I; i += 4 ) {
          if( i == k ) continue;
          variance_k *= (1.0 - temp_vec_rel[i]->GetExpectancy())*(1.0 - temp_vec_rel[i+3]->GetExpectancy());
        }
        variance_k *= variance_k*temp_vec_rel[k+3]->GetVariance();
        m_linear_uncert[temp_vec_rel[k+3]->GetID()]->SetBinContent(&coord.front(), m_linear_uncert[temp_vec_rel[k+3]->GetID()]->GetBinContent(&coord.front())+sqrt(variance_k));
        m_variance_nocorr += variance_k;
      }
    } 
    
  }
   
  /* these are the weights for ANDed triggers. Shouldn't be used but for examples. This will treat everything as ANDed. */
  else if (evt_weight->GetType() == APEvtWeight::kANDed ) {
    for (unsigned int iAll = 0, IAll = temp_vec_all.size(); iAll < IAll; ++iAll ) {
      for (unsigned int i = 0, I = temp_vec_all[iAll].size(); i < I; ++i ) {
        vector<int> coord = temp_vec_all[iAll][i]->GetCoords();
        double weight_uncert = sqrt(temp_vec_all[iAll][i]->GetVariance());
        weight_uncert *= (temp_vec_all[iAll][i]->GetExpectancy() <= numeric_limits<double>::epsilon() ) ? 0. : evt_weight->GetWeight()/temp_vec_all[iAll][i]->GetExpectancy();
        m_linear_uncert[temp_vec_all[iAll][i]->GetID()]->SetBinContent(&coord.front(), m_linear_uncert[temp_vec_all[iAll][i]->GetID()]->GetBinContent(&coord.front())+weight_uncert); 
        m_variance_nocorr += weight_uncert*weight_uncert;
      }
    }
  }

  /* these are weights for ORed triggers, for instance dimuon OR single muon */
  /* treat everything as ORed: calculate weight for each type, then combine weights */
  /* objects of same type are ORed: kMuon, kTau, kElectron, kJet, kMuonMO, kTauMO, kElectronMO, kJetMo */
  /* objects of different type are handled differently */
  /* if type is kMOANDed, everything is ANDed.*/
  /* if type is kMOORed, kMuonMO, kTauMO, kElectronMO, kJetMO are ANDed; all others are ORed with each other! */
  else if (evt_weight->GetType() >= APEvtWeight::kORed ) {
    
    vector<double> temp_weight_rel(12,1.);
    
    /* calculate single object trigger weights: OR of all objects of same type: kMuon, kTau, kElectron, kJet, kMuonMO, kTauMO, kElectronMO, kJetMO */
    for (unsigned int j = 0; j < 8; ++j) {
      for (unsigned int i = 0, I = temp_vec_all[j].size(); i < I; ++i ) temp_weight_rel[j] *= (1.0 - temp_vec_all[j][i]->GetExpectancy());
      // for triggers intended to be used in ORing, set weight to 1. if there is no object and an AND is require: (if no object is added, assume none is required)
      if( j < 4 ) temp_weight_rel[j] = (temp_vec_all[j].size() > 0 || evt_weight->GetType() == APEvtWeight::kORed || evt_weight->GetType() == APEvtWeight::kMOORed) ? (1.0 - temp_weight_rel[j]) : 1.;
      // for triggers inteded to be used in ANDing, set weight to 1. if there is no object added (if none is added assume none is required)
      else temp_weight_rel[j] = (temp_vec_all[j].size() > 0) ? (1.0 - temp_weight_rel[j]) : 1.;
    }

    
    /* calculate diobject trigger weights: OR of all objects of same type but require at least 2: kDiMuon, kDiTau, kDiElectron, kDiJet */
    for (unsigned int j = 8; j < 12; ++j) {
      if( temp_vec_all[j].size() >= 2 ) {
        for (unsigned int i = 0, I = temp_vec_all[j].size(); i < I; ++i) temp_weight_rel[j] *= (1.0 - temp_vec_all[j][i]->GetExpectancy());
        for (unsigned int i = 0, I = temp_vec_all[j].size(); i < I; ++i) {
          double temp_weight = temp_vec_all[j][i]->GetExpectancy();
          for (unsigned int k = 0; k < I; ++k ) {
            if( k == i ) continue;
            temp_weight *= (1.0 - temp_vec_all[j][k]->GetExpectancy());
          }
          temp_weight_rel[j] += temp_weight;
        }
      }
      temp_weight_rel[j] = (temp_vec_all[j].size() > 0 || evt_weight->GetType() == APEvtWeight::kORed || evt_weight->GetType() == APEvtWeight::kMOORed) ? (1.0 - temp_weight_rel[j]) : 1.;
    }
   
    /* calculat weight for multiobject trigger (AND of all MO objects) */
    double temp_weight_MO = 1.;
    int n_noObject_MO = 0;
    for (unsigned int l = 4; l < 8; ++l ) {
      temp_weight_MO *= temp_weight_rel[l];
      if( temp_vec_all[l].size() == 0 ) n_noObject_MO += 1;
    }
    if( /*(evt_weight->GetType() == APEvtWeight::kORed || evt_weight->GetType() == APEvtWeight::kMOORed) &&*/ n_noObject_MO >= 2 ) temp_weight_MO = 0.; 

    /* all partial weights calculated; now calculate uncertainties */
    /* single object triggers */
    for (unsigned int j = 0; j < 8; ++j) {
      for (unsigned int i = 0, I = temp_vec_all[j].size(); i < I; ++i ) {
        vector<int> coord = temp_vec_all[j][i]->GetCoords();
        double weight_uncert = sqrt(temp_vec_all[j][i]->GetVariance());
        for (unsigned int k = 0; k < I; ++k ) {
          if( k == i ) continue;
          weight_uncert *= (1.0 - temp_vec_all[j][k]->GetExpectancy());
        }
        for( unsigned int l = 0; l < 4; ++l ) {
          if( l == j ) continue;
          if( evt_weight->GetType() == APEvtWeight::kMOANDed ) weight_uncert *= temp_weight_rel[l];
          else if( evt_weight->GetType() == APEvtWeight::kORed || evt_weight->GetType() == APEvtWeight::kMOORed ) weight_uncert *= (1.0 - temp_weight_rel[l]); 
          else cout << "WARNING: handling for this weight type is unknown! uncertainties will be incorrect" << endl;
        }
        for( unsigned int l = 8; l < 12; ++l ) {
          if( l == j ) continue;
          if( evt_weight->GetType() == APEvtWeight::kMOANDed ) weight_uncert *= temp_weight_rel[l];
          else if( evt_weight->GetType() == APEvtWeight::kORed || evt_weight->GetType() == APEvtWeight::kMOORed ) weight_uncert *= (1.0 - temp_weight_rel[l]);
          else cout << "WARNING: handling for this weight type is unknown! uncertainties will be incorrect" << endl;
        }

        if( j < 4 || j > 7 ) {
          if( evt_weight->GetType() == APEvtWeight::kMOANDed ) weight_uncert *= temp_weight_MO;
          else if( evt_weight->GetType() == APEvtWeight::kORed || evt_weight->GetType() == APEvtWeight::kMOORed ) weight_uncert *= (1.0 - temp_weight_MO);
          else cout << "WARNING: handling for this weight type is unknown! uncertainties will be incorrect" << endl;
        }
        else if( j >= 4 && j <= 7 && temp_weight_rel[j] > numeric_limits<double>::epsilon() ) weight_uncert = weight_uncert*temp_weight_MO/temp_weight_rel[j];
        m_linear_uncert[temp_vec_all[j][i]->GetID()]->SetBinContent(&coord.front(), m_linear_uncert[temp_vec_all[j][i]->GetID()]->GetBinContent(&coord.front())+weight_uncert);
        m_variance_nocorr += weight_uncert*weight_uncert;
      }
    }
    
    /* diobject triggers */
    for (unsigned int j = 8; j < 12; ++j) {
      if( temp_vec_all[j].size() >= 2 ) {
        for (unsigned int i = 0, I = temp_vec_all[j].size(); i < I; ++i ) {
          vector<int> coord = temp_vec_all[j][i]->GetCoords();
          double weight_uncert = sqrt(temp_vec_all[j][i]->GetVariance());
          double weight_derivative = 0.;
          for (unsigned int k = 0; k < I; ++k ) {
            if( k == i ) continue;
            double weight_derivative_temp = temp_vec_all[j][k]->GetExpectancy();
            for (unsigned int l = 0; l < I; ++l ) {
              if( l == k || l == i ) continue;
              weight_derivative_temp *= (1.0 - temp_vec_all[j][l]->GetExpectancy());
            }
            weight_derivative += weight_derivative_temp;
          }
          weight_uncert *= weight_derivative;
          for( unsigned int l = 0; l < 4; ++l ) {
            if( l == j ) continue;
            if( evt_weight->GetType() == APEvtWeight::kMOANDed ) weight_uncert *= temp_weight_rel[l];
            else if( evt_weight->GetType() == APEvtWeight::kORed || evt_weight->GetType() == APEvtWeight::kMOORed ) weight_uncert *= (1.0 - temp_weight_rel[l]);
            else cout << "WARNING: handling for this weight type is unknown! uncertainties will be incorrect" << endl;
          }
          for( unsigned int l = 8; l < 12; ++l ) {
            if( l == j ) continue;
            if( evt_weight->GetType() == APEvtWeight::kMOANDed ) weight_uncert *= temp_weight_rel[l];
            else if( evt_weight->GetType() == APEvtWeight::kORed || evt_weight->GetType() == APEvtWeight::kMOORed ) weight_uncert *= (1.0 - temp_weight_rel[l]);
            else cout << "WARNING: handling for this weight type is unknown! uncertainties will be incorrect" << endl;
          }
          if( evt_weight->GetType() == APEvtWeight::kMOANDed ) weight_uncert *= temp_weight_MO;
          else if( evt_weight->GetType() == APEvtWeight::kORed || evt_weight->GetType() == APEvtWeight::kMOORed ) weight_uncert *= (1.0 - temp_weight_MO);
          else cout << "WARNING: handling for this weight type is unknown! uncertainties will be incorrect" << endl;
          
          if( j >= 4 && j <= 7 && temp_weight_rel[j] > numeric_limits<double>::epsilon() ) weight_uncert /= temp_weight_rel[j];
          
          m_linear_uncert[temp_vec_all[j][i]->GetID()]->SetBinContent(&coord.front(), m_linear_uncert[temp_vec_all[j][i]->GetID()]->GetBinContent(&coord.front())+weight_uncert);
          m_variance_nocorr += weight_uncert*weight_uncert; 
        }
      }
    }
  }

  m_variance_sys += ext_weight * evt_weight->GetSysVariance();
  
  m_isComputed = false;
}

void APWeightSum::Compute() {
  m_variance = 0.;
  m_variance_fullcorr = 0.;
  for (unsigned int iLinearUncert = 0, ILinearUncert = m_linear_uncert.size(); iLinearUncert < ILinearUncert; ++iLinearUncert) {
    if( m_linear_uncert[iLinearUncert] == 0 ) continue;
    double temp_linear_uncerts = 0;
    for (unsigned int i = 0, I = m_linear_uncert[iLinearUncert]->GetNbins(); i < I; ++i ) {
      double bin_content = m_linear_uncert[iLinearUncert]->GetBinContent(i);
      m_variance += bin_content*bin_content;
      temp_linear_uncerts += bin_content;
    }
    m_variance_fullcorr += temp_linear_uncerts*temp_linear_uncerts;
  }
  
  m_isComputed = true;
}

THnSparse* APWeightSum::GetUncertHistogram( APReweightBase* weighter ) {
  unsigned int temp_ID = weighter->GetID();
  if( temp_ID > m_linear_uncert.size()-1 ) {
    cout << "WARNING: ID unknown. Returning 0-pointer!" << endl;
    return 0;
  }
  else return m_linear_uncert[temp_ID];
}

vector<THnSparse*> APWeightSum::GetAllUncertHistograms( ) {
  return m_linear_uncert;
}
