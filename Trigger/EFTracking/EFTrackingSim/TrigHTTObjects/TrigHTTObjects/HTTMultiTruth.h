/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHTTOBJECTS_MULTITRUTH_H
#define TRIGHTTOBJECTS_MULTITRUTH_H

#include <TObject.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

// HTTMultiTruth
// ================================================================
// code to match clusters and tracks to GEANT charge deposition information
// ================================================================
// 20-04-2009 Antonio Boveia (boveia@hep.uchicago.edu)
//
// class which represents the relative contributions of one or more
// truth particles (identified by a unique "barcode") to some
// measurement. for example, a single silicon pixel may record the
// summed ionization from two different charged particles passing
// through it during the same event. one particle may (and likely
// will) contribute more charge than another, so the class stores a
// weight for each barcode which can be used to incorporate this
// information.
//
// there are two ways of combining the information: the "add" function
// and the "maximize" function. "add" sums the weights of individual
// contributions, appropriate for combining the truth for individual
// hits to obtain the truth for a track. in this case, "best" then
// returns the barcode with the largest sum of weights. "maximize"
// simply remembers the barcode with the greatest weight, and "best"
// then returns that barcode.
//

class HTTMultiTruth : public TObject {
public:

  typedef std::pair<long, long> Barcode; // = (event index, barcode)
  typedef float Weight;
  typedef std::map<Barcode, Weight> TruthMap;

  HTTMultiTruth() = default;
  HTTMultiTruth(const HTTMultiTruth::Barcode& code, const HTTMultiTruth::Weight weight = 1.) { m_truth[code] = weight; }
  virtual ~HTTMultiTruth() = default;

  struct AddAccumulator {
    auto operator()(const HTTMultiTruth& result, const HTTMultiTruth& a) const { return result.add(a); }
  };

  struct MaxAccumulator {
    auto operator()(const HTTMultiTruth& result, const HTTMultiTruth& a) const { return result.maximize(a); }
  };

  auto begin() { return m_truth.begin(); }
  auto end() { return m_truth.end(); }
  auto begin() const { return m_truth.begin(); }
  auto end() const { return m_truth.end(); }

  bool isEmpty() const { return m_truth.empty(); }

  long best_barcode() const;

  void add(const HTTMultiTruth::Barcode& code, const HTTMultiTruth::Weight& weight);
  void add(const HTTMultiTruth& rval);

  void maximize(const HTTMultiTruth::Barcode& code, const HTTMultiTruth::Weight& weight);
  void maximize(const HTTMultiTruth& rval);

  void assign_equal_normalization();
  void display() const; // write contents of this object to std::cout

  inline unsigned multiplicity() const { return m_truth.size(); }

  // Finds the best barcode and its normalized weight, returning them by reference.
  // Returns true on success.
  inline bool best(HTTMultiTruth::Barcode& code, HTTMultiTruth::Weight& weight) const
  {
    if (m_truth.empty()) return false;
    auto i = std::max_element(m_truth.begin(), m_truth.end(), TruthMapWeightLt());
    code = i->first;
    weight = total_weight() > 0. ? (i->second) / total_weight() : 0.;
    return true;
  }


private:

  struct TruthMapWeightAcc {
    auto operator()(const HTTMultiTruth::Weight& result, const TruthMap::value_type& a) const { return result + a.second; }
  };

  struct TruthMapWeightLt {
    bool operator()(const TruthMap::value_type& a, const TruthMap::value_type& b) const {
      const bool a_info = (a.first.first != -1) && (a.first.second != -1);
      const bool b_info = (b.first.first != -1) && (b.first.second != -1);
      return a_info && b_info ? a.second < b.second : b_info;
    }
  };

  // add and mult for std::accumulate
  const HTTMultiTruth add(const HTTMultiTruth& rval) const;
  const HTTMultiTruth maximize(const HTTMultiTruth& rval) const;

  // matching probability definition and maximization logic
  inline HTTMultiTruth::Weight total_weight() const { return std::accumulate(m_truth.begin(), m_truth.end(), 0., TruthMapWeightAcc()); }

  inline HTTMultiTruth::Weight weight(const HTTMultiTruth::Barcode& code) const {
    return m_truth.empty() || (m_truth.find(code) == m_truth.end()) ? 0. : ((m_truth.find(code))->second) / total_weight();
  }

  TruthMap m_truth;


  ClassDef(HTTMultiTruth, 2) // this is a TObject to be stored in the TrigHTTSim ROOT output streams
};

#endif // TRIGHTTOBJECTS_MULTITRUTH_H
