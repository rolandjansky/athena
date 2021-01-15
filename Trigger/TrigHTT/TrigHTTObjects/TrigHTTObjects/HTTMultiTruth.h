/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef WRAP_MULTITRUTH_H
#define WRAP_MULTITRUTH_H

// HTTMultiTruth
// ================================================================
// code to match clusters and tracks to GEANT charge deposition information
// ================================================================
// 20-04-2009 Antonio Boveia (boveia@hep.uchicago.edu)

#include <map>
#include <utility>
#include <limits>
#include <algorithm>
#include <numeric>
#include <TObject.h>
#include <iostream>
#include "TrigHTTUtils/HTTMacros.h"

// HTTMultiTruth
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

class
HTTMultiTruth : public TObject
{
public:
  typedef std::pair<long int,long int> Barcode; // = (event index, barcode)
  typedef float Weight;
  struct MaxAccumulator {
    HTTMultiTruth operator()( const HTTMultiTruth& result , const HTTMultiTruth& a ) const { return result.maximize(a); }
  };
  struct AddAccumulator {
    HTTMultiTruth operator()( const HTTMultiTruth& result , const HTTMultiTruth& a ) const { return result.add(a); }
  };
protected:
  typedef std::map<Barcode,Weight> TruthMap;
  struct TruthMapWeightLt {
    bool operator()( const TruthMap::value_type& a , const TruthMap::value_type& b ) const {
      const bool a_info = a.first.first!=-1 && a.first.second!=-1;
      const bool b_info = b.first.first!=-1 && b.first.second!=-1;
      if( a_info && b_info ) {
        return( a.second < b.second );
      } else {
        return b_info;
      }
    }
  };
  struct TruthMapWeightAcc {
    HTTMultiTruth::Weight operator()( const HTTMultiTruth::Weight& result , const TruthMap::value_type& a ) const { return( result + a.second ); }
  };
  TruthMap      m_truth;
 private:
 public:

 HTTMultiTruth() : TObject() {}

 HTTMultiTruth(const HTTMultiTruth::Barcode& code,const HTTMultiTruth::Weight weight=1.) : TObject() { m_truth[code] = weight; }

 HTTMultiTruth(const HTTMultiTruth &cpy) : TObject(cpy) { m_truth = cpy.m_truth; }

  // tricky: overloading derived class assignment operator
  HTTMultiTruth& operator=(const HTTMultiTruth& cpy) {
    if(this != &cpy)
      {
	this->TObject::operator=(cpy);
	m_truth = cpy.m_truth;
      }
    return *this;
  }

  ///  virtual ~HTTMultiTruth() { if( !m_truth.empty() ) m_truth.clear(); }
  ~HTTMultiTruth() { if( !m_truth.empty() ) m_truth.clear(); }

  TruthMap::const_iterator begin() const { return m_truth.begin(); }
  TruthMap::const_iterator end() const { return m_truth.end(); }

  void add(const HTTMultiTruth::Barcode& code,const HTTMultiTruth::Weight& weight) {
    HTT_ASSERT_THROW(weight >= std::numeric_limits<float>::epsilon()); // weight should be non-zero
    TruthMap::iterator i=m_truth.find(code);
    if( i==m_truth.end() ) {
      m_truth[code] = weight;
    } else {
      i->second += weight;
    }
  }

  void add(const HTTMultiTruth& rval) {
    for( TruthMap::const_iterator i=rval.m_truth.begin(), f=rval.m_truth.end(); i!=f; ++i ) {
      TruthMap::iterator j=m_truth.find( i->first );
      if( j!=m_truth.end() ) {
        j->second += i->second;
      } else {
        m_truth[ i->first ] = i->second;
      }
    }
  }

  void assign_equal_normalization() {
    if( m_truth.empty() ) { return; }
    unsigned int ncodes = m_truth.size();
    for( TruthMap::iterator i=m_truth.begin(), f=m_truth.end(); i!=f; ++i ) {
      i->second = 1./ncodes;
    }
  }

  void maximize(const HTTMultiTruth::Barcode& code,const HTTMultiTruth::Weight& weight) {
    // if this doesn't have any truth info, assign maxweight_barcode to be the truth.
    // otherwise, this should only have the barcode of the largest weight seen so far;
    // compare with the input barcode and keep the best.
    if( m_truth.empty() ) {
      m_truth[ code ] = weight;
    } else {
      HTT_EQUAL_THROW(m_truth.size(), 1); // want exactly one truth value or else throw an exception
      if( m_truth.begin()->second < weight ) {
        m_truth.clear();
        m_truth[ code ] = weight;
      }
    }
  }

  void maximize(const HTTMultiTruth& rval) {
    // if rval has no truth info, do nothing.
    if( rval.m_truth.empty() ) { return; }
    // find input barcode with maximum weight;
    Weight maxweight=0.;
    Barcode maxweight_barcode(-1l,-1l);
    for( TruthMap::const_iterator i=rval.m_truth.begin(), f=rval.m_truth.end(); i!=f; ++i ) {
      if( i->second > maxweight ) {
        maxweight = i->second;
        maxweight_barcode = i->first;
      }
    }
    maximize(maxweight_barcode,maxweight);
  }

  // add and mult for std::accumulate
  HTTMultiTruth add(const HTTMultiTruth& rval) const {
    HTTMultiTruth result(*this);
    result.add(rval);
    return( result );
  }

  HTTMultiTruth maximize(const HTTMultiTruth& rval) const {
    HTTMultiTruth result(*this);
    result.maximize(rval);
    return( result );
  }

  bool isEmpty() const {
    if( m_truth.empty() ) { return true; }
    return false;
  }

  // matching probability definition and maximization logic
  inline HTTMultiTruth::Weight total_weight() const { return std::accumulate( m_truth.begin() , m_truth.end() , 0. , TruthMapWeightAcc() ); }

  inline HTTMultiTruth::Weight weight(const HTTMultiTruth::Barcode& code) const {
    if( m_truth.empty() ) { return 0.; }
    TruthMap::const_iterator i=m_truth.find(code);
    if( i==m_truth.end() ) { return 0.; }
    const HTTMultiTruth::Weight tot = total_weight();
    if (tot>0.) return( i->second / tot );
    return 0.;
  }

  inline bool best( HTTMultiTruth::Barcode& code , HTTMultiTruth::Weight& weight ) const {
    if( m_truth.empty() ) { return false; }
    TruthMap::const_iterator i = std::max_element( m_truth.begin() , m_truth.end() , TruthMapWeightLt() );
    code = i->first;
    const HTTMultiTruth::Weight tot = total_weight();
    weight = tot>0. ? i->second/tot : 0.;
    return true;
  }

  inline unsigned int multiplicity() const { return m_truth.size(); }
  long int best_barcode() const;

  // this is a TObject to be stored in the TrigHTTSim ROOT output streams
  ClassDef(HTTMultiTruth,2);
};

#endif // WRAP_MULTITRUTH_H
