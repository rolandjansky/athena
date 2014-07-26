/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////////
//                      Random Number class for ExHuME
//                     Written by A.Pilkington 21/02/06
//
//          Change whats in operator if you want a different rand()
//
////////////////////////////////////////////////////////////////////////////////

#ifndef EXHUME_RANDOM_GENERATOR_HH
#define EXHUME_RANDOM_GENERATOR_HH

#include <cmath>
#include <functional>

#include "CLHEP/Random/RandFlat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

namespace Exhume{
  class RandomNumber{
  public:
    RandomNumber(){
      Exhume_stream = "ExhumeRand";
      p_AtRndmGenSvc=0;
      return;
    }
    double operator()(){
      CLHEP::HepRandomEngine* engine= p_AtRndmGenSvc->GetEngine(Exhume_stream);
      return(CLHEP::RandFlat::shoot(engine));
      //return(1.0);
    }
    
    void SetSeeds(int _one){
      srand(_one);
    }
    static IAtRndmGenSvc* p_AtRndmGenSvc;
    static std::string Exhume_stream;
    static std::string Pythia_stream;
  };
}
#endif
