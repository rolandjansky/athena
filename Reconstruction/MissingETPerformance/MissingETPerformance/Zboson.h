/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Z_BOSON_H
#define Z_BOSON_H

#include <TLorentzVector.h>
#include <TVector2.h>
#include <math.h>

class Zboson {
 private:
  TLorentzVector Z;

  TLorentzVector daughter1;
  TLorentzVector daughter2;

 public:
  Zboson() {;}
  Zboson(TLorentzVector a, TLorentzVector b);

  double mass() {return Z.M();}
  double Px() {return Z.Px();}
  double Py() {return Z.Py();}
  double Pt() {return Z.Pt();}
  double Phi() {return Z.Phi();}
  double Eta() {return Z.Eta();}
  double SignedDeltaPhi() {return daughter1.DeltaPhi(daughter2);}
  double DeltaPhi() {return fabs(daughter1.DeltaPhi(daughter2));}

  const TLorentzVector &get_daughter1() {return daughter1;}
  const TLorentzVector &get_daughter2() {return daughter2;}

  TVector2 GetVec();
  TVector2 GetPerpendicular();
  TVector2 GetParallel();
  TVector2 GetThrust();
  TVector2 GetAntiThrust();
  TVector2 GetPtAxis();
  TVector2 GetAntiPtAxis();
  TVector2 GetAL();
  TVector2 GetAxis();
  TVector2 GetAntiAxis();
  double Res_Axis();
  double Res_AntiAxis();
  double Res_Perp(TVector2 object);
  double Res_Par(TVector2 object);
  double Res_AL(TVector2 object);
  //double GetHadronicRecoil();
  double GetZbosonPt();
  double GetZbosonPerp();
  double GetZbosonPar();
  double GetDiLeptonPerp();
  double GetDiLeptonPar();
};

#endif
