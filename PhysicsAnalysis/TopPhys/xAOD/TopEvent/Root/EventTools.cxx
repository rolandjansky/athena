/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/EventTools.h"
#include "TopEvent/Event.h"

#include "CxxUtils/fpcompare.h"

#include "xAODBase/IParticle.h"

namespace top {

void check(bool thingToCheck, const std::string& usefulFailureMessage) {
    if (!thingToCheck) {
        std::cout << usefulFailureMessage << "\n";
        exit(1);
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

}
