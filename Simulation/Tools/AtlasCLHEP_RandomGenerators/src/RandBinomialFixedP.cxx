/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#define __STDCPP_WANT_MATH_SPEC_FUNCS__ 1
#include <cmath>
#include <iostream>
#include <algorithm>

#include "AtlasCLHEP_RandomGenerators/RandBinomialFixedP.h"

namespace CLHEP {

std::string RandBinomialFixedP::name() const {return "RandBinomialFixedP";}

RandBinomialFixedP::~RandBinomialFixedP() {
}

inline double binom(int n, int k) { 
  return 1/((n+1)*std::beta(n-k+1,k+1)); 
}

void RandBinomialFixedP::initLookupTable(long Nmaxlookup, double p) {
  defaultP=p;
  LookupTable.resize(Nmaxlookup);
  for(unsigned int n=1;n<=Nmaxlookup;++n) {
    unsigned int in=n-1;
    double q=1-p;
    LookupTable[in].resize(n);
    //LookupTable[in][n]=1 always, so don't need to store
    for(unsigned int k=0;k<n;++k) {
      //p(k;n,p)=n!/(k!*(n-k)!)*p^k*(1-p)^(n-k)
      double prop=binom(n,k)*std::pow(p,k)*std::pow(q,n-k);
      if(k>0) prop+=LookupTable[in][k-1];
      LookupTable[in][k]=prop;
    }
  }
}

double RandBinomialFixedP::fire( HepRandomEngine* anEngine, long n ) {
  if(n==1) {
    double prop=anEngine->flat();
    if(prop>defaultP) return 0;
    return 1;
  }
  if(n>(long)LookupTable.size()) return genBinomial( anEngine, n, defaultP );
  
  auto& table=LookupTable[n-1];
  
  double prop=anEngine->flat();
  auto itk = std::upper_bound(table.begin(),table.end(),prop);
  return std::distance(table.begin(),itk);
}

void RandBinomialFixedP::fireArray( const int size, double* vect)
{
  for( double* v = vect; v != vect+size; ++v )
    *v = fire(defaultN);
}

void RandBinomialFixedP::fireArray( const int size, double* vect,
                           long n )
{
  for( double* v = vect; v != vect+size; ++v )
    *v = fire(n);
}

}  // namespace CLHEP
