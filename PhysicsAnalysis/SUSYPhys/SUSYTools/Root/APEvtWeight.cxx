/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define APEvtWeight_cxx
#include "SUSYTools/APEvtWeight.h"
#include "SUSYTools/APWeightEntry.h"
#include <cmath>

#ifndef SUSY_ATHENA
ClassImp(APEvtWeight)
#endif

APEvtWeight::APEvtWeight() {
  _n_entries = 0;
  isComputed = false;
}

APEvtWeight::~APEvtWeight() {
}

void APEvtWeight::AddWeightToEvt(APWeightEntry* weight) {
  _current_evt_weights.push_back(weight->GetExpectancy());
  _current_evt_variances.push_back(weight->GetVariance());
  _current_evt_sysuncert2s.push_back(weight->GetSysUncert2());
  ++_n_entries;
  isComputed = false;
}

const APEvtWeight operator&&(const APEvtWeight& a_in, const APEvtWeight& b_in) {
  APEvtWeight ret;
  APEvtWeight a = a_in;
  APEvtWeight b = b_in;
  if (!b.isComputed) b.Compute();
  if (!a.isComputed) a.Compute();
  ret._n_entries = a._n_entries + b._n_entries;
  ret._k_evt_weight = a._k_evt_weight * b._k_evt_weight;
  ret._variance = a._variance * b._k_evt_weight * b._k_evt_weight + b._variance * a._k_evt_weight * a._k_evt_weight;
  ret._variance_sys = a._variance_sys * b._k_evt_weight * b._k_evt_weight + b._variance_sys * a._k_evt_weight * a._k_evt_weight;
  ret.isComputed = true;
  return ret;
}

const APEvtWeight operator||(const APEvtWeight& a_in, const APEvtWeight& b_in) {
  APEvtWeight ret;
  APEvtWeight a = a_in;
  APEvtWeight b = b_in;
  if (!b.isComputed) b.Compute();
  if (!a.isComputed) a.Compute();
  ret._n_entries = a._n_entries + b._n_entries;
  ret._k_evt_weight = 1.0 - (1.0 - a._k_evt_weight) * (1.0 - b._k_evt_weight);
  ret._variance = a._variance * (1.0 - b._k_evt_weight) * (1.0 - b._k_evt_weight) + b._variance * (1.0 - a._k_evt_weight) * (1.0 - a._k_evt_weight);
  ret._variance_sys = a._variance_sys * (1.0 - b._k_evt_weight) * (1.0 - b._k_evt_weight) + b._variance_sys * (1.0 - a._k_evt_weight) * (1.0 - a._k_evt_weight);
  ret.isComputed = true;
  return ret;
}

double APEvtWeight::GetWeight() {
  if (!isComputed) Compute();
  return _k_evt_weight;
}

double APEvtWeight::GetStdDev() {
  if (!isComputed) Compute();
  return sqrt(_variance);
}

double APEvtWeight::GetSysUncert() {
  if (!isComputed) Compute();
  return sqrt(_variance_sys);
}

unsigned long APEvtWeight::NEntries() {
  return _n_entries;
}

void APEvtWeight::Compute() {
  double evt_weight = 1.0;
  double variance = 0.0;
  double variance_sys = 0.0;
  for (unsigned int i = 0, I = _current_evt_weights.size(); i < I; ++i) {
    double variance_summand = 1.0;
    evt_weight *= (1. - _current_evt_weights[i]);
    for (unsigned int k = 0; k < I; ++k) {
      if (i != k) variance_summand *= (1. - _current_evt_weights[k]);
    }
    variance += (variance_summand * variance_summand * _current_evt_variances[i]);
    variance_sys += (variance_summand * variance_summand * _current_evt_sysuncert2s[i]);
  }
  _k_evt_weight = (1. - evt_weight);
  _variance = variance;
  _variance_sys = variance_sys;
  isComputed = true;
}
