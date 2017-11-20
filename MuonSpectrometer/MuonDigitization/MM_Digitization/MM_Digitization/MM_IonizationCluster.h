/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATION_MM_IONIZATIONCLUSTER_H
#define MM_DIGITIZATION_MM_IONIZATIONCLUSTER_H
//
// MM_IonizationCluster.cxx
//     Primary Ionization Cluster made up of primary electrons
//


#include "MM_Digitization/MM_Electron.h"

#include "TRandom3.h"
#include "TVector2.h"
#include "TH1D.h"

#include <vector>
#include <memory>

class MM_NelectronProb {
 public:
  TH1D NelectronProb;
  MM_NelectronProb() {
    NelectronProb = TH1D("NelectronProb", "", 300, 0., 300.);
    NelectronProb.SetBinContent(1,65.6);
    NelectronProb.SetBinContent(2,15);
    NelectronProb.SetBinContent(3,6.4);
    NelectronProb.SetBinContent(4,3.5);
    NelectronProb.SetBinContent(5,2.25);
    NelectronProb.SetBinContent(6,1.55);
    NelectronProb.SetBinContent(7,1.05);
    NelectronProb.SetBinContent(8,0.81);
    NelectronProb.SetBinContent(9,0.61);
    NelectronProb.SetBinContent(10,0.49);
    NelectronProb.SetBinContent(11,0.39);
    NelectronProb.SetBinContent(12,0.30);
    NelectronProb.SetBinContent(13,0.25);
    NelectronProb.SetBinContent(14,0.20);
    NelectronProb.SetBinContent(15,0.16);
    NelectronProb.SetBinContent(16,0.12);
    NelectronProb.SetBinContent(17,0.095);
    NelectronProb.SetBinContent(18,0.0750);
    NelectronProb.SetBinContent(19,0.0630);
    NelectronProb.SetBinContent(20,0.0540);
    NelectronProb.SetBinContent(21,0.0490);
    NelectronProb.SetBinContent(22,0.0450);
    NelectronProb.SetBinContent(23,0.0440);

    for (uint Nelectron = 24; Nelectron <= 300; Nelectron++)
      NelectronProb.SetBinContent(Nelectron, 21.6/((Nelectron)*(Nelectron)));
  }
  int FindBin(double val) { return NelectronProb.FindBin(val); }
  double GetRandom() { return NelectronProb.GetRandom(); }
};

class MM_IonizationCluster {


  // Members supplied by user
  std::vector<MM_Electron*> Electrons;
  float HitX;
  TVector2 IonizationStart;

  static MM_NelectronProb NelectronProb;

 public:


  MM_IonizationCluster ();
  MM_IonizationCluster (float _HitX, float _IonizationX, float _IonizationY);
  MM_IonizationCluster (const MM_IonizationCluster& _MM_IonizationCluster);
  void createElectrons(TRandom3* rndm);
  void diffuseElectrons(float LongitudinalSigma, float TransverseSigma, TRandom3* rndm);
  void propagateElectrons(float lorentzAngle, float driftVel);
  void avalancheElectrons(float gain, TRandom3* rndm);
  std::vector<MM_Electron*> getElectrons() const;
  float getHitX() const { return HitX; }
  TVector2 getIonizationStart() const { return IonizationStart; }
};

#endif
