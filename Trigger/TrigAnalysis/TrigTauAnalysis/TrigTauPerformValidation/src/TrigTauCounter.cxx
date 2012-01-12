/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauPerformValidation/TrigTauCounter.h"

#include <iostream>
#include <math.h>

using namespace std;

TrigTauCounter::TrigTauCounter() : _n(100), _wt(100), _wt2(100), _name(100)
{}

void
TrigTauCounter::print() const {
  vector<string>::const_iterator istr = _name.begin();
  vector<double>::const_iterator in = _n.begin();
  vector<double>::const_iterator iwt = _wt.begin();
  vector<double>::const_iterator iwt2 = _wt2.begin();

  for (in = _n.begin(); in != _n.end(); in++) {

    if (istr < _name.end()) {
      cout << *istr << " ";
    }
    cout << "N:" << *in << " ";
    cout << "Wt: " << *iwt  << " +/- ";
    cout << sqrt(*iwt2);
    cout << endl;
    istr++;
    iwt++;
    iwt2++;
  }
}

void
TrigTauCounter::reset() {
  _name.clear();
  clear();
}

void
TrigTauCounter::clear() {
  _n.clear();
  _wt.clear();
  _wt2.clear();
}

void 
TrigTauCounter::fill(unsigned long entry, double weight) {
  if (entry >= getSize()) {
    _n.resize(entry+1);
    _wt.resize(entry+1);
    _wt2.resize(entry+1);
  }
  _n[entry]++;
  _wt[entry] += weight;
  _wt2[entry] += weight*weight;
}

unsigned long
TrigTauCounter::getSize() const {
  return _wt.size();
}

void
TrigTauCounter::add(const TrigTauCounter& other) {

  if (getSize() == 0) {
    _n.resize(other.getSize());
    _wt.resize(other.getSize());
    _wt2.resize(other.getSize());
  } else if (getSize() != other.getSize()) {
    cout << "TrigTauCounter::add() - Size mismatch!" << endl;
    return;
  }

  for (unsigned long entry = 0; entry < getSize(); entry++) {
    _n[entry] += other.getN(entry);
    _wt[entry] += other.getWt(entry);
    _wt2[entry] += other.getWt2(entry);
  }
}

void 
TrigTauCounter::name(unsigned long entry, string name) {
  if (entry >= _name.size()) {
    _name.resize(entry+1);
  }
  _name[entry] = name;
}

double
TrigTauCounter::getN(unsigned long entry) const {
  return _n[entry];
}

double
TrigTauCounter::getWt(unsigned long entry) const {
  return _wt[entry];
}

double
TrigTauCounter::getWt2(unsigned long entry) const {
  return _wt2[entry];
}

double
TrigTauCounter::getDWt(unsigned long entry) const {
  return sqrt(_wt2[entry]);
}

string
TrigTauCounter::getName(unsigned long entry) const {
  return _name[entry];
}
