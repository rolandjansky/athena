/* emacs: this is -*- c++ -*- */
/**
 **     @file    generate.h
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:06:39 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/

#ifndef GENERATE_H
#define GENERATE_H


#include <cmath>
#include <vector>
#include <iostream>

#include "TH1D.h"

#include "BasicRandom.h"


namespace generate { 

/// mean and rms 
double get_mean( const std::vector<double>& m );
double get_rms( const std::vector<double>& m );


/// generate an integrated distribution
TH1D* integrate( TH1D* hin );

/// scale a distribution by the integral
void ScaleIntegral(TH1D* hin );

/// generate a (normalised) pdf from a distribution
TH1D* PDF(TH1D* hin);

/// smooth a distribution about the maximum
/// NB: maximum is not smoothed
TH1D* smooth( TH1D* hin, bool sym=true );


/// base class for a "generator" class
class generator_base { 
public:
  virtual ~generator_base() { }
  virtual double generate() const = 0;
};



/// breit-wigner generator
class breit_generator : public generator_base { 

 public: 
  
  virtual double generate() const {
    static BasicRandom r;
    double x = std::tan((r.uniform()-0.5)*M_PI);
    return x;
  }
  
};




/// generate a distribution according to an input 
/// histogram (after smoothing)
class hist_generator : public generator_base { 

public:

  hist_generator(TH1D* h, bool _smooth=true );
  
  virtual ~hist_generator() { delete ms; if ( mrandom ) delete mrandom; }

  /// actually generate a random number from the distribution
  double generate() const {
    return invert(mrandom->uniform()); 
  }

  TH1D* histogram()       { return ms; }
  TH1D* rawhistogram()    { return mraw; }
  TH1D* smoothhistogram() { return msmooth; }

private:

  int getbin( double y ) const {
    for ( unsigned i=0 ; i<my.size() ; i++ ) if ( y<=my[i] ) return i; 
    return my.size()-1;
  } 
    
  double invert( double y ) const {
    int i = getbin(y);
    if ( i==0 ) return 0;
    else return (y-my[i-1])*mdxdy[i-1]+mx[i-1];
  }


private: 
 
  std::vector<double> my;
  std::vector<double> mx;
  

  std::vector<double> mdx;
  std::vector<double> mdy;
  
  std::vector<double> mdxdy;

  TH1D* ms;   // accumated histogram
  TH1D* mraw;    // raw histogram
  TH1D* msmooth; // smoothed raw histogram

  BasicRandom* mrandom;

};




/// given a histogram, get a generator for the distribution 
/// in that histogram, then run some pseudo-experiments and 
/// collect the mean and rms  

class experiment { 

public:

  experiment( TH1D* h, int Nexperiments=10, int fevents=0 );

  double hmean() const { return m_hmean; }  
  double hrms()  const { return m_hrms; }  

  double mean()       const { return m_global_mean; }  
  double mean_error() const { return m_global_mean_error; }  

  double rms()       const { return m_global_rms; }  
  double rms_error() const { return m_global_rms_error; }  

  generator_base* gen() { return mg; }

  TH1D* rmshisto() { return m_THrms; } 
  
private:

  int m_N; /// number of experiments

  std::vector<double> m_mean;  /// means from each experiment
  std::vector<double> m_rms;   /// rms from each experiment

  double m_hmean; /// actual histogram mean95
  double m_hrms;  /// actual histogram rms95

  /// overall mean and rms from the pseudo experiments
  double m_global_mean;
  double m_global_mean_error;

  double m_global_rms;
  double m_global_rms_error;
    
  generator_base* mg;

  TH1D* m_THrms;
};



} // namespace generate

#endif // GENERATE_H
