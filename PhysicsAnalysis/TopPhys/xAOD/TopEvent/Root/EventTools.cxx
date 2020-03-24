/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEvent/EventTools.h"
#include "TopEvent/Event.h"

#include "CxxUtils/fpcompare.h"

#include "xAODBase/IParticle.h"

#include "TRandom3.h"

namespace top {
  void check(bool thingToCheck, const std::string& usefulFailureMessage) {
    if (!thingToCheck) {
      throw std::runtime_error("top::check: " + usefulFailureMessage);
    }
  }

  double deltaR(const xAOD::IParticle& p1, const xAOD::IParticle& p2) {
    return p1.p4().DeltaR(p2.p4());
  }

  double deltaPhi(const xAOD::IParticle& p1, const xAOD::IParticle& p2) {
    return p1.p4().DeltaPhi(p2.p4());
  }

  double mwt(const xAOD::IParticle& lepton, const xAOD::MissingET& met) {
    return sqrt(2. * lepton.pt() * met.met() * (1. - cos(lepton.phi() - met.phi())));
  }

  double ht(const top::Event& event) {
    double sumHt = 0.;

    for (const auto el : event.m_electrons)
      sumHt += el->pt();

    for (const auto mu : event.m_muons)
      sumHt += mu->pt();

    for (const auto jet : event.m_jets)
      sumHt += jet->pt();

    return sumHt;
  }

  double invariantMass(const xAOD::IParticle& p1, const xAOD::IParticle& p2) {
    return (p1.p4() + p2.p4()).M();
  }

  bool isSimulation(const top::Event& event) {
    return event.m_info->eventType(xAOD::EventInfo::IS_SIMULATION);
  }

  bool descendingPtSorter(const xAOD::IParticle* p1, const xAOD::IParticle* p2) {
    return CxxUtils::fpcompare::greater(p1->pt(), p2->pt());
  }

  std::vector<int> calculateBootstrapWeights(int nreplicas, int eventNumber, int mcChannelNumber) {
    TRandom3 rand;
    unsigned long long seed = eventNumber + 100 * mcChannelNumber; // Set the seed to be unique for this event, but
                                                                   // reproducible for systematics

    rand.SetSeed(seed);
    std::vector<int> weight_poisson(nreplicas, 0); // Initialise vector of length nreplicas, to all have weight of 0
    for (int i = 0; i < nreplicas; i++) {
      weight_poisson.at(i) = rand.PoissonD(1); // Fill with a weight drawn from Poisson distribution with mean of 1
    }
    // Return the vector to be stored in top::Event
    return weight_poisson;
  }
}
