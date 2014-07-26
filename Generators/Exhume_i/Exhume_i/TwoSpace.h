/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-*-c++-*-
//-*-TwoSpace.h-*-
//   Written by James Monk and Andrew Pilkington - 20/08/05

#ifndef TWOSPACE_HH
#define TWOSPACE_HH

#include "CrossSection.h"
#include "Weight.h"

namespace Exhume{

  class TwoSpace : public CrossSection,Weight{

  public:
    TwoSpace(std::string _ExhumeControlParams);
    ~TwoSpace();
    double SubParameterRange();
    void MaximiseSubParameters();
    void SetSubParameters(std::vector<double>);
    void SetPartons();
    virtual double SubProcess()=0;
    double SubParameterWeight();
    void SetThetaMin(const double &);
    int GetNumberOfSubParameters();
    //allows user to define an amplitude (Msq) and use all our pre defined funcs
    //virtual double AmplitudeSq()=0;
    virtual void Amp2LIPS()=0;
    virtual void LIPS2Amp()=0;
    double WeightFunc(const double&);
    void SetWeightMass(double);
    
    

  protected:

    
    double CosTheta,Phi;//lips parameters
    double ThetaMin,ThetaMax;
    double MaximumSubProcessValue,MaximumSubProcessCosTheta;
    double PhiMax,CosThetaMax,CosThetaMin; 
    double PartonMass;
    bool dirty_weighting;
    double WeightMass;
    


  };
}

#endif
