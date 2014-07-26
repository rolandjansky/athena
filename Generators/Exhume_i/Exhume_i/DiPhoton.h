/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-*-c++-*-
//-*-DiPhoton.h-*-
//   Written by James Monk and Andrew Pilkington
/////////////////////////////////////////////////////////////////////////////
#ifndef DI_PHOTON_HH
#define DI_PHOTON_HH

#include "TwoSpace.h"

namespace Exhume{
  class DiPhoton : public TwoSpace{

  public:
    DiPhoton(std::string _ExhumeControlParams);
   
    //declare inherited functions
    double SubProcess();
    void LIPS2Amp();
    void Amp2LIPS();
  
  private:
    double MatrixElement();
    //internal functions
 
    //internal Parameters
    double t_;//internal 
   
    double PI2, Inv64PI2;
    int Nc;
    double MatFact;
    int Nup;
    int Ndown;
  };
}
#endif
