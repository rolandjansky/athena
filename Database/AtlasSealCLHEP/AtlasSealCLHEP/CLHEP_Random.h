/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CLHEP/Random/Randomize.h"
#include "CxxUtils/unused.h"

namespace {
  // Suppress unused variable warning from clhep header.
  static const int UNUSED(HepRandomGenActive_x) = CLHEP::HepRandomGenActive;
}

//----Functions missing in the implementation and required by the dictionary
//    [ Bug #1806 ] Missing method implementations

inline void CLHEP::RandPoissonT::shootArray(class CLHEP::HepRandomEngine *,int,long *,double) {}
inline void CLHEP::RandPoissonT::fireArray(int,long *) {}
inline void CLHEP::RandPoissonQ::shootArray(class CLHEP::HepRandomEngine *,int,long *,double){}
inline void CLHEP::RandPoissonQ::fireArray(int,long *) {}
//// DRQ: The following function was removed in CLHEP 1.9.2.1
////inline void HepRandomEngine::getTableSeeds(long *,int)const  {}

