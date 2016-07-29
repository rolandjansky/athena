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
  TLorentzVector m_Z;

  TLorentzVector m_daughter1;
  TLorentzVector m_daughter2;

 public:
  Zboson() {;}
  Zboson(TLorentzVector a, TLorentzVector b);

  double mass() {return m_Z.M();}
  double Px() {return m_Z.Px();}
  double Py() {return m_Z.Py();}
  double Pt() {return m_Z.Pt();}
  double Phi() {return m_Z.Phi();}
  double Eta() {return m_Z.Eta();}
  double SignedDeltaPhi() {return m_daughter1.DeltaPhi(m_daughter2);}
  double DeltaPhi() {return fabs(m_daughter1.DeltaPhi(m_daughter2));}

  const TLorentzVector &get_daughter1() {return m_daughter1;}
  const TLorentzVector &get_daughter2() {return m_daughter2;}

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
