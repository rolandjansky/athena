/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Exhume_i/TwoSpace.h"



Exhume::TwoSpace::TwoSpace(std::string _ExhumeControlParams):
  CrossSection( _ExhumeControlParams){
  Partons.resize(2);
 
  dirty_weighting=true;
  PhiMax=0.0;
  PartonMass=0.0; //default, override in constructor of derived class
  MaximumSubProcessValue=0.0;
  this->SetWeightMass(10.0);
  
  CosThetaMax = 0.;
  CosThetaMin = 0.; 
  CosTheta  = 0.;
  MaximumSubProcessCosTheta = 0.;
  Phi  = 0.;
  ThetaMax = 0.;
  ThetaMin = 0.;
}

Exhume::TwoSpace::~TwoSpace(){

}

int Exhume::TwoSpace::GetNumberOfSubParameters(){
  return(2);
}

void Exhume::TwoSpace::SetWeightMass(double _M1){
  WeightMass = _M1;
  return;
}

void Exhume::TwoSpace::SetSubParameters(std::vector<double> _RandList){
  if(_RandList.size()==2){
    if(_RandList[0]>=0.0&&_RandList[0]<=1.0) CosTheta = GetValue(_RandList[0]);
    if(_RandList[1]>=0.0&&_RandList[1]<=1.0) Phi = 2*PI*_RandList[1];
  }
  //std::cout<<CosTheta<<std::endl;
  LIPS2Amp();
}

void Exhume::TwoSpace::MaximiseSubParameters(){
   
 
  if(dirty_weighting==true){
    //  std::cout<<CosThetaMax<<std::endl;
    WeightInit(CosThetaMin,CosThetaMax);
    dirty_weighting=false;
  }
  CosTheta=MaximumSubProcessCosTheta;
  //  std::cout<<CosTheta<<std::endl;
  Phi=PhiMax;
  LIPS2Amp();
  return;
}

void Exhume::TwoSpace::SetPartons(){

  

  Amp2LIPS();//set lips parameters, not needed?
  double _SinTheta = sin(acos(CosTheta));
  double _E = 0.5*SqrtsHat;
  double _Pmod = sqrt(_E*_E - PartonMass*PartonMass); 
  double _Px = _Pmod*_SinTheta*cos(Phi);
  double _Py = _Pmod*_SinTheta*sin(Phi);
  double _Pz = _Pmod*CosTheta;

  

  Partons[0].p.setPx(_Px);
  Partons[0].p.setPy(_Py);
  Partons[0].p.setPz(_Pz);
  Partons[0].p.setE(_E);

  Partons[1].p.setPx(-_Px);
  Partons[1].p.setPy(-_Py);
  Partons[1].p.setPz(-_Pz);
  Partons[1].p.setE(_E);

  Partons[0].p.boost(CentralVector.boostVector());
  Partons[1].p.boost(CentralVector.boostVector());


  return;
}

double Exhume::TwoSpace::SubParameterWeight(){
  //bit of a misnomer SubParameterWeight tells the Event class the difference between the subprocess Maximum and this specific chance
  Amp2LIPS();//not needed?
  double _SubWgt;
  _SubWgt = GetFunc(CosTheta);
  //std::cout<<_SubWgt/MaximumSubProcessValue<<std::endl;
  return(_SubWgt/MaximumSubProcessValue);
}

double Exhume::TwoSpace::SubParameterRange(){

  return(2*PI*TotalIntegral/MaximumSubProcessValue);
}
//
//double Exhume::TwoSpace::SubProcess(){
  //return(AmplitudeSq*TwoParticleLIPS*Amp2CrossSection);
  //define overall factor in font of Msq + DLIPS factor
//}
double Exhume::TwoSpace::WeightFunc(const double& _CosTheta){
  //double _SqrtsHat=sqrt(4*PartonMass*PartonMass + 10*10);
  //double _SqrtsHat=250;
  //SetKinematics(_SqrtsHat,0.0,0.0,0.0,0.0,0.0); 
  //arbitrary weighting at low mass
  //std::cout<<_CosTheta<<"\t"<<_SqrtsHat<<std::endl;  
  CosTheta=_CosTheta;
  //std::cout<<CosTheta<<std::endl;
  Phi=PhiMax;//arbitrary angle but amp might use it but shouldn't
  double _SqrtsHat;
  if(PartonMass>1.0){
    _SqrtsHat = WeightMass;
  }
  else{
    _SqrtsHat=sqrt(4*PartonMass*PartonMass + 10*10);
  }
  SetKinematics(_SqrtsHat,0.0,0.0,0.0,0.0,0.0); 
  LIPS2Amp();//
  double _wgt = SubProcess();//AmplitudeSq();
  if(_wgt>MaximumSubProcessValue){
    MaximumSubProcessCosTheta=_CosTheta;
    MaximumSubProcessValue=_wgt;
    std::cout<< MaximumSubProcessCosTheta<<std::endl;
    std::cout<< MaximumSubProcessValue<<std::endl;
    
  }
  //std::cout<<_wgt<<std::endl;
  return(_wgt);
}

void Exhume::TwoSpace::SetThetaMin(const double& ThetaMin_){
  ThetaMin = ThetaMin_;
  ThetaMax = PI - ThetaMin_;
  // SinThetaMin = sin(ThetaMin);
  CosThetaMin = cos(ThetaMax);
  CosThetaMax = cos(ThetaMin);
  MaximumSubProcessCosTheta=CosThetaMax;
  
  //std::cout<<"called\t"<<CosThetaMin<<std::endl;

  return;
}


//Params outside in event are M,y,t1,t2,phi1,phi2
