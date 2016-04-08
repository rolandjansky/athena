/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleJetTools/ParticleJetDeltaRLabelTool.h"
#include "xAODJet/JetContainer.h"
#include "AsgTools/Check.h"

using namespace std;

ParticleJetDeltaRLabelTool::ParticleJetDeltaRLabelTool(const std::string& name)
        : AsgTool(name) {
    declareProperty("LabelName", m_labelname="", "Name of the jet label attribute to be added.");
    declareProperty("BLabelName", m_bottomlabelname="", "Name of the attribute to be added for matched B hadrons.");
    declareProperty("CLabelName", m_charmlabelname="", "Name of the attribute to be added for matched C hadrons.");
    declareProperty("TauLabelName", m_taulabelname="", "Name of the attribute to be added for matched taus.");
    declareProperty("BParticleCollection", m_bottompartcollection="", "Name of the particle collection used for matching B hadrons");
    declareProperty("CParticleCollection", m_charmpartcollection="", "Name of the particle collection used for matching C hadrons");
    declareProperty("TauParticleCollection", m_taupartcollection="", "Name of the particle collection used for matching taus");
    declareProperty("PartPtMin", m_partptmin=5000, "Minimum pT of particles for labeling (MeV)");
    declareProperty("JetPtMin", m_jetptmin=10000, "Minimum pT of jets to be lebeled (MeV)");
    declareProperty("DRMax", m_drmax=0.3, "Maximum deltaR between a particle and jet to be labeled");
    declareProperty("MatchMode", m_matchmode="MinDR",
            "In the case that a particle matches two jets, the closest (MatchMode=MinDR) or highest-pT (MatchMode=MaxPt) jet will be labeled");
}


int ParticleJetDeltaRLabelTool::modify(xAOD::JetContainer& jets) const {

    // Retrieve the particle and jet containers
    const xAOD::TruthParticleContainer* taus = NULL;
    const xAOD::TruthParticleContainer* bs = NULL;
    const xAOD::TruthParticleContainer* cs = NULL;
    ASG_CHECK( evtStore()->retrieve( taus, m_taupartcollection));
    ASG_CHECK( evtStore()->retrieve( bs, m_bottompartcollection));
    ASG_CHECK( evtStore()->retrieve( cs, m_charmpartcollection));

    vector<vector<const xAOD::TruthParticle*> > jetlabelpartsb = match(*bs, jets);
    vector<vector<const xAOD::TruthParticle*> > jetlabelpartsc = match(*cs, jets);
    vector<vector<const xAOD::TruthParticle*> > jetlabelpartstau = match(*taus, jets);

    for (unsigned int iJet = 0; iJet < jets.size(); iJet++) {
        xAOD::Jet& jet = *jets.at(iJet);
        jet.setAssociatedObjects(m_bottomlabelname, jetlabelpartsb.at(iJet));
        jet.setAssociatedObjects(m_charmlabelname, jetlabelpartsc.at(iJet));
        jet.setAssociatedObjects(m_taulabelname, jetlabelpartstau.at(iJet));

        // set truth label to -99 for jets below pt threshold
        if (jet.pt() < m_jetptmin) {
            jet.setAttribute<int>(m_labelname, -99);
            continue;
        }

        // set truth label for jets above pt threshold
        // hierarchy: b > c > tau > light
        if (jetlabelpartsb.at(iJet).size())
            jet.setAttribute<int>(m_labelname, 5);
        else if (jetlabelpartsc.at(iJet).size())
            jet.setAttribute<int>(m_labelname, 4);
        else if (jetlabelpartstau.at(iJet).size())
            jet.setAttribute<int>(m_labelname, 15);
        else 
            jet.setAttribute<int>(m_labelname, 0);
    }

    return 0;
}


vector<vector<const xAOD::TruthParticle*> >
ParticleJetDeltaRLabelTool::match(
        const xAOD::TruthParticleContainer& parts,
        const xAOD::JetContainer& jets) const {

    vector<vector<const xAOD::TruthParticle*> > jetlabelparts(jets.size(), vector<const xAOD::TruthParticle*>());

    // determine the match mode
    bool mindrmatch;
    if (m_matchmode == "MinDR")
        mindrmatch = true;
    else if (m_matchmode == "MaxPt")
        mindrmatch = false;
    else {
        ATH_MSG_FATAL("MatchMode must be MinDR or MaxPt");
        return jetlabelparts;
    }

    // loop over particles and find the best matched jet
    for (xAOD::TruthParticleContainer::const_iterator part_itr = parts.begin();
            part_itr != parts.end(); ++part_itr) {

        const xAOD::TruthParticle* part = *part_itr;

        if (part->pt() < m_partptmin)
            continue;

        double mindr = DBL_MAX;
        double maxpt = 0;
        int mindrjetidx = -1;
        int maxptjetidx = -1;
        for (unsigned int iJet = 0; iJet < jets.size(); iJet++) {

            const xAOD::Jet& jet = *jets.at(iJet);

            double pt = jet.pt();
            if (pt < m_jetptmin)
                continue;

            double dr = jet.p4().DeltaR(part->p4());
            // too far for matching criterion
            if (dr > m_drmax)
                continue;

            // store the matched jet
            if (dr < mindr) {
                mindr = dr;
                mindrjetidx = iJet;
            }

            if (pt > maxpt) {
                maxpt = pt;
                maxptjetidx = iJet;
            }
        }

        // store the label particle with the jet
        if (mindrmatch && mindrjetidx >= 0)
            jetlabelparts.at(mindrjetidx).push_back(part);
        else if (maxptjetidx >= 0)
            jetlabelparts.at(maxptjetidx).push_back(part);
    }

    return jetlabelparts;
}
