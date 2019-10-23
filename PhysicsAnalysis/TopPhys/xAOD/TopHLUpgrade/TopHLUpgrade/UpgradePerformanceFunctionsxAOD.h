/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef UPGRADEPERFORMANCEFUNCTIONSXAOD_H
#define UPGRADEPERFORMANCEFUNCTIONSXAOD_H

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"

#include "TRandom3.h"

#include "xAODTruth/TruthParticle.h"
#include "xAODEgamma/Electron.h"
#include "xAODJet/Jet.h"

/*!
 * @brief Class to use the upgrade performance functions to smear xAOD objects.
 *
 * The class inherits from UpgradePerformanceFunctions and provides a few functions
 * to smear electrons, muons and jets.
 *
 * @author Mark Owen <markowen@cern.ch>
 */
class UpgradePerformanceFunctionsxAOD: public Upgrade::UpgradePerformanceFunctions {
public:
  /// Default constructor
  UpgradePerformanceFunctionsxAOD(const std::string& name);

  /// Constructor specifing layout & mu
  UpgradePerformanceFunctionsxAOD(const std::string& name, UpgradeLayout layout, double avgMu);

  /// Destructor
  ~UpgradePerformanceFunctionsxAOD() {};

  /// Smear an Electron
  void smearElectron(xAOD::TruthParticle& electron);

  /// Get smeared fake electron from truth jet
  void smearFakeElectron(xAOD::TruthParticle& electron, xAOD::Jet& jet);
  void smearFakeElectron(xAOD::TruthParticle& electron, TLorentzVector& jet);

  /// Smear a Muon
  void smearMuon(xAOD::TruthParticle& muon);

  /// Smear a Jet
  void smearJet(xAOD::Jet& jet);

  /// Smear a fat jet
  void smearFatJet(xAOD::Jet& jet);

  /// Smear a Photon
  void smearPhoton(xAOD::TruthParticle& photon);

  /// Get smeared fake photon from truth jet
  void smearFakePhoton(xAOD::TruthParticle& photon, xAOD::Jet& jet);
  void smearFakePhoton(xAOD::TruthParticle& photon, TLorentzVector& jet);

  /// Access to random number generator
  inline TRandom3* getRandom3() {return &m_randgen;}
private:
  TRandom3 m_randgen;
};//class UpgradePerformanceFunctionsxAOD

#endif //UPGRADEPERFORMANCEFUNCTIONSXAOD_H
