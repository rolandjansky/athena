/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UPGRADEPERFORMANCEFUNCTIONSXAOD_H
#define UPGRADEPERFORMANCEFUNCTIONSXAOD_H

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"

#include "TRandom3.h"

#include "xAODTruth/TruthParticle.h"
#include "xAODJet/Jet.h"

/*!
 * @brief Class to use the upgrade performance functions to smear xAOD objects.
 *
 * The class inherits from UpgradePerformanceFunctions and provides a few functions
 * to smear electrons, muons and jets.
 *
 * @author Mark Owen <markowen@cern.ch>
 */
class UpgradePerformanceFunctionsxAOD : public UpgradePerformanceFunctions {

 public:

  /// Default constructor
  UpgradePerformanceFunctionsxAOD();

  /// Constructor specifing layout & mu
  UpgradePerformanceFunctionsxAOD(UpgradeLayout layout, double avgMu);

  /// Destructor
  ~UpgradePerformanceFunctionsxAOD() {};

  /// Smear an Electron
  void smearElectron(xAOD::TruthParticle& electron);

  /// Smear a Muon
  void smearMuon(xAOD::TruthParticle& muon);

  /// Smear a Jet
  void smearJet(xAOD::Jet& jet);

  /// Access to random number generator
  inline TRandom3* getRandom3() {return &m_randgen;}

 private:

  TRandom3 m_randgen;

};//class UpgradePerformanceFunctionsxAOD

#endif //UPGRADEPERFORMANCEFUNCTIONSXAOD_H
