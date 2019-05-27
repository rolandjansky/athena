/* emacs: this is -*- c++ -*- */
/**
 **     @file    Random.h
 **
 **       singleton random number generator wrapper, for the 
 **       root TRandom3 Mersenne Twistor generator, 
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


#ifndef   RANDOM_H
#define   RANDOM_H

#include <iostream>

#include "TRandom3.h"

class Random : TRandom3 {

public:

  static Random& RandomBuilder() { 
    static Random _r;
    return _r;
  }

  //  virtual ~Random() { delete m_rand; } 
  virtual ~Random() { } 
  
  static double exp()     { return RandomBuilder().Exp(1); } 
  static double gauss()   { return RandomBuilder().Gaus(); } 
  static double uniform() { return RandomBuilder().Uniform(); } 


private:
  
  Random(int seed=4357) : TRandom3(seed) { } 

};

inline std::ostream& operator<<( std::ostream& s, const Random& _r ) { 
  return s;
}



#endif  //   RANDOM_H 










