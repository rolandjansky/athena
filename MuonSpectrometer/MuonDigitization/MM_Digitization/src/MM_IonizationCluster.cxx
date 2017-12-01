/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MM_Digitization/MM_IonizationCluster.h"

using namespace std;

MM_NelectronProb MM_IonizationCluster::NelectronProb;

MM_IonizationCluster::MM_IonizationCluster () {}

MM_IonizationCluster::MM_IonizationCluster (float _HitX, float _IonizationX, float _IonizationY) : HitX(_HitX), IonizationStart(_IonizationX, _IonizationY) {}

MM_IonizationCluster::MM_IonizationCluster (const MM_IonizationCluster& _MM_IonizationCluster) {

  HitX = _MM_IonizationCluster.getHitX();
  IonizationStart = _MM_IonizationCluster.getIonizationStart();
  Electrons = _MM_IonizationCluster.getElectrons();

}

void MM_IonizationCluster::createElectrons(TRandom3* rndm) {

  gRandom = rndm;
  int Nelectron = MM_IonizationCluster::NelectronProb.FindBin(MM_IonizationCluster::NelectronProb.GetRandom());
  Electrons.reserve(Nelectron);
  for (int iElectron = 0; iElectron<Nelectron; iElectron++)
    Electrons.push_back(new MM_Electron(IonizationStart.X(), IonizationStart.Y()));
}

void MM_IonizationCluster::propagateElectrons(float lorentzAngle, float driftVel) {

  for (auto& Electron : Electrons)
    Electron->propagateElectron(lorentzAngle, driftVel);

}

// accessors
std::vector<MM_Electron*> MM_IonizationCluster::getElectrons() const { return Electrons; }
