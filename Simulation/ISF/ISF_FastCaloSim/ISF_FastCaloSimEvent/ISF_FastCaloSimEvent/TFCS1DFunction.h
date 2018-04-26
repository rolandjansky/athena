/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCS1DFunction_h
#define ISF_FASTCALOSIMEVENT_TFCS1DFunction_h

// STL includes
#include <string>

#include "TH1.h"
#include "TTree.h"
#include "ISF_FastCaloSimEvent/TFCSFunction.h"

class TFCS1DFunction:public TFCSFunction
{
  public:
    TFCS1DFunction() {};
    ~TFCS1DFunction() {};
    
    virtual int ndim() const {return 1;};

    ///Function gets array of random numbers rnd[] in the range [0,1) as arguments 
    ///and returns function value in array value.
    ///For a n-dimensional function, value and rnd should both have n elements.
    virtual void rnd_to_fct(float value[],const float rnd[]);
   
    ///Function gets random number rnd in the range [0,1) as argument and returns function value
    virtual double rnd_to_fct(double rnd);

    static double get_maxdev(TH1* , TH1* );

  private:

  ClassDef(TFCS1DFunction,2)  //TFCS1DFunction

};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCS1DFunction+;
#endif

#endif
