/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MM_Digitization/MM_IonizationCluster.h"

using namespace std;

MM_NelectronProb MM_IonizationCluster::s_NelectronProb;

MM_IonizationCluster::MM_IonizationCluster () {} 

MM_IonizationCluster::MM_IonizationCluster (float HitX, float IonizationX, float IonizationY) : m_HitX(HitX), m_IonizationStart(IonizationX, IonizationY) {}

MM_IonizationCluster::MM_IonizationCluster (const MM_IonizationCluster& other) {
  
  m_HitX = other.getHitX();
  m_IonizationStart = other.getIonizationStart();
  m_Electrons = other.getElectrons();
  
}

void MM_IonizationCluster::createElectrons(TRandom3* rndm) {

  gRandom = rndm;
  int Nelectron = MM_IonizationCluster::s_NelectronProb.FindBin(MM_IonizationCluster::s_NelectronProb.GetRandom());
  m_Electrons.reserve(Nelectron);
  for (int iElectron = 0; iElectron<Nelectron; iElectron++) 
    m_Electrons.push_back(new MM_Electron(m_HitX+m_IonizationStart.X(), m_IonizationStart.Y()));
  // I'm not sure why m_HitX should be added, but x pos is not used later.
}

void MM_IonizationCluster::diffuseElectrons(float LongitudinalSigma, float TransverseSigma, TRandom3* rndm) {

  for (auto& Electron : m_Electrons)
    Electron->diffuseElectron(LongitudinalSigma, TransverseSigma, rndm);
  
}

void MM_IonizationCluster::propagateElectrons(float driftVelx, float driftVely, float driftVel) {
  
  for (auto& Electron : m_Electrons)
    Electron->propagateElectron(driftVelx, driftVely, driftVel);
  
}

void MM_IonizationCluster::avalancheElectrons(float gain, TRandom3* rndm) {

  for (auto& Electron : m_Electrons)
    Electron->avalancheElectron(gain, rndm);
  
}
  
// accessors
std::vector<MM_Electron*> MM_IonizationCluster::getElectrons() const { return m_Electrons; }
