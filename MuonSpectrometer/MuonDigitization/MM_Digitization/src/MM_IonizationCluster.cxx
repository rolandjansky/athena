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
    Electrons.push_back(new MM_Electron(HitX+IonizationStart.X(), IonizationStart.Y()));
  // I'm not sure why HitX should be added, but x pos is not used later.
}

void MM_IonizationCluster::diffuseElectrons(float LongitudinalSigma, float TransverseSigma, TRandom3* rndm) {

  for (auto& Electron : Electrons)
    Electron->diffuseElectron(LongitudinalSigma, TransverseSigma, rndm);

}

void MM_IonizationCluster::propagateElectrons(float driftVelx, float driftVely, float driftVel) {

  for (auto& Electron : Electrons)
    Electron->propagateElectron(driftVelx, driftVely, driftVel);

}

void MM_IonizationCluster::avalancheElectrons(float gain, TRandom3* rndm) {

  for (auto& Electron : Electrons)
    Electron->avalancheElectron(gain, rndm);

}

// accessors
std::vector<MM_Electron*> MM_IonizationCluster::getElectrons() const { return Electrons; }
