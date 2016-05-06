/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef WRAP_MULTITRUTH_H
#define WRAP_MULTITRUTH_H

// MultiTruth
// ================================================================
// code to match clusters and tracks to GEANT charge deposition information
// ================================================================
// 20-04-2009 Antonio Boveia (boveia@hep.uchicago.edu)

#include <map>
#include <utility>
#include <limits>
#include <algorithm>
#include <numeric>
#include <cassert>
#include <TObject.h>

// MultiTruth
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
MultiTruth : public TObject
{
public:
  typedef std::pair<long int,long int> Barcode; // = (event index, barcode)
  typedef float Weight;
  struct MaxAccumulator {
    MultiTruth operator()( const MultiTruth& result , const MultiTruth& a ) const { return result.maximize(a); }
  };
  struct AddAccumulator {
    MultiTruth operator()( const MultiTruth& result , const MultiTruth& a ) const { return result.add(a); }
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
    MultiTruth::Weight operator()( const MultiTruth::Weight& result , const TruthMap::value_type& a ) const { return( result + a.second ); }
  };
  TruthMap      _truth;
private:
public:
  MultiTruth() : TObject() {}

  MultiTruth(const MultiTruth::Barcode& code,const MultiTruth::Weight weight=1.) : TObject() { _truth[code] = weight; }

  MultiTruth(const MultiTruth &cpy) : TObject(cpy) { _truth = cpy._truth; }

  // tricky: overloading derived class assignment operator
  MultiTruth& operator=(const MultiTruth& cpy) {
#ifdef PROTECT_SA // speedup
    if(this != &cpy)
#endif
      {
	this->TObject::operator=(cpy);
	_truth = cpy._truth;
      }
    return *this;
  }

  virtual ~MultiTruth() { if( !_truth.empty() ) _truth.clear(); }

  TruthMap::const_iterator begin() const { return _truth.begin(); }
  TruthMap::const_iterator end() const { return _truth.end(); }

  void add(const MultiTruth::Barcode& code,const MultiTruth::Weight& weight) {
    assert( weight >= 0. );
    TruthMap::iterator i=_truth.find(code);
    if( i==_truth.end() ) {
      _truth[code] = weight;
    } else {
      i->second += weight;
    }
  }

  void add(const MultiTruth& rval) {
    for( TruthMap::const_iterator i=rval._truth.begin(), f=rval._truth.end(); i!=f; ++i ) {
      TruthMap::iterator j=_truth.find( i->first );
      if( j!=_truth.end() ) {
        j->second += i->second;
      } else {
        _truth[ i->first ] = i->second;
      }
    }
  }

  void assign_equal_normalization() {
    if( _truth.empty() ) { return; }
    unsigned int ncodes = _truth.size();
    for( TruthMap::iterator i=_truth.begin(), f=_truth.end(); i!=f; ++i ) {
      i->second = 1./ncodes;
    }
  }

  void maximize(const MultiTruth::Barcode& code,const MultiTruth::Weight& weight) {
    // if this doesn't have any truth info, assign maxweight_barcode to be the truth.
    // otherwise, this should only have the barcode of the largest weight seen so far;
    // compare with the input barcode and keep the best.
    if( _truth.empty() ) {
      _truth[ code ] = weight;
    } else {
      assert( _truth.size() == 1 );
      if( _truth.begin()->second < weight ) {
        _truth.clear();
        _truth[ code ] = weight;
      }
    }
  }

  void maximize(const MultiTruth& rval) {
    // if rval has no truth info, do nothing.
    if( rval._truth.empty() ) { return; }
    // find input barcode with maximum weight;
    Weight maxweight=0.;
    Barcode maxweight_barcode(-1l,-1l);
    for( TruthMap::const_iterator i=rval._truth.begin(), f=rval._truth.end(); i!=f; ++i ) {
      if( i->second > maxweight ) {
        maxweight = i->second;
        maxweight_barcode = i->first;
      }
    }
    maximize(maxweight_barcode,maxweight);
  }

  // add and mult for std::accumulate
  const MultiTruth add(const MultiTruth& rval) const {
    MultiTruth result(*this);
    result.add(rval);
    return( result );
  }

  const MultiTruth maximize(const MultiTruth& rval) const {
    MultiTruth result(*this);
    result.maximize(rval);
    return( result );
  }

  // matching probability definition and maximization logic
  inline MultiTruth::Weight total_weight() const { return std::accumulate( _truth.begin() , _truth.end() , 0. , TruthMapWeightAcc() ); }

  inline MultiTruth::Weight weight(const MultiTruth::Barcode& code) const {
    if( _truth.empty() ) { return 0.; }
    TruthMap::const_iterator i=_truth.find(code);
    if( i==_truth.end() ) { return 0.; }
    const MultiTruth::Weight tot = total_weight();
    return( i->second / tot );
  }

  inline bool best( MultiTruth::Barcode& code , MultiTruth::Weight& weight ) const {
    if( _truth.empty() ) { return false; }
    TruthMap::const_iterator i = std::max_element( _truth.begin() , _truth.end() , TruthMapWeightLt() );
    code = i->first;
    const MultiTruth::Weight tot = total_weight();
    weight = tot>0. ? i->second/tot : 0.;
    return true;
  }

  inline unsigned int multiplicity() const { return _truth.size(); }
  long int best_barcode() const;
  // write contents of this object to stdout
  void display() const;
  // this is a TObject to be stored in the TrigFTKSim ROOT output streams
  ClassDef(MultiTruth,3);
};

#endif // WRAP_MULTITRUTH_H
