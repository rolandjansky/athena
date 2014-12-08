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
  : _k_evt_weight(0),
    _variance(0),
    _variance_sys(0)
{
  _n_entries = 0;
  _current_evt_weights = vector< vector< APWeightEntry* > >(12);
  if (type > APEvtWeight::kDiJet) _isComputed = true;
  else _isComputed = false;
  _type = type;
}

APEvtWeight::~APEvtWeight() {
  _current_evt_weights.clear();
}

void APEvtWeight::AddWeightToEvt(APWeightEntry* weight) {
  if (weight->IsTrig()) {
    if (_type <= APEvtWeight::kDiJet ) {
      _current_evt_weights[_type].push_back(weight);
      ++_n_entries;
      _isComputed = false;
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
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kMuon].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kMuon].push_back(a._current_evt_weights[APEvtWeight::kMuon][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kMuon].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kMuon].push_back(b._current_evt_weights[APEvtWeight::kMuon][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kTau].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kTau].push_back(a._current_evt_weights[APEvtWeight::kTau][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kTau].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kTau].push_back(b._current_evt_weights[APEvtWeight::kTau][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kElectron].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kElectron].push_back(a._current_evt_weights[APEvtWeight::kElectron][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kElectron].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kElectron].push_back(b._current_evt_weights[APEvtWeight::kElectron][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kJet].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kJet].push_back(a._current_evt_weights[APEvtWeight::kJet][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kJet].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kJet].push_back(b._current_evt_weights[APEvtWeight::kJet][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kMuonMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kMuonMO].push_back(a._current_evt_weights[APEvtWeight::kMuonMO][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kMuonMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kMuonMO].push_back(b._current_evt_weights[APEvtWeight::kMuonMO][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kTauMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kTauMO].push_back(a._current_evt_weights[APEvtWeight::kTauMO][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kTauMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kTauMO].push_back(b._current_evt_weights[APEvtWeight::kTauMO][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kElectronMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kElectronMO].push_back(a._current_evt_weights[APEvtWeight::kElectronMO][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kElectronMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kElectronMO].push_back(b._current_evt_weights[APEvtWeight::kElectronMO][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kJetMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kJetMO].push_back(a._current_evt_weights[APEvtWeight::kJetMO][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kJetMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kJetMO].push_back(b._current_evt_weights[APEvtWeight::kJetMO][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kDiMuon].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiMuon].push_back(a._current_evt_weights[APEvtWeight::kDiMuon][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kDiMuon].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiMuon].push_back(b._current_evt_weights[APEvtWeight::kDiMuon][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kDiTau].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiTau].push_back(a._current_evt_weights[APEvtWeight::kDiTau][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kDiTau].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiTau].push_back(b._current_evt_weights[APEvtWeight::kDiTau][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kDiElectron].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiElectron].push_back(a._current_evt_weights[APEvtWeight::kDiElectron][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kDiElectron].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiElectron].push_back(b._current_evt_weights[APEvtWeight::kDiElectron][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kDiJet].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiJet].push_back(a._current_evt_weights[APEvtWeight::kDiJet][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kDiJet].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiJet].push_back(b._current_evt_weights[APEvtWeight::kDiJet][i]);

    if (!b._isComputed) b.Compute();
    if (!a._isComputed) a.Compute();
    ret._n_entries = a._n_entries + b._n_entries;
    ret._k_evt_weight = a._k_evt_weight * b._k_evt_weight;
    ret._variance = a._variance * b._k_evt_weight * b._k_evt_weight + b._variance * a._k_evt_weight * a._k_evt_weight;
    ret._variance_sys = a._variance_sys * b._k_evt_weight * b._k_evt_weight + b._variance_sys * a._k_evt_weight * a._k_evt_weight;
    return ret;
 
  }  
  else {
    APEvtWeight ret(APEvtWeight::kANDed);
    if (a_type > APEvtWeight::kJetMO || b_type > APEvtWeight::kJetMO) {
      if ((a._current_evt_weights[APEvtWeight::kMuon].size() > 0 && b._current_evt_weights[APEvtWeight::kMuon].size() > 0) || (a._current_evt_weights[APEvtWeight::kTau].size() > 0 && b._current_evt_weights[APEvtWeight::kTau].size() > 0) || (a._current_evt_weights[APEvtWeight::kElectron].size() > 0 && b._current_evt_weights[APEvtWeight::kElectron].size() > 0) || (a._current_evt_weights[APEvtWeight::kJet].size() > 0 && b._current_evt_weights[APEvtWeight::kJet].size() > 0) ||(a._current_evt_weights[APEvtWeight::kMuonMO].size() > 0 && b._current_evt_weights[APEvtWeight::kMuonMO].size() > 0) || (a._current_evt_weights[APEvtWeight::kTauMO].size() > 0 && b._current_evt_weights[APEvtWeight::kTauMO].size() > 0) || (a._current_evt_weights[APEvtWeight::kElectronMO].size() > 0 && b._current_evt_weights[APEvtWeight::kElectronMO].size() > 0) || (a._current_evt_weights[APEvtWeight::kJetMO].size() > 0 && b._current_evt_weights[APEvtWeight::kJetMO].size() > 0) )
        cout << "WARNING in APEvtWeight::operator&&: Trying to combine already combined event weights with overlapping objects. Uncertainties will be incorrect." << endl;
    }
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kMuon].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kMuon].push_back(a._current_evt_weights[APEvtWeight::kMuon][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kMuon].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kMuon].push_back(b._current_evt_weights[APEvtWeight::kMuon][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kTau].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kTau].push_back(a._current_evt_weights[APEvtWeight::kTau][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kTau].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kTau].push_back(b._current_evt_weights[APEvtWeight::kTau][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kElectron].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kElectron].push_back(a._current_evt_weights[APEvtWeight::kElectron][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kElectron].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kElectron].push_back(b._current_evt_weights[APEvtWeight::kElectron][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kJet].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kJet].push_back(a._current_evt_weights[APEvtWeight::kJet][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kJet].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kJet].push_back(b._current_evt_weights[APEvtWeight::kJet][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kMuonMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kMuonMO].push_back(a._current_evt_weights[APEvtWeight::kMuonMO][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kMuonMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kMuonMO].push_back(b._current_evt_weights[APEvtWeight::kMuonMO][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kTauMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kTauMO].push_back(a._current_evt_weights[APEvtWeight::kTauMO][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kTauMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kTauMO].push_back(b._current_evt_weights[APEvtWeight::kTauMO][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kElectronMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kElectronMO].push_back(a._current_evt_weights[APEvtWeight::kElectronMO][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kElectronMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kElectronMO].push_back(b._current_evt_weights[APEvtWeight::kElectronMO][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kJetMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kJetMO].push_back(a._current_evt_weights[APEvtWeight::kJetMO][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kJetMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kJetMO].push_back(b._current_evt_weights[APEvtWeight::kJetMO][i]); 
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kDiMuon].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiMuon].push_back(a._current_evt_weights[APEvtWeight::kDiMuon][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kDiMuon].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiMuon].push_back(b._current_evt_weights[APEvtWeight::kDiMuon][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kDiTau].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiTau].push_back(a._current_evt_weights[APEvtWeight::kDiTau][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kDiTau].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiTau].push_back(b._current_evt_weights[APEvtWeight::kDiTau][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kDiElectron].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiElectron].push_back(a._current_evt_weights[APEvtWeight::kDiElectron][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kDiElectron].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiElectron].push_back(b._current_evt_weights[APEvtWeight::kDiElectron][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kDiJet].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiJet].push_back(a._current_evt_weights[APEvtWeight::kDiJet][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kDiJet].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiJet].push_back(b._current_evt_weights[APEvtWeight::kDiJet][i]);

    if (!b._isComputed) b.Compute();
    if (!a._isComputed) a.Compute();
    ret._n_entries = a._n_entries + b._n_entries;
    ret._k_evt_weight = a._k_evt_weight * b._k_evt_weight;
    ret._variance = a._variance * b._k_evt_weight * b._k_evt_weight + b._variance * a._k_evt_weight * a._k_evt_weight;
    ret._variance_sys = a._variance_sys * b._k_evt_weight * b._k_evt_weight + b._variance_sys * a._k_evt_weight * a._k_evt_weight;
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
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kMuon].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kMuon].push_back(a._current_evt_weights[APEvtWeight::kMuon][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kMuon].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kMuon].push_back(b._current_evt_weights[APEvtWeight::kMuon][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kTau].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kTau].push_back(a._current_evt_weights[APEvtWeight::kTau][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kTau].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kTau].push_back(b._current_evt_weights[APEvtWeight::kTau][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kElectron].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kElectron].push_back(a._current_evt_weights[APEvtWeight::kElectron][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kElectron].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kElectron].push_back(b._current_evt_weights[APEvtWeight::kElectron][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kJet].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kJet].push_back(a._current_evt_weights[APEvtWeight::kJet][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kJet].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kJet].push_back(b._current_evt_weights[APEvtWeight::kJet][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kMuonMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kMuonMO].push_back(a._current_evt_weights[APEvtWeight::kMuonMO][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kMuonMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kMuonMO].push_back(b._current_evt_weights[APEvtWeight::kMuonMO][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kTauMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kTauMO].push_back(a._current_evt_weights[APEvtWeight::kTauMO][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kTauMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kTauMO].push_back(b._current_evt_weights[APEvtWeight::kTauMO][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kElectronMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kElectronMO].push_back(a._current_evt_weights[APEvtWeight::kElectronMO][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kElectronMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kElectronMO].push_back(b._current_evt_weights[APEvtWeight::kElectronMO][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kJetMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kJetMO].push_back(a._current_evt_weights[APEvtWeight::kJetMO][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kJetMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kJetMO].push_back(b._current_evt_weights[APEvtWeight::kJetMO][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kDiMuon].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiMuon].push_back(a._current_evt_weights[APEvtWeight::kDiMuon][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kDiMuon].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiMuon].push_back(b._current_evt_weights[APEvtWeight::kDiMuon][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kDiTau].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiTau].push_back(a._current_evt_weights[APEvtWeight::kDiTau][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kDiTau].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiTau].push_back(b._current_evt_weights[APEvtWeight::kDiTau][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kDiElectron].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiElectron].push_back(a._current_evt_weights[APEvtWeight::kDiElectron][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kDiElectron].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiElectron].push_back(b._current_evt_weights[APEvtWeight::kDiElectron][i]);
    for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kDiJet].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiJet].push_back(a._current_evt_weights[APEvtWeight::kDiJet][i]);
    for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kDiJet].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiJet].push_back(b._current_evt_weights[APEvtWeight::kDiJet][i]);
  
    if (!b._isComputed) b.Compute();
    if (!a._isComputed) a.Compute();
    
    ret._n_entries = a._n_entries + b._n_entries;
    ret._k_evt_weight = a._k_evt_weight + b._k_evt_weight * ( 1.0 - a._k_evt_weight );
    ret._variance = a._variance * ( 1.0 - b._k_evt_weight ) * ( 1.0 - b._k_evt_weight ) + b._variance * ( 1.0 - a._k_evt_weight ) * ( 1.0 - a._k_evt_weight );
    ret._variance_sys = a._variance_sys * ( 1.0 - b._k_evt_weight ) * ( 1.0 - b._k_evt_weight ) + b._variance_sys * ( 1.0 - a._k_evt_weight ) * ( 1.0 - a._k_evt_weight );
    return ret;
  }
  else if (a_type > APEvtWeight::kJetMO || b_type > APEvtWeight::kJetMO) {
    if ((a._current_evt_weights[APEvtWeight::kMuon].size() > 0 && b._current_evt_weights[APEvtWeight::kMuon].size() > 0) || (a._current_evt_weights[APEvtWeight::kTau].size() > 0 && b._current_evt_weights[APEvtWeight::kTau].size() > 0) || (a._current_evt_weights[APEvtWeight::kElectron].size() > 0 && b._current_evt_weights[APEvtWeight::kElectron].size() > 0) || (a._current_evt_weights[APEvtWeight::kJet].size() > 0 && b._current_evt_weights[APEvtWeight::kJet].size() > 0) || (a._current_evt_weights[APEvtWeight::kMuonMO].size() > 0 && b._current_evt_weights[APEvtWeight::kMuonMO].size() > 0) || (a._current_evt_weights[APEvtWeight::kTauMO].size() > 0 && b._current_evt_weights[APEvtWeight::kTauMO].size() > 0) || (a._current_evt_weights[APEvtWeight::kElectronMO].size() > 0 && b._current_evt_weights[APEvtWeight::kElectronMO].size() > 0) || (a._current_evt_weights[APEvtWeight::kJetMO].size() > 0 && b._current_evt_weights[APEvtWeight::kJetMO].size() > 0) )
      cout << "WARNING in APEvtWeight::operator||: Trying to combine already combined event weights with overlapping objects. Uncertainties will be incorrect." << endl;
  }
  
  APEvtWeight ret(APEvtWeight::kORed);

  for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kMuon].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kMuon].push_back(a._current_evt_weights[APEvtWeight::kMuon][i]);
  for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kMuon].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kMuon].push_back(b._current_evt_weights[APEvtWeight::kMuon][i]);
  for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kTau].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kTau].push_back(a._current_evt_weights[APEvtWeight::kTau][i]);
  for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kTau].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kTau].push_back(b._current_evt_weights[APEvtWeight::kTau][i]);
  for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kElectron].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kElectron].push_back(a._current_evt_weights[APEvtWeight::kElectron][i]);
  for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kElectron].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kElectron].push_back(b._current_evt_weights[APEvtWeight::kElectron][i]);
  for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kJet].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kJet].push_back(a._current_evt_weights[APEvtWeight::kJet][i]);
  for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kJet].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kJet].push_back(b._current_evt_weights[APEvtWeight::kJet][i]);
  for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kMuonMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kMuonMO].push_back(a._current_evt_weights[APEvtWeight::kMuonMO][i]);
  for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kMuonMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kMuonMO].push_back(b._current_evt_weights[APEvtWeight::kMuonMO][i]);
  for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kTauMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kTauMO].push_back(a._current_evt_weights[APEvtWeight::kTauMO][i]);
  for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kTauMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kTauMO].push_back(b._current_evt_weights[APEvtWeight::kTauMO][i]);
  for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kElectronMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kElectronMO].push_back(a._current_evt_weights[APEvtWeight::kElectronMO][i]);
  for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kElectronMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kElectronMO].push_back(b._current_evt_weights[APEvtWeight::kElectronMO][i]);
  for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kJetMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kJetMO].push_back(a._current_evt_weights[APEvtWeight::kJetMO][i]);
  for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kJetMO].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kJetMO].push_back(b._current_evt_weights[APEvtWeight::kJetMO][i]);
  for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kDiMuon].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiMuon].push_back(a._current_evt_weights[APEvtWeight::kDiMuon][i]);
  for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kDiMuon].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiMuon].push_back(b._current_evt_weights[APEvtWeight::kDiMuon][i]);
  for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kDiTau].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiTau].push_back(a._current_evt_weights[APEvtWeight::kDiTau][i]);
  for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kDiTau].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiTau].push_back(b._current_evt_weights[APEvtWeight::kDiTau][i]);
  for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kDiElectron].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiElectron].push_back(a._current_evt_weights[APEvtWeight::kDiElectron][i]);
  for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kDiElectron].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiElectron].push_back(b._current_evt_weights[APEvtWeight::kDiElectron][i]);
  for (unsigned int i = 0, I = a._current_evt_weights[APEvtWeight::kDiJet].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiJet].push_back(a._current_evt_weights[APEvtWeight::kDiJet][i]);
  for (unsigned int i = 0, I = b._current_evt_weights[APEvtWeight::kDiJet].size(); i < I; ++i) ret._current_evt_weights[APEvtWeight::kDiJet].push_back(b._current_evt_weights[APEvtWeight::kDiJet][i]);

  if (!b._isComputed) b.Compute();
  if (!a._isComputed) a.Compute();

  ret._n_entries = a._n_entries + b._n_entries;
  ret._k_evt_weight = 1.0 - (1.0 - a._k_evt_weight) * (1.0 - b._k_evt_weight);
  ret._variance = a._variance * (1.0 - b._k_evt_weight) * (1.0 - b._k_evt_weight) + b._variance * (1.0 - a._k_evt_weight) * (1.0 - a._k_evt_weight);
  ret._variance_sys = a._variance_sys * (1.0 - b._k_evt_weight) * (1.0 - b._k_evt_weight) + b._variance_sys * (1.0 - a._k_evt_weight) * (1.0 - a._k_evt_weight);
  return ret;
}

const APEvtWeight operator!(const APEvtWeight& a_in ) {

  APEvtWeight a = a_in;
  APEvtWeight ret = a;
  if(!a._isComputed && a.GetType() <= APEvtWeight::kDiJet ) a.Compute();
  if(ret.GetType() <= APEvtWeight::kDiJet) ret.Compute();
  ret._k_evt_weight = 1.0 - a._k_evt_weight;
  return ret;

}

double APEvtWeight::GetWeight() {
  if (!_isComputed) Compute();
  //if ( _type >= APEvtWeight::kMuonMO && _type <= APEvtWeight::kJetMO ) cout << "WARNING in APEvtWeight::GetWeight: Trying to access weight for single component of multiobject trigger. You shouldn't do this!" << endl;
  return _k_evt_weight;
}

double APEvtWeight::GetStdDev() {
  if (!_isComputed) Compute();
  //if ( _type >= APEvtWeight::kMuonMO && _type <= APEvtWeight::kJetMO ) cout << "WARNING in APEvtWeight::GetStdDev: Trying to access StdDev for single component of multiobject trigger. You shouldn't do this!" << endl;
  return sqrt(_variance);
}

double APEvtWeight::GetVariance() {
  if (!_isComputed) Compute();
  //if ( _type >= APEvtWeight::kMuonMO && _type <= APEvtWeight::kJetMO ) cout << "WARNING in APEvtWeight::GetVariance: Trying to access variance for single component of multiobject trigger. You shouldn't do this!" << endl;
  return _variance;
}

double APEvtWeight::GetSysUncert() {
  if (!_isComputed) Compute();
  return sqrt(_variance_sys);
}

double APEvtWeight::GetSysVariance() {
  if (!_isComputed) Compute();
  return _variance_sys;
}

vector< APWeightEntry* > APEvtWeight::GetWeightObjects(ObjType type) {
  if (type > APEvtWeight::kDiJet) {
    cout << "WARNING in APEvtWeight::GetWeightObjects: Trying to get non-defined object type. Returning empty vector." << endl;
    return vector< APWeightEntry* >();
  }
  return _current_evt_weights[type];
}

unsigned long APEvtWeight::NEntries() {
  return _n_entries;
}

APEvtWeight::ObjType APEvtWeight::GetType() {
  return _type;
}

void APEvtWeight::Compute() {
  if (_type > APEvtWeight::kDiJet ) {
    cout << "ERROR in APEvtWeight::Compute: Trying to compute combined event weight. Ignoring command." << endl;
  } else {
    double evt_weight = 1.0;
    double variance = 0.0;
    double variance_sys = 0.0;
    if ( _type <= APEvtWeight::kJetMO ) {
      for (unsigned int i = 0, I = _current_evt_weights[_type].size(); i < I; ++i) {
        double variance_summand = 1.0;
        evt_weight *= (1. - _current_evt_weights[_type][i]->GetExpectancy());
        for (unsigned int k = 0; k < I; ++k) {
          if (i != k) variance_summand *= (1. - _current_evt_weights[_type][k]->GetExpectancy());
        }
        variance += (variance_summand * variance_summand * _current_evt_weights[_type][i]->GetVariance());
        variance_sys += (variance_summand * variance_summand * _current_evt_weights[_type][i]->GetSysUncert2());
      }
    }
    else if ( _type <= APEvtWeight::kDiJet ) {
      if ( _current_evt_weights[_type].size() >= 2 ) {
        
        /* preliminary code to implement asymmetric triggers */
        bool isAsymTrig = false;
        vector<unsigned int> temp_vec_IDs;
        temp_vec_IDs.push_back( _current_evt_weights[_type][0]->GetID() );
        for( unsigned int i = 1, I = _current_evt_weights[_type].size(); i < I; ++i ) {
          bool knownID = false;
          for( unsigned int j = 0, J = temp_vec_IDs.size(); j < J; ++j ) {
            if( _current_evt_weights[_type][i]->GetID() == temp_vec_IDs[j] ) { knownID = true; break; }
          }
          if( !knownID ) temp_vec_IDs.push_back( _current_evt_weights[_type][i]->GetID() );
        }
        if( temp_vec_IDs.size() != 1 ) isAsymTrig = true;
        
        if( !isAsymTrig ) {
          for( unsigned int i = 0, I = _current_evt_weights[_type].size(); i < I; ++i ) {
            evt_weight *= (1. - _current_evt_weights[_type][i]->GetExpectancy());
          }
          for( unsigned int i = 0, I = _current_evt_weights[_type].size(); i < I; ++i ) {
            double variance_summand = 0.0;
            double temp_weight = _current_evt_weights[_type][i]->GetExpectancy();
            for( unsigned int k = 0; k < I; ++k ) {
              double temp_variance_summand = 0.0;
              if ( i != k ) {
                temp_weight *= (1.-_current_evt_weights[_type][k]->GetExpectancy());
                temp_variance_summand = _current_evt_weights[_type][k]->GetExpectancy();
              }  
              for( unsigned int j = 0; j < I; ++j ) {
                if ( j != i && j != k ) temp_variance_summand *= (1.-_current_evt_weights[_type][j]->GetExpectancy());
              }
              variance_summand += temp_variance_summand;
            }
            variance += (variance_summand * variance_summand * _current_evt_weights[_type][i]->GetVariance());
            variance_sys += (variance_summand * variance_summand * _current_evt_weights[_type][i]->GetSysUncert2());
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
          else if( _current_evt_weights[_type].size() < 2*temp_vec_IDs.size() ) { std::cout << "less than 2 leptons added. Weight is 0." << std::endl; }
          else {
            // first the weight //
            double evt_weight_leg1 = 1.;
            double evt_weight_leg2 = 1.;
            double evt_weight_leg2_cond = 1.;
            double evt_weight_singleHit = 0.;
            for( unsigned int i = 0, I = _current_evt_weights[_type].size(); i < I; i += 4 ) {
              evt_weight_leg1 *= (1. - _current_evt_weights[_type][i]->GetExpectancy());
              evt_weight_leg2 *= (1. - _current_evt_weights[_type][i+1]->GetExpectancy());
              evt_weight_leg2_cond *= (1. - _current_evt_weights[_type][i]->GetExpectancy())*(1. - _current_evt_weights[_type][i+2]->GetExpectancy());
            }
            for( unsigned int j = 0, J = _current_evt_weights[_type].size(); j < J; j += 4 ) {
              double temp_weight = 0.;
              temp_weight += _current_evt_weights[_type][j]->GetExpectancy()*_current_evt_weights[_type][j+3]->GetExpectancy();
              for( unsigned int i = 0, I = _current_evt_weights[_type].size(); i < I; i += 4 ) {
                if( i == j ) continue;
                temp_weight *= (1. - _current_evt_weights[_type][i]->GetExpectancy() )*(1. - _current_evt_weights[_type][i+2]->GetExpectancy() );
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
            for( unsigned int k = 0, K = _current_evt_weights[_type].size(); k < K; k += 4 ) {
              double variance_k = 0.;
              double variance_temp = 1.;
              for( unsigned int i = 0, I = _current_evt_weights[_type].size(); i < I; i += 4 ) {
                if( i == k ) continue;
                variance_temp *= (1. - _current_evt_weights[_type][i]->GetExpectancy());
              }
              variance_k += variance_temp;
              variance_temp = -1.;
              for( unsigned int i = 0, I = _current_evt_weights[_type].size(); i < I; i += 4 ) {
                variance_temp *= (1.0 - _current_evt_weights[_type][i+2]->GetExpectancy());
                if( i == k ) continue;
                variance_temp *= (1.0 - _current_evt_weights[_type][i]->GetExpectancy());
              }
              variance_k += variance_temp;
              variance_temp = -_current_evt_weights[_type][k+3]->GetExpectancy();
              for( unsigned int i = 0, I = _current_evt_weights[_type].size(); i < I; i += 4 ) {
                if( i == k ) continue;
                variance_temp *= (1.0 - _current_evt_weights[_type][i]->GetExpectancy())*(1.0 - _current_evt_weights[_type][i+2]->GetExpectancy());
              }
              variance_k += variance_temp;
              variance_temp = 0.;
              for( unsigned int j = 0, J = _current_evt_weights[_type].size(); j < J; j+= 4 ) {
                if( j == k ) continue;
                double variance_ijk_temp = _current_evt_weights[_type][j]->GetExpectancy()*_current_evt_weights[_type][j+3]->GetExpectancy();
                for( unsigned int i = 0, I = _current_evt_weights[_type].size(); i < I; i += 4 ) {
                  if( i == j ) continue;
                  variance_ijk_temp *= (1.0 -  _current_evt_weights[_type][i+2]->GetExpectancy());
                  if( i == k ) continue;
                  variance_ijk_temp *= (1.0 - _current_evt_weights[_type][i]->GetExpectancy());
                }
                variance_temp += variance_ijk_temp;
              }
              variance_k += variance_temp;
              variance_leg1 += variance_k*variance_k*_current_evt_weights[_type][k]->GetVariance();
            }
            
            // second leg of the trigger; standalone
            for( unsigned int k = 0, K = _current_evt_weights[_type].size(); k < K; k += 4 ) {
              double variance_temp = 1.;
              for( unsigned int i = 0, I = _current_evt_weights[_type].size(); i < I; i += 4 ) {
                if( i == k ) continue;
                variance_temp *= (1. - _current_evt_weights[_type][i+1]->GetExpectancy());
              }
              variance_leg2 += variance_temp*variance_temp*_current_evt_weights[_type][k+1]->GetVariance();
            }
            
            // second leg of the trigger under condition of having the first leg
            for( unsigned int k = 0, K = _current_evt_weights[_type].size(); k < K; k += 4 ) {
              double variance_k = 0.;
              double variance_temp = 1.;
              for( unsigned int i = 0, I = _current_evt_weights[_type].size(); i < I; i += 4 ) {
                variance_temp *= (1. - _current_evt_weights[_type][i]->GetExpectancy());
                if( i == k ) continue;
                variance_temp *= (1. - _current_evt_weights[_type][i+2]->GetExpectancy());
              }
              variance_k += variance_temp;
              variance_temp = 0.;
              for( unsigned int j = 0, J = _current_evt_weights[_type].size(); j < J; j+= 4 ) {
                double variance_ijk_temp = _current_evt_weights[_type][j]->GetExpectancy()*_current_evt_weights[_type][j+3]->GetExpectancy();
                for( unsigned int i = 0, I = _current_evt_weights[_type].size(); i < I; i += 4 ) {
                  if( j == i ) continue;
                  variance_ijk_temp *= (1.0 - _current_evt_weights[_type][i]->GetExpectancy());
                  if( i == k ) continue;
                  variance_ijk_temp *= (1.0 -  _current_evt_weights[_type][i+2]->GetExpectancy());
                }
                variance_temp += variance_ijk_temp;
              }
              variance_k += variance_temp;
              variance_leg2_cond += variance_k*variance_k*_current_evt_weights[_type][k+2]->GetVariance();
            }
            
            // and the single Hit term
            for( unsigned int k = 0, K = _current_evt_weights[_type].size(); k < K; k += 4 ) {
              double variance_k = - _current_evt_weights[_type][k]->GetExpectancy();
              for( unsigned int i = 0, I = _current_evt_weights[_type].size(); i < I; i += 4 ) {
                if( i == k ) continue;
                variance_k *= (1.0 - _current_evt_weights[_type][i]->GetExpectancy())*(1.0 - _current_evt_weights[_type][i+3]->GetExpectancy());
              }
              variance_singleHit += variance_k * variance_k *_current_evt_weights[_type][k+3]->GetVariance();
            }
            
            // now add everything
            variance = variance_leg1 + variance_leg2 + variance_leg2_cond + variance_singleHit;

          }
        }
        
      }
    }
    _k_evt_weight = (1. - evt_weight);
    _variance = variance;
    _variance_sys = variance_sys;
  }
  _isComputed = true;
}
