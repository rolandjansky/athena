/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MissingETPerformance/Zboson.h"

Zboson::Zboson(TLorentzVector a, TLorentzVector b) {
  daughter1 = a;
  daughter2 = b;

  Z = a + b;
}

TVector2 Zboson::GetVec() {
  TVector2 RetVec(Px(), Py());
  return RetVec;
}


TVector2 Zboson::GetPerpendicular(){
  TVector2 Object1(daughter1.Px(),daughter1.Py());
  TVector2 Object2(daughter2.Px(),daughter2.Py());
  TVector2 perp=(Object1/Object1.Mod())+(Object2/Object2.Mod());
  TVector2 Perpendicular=perp/perp.Mod();
  return Perpendicular;
}

TVector2 Zboson::GetParallel(){
  TVector2 Parallel=GetPerpendicular().Rotate(TMath::Pi()/2);
  return Parallel;
}

TVector2 Zboson::GetPtAxis(){
  TVector2 Object1(daughter1.Px(),daughter1.Py());
  TVector2 Object2(daughter2.Px(),daughter2.Py());
  TVector2 Sum=(Object1+Object2);
  TVector2 PtAxis=Sum/Sum.Mod();
  return PtAxis;
}

TVector2 Zboson::GetAntiPtAxis(){
  TVector2 AntiPtAxis=GetPtAxis().Rotate(TMath::Pi()/2);
  return AntiPtAxis;
}

TVector2 Zboson::GetThrust(){
  TVector2 Object1(daughter1.Px(),daughter1.Py());
  TVector2 Object2(daughter2.Px(),daughter2.Py());
  TVector2 difference=Object1-Object2;
  TVector2 Thrust=difference/difference.Mod();
  return Thrust;
}

TVector2 Zboson::GetAntiThrust(){ 
  TVector2 AntiThrust=GetThrust().Rotate(TMath::Pi()/2);
  return AntiThrust;
}
  
TVector2 Zboson::GetAL(){
  TVector2 AntiThrust=GetAntiThrust();
  TVector2 PtAxis=GetPtAxis();
  TVector2 Axis;

  if(DeltaPhi()<=0.5*TMath::Pi())
    Axis=PtAxis;
  else
    Axis=AntiThrust;
  return Axis;
}

TVector2 Zboson::GetAxis(){
  TVector2 Perpendicular=GetPerpendicular();
  TVector2 Parallel=GetParallel();
  TVector2 Axis;

  if(DeltaPhi()<=0.5*TMath::Pi())
    Axis=Parallel;
  else
    Axis=Perpendicular;
  return Axis;
}

TVector2 Zboson::GetAntiAxis(){
  return GetAxis().Rotate(TMath::Pi()/2);
}

double Zboson::GetDiLeptonPerp(){
  TVector2 perp=GetPerpendicular();
  TVector2 Object1(daughter1.Px(),daughter1.Py());
  TVector2 Object2(daughter2.Px(),daughter2.Py());
  double diLepPerp=perp*Object1+perp*Object2;
  return diLepPerp/perp.Mod();
}

double Zboson::GetDiLeptonPar(){
  TVector2 par=GetParallel();
  TVector2 Object1(daughter1.Px(),daughter1.Py());
  TVector2 Object2(daughter2.Px(),daughter2.Py());
  double diLepPar=fabs(par*Object1+par*Object2);
  return diLepPar/par.Mod();
}

double Zboson::Res_Perp(TVector2 object){
  return object*GetPerpendicular();
}

double Zboson::Res_Par(TVector2 object){
  return object*GetParallel();
}

double Zboson::Res_AL(TVector2 object){
  return object*GetAL();
}

double Zboson::GetZbosonPt(){
  double Object_Axis=GetVec()*GetPtAxis();
  return Object_Axis;
}

double Zboson::GetZbosonPerp(){
  double Object_Axis=GetVec()*GetPerpendicular();
  return Object_Axis;
}

double Zboson::GetZbosonPar(){
  double Object_Axis=GetVec()*GetParallel();
  return Object_Axis;
}

double Zboson::Res_Axis(){
  double Object_Axis=GetVec()*GetAxis();
  return Object_Axis;
}

double Zboson::Res_AntiAxis(){
  double Object_AntiAxis=GetVec()*GetAntiAxis();
  return Object_AntiAxis;
}
