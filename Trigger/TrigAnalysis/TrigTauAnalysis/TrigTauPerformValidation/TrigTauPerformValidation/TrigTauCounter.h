/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRIGTAUCOUNTER_H
#define _TRIGTAUCOUNTER_H

#include <vector>
#include <string>

class TrigTauCounter {

public:
  TrigTauCounter();
  virtual ~TrigTauCounter() {}

  void print() const; // Dump all counter values
  void reset(); // Delete everything
  void clear(); // Set all counts to zero

  // Accumulate data into counter
  void fill(unsigned long entry, double weight=1.);

  // Give the counter a name
  void name(unsigned long entry, std::string name);

  // Add contents of another counter
  void add(const TrigTauCounter& otherCounter);

  // Get counter values
  double getN(unsigned long entry) const;
  double getWt(unsigned long entry) const;
  double getWt2(unsigned long entry) const;
  double getDWt(unsigned long entry) const;
  std::string getName(unsigned long entry) const;

  unsigned long getSize() const;

private:
  std::vector<double> _n;
  std::vector<double> _wt;
  std::vector<double> _wt2;

  std::vector<std::string> _name;
};

#endif
