/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopExamples/EventLoaderDirect.h"

#include "TopEvent/EventTools.h"

#include "xAODRootAccess/TEvent.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODTruth/TruthParticleContainer.h"

namespace top {

EventLoaderDirect::EventLoaderDirect(const ContainerNames& names) :
        m_containerNames(names) {
}

EventLoaderDirect::~EventLoaderDirect() {
}

top::Event EventLoaderDirect::loadEvent(xAOD::TEvent& xaodEvent) {
    top::Event te;

    //EventInfo
    top::check(xaodEvent.retrieve(te.m_info, m_containerNames.eventInfoName), "xAOD::TEvent retrieve failed to get " + m_containerNames.eventInfoName);

    //Electrons
    if (m_containerNames.electronCollectionName != "None") {
        const xAOD::ElectronContainer* electrons(0);
        top::check(xaodEvent.retrieve(electrons, m_containerNames.electronCollectionName), "xAOD::TEvent retrieve failed to get " +  m_containerNames.electronCollectionName);
        te.m_electrons = *electrons;
    }

    //Muons
    if (m_containerNames.muonCollectionName != "None") {
        const xAOD::MuonContainer* muons(0);
        top::check(xaodEvent.retrieve(muons, m_containerNames.muonCollectionName), "xAOD::TEvent retrieve failed to get " +  m_containerNames.muonCollectionName);
        te.m_muons = *muons;
    }

    //Jets
    if (m_containerNames.jetCollectionName != "None") {
        const xAOD::JetContainer* jets(0);
        top::check(xaodEvent.retrieve(jets, m_containerNames.jetCollectionName), "xAOD::TEvent retrieve failed to get " +  m_containerNames.jetCollectionName);
        te.m_jets = *jets;
    }

    //LargeJets
    if (m_containerNames.largeJetCollectionName != "None") {
        const xAOD::JetContainer* largeJets(0);
        top::check(xaodEvent.retrieve(largeJets, m_containerNames.largeJetCollectionName), "xAOD::TEvent retrieve failed to get " +  m_containerNames.largeJetCollectionName);
        te.m_largeJets = *largeJets;
    }

    //Taus
    if (m_containerNames.tauCollectionName != "None") {
        const xAOD::TauJetContainer* taus(0);
        top::check(xaodEvent.retrieve(taus, m_containerNames.tauCollectionName), "xAOD::TEvent retrieve failed to get " +  m_containerNames.tauCollectionName);
        te.m_tauJets = *taus;
    }

    //MET
    if (m_containerNames.metName != "None") {

        const xAOD::MissingETContainer* mets(0);
        check(xaodEvent.retrieve(mets, m_containerNames.metName), "xAOD::TEvent retrieve failed to get " + m_containerNames.metName);
	te.m_met = (*mets)[MissingETBase::Source::total()];

    }

    return te;
}

void EventLoaderDirect::print(std::ostream& os) const {
    os << "EventLoaderDirect Configuration\n";
    os << "No object corrections and no object selection\n";

    os << "\n";
    os << "Electron ContainerName: " << m_containerNames.electronCollectionName << "\n";
    os << "Muon ContainerName: " << m_containerNames.muonCollectionName << "\n";
    os << "Tau ContainerName: " << m_containerNames.tauCollectionName << "\n";
    os << "Jet ContainerName: " << m_containerNames.jetCollectionName << "\n";
    os << "LargeJet ContainerName: " << m_containerNames.largeJetCollectionName << "\n";
    os << "MET ContainerName: " << m_containerNames.metName << "\n";
}

}

