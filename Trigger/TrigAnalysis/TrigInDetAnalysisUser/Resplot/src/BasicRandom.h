/* emacs: this is -*- c++ -*- */
/**
 **     @file    BasicRandom.h
 **
 **       a Randomnumber generator with a data member TRandom3
 **       so that we can either use a shared generator
 **       or a dediceted one.
 **       NB: there will be seperate instances of this class
 **       but in the case of a shared generator, they will 
 **       all use the same TRandom3
 **       uses the root TRandom3 Mersenne Twistor generator, 
 **       623-diminsionally equidistributed uniform generator.                
 **       from ACM Trans. Mod. Comp. Sim. Vol 8-1 (1998) 3.
 **       This wrapper is to avoid global variables cluttering 
 **       up the place - root is bad enough as it is.
 **
 **     @author  mark sutton
 **     @date    Fri 27 Jan 2012 11:39:47 GMT 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef   BASICRANDOM_H
#define   BASICRANDOM_H

#include <iostream>

#include "TRandom3.h"


class BasicRandom {

public:

  BasicRandom(int shared=true, int seed=4357) 
    : m_shared(shared), m_random(0) { 
    if ( shared ) { 
      static TRandom3 _r(seed);
      m_random = &_r;
    }
    else {
      m_random = new TRandom3(seed);
    } 
  }

  //  virtual ~Random() { delete m_rand; } 
  virtual ~BasicRandom() {  if ( !m_shared ) delete m_random; } 
  
  double exp()     { return m_random->Exp(1); } 
  double gauss()   { return m_random->Gaus(); } 
  double uniform() { return m_random->Uniform(); } 


private:
  
  bool      m_shared;
  TRandom3* m_random;

};


#endif  /// BASICRANDOM_H 










