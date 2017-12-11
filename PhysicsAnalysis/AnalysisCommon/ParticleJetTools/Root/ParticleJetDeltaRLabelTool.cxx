/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleJetTools/ParticleJetDeltaRLabelTool.h"
#include "xAODJet/JetContainer.h"
#include "AsgTools/Check.h"

using namespace std;
using namespace xAOD;

ParticleJetDeltaRLabelTool::ParticleJetDeltaRLabelTool(const std::string& name)
        : AsgTool(name) {
    declareProperty("LabelName", m_labelname="", "Name of the jet label attribute to be added.");
    declareProperty("DoubleLabelName", m_doublelabelname="", "Name of the jet label attribute to be added (with the possibility of up to 2 matched hadrons).");
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

namespace {

    // TODO
    // can we do better by only looking at hadrons?
    inline bool isChild
        ( const TruthParticle* p
        , const TruthParticle* c
        ) {

        if (p->barcode() == c->barcode())
            return false;


        for (size_t iC = 0; iC < p->nChildren(); iC++) {
            const TruthParticle* cc = p->child(iC);
            if (cc->barcode() == c->barcode()) {
                return true;
            }

            if (isChild(cc, c)) {
                return true;
            }
        }

        return false;
    }


    inline void childrenRemoved
        ( const vector<const TruthParticle*>& parents
        , vector<const TruthParticle*>& children
        ) {

        // TODO
        // this is probably very inefficient,
        // but it's simple.

        // for instance: if we remove a child from info on one parent,
        // we still loop over the child again for the next parent.

        // also, we're passing around vectors rather than their
        // references.

        // for each of the parents
        for ( size_t ip = 0
            ; ip != parents.size()
            ; ip++ ) {

            const TruthParticle* p = parents[ip];

            // the current child index
            size_t ic = 0;

            // (x) each of the potential children
            while (ic != children.size()) {

                const TruthParticle* c = children[ic];

                // if citer is (recursively) a child of piter
                // remove it.
                if (isChild(p, c)) {
                    children.erase(children.begin() + ic);
                    // do not increment ic: we just removed a child.
                    continue;

                } else {
                    // increment ic: we did *not* remove a child.
                    ic++;
                    continue;
                }
            }
        }

        return;
    }

}


int ParticleJetDeltaRLabelTool::modify(JetContainer& jets) const {

    // Retrieve the particle and jet containers
    const TruthParticleContainer* taus = NULL;
    const TruthParticleContainer* bs = NULL;
    const TruthParticleContainer* cs = NULL;
    ASG_CHECK( evtStore()->retrieve( taus, m_taupartcollection), 1);
    ASG_CHECK( evtStore()->retrieve( bs, m_bottompartcollection), 1);
    ASG_CHECK( evtStore()->retrieve( cs, m_charmpartcollection), 1);

    vector<vector<const TruthParticle*> > jetlabelpartsb = match(*bs, jets);
    vector<vector<const TruthParticle*> > jetlabelpartsc = match(*cs, jets);
    vector<vector<const TruthParticle*> > jetlabelpartstau = match(*taus, jets);

    for (unsigned int iJet = 0; iJet < jets.size(); iJet++) {
        // remove children whose parent hadrons are also in the jet.
        // don't care about double tau jets
        // so leave them for now.

        childrenRemoved(jetlabelpartsb[iJet], jetlabelpartsb[iJet]);
        childrenRemoved(jetlabelpartsb[iJet], jetlabelpartsc[iJet]);
        childrenRemoved(jetlabelpartsc[iJet], jetlabelpartsc[iJet]);


        Jet& jet = *jets.at(iJet);
        jet.setAssociatedObjects(m_bottomlabelname, jetlabelpartsb.at(iJet));
        jet.setAssociatedObjects(m_charmlabelname, jetlabelpartsc.at(iJet));
        jet.setAssociatedObjects(m_taulabelname, jetlabelpartstau.at(iJet));

        // set truth label to -99 for jets below pt threshold
        if (jet.pt() < m_jetptmin) {
            jet.setAttribute<int>(m_labelname, -99);
            jet.setAttribute<int>(m_doublelabelname, -99);
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

        if (jetlabelpartsb.at(iJet).size()) {
            if (jetlabelpartsb.at(iJet).size() >= 2)
                jet.setAttribute<int>(m_doublelabelname, 55);

            else if (jetlabelpartsc.at(iJet).size())
                jet.setAttribute<int>(m_doublelabelname, 54);

            else
                jet.setAttribute<int>(m_doublelabelname, 5);

        } else if (jetlabelpartsc.at(iJet).size()) {
            if (jetlabelpartsc.at(iJet).size() >= 2)
                jet.setAttribute<int>(m_doublelabelname, 44);

            else
                jet.setAttribute<int>(m_doublelabelname, 4);

        } else if (jetlabelpartstau.at(iJet).size())
            jet.setAttribute<int>(m_doublelabelname, 15);

        else 
            jet.setAttribute<int>(m_doublelabelname, 0);
    }

    return 0;
}


vector<vector<const TruthParticle*> >
ParticleJetDeltaRLabelTool::match(
        const TruthParticleContainer& parts,
        const JetContainer& jets) const {

    vector<vector<const TruthParticle*> > jetlabelparts(jets.size(), vector<const TruthParticle*>());

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
    for (TruthParticleContainer::const_iterator part_itr = parts.begin();
            part_itr != parts.end(); ++part_itr) {

        const TruthParticle* part = *part_itr;

        if (part->pt() < m_partptmin)
            continue;

        double mindr = DBL_MAX;
        double maxpt = 0;
        int mindrjetidx = -1;
        int maxptjetidx = -1;
        for (unsigned int iJet = 0; iJet < jets.size(); iJet++) {

            const Jet& jet = *jets.at(iJet);

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

