/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-*-C++-*-
//-*-Higgs.h-*-
//   Written by James Monk and Andrew Pilkington
/////////////////////////////////////////////////////////////////////////////

#ifndef HIGGS_HH
#define HIGGS_HH


#include "CrossSection.h"

namespace Exhume{

  class Higgs : public CrossSection{

  public:

    Higgs(std::string _ExhumeControlParams);

    double SubProcess();
    void SetPartons();
    void SetSubParameters(std::vector<double>);
    double SubParameterWeight();
    void MaximiseSubParameters();
    double SubParameterRange();
    void SetHiggsMass(const double&);
    int GetNumberOfSubParameters(){return(0);}
    inline double GetC(){
      return(C);
    };
    inline std::complex<double> GetPropagator(){
      return(Propagator());
    };

    // Currently does nothing!
    void SetHiggsDecay(const int&);

  private:

    double HiggsWidth_();

    void SetC();

    inline std::complex<double> Propagator(){
      //See hep-ph 9505211

      return(I * (1.0 + I * HiggsWidth/HiggsMass)/
	     (sHat - HiggsMass2 + I * HiggsWidth * sHat/HiggsMass));
    };

    inline std::complex<double> GluGlu2HiggsAmp(){
      return(GGHConst * sHat * AlphaS_ *
	     (Fsf(sHat * FsfTop) + Fsf(sHat * FsfBottom)));
    };

    std::complex<double> GGHConst;
    double AlphaS_, FsfTop, FsfBottom,  NLOConst;
    double HiggsMass2, HiggsWidth, TotWidth;
    double C, One;
    double *BR;


  };

}

#endif
