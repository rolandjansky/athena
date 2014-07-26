/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-*-c++-*-
//-*-QQ.h-*-
//   Written by James Monk and Andrew Pilkington
/////////////////////////////////////////////////////////////////////////////
#ifndef QQ_HH
#define QQ_HH

#include "TwoSpace.h"

namespace Exhume{
  class QQ : public TwoSpace{

  public:
    QQ(std::string _ExhumeControlParams);
    
    //declare inherited functions
    double SubProcess();
    void LIPS2Amp();
    void Amp2LIPS();
    void SetQuarkType(const int&);
  private:
    double InvSinTheta;

    /*
    void SetPartons();
    void SetSubParameters();
    double SubParameterWeight();
    void MaximiseSubParameters();
    double SubParameterRange();
      
    //declare sub-process specific functions
    void SetThetaMin(const double&);
		void SetQuarkType(const int&);
  private:
    //internal functions
    double WeightFunc(const double&);
    //internal Parameters
    double qMass,qMassSq;
    double CosTheta,CosThetaMin,CosThetaMax;
    double ThetaMin,ThetaMax, SinThetaMin;
    double Theta,SinTheta,InvSinTheta; 
    double E,P,Px,Py,Pz,Phi;
    double SubWgt;
    */
  };
}
#endif
