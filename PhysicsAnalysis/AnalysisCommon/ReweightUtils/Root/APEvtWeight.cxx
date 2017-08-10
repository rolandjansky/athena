/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define APEvtWeight_cxx
#include "ReweightUtils/APEvtWeight.h"
#include "ReweightUtils/APWeightEntry.h"
#include <cmath>
#include <iostream>

using namespace std;

APEvtWeight::APEvtWeight(ObjType type)
  : m_k_evt_weight(0),
    m_variance(0),
    m_variance_sys(0)
{
  m_n_entries = 0;
  m_current_evt_weights = vector< vector< APWeightEntry* > >(12);
  if (type > APEvtWeight::kDiJet) m_isComputed = true;
  else m_isComputed = false;
  m_type = type;
}

APEvtWeight::~APEvtWeight() {
  m_current_evt_weights.clear();
}

void APEvtWeight::AddWeightToEvt(APWeightEntry* weight) {
  if (weight->IsTrig()) {
    if (m_type <= APEvtWeight::kDiJet ) {
      m_current_evt_weights[m_type].push_back(weight);
      ++m_n_entries;
      m_isComputed = false;
    } else {
      cout << "ERROR in APEvtWeight::AddWeightToEvt: Trying to add a weight entry to a combined event weight. Ignoring command (not adding weight entry)." << endl;
    }
  } else {
    cout << "ERROR in APEvtWeight::AddWeightToEvt: Trying to add a non-trigger weight entry to trigger event weight. Ignoring command (not adding weight entry)." << endl;
  }
}

const APEvtWeight operator&&(const APEvtWeight& a_in, const APEvtWeight& b_in) {
  APEvtWeight a = a_in;
  APEvtWeight b = b_in;
  APEvtWeight::ObjType a_type = a.GetType();
  APEvtWeight::ObjType b_type = b.GetType();
  
  if ( (a_type >= APEvtWeight::kDiMuon && a_type <= APEvtWeight::kDiJet) || (b_type >= APEvtWeight::kDiMuon && b_type <= APEvtWeight::kDiJet) ) cout << "WARNING in APEvtWeight::operator&&: Trying to logically AND diobject weights. You shouldn't do this. Uncertainties will be incorrect." << endl;
  if ( ((a_type >= APEvtWeight::kMuon && a_type <= APEvtWeight::kJet) || (b_type >= APEvtWeight::kMuon && b_type <= APEvtWeight::kJet)) && (a_type != b_type) ) cout << "WARNING in APEvtWeight::operator&&: Trying to logically AND weights for single component triggers in multiobject configuration. You shouldn't do this, but use kXxxMO types instead. Uncertainties will be incorrect." << endl;

  if ( a_type != b_type ) {
    APEvtWeight ret(APEvtWeight::kMOANDed);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kMuon].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kMuon].push_back(a.m_current_evt_weights[APEvtWeight::kMuon][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kMuon].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kMuon].push_back(b.m_current_evt_weights[APEvtWeight::kMuon][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kTau].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kTau].push_back(a.m_current_evt_weights[APEvtWeight::kTau][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kTau].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kTau].push_back(b.m_current_evt_weights[APEvtWeight::kTau][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kElectron].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kElectron].push_back(a.m_current_evt_weights[APEvtWeight::kElectron][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kElectron].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kElectron].push_back(b.m_current_evt_weights[APEvtWeight::kElectron][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kJet].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kJet].push_back(a.m_current_evt_weights[APEvtWeight::kJet][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kJet].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kJet].push_back(b.m_current_evt_weights[APEvtWeight::kJet][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kMuonMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kMuonMO].push_back(a.m_current_evt_weights[APEvtWeight::kMuonMO][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kMuonMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kMuonMO].push_back(b.m_current_evt_weights[APEvtWeight::kMuonMO][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kTauMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kTauMO].push_back(a.m_current_evt_weights[APEvtWeight::kTauMO][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kTauMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kTauMO].push_back(b.m_current_evt_weights[APEvtWeight::kTauMO][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kElectronMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kElectronMO].push_back(a.m_current_evt_weights[APEvtWeight::kElectronMO][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kElectronMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kElectronMO].push_back(b.m_current_evt_weights[APEvtWeight::kElectronMO][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kJetMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kJetMO].push_back(a.m_current_evt_weights[APEvtWeight::kJetMO][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kJetMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kJetMO].push_back(b.m_current_evt_weights[APEvtWeight::kJetMO][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kDiMuon].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiMuon].push_back(a.m_current_evt_weights[APEvtWeight::kDiMuon][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kDiMuon].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiMuon].push_back(b.m_current_evt_weights[APEvtWeight::kDiMuon][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kDiTau].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiTau].push_back(a.m_current_evt_weights[APEvtWeight::kDiTau][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kDiTau].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiTau].push_back(b.m_current_evt_weights[APEvtWeight::kDiTau][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kDiElectron].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiElectron].push_back(a.m_current_evt_weights[APEvtWeight::kDiElectron][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kDiElectron].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiElectron].push_back(b.m_current_evt_weights[APEvtWeight::kDiElectron][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kDiJet].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiJet].push_back(a.m_current_evt_weights[APEvtWeight::kDiJet][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kDiJet].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiJet].push_back(b.m_current_evt_weights[APEvtWeight::kDiJet][i]);

    if (!b.m_isComputed) b.Compute();
    if (!a.m_isComputed) a.Compute();
    ret.m_n_entries = a.m_n_entries + b.m_n_entries;
    ret.m_k_evt_weight = a.m_k_evt_weight * b.m_k_evt_weight;
    ret.m_variance = a.m_variance * b.m_k_evt_weight * b.m_k_evt_weight + b.m_variance * a.m_k_evt_weight * a.m_k_evt_weight;
    ret.m_variance_sys = a.m_variance_sys * b.m_k_evt_weight * b.m_k_evt_weight + b.m_variance_sys * a.m_k_evt_weight * a.m_k_evt_weight;
    return ret;
 
  }  
  else {
    APEvtWeight ret(APEvtWeight::kANDed);
    if (a_type > APEvtWeight::kJetMO || b_type > APEvtWeight::kJetMO) {
      if ((a.m_current_evt_weights[APEvtWeight::kMuon].size() > 0 && b.m_current_evt_weights[APEvtWeight::kMuon].size() > 0) || (a.m_current_evt_weights[APEvtWeight::kTau].size() > 0 && b.m_current_evt_weights[APEvtWeight::kTau].size() > 0) || (a.m_current_evt_weights[APEvtWeight::kElectron].size() > 0 && b.m_current_evt_weights[APEvtWeight::kElectron].size() > 0) || (a.m_current_evt_weights[APEvtWeight::kJet].size() > 0 && b.m_current_evt_weights[APEvtWeight::kJet].size() > 0) ||(a.m_current_evt_weights[APEvtWeight::kMuonMO].size() > 0 && b.m_current_evt_weights[APEvtWeight::kMuonMO].size() > 0) || (a.m_current_evt_weights[APEvtWeight::kTauMO].size() > 0 && b.m_current_evt_weights[APEvtWeight::kTauMO].size() > 0) || (a.m_current_evt_weights[APEvtWeight::kElectronMO].size() > 0 && b.m_current_evt_weights[APEvtWeight::kElectronMO].size() > 0) || (a.m_current_evt_weights[APEvtWeight::kJetMO].size() > 0 && b.m_current_evt_weights[APEvtWeight::kJetMO].size() > 0) )
        cout << "WARNING in APEvtWeight::operator&&: Trying to combine already combined event weights with overlapping objects. Uncertainties will be incorrect." << endl;
    }
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kMuon].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kMuon].push_back(a.m_current_evt_weights[APEvtWeight::kMuon][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kMuon].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kMuon].push_back(b.m_current_evt_weights[APEvtWeight::kMuon][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kTau].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kTau].push_back(a.m_current_evt_weights[APEvtWeight::kTau][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kTau].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kTau].push_back(b.m_current_evt_weights[APEvtWeight::kTau][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kElectron].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kElectron].push_back(a.m_current_evt_weights[APEvtWeight::kElectron][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kElectron].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kElectron].push_back(b.m_current_evt_weights[APEvtWeight::kElectron][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kJet].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kJet].push_back(a.m_current_evt_weights[APEvtWeight::kJet][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kJet].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kJet].push_back(b.m_current_evt_weights[APEvtWeight::kJet][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kMuonMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kMuonMO].push_back(a.m_current_evt_weights[APEvtWeight::kMuonMO][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kMuonMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kMuonMO].push_back(b.m_current_evt_weights[APEvtWeight::kMuonMO][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kTauMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kTauMO].push_back(a.m_current_evt_weights[APEvtWeight::kTauMO][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kTauMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kTauMO].push_back(b.m_current_evt_weights[APEvtWeight::kTauMO][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kElectronMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kElectronMO].push_back(a.m_current_evt_weights[APEvtWeight::kElectronMO][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kElectronMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kElectronMO].push_back(b.m_current_evt_weights[APEvtWeight::kElectronMO][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kJetMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kJetMO].push_back(a.m_current_evt_weights[APEvtWeight::kJetMO][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kJetMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kJetMO].push_back(b.m_current_evt_weights[APEvtWeight::kJetMO][i]); 
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kDiMuon].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiMuon].push_back(a.m_current_evt_weights[APEvtWeight::kDiMuon][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kDiMuon].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiMuon].push_back(b.m_current_evt_weights[APEvtWeight::kDiMuon][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kDiTau].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiTau].push_back(a.m_current_evt_weights[APEvtWeight::kDiTau][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kDiTau].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiTau].push_back(b.m_current_evt_weights[APEvtWeight::kDiTau][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kDiElectron].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiElectron].push_back(a.m_current_evt_weights[APEvtWeight::kDiElectron][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kDiElectron].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiElectron].push_back(b.m_current_evt_weights[APEvtWeight::kDiElectron][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kDiJet].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiJet].push_back(a.m_current_evt_weights[APEvtWeight::kDiJet][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kDiJet].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiJet].push_back(b.m_current_evt_weights[APEvtWeight::kDiJet][i]);

    if (!b.m_isComputed) b.Compute();
    if (!a.m_isComputed) a.Compute();
    ret.m_n_entries = a.m_n_entries + b.m_n_entries;
    ret.m_k_evt_weight = a.m_k_evt_weight * b.m_k_evt_weight;
    ret.m_variance = a.m_variance * b.m_k_evt_weight * b.m_k_evt_weight + b.m_variance * a.m_k_evt_weight * a.m_k_evt_weight;
    ret.m_variance_sys = a.m_variance_sys * b.m_k_evt_weight * b.m_k_evt_weight + b.m_variance_sys * a.m_k_evt_weight * a.m_k_evt_weight;
    return ret;
  }
}

const APEvtWeight operator||(const APEvtWeight& a_in, const APEvtWeight& b_in) {
  APEvtWeight a = a_in;
  APEvtWeight b = b_in;
  APEvtWeight::ObjType a_type = a.GetType();
  APEvtWeight::ObjType b_type = b.GetType();
  
  if ( (a_type != b_type) || (a_type == APEvtWeight::kElectron && b_type == APEvtWeight::kDiElectron ) || (a_type == APEvtWeight::kDiElectron && b_type == APEvtWeight::kElectron ) || (a_type == APEvtWeight::kMuon && b_type == APEvtWeight::kDiMuon ) || (a_type == APEvtWeight::kDiMuon && b_type == APEvtWeight::kMuon ) || (a_type == APEvtWeight::kTau && b_type == APEvtWeight::kDiTau ) || ( a_type == APEvtWeight::kDiTau && b_type == APEvtWeight::kTau ) || (a_type == APEvtWeight::kJet && b_type == APEvtWeight::kDiJet ) || (a_type == APEvtWeight::kDiJet && b_type == APEvtWeight::kJet ) ) {    // for what reason were all the extra conditions included?!
    APEvtWeight ret(APEvtWeight::kMOORed );
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kMuon].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kMuon].push_back(a.m_current_evt_weights[APEvtWeight::kMuon][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kMuon].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kMuon].push_back(b.m_current_evt_weights[APEvtWeight::kMuon][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kTau].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kTau].push_back(a.m_current_evt_weights[APEvtWeight::kTau][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kTau].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kTau].push_back(b.m_current_evt_weights[APEvtWeight::kTau][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kElectron].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kElectron].push_back(a.m_current_evt_weights[APEvtWeight::kElectron][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kElectron].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kElectron].push_back(b.m_current_evt_weights[APEvtWeight::kElectron][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kJet].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kJet].push_back(a.m_current_evt_weights[APEvtWeight::kJet][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kJet].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kJet].push_back(b.m_current_evt_weights[APEvtWeight::kJet][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kMuonMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kMuonMO].push_back(a.m_current_evt_weights[APEvtWeight::kMuonMO][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kMuonMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kMuonMO].push_back(b.m_current_evt_weights[APEvtWeight::kMuonMO][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kTauMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kTauMO].push_back(a.m_current_evt_weights[APEvtWeight::kTauMO][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kTauMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kTauMO].push_back(b.m_current_evt_weights[APEvtWeight::kTauMO][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kElectronMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kElectronMO].push_back(a.m_current_evt_weights[APEvtWeight::kElectronMO][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kElectronMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kElectronMO].push_back(b.m_current_evt_weights[APEvtWeight::kElectronMO][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kJetMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kJetMO].push_back(a.m_current_evt_weights[APEvtWeight::kJetMO][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kJetMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kJetMO].push_back(b.m_current_evt_weights[APEvtWeight::kJetMO][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kDiMuon].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiMuon].push_back(a.m_current_evt_weights[APEvtWeight::kDiMuon][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kDiMuon].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiMuon].push_back(b.m_current_evt_weights[APEvtWeight::kDiMuon][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kDiTau].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiTau].push_back(a.m_current_evt_weights[APEvtWeight::kDiTau][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kDiTau].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiTau].push_back(b.m_current_evt_weights[APEvtWeight::kDiTau][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kDiElectron].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiElectron].push_back(a.m_current_evt_weights[APEvtWeight::kDiElectron][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kDiElectron].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiElectron].push_back(b.m_current_evt_weights[APEvtWeight::kDiElectron][i]);
    for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kDiJet].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiJet].push_back(a.m_current_evt_weights[APEvtWeight::kDiJet][i]);
    for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kDiJet].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiJet].push_back(b.m_current_evt_weights[APEvtWeight::kDiJet][i]);
  
    if (!b.m_isComputed) b.Compute();
    if (!a.m_isComputed) a.Compute();
    
    ret.m_n_entries = a.m_n_entries + b.m_n_entries;
    ret.m_k_evt_weight = a.m_k_evt_weight + b.m_k_evt_weight * ( 1.0 - a.m_k_evt_weight );
    ret.m_variance = a.m_variance * ( 1.0 - b.m_k_evt_weight ) * ( 1.0 - b.m_k_evt_weight ) + b.m_variance * ( 1.0 - a.m_k_evt_weight ) * ( 1.0 - a.m_k_evt_weight );
    ret.m_variance_sys = a.m_variance_sys * ( 1.0 - b.m_k_evt_weight ) * ( 1.0 - b.m_k_evt_weight ) + b.m_variance_sys * ( 1.0 - a.m_k_evt_weight ) * ( 1.0 - a.m_k_evt_weight );
    return ret;
  }
  else if (a_type > APEvtWeight::kJetMO || b_type > APEvtWeight::kJetMO) {
    if ((a.m_current_evt_weights[APEvtWeight::kMuon].size() > 0 && b.m_current_evt_weights[APEvtWeight::kMuon].size() > 0) || (a.m_current_evt_weights[APEvtWeight::kTau].size() > 0 && b.m_current_evt_weights[APEvtWeight::kTau].size() > 0) || (a.m_current_evt_weights[APEvtWeight::kElectron].size() > 0 && b.m_current_evt_weights[APEvtWeight::kElectron].size() > 0) || (a.m_current_evt_weights[APEvtWeight::kJet].size() > 0 && b.m_current_evt_weights[APEvtWeight::kJet].size() > 0) || (a.m_current_evt_weights[APEvtWeight::kMuonMO].size() > 0 && b.m_current_evt_weights[APEvtWeight::kMuonMO].size() > 0) || (a.m_current_evt_weights[APEvtWeight::kTauMO].size() > 0 && b.m_current_evt_weights[APEvtWeight::kTauMO].size() > 0) || (a.m_current_evt_weights[APEvtWeight::kElectronMO].size() > 0 && b.m_current_evt_weights[APEvtWeight::kElectronMO].size() > 0) || (a.m_current_evt_weights[APEvtWeight::kJetMO].size() > 0 && b.m_current_evt_weights[APEvtWeight::kJetMO].size() > 0) )
      cout << "WARNING in APEvtWeight::operator||: Trying to combine already combined event weights with overlapping objects. Uncertainties will be incorrect." << endl;
  }
  
  APEvtWeight ret(APEvtWeight::kORed);

  for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kMuon].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kMuon].push_back(a.m_current_evt_weights[APEvtWeight::kMuon][i]);
  for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kMuon].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kMuon].push_back(b.m_current_evt_weights[APEvtWeight::kMuon][i]);
  for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kTau].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kTau].push_back(a.m_current_evt_weights[APEvtWeight::kTau][i]);
  for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kTau].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kTau].push_back(b.m_current_evt_weights[APEvtWeight::kTau][i]);
  for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kElectron].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kElectron].push_back(a.m_current_evt_weights[APEvtWeight::kElectron][i]);
  for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kElectron].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kElectron].push_back(b.m_current_evt_weights[APEvtWeight::kElectron][i]);
  for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kJet].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kJet].push_back(a.m_current_evt_weights[APEvtWeight::kJet][i]);
  for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kJet].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kJet].push_back(b.m_current_evt_weights[APEvtWeight::kJet][i]);
  for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kMuonMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kMuonMO].push_back(a.m_current_evt_weights[APEvtWeight::kMuonMO][i]);
  for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kMuonMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kMuonMO].push_back(b.m_current_evt_weights[APEvtWeight::kMuonMO][i]);
  for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kTauMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kTauMO].push_back(a.m_current_evt_weights[APEvtWeight::kTauMO][i]);
  for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kTauMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kTauMO].push_back(b.m_current_evt_weights[APEvtWeight::kTauMO][i]);
  for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kElectronMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kElectronMO].push_back(a.m_current_evt_weights[APEvtWeight::kElectronMO][i]);
  for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kElectronMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kElectronMO].push_back(b.m_current_evt_weights[APEvtWeight::kElectronMO][i]);
  for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kJetMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kJetMO].push_back(a.m_current_evt_weights[APEvtWeight::kJetMO][i]);
  for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kJetMO].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kJetMO].push_back(b.m_current_evt_weights[APEvtWeight::kJetMO][i]);
  for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kDiMuon].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiMuon].push_back(a.m_current_evt_weights[APEvtWeight::kDiMuon][i]);
  for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kDiMuon].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiMuon].push_back(b.m_current_evt_weights[APEvtWeight::kDiMuon][i]);
  for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kDiTau].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiTau].push_back(a.m_current_evt_weights[APEvtWeight::kDiTau][i]);
  for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kDiTau].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiTau].push_back(b.m_current_evt_weights[APEvtWeight::kDiTau][i]);
  for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kDiElectron].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiElectron].push_back(a.m_current_evt_weights[APEvtWeight::kDiElectron][i]);
  for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kDiElectron].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiElectron].push_back(b.m_current_evt_weights[APEvtWeight::kDiElectron][i]);
  for (unsigned int i = 0, I = a.m_current_evt_weights[APEvtWeight::kDiJet].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiJet].push_back(a.m_current_evt_weights[APEvtWeight::kDiJet][i]);
  for (unsigned int i = 0, I = b.m_current_evt_weights[APEvtWeight::kDiJet].size(); i < I; ++i) ret.m_current_evt_weights[APEvtWeight::kDiJet].push_back(b.m_current_evt_weights[APEvtWeight::kDiJet][i]);

  if (!b.m_isComputed) b.Compute();
  if (!a.m_isComputed) a.Compute();

  ret.m_n_entries = a.m_n_entries + b.m_n_entries;
  ret.m_k_evt_weight = 1.0 - (1.0 - a.m_k_evt_weight) * (1.0 - b.m_k_evt_weight);
  ret.m_variance = a.m_variance * (1.0 - b.m_k_evt_weight) * (1.0 - b.m_k_evt_weight) + b.m_variance * (1.0 - a.m_k_evt_weight) * (1.0 - a.m_k_evt_weight);
  ret.m_variance_sys = a.m_variance_sys * (1.0 - b.m_k_evt_weight) * (1.0 - b.m_k_evt_weight) + b.m_variance_sys * (1.0 - a.m_k_evt_weight) * (1.0 - a.m_k_evt_weight);
  return ret;
}

const APEvtWeight operator!(const APEvtWeight& a_in ) {

  APEvtWeight a = a_in;
  APEvtWeight ret = a;
  if(!a.m_isComputed && a.GetType() <= APEvtWeight::kDiJet ) a.Compute();
  if(ret.GetType() <= APEvtWeight::kDiJet) ret.Compute();
  ret.m_k_evt_weight = 1.0 - a.m_k_evt_weight;
  return ret;

}

double APEvtWeight::GetWeight() {
  if (!m_isComputed) Compute();
  //if ( m_type >= APEvtWeight::kMuonMO && m_type <= APEvtWeight::kJetMO ) cout << "WARNING in APEvtWeight::GetWeight: Trying to access weight for single component of multiobject trigger. You shouldn't do this!" << endl;
  return m_k_evt_weight;
}

double APEvtWeight::GetStdDev() {
  if (!m_isComputed) Compute();
  //if ( m_type >= APEvtWeight::kMuonMO && m_type <= APEvtWeight::kJetMO ) cout << "WARNING in APEvtWeight::GetStdDev: Trying to access StdDev for single component of multiobject trigger. You shouldn't do this!" << endl;
  return sqrt(m_variance);
}

double APEvtWeight::GetVariance() {
  if (!m_isComputed) Compute();
  //if ( m_type >= APEvtWeight::kMuonMO && m_type <= APEvtWeight::kJetMO ) cout << "WARNING in APEvtWeight::GetVariance: Trying to access variance for single component of multiobject trigger. You shouldn't do this!" << endl;
  return m_variance;
}

double APEvtWeight::GetSysUncert() {
  if (!m_isComputed) Compute();
  return sqrt(m_variance_sys);
}

double APEvtWeight::GetSysVariance() {
  if (!m_isComputed) Compute();
  return m_variance_sys;
}

vector< APWeightEntry* > APEvtWeight::GetWeightObjects(ObjType type) {
  if (type > APEvtWeight::kDiJet) {
    cout << "WARNING in APEvtWeight::GetWeightObjects: Trying to get non-defined object type. Returning empty vector." << endl;
    return vector< APWeightEntry* >();
  }
  return m_current_evt_weights[type];
}

unsigned long APEvtWeight::NEntries() {
  return m_n_entries;
}

APEvtWeight::ObjType APEvtWeight::GetType() {
  return m_type;
}

void APEvtWeight::Compute() {
  if (m_type > APEvtWeight::kDiJet ) {
    cout << "ERROR in APEvtWeight::Compute: Trying to compute combined event weight. Ignoring command." << endl;
  } else {
    double evt_weight = 1.0;
    double variance = 0.0;
    double variance_sys = 0.0;
    if ( m_type <= APEvtWeight::kJetMO ) {
      for (unsigned int i = 0, I = m_current_evt_weights[m_type].size(); i < I; ++i) {
        double variance_summand = 1.0;
        evt_weight *= (1. - m_current_evt_weights[m_type][i]->GetExpectancy());
        for (unsigned int k = 0; k < I; ++k) {
          if (i != k) variance_summand *= (1. - m_current_evt_weights[m_type][k]->GetExpectancy());
        }
        variance += (variance_summand * variance_summand * m_current_evt_weights[m_type][i]->GetVariance());
        variance_sys += (variance_summand * variance_summand * m_current_evt_weights[m_type][i]->GetSysUncert2());
      }
    }
    else if ( m_type <= APEvtWeight::kDiJet ) {
      if ( m_current_evt_weights[m_type].size() >= 2 ) {
        
        /* preliminary code to implement asymmetric triggers */
        bool isAsymTrig = false;
        vector<unsigned int> temp_vec_IDs;
        temp_vec_IDs.push_back( m_current_evt_weights[m_type][0]->GetID() );
        for( unsigned int i = 1, I = m_current_evt_weights[m_type].size(); i < I; ++i ) {
          bool knownID = false;
          for( unsigned int j = 0, J = temp_vec_IDs.size(); j < J; ++j ) {
            if( m_current_evt_weights[m_type][i]->GetID() == temp_vec_IDs[j] ) { knownID = true; break; }
          }
          if( !knownID ) temp_vec_IDs.push_back( m_current_evt_weights[m_type][i]->GetID() );
        }
        if( temp_vec_IDs.size() != 1 ) isAsymTrig = true;
        
        if( !isAsymTrig ) {
          for( unsigned int i = 0, I = m_current_evt_weights[m_type].size(); i < I; ++i ) {
            evt_weight *= (1. - m_current_evt_weights[m_type][i]->GetExpectancy());
          }
          for( unsigned int i = 0, I = m_current_evt_weights[m_type].size(); i < I; ++i ) {
            double variance_summand = 0.0;
            double temp_weight = m_current_evt_weights[m_type][i]->GetExpectancy();
            for( unsigned int k = 0; k < I; ++k ) {
              double temp_variance_summand = 0.0;
              if ( i != k ) {
                temp_weight *= (1.-m_current_evt_weights[m_type][k]->GetExpectancy());
                temp_variance_summand = m_current_evt_weights[m_type][k]->GetExpectancy();
              }  
              for( unsigned int j = 0; j < I; ++j ) {
                if ( j != i && j != k ) temp_variance_summand *= (1.-m_current_evt_weights[m_type][j]->GetExpectancy());
              }
              variance_summand += temp_variance_summand;
            }
            variance += (variance_summand * variance_summand * m_current_evt_weights[m_type][i]->GetVariance());
            variance_sys += (variance_summand * variance_summand * m_current_evt_weights[m_type][i]->GetSysUncert2());
            evt_weight += temp_weight;
          }
        }
        else {
          /* preliminary code to implement asymmetric triggers */
          /* needs to be implemented */
          /* this trigger will need : */
          /* - absolute efficiency of first leg   */
          /* - absolute efficiency of second leg */
          /* - conditional efficiency: efficiency of second leg under the condition the first leg fired */
          /* - conditional efficiecny: efficiecny of second leg under the condition the first leg did not fire */
          //std::cout << "this is an asymmetric trigger" << std::endl;
          if( temp_vec_IDs.size() != 4 ) { std::cout << "WARNING! Required exactly 4 different efficiencies, while " << temp_vec_IDs.size() << " efficiencies are provided! Please check your implementation! Weight is set to 0! " << std::endl; }
          else if( m_current_evt_weights[m_type].size() < 2*temp_vec_IDs.size() ) { std::cout << "less than 2 leptons added. Weight is 0." << std::endl; }
          else {
            // first the weight //
            double evt_weight_leg1 = 1.;
            double evt_weight_leg2 = 1.;
            double evt_weight_leg2_cond = 1.;
            double evt_weight_singleHit = 0.;
            for( unsigned int i = 0, I = m_current_evt_weights[m_type].size(); i < I; i += 4 ) {
              evt_weight_leg1 *= (1. - m_current_evt_weights[m_type][i]->GetExpectancy());
              evt_weight_leg2 *= (1. - m_current_evt_weights[m_type][i+1]->GetExpectancy());
              evt_weight_leg2_cond *= (1. - m_current_evt_weights[m_type][i]->GetExpectancy())*(1. - m_current_evt_weights[m_type][i+2]->GetExpectancy());
            }
            for( unsigned int j = 0, J = m_current_evt_weights[m_type].size(); j < J; j += 4 ) {
              double temp_weight = 0.;
              temp_weight += m_current_evt_weights[m_type][j]->GetExpectancy()*m_current_evt_weights[m_type][j+3]->GetExpectancy();
              for( unsigned int i = 0, I = m_current_evt_weights[m_type].size(); i < I; i += 4 ) {
                if( i == j ) continue;
                temp_weight *= (1. - m_current_evt_weights[m_type][i]->GetExpectancy() )*(1. - m_current_evt_weights[m_type][i+2]->GetExpectancy() );
              }
              evt_weight_singleHit += temp_weight;
            }
            //std::cout << "got " << evt_weight_leg1 << " and " << evt_weight_leg2 << " and " << evt_weight_leg2_cond << " and " << evt_weight_singleHit << std::endl;
            evt_weight = evt_weight_leg1 + evt_weight_leg2 - evt_weight_leg2_cond + evt_weight_singleHit;
            // done; now the uncertainty //
            double variance_leg1 = 0.;
            double variance_leg2 = 0.;
            double variance_leg2_cond = 0.;
            double variance_singleHit = 0.;
            // first leg of the trigger
            for( unsigned int k = 0, K = m_current_evt_weights[m_type].size(); k < K; k += 4 ) {
              double variance_k = 0.;
              double variance_temp = 1.;
              for( unsigned int i = 0, I = m_current_evt_weights[m_type].size(); i < I; i += 4 ) {
                if( i == k ) continue;
                variance_temp *= (1. - m_current_evt_weights[m_type][i]->GetExpectancy());
              }
              variance_k += variance_temp;
              variance_temp = -1.;
              for( unsigned int i = 0, I = m_current_evt_weights[m_type].size(); i < I; i += 4 ) {
                variance_temp *= (1.0 - m_current_evt_weights[m_type][i+2]->GetExpectancy());
                if( i == k ) continue;
                variance_temp *= (1.0 - m_current_evt_weights[m_type][i]->GetExpectancy());
              }
              variance_k += variance_temp;
              variance_temp = -m_current_evt_weights[m_type][k+3]->GetExpectancy();
              for( unsigned int i = 0, I = m_current_evt_weights[m_type].size(); i < I; i += 4 ) {
                if( i == k ) continue;
                variance_temp *= (1.0 - m_current_evt_weights[m_type][i]->GetExpectancy())*(1.0 - m_current_evt_weights[m_type][i+2]->GetExpectancy());
              }
              variance_k += variance_temp;
              variance_temp = 0.;
              for( unsigned int j = 0, J = m_current_evt_weights[m_type].size(); j < J; j+= 4 ) {
                if( j == k ) continue;
                double variance_ijk_temp = m_current_evt_weights[m_type][j]->GetExpectancy()*m_current_evt_weights[m_type][j+3]->GetExpectancy();
                for( unsigned int i = 0, I = m_current_evt_weights[m_type].size(); i < I; i += 4 ) {
                  if( i == j ) continue;
                  variance_ijk_temp *= (1.0 -  m_current_evt_weights[m_type][i+2]->GetExpectancy());
                  if( i == k ) continue;
                  variance_ijk_temp *= (1.0 - m_current_evt_weights[m_type][i]->GetExpectancy());
                }
                variance_temp += variance_ijk_temp;
              }
              variance_k += variance_temp;
              variance_leg1 += variance_k*variance_k*m_current_evt_weights[m_type][k]->GetVariance();
            }
            
            // second leg of the trigger; standalone
            for( unsigned int k = 0, K = m_current_evt_weights[m_type].size(); k < K; k += 4 ) {
              double variance_temp = 1.;
              for( unsigned int i = 0, I = m_current_evt_weights[m_type].size(); i < I; i += 4 ) {
                if( i == k ) continue;
                variance_temp *= (1. - m_current_evt_weights[m_type][i+1]->GetExpectancy());
              }
              variance_leg2 += variance_temp*variance_temp*m_current_evt_weights[m_type][k+1]->GetVariance();
            }
            
            // second leg of the trigger under condition of having the first leg
            for( unsigned int k = 0, K = m_current_evt_weights[m_type].size(); k < K; k += 4 ) {
              double variance_k = 0.;
              double variance_temp = 1.;
              for( unsigned int i = 0, I = m_current_evt_weights[m_type].size(); i < I; i += 4 ) {
                variance_temp *= (1. - m_current_evt_weights[m_type][i]->GetExpectancy());
                if( i == k ) continue;
                variance_temp *= (1. - m_current_evt_weights[m_type][i+2]->GetExpectancy());
              }
              variance_k += variance_temp;
              variance_temp = 0.;
              for( unsigned int j = 0, J = m_current_evt_weights[m_type].size(); j < J; j+= 4 ) {
                double variance_ijk_temp = m_current_evt_weights[m_type][j]->GetExpectancy()*m_current_evt_weights[m_type][j+3]->GetExpectancy();
                for( unsigned int i = 0, I = m_current_evt_weights[m_type].size(); i < I; i += 4 ) {
                  if( j == i ) continue;
                  variance_ijk_temp *= (1.0 - m_current_evt_weights[m_type][i]->GetExpectancy());
                  if( i == k ) continue;
                  variance_ijk_temp *= (1.0 -  m_current_evt_weights[m_type][i+2]->GetExpectancy());
                }
                variance_temp += variance_ijk_temp;
              }
              variance_k += variance_temp;
              variance_leg2_cond += variance_k*variance_k*m_current_evt_weights[m_type][k+2]->GetVariance();
            }
            
            // and the single Hit term
            for( unsigned int k = 0, K = m_current_evt_weights[m_type].size(); k < K; k += 4 ) {
              double variance_k = - m_current_evt_weights[m_type][k]->GetExpectancy();
              for( unsigned int i = 0, I = m_current_evt_weights[m_type].size(); i < I; i += 4 ) {
                if( i == k ) continue;
                variance_k *= (1.0 - m_current_evt_weights[m_type][i]->GetExpectancy())*(1.0 - m_current_evt_weights[m_type][i+3]->GetExpectancy());
              }
              variance_singleHit += variance_k * variance_k *m_current_evt_weights[m_type][k+3]->GetVariance();
            }
            
            // now add everything
            variance = variance_leg1 + variance_leg2 + variance_leg2_cond + variance_singleHit;

          }
        }
        
      }
    }
    m_k_evt_weight = (1. - evt_weight);
    m_variance = variance;
    m_variance_sys = variance_sys;
  }
  m_isComputed = true;
}
