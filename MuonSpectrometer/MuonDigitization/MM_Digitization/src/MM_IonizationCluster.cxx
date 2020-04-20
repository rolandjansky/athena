/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MM_Digitization/MM_IonizationCluster.h"

MM_NelectronProb MM_IonizationCluster::s_NelectronProb;

MM_IonizationCluster::MM_IonizationCluster () {}

MM_IonizationCluster::MM_IonizationCluster (float HitX, float IonizationX, float IonizationY) : m_HitX(HitX), m_IonizationStart(IonizationX, IonizationY) {}

MM_IonizationCluster::MM_IonizationCluster (const MM_IonizationCluster& MM_IonizationCluster) {

  m_HitX = MM_IonizationCluster.getHitX();
  m_IonizationStart = MM_IonizationCluster.getIonizationStart();
  m_Electrons = MM_IonizationCluster.getElectrons();

}

void MM_IonizationCluster::createElectrons(TRandom3* rndm) {

  gRandom = rndm;
  int Nelectron = MM_IonizationCluster::s_NelectronProb.FindBin(MM_IonizationCluster::s_NelectronProb.GetRandom());
  m_Electrons.reserve(Nelectron);
  for (int iElectron = 0; iElectron<Nelectron; iElectron++)
    m_Electrons.push_back(new MM_Electron(m_IonizationStart.X()+m_HitX, m_IonizationStart.Y()));
}

void MM_IonizationCluster::propagateElectrons(float lorentzAngle, float driftVel) {

  for (auto& Electron : m_Electrons)
    Electron->propagateElectron(lorentzAngle, driftVel);

}

// accessors
std::vector<MM_Electron*> MM_IonizationCluster::getElectrons() const { return m_Electrons; }
