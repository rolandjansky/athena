/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventReconstructionTools/Sonnenschein.h"
#include "TopEvent/Event.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertex.h"

#include <iostream>

namespace top {

Sonnenschein::Sonnenschein() {
}

Sonnenschein::~Sonnenschein() {
}

bool Sonnenschein::tWb(const top::Event& event, int start, TLorentzVector& t, TLorentzVector& W, TLorentzVector& b, TLorentzVector& l, TLorentzVector& nu) const {
    bool hasT = false;
    bool hasB = false;
    bool hasL = false;
    bool hasNu = false;

    for (const xAOD::TruthParticle* particle : *event.m_truth) {
        if (particle->pdgId() == start) {
            //std::cout << *particle << std::endl;
            t = particle->p4();
            hasT = true;

            if (particle->hasDecayVtx()) {
                const xAOD::TruthVertex* vtx = particle->decayVtx();
                for (size_t i = 0; i < vtx->nOutgoingParticles(); ++i) {
                    const xAOD::TruthParticle* tDecayProduct = vtx->outgoingParticle(i);
                    //std::cout << "    " << *tDecayProduct << std::endl;

                    if (abs(tDecayProduct->pdgId()) == 24 && tDecayProduct->hasDecayVtx()) {
                        W = tDecayProduct->p4();

                        const xAOD::TruthVertex* wDecayVtx = tDecayProduct->decayVtx();
                        for (size_t j = 0; j < wDecayVtx->nOutgoingParticles(); ++j) {
                            const xAOD::TruthParticle* wDecayProduct = wDecayVtx->outgoingParticle(j);
                            //std::cout << "       " << *wDecayProduct << std::endl;

                            if (abs(wDecayProduct->pdgId()) == 11 || abs(wDecayProduct->pdgId()) == 13 || abs(wDecayProduct->pdgId()) == 15) {
                                l = wDecayProduct->p4();
                                hasL = true;
                            }

                            if (abs(wDecayProduct->pdgId()) == 12 || abs(wDecayProduct->pdgId()) == 14 || abs(wDecayProduct->pdgId()) == 16) {
                                nu = wDecayProduct->p4();
                                hasNu = true;
                            }

                        }
                    } else if (abs(tDecayProduct->pdgId()) == 5) {
                        b = tDecayProduct->p4();
                        hasB = true;
                    }
                }
            }
        }
    }

    return hasT && hasB && hasL && hasNu;
}

bool Sonnenschein::apply(const top::Event& event) const {
    TLorentzVector t;
    TLorentzVector Wp;
    TLorentzVector b;
    TLorentzVector lp;
    TLorentzVector nu;
    bool cando1 = tWb(event, 6, t, Wp, b, lp, nu);

    TLorentzVector tbar;
    TLorentzVector Wm;
    TLorentzVector bbar;
    TLorentzVector lm;
    TLorentzVector nubar;
    bool cando2 = tWb(event, -6, tbar, Wm, bbar, lm, nubar);

    //isn't dilepton at truth level, so return
    if (!cando1 || !cando2)
        return true;

    const double met_ex = nu.Px() + nubar.Px();
    const double met_ey = nu.Py() + nubar.Py();

    //solve
    std::vector<std::pair<TLorentzVector, TLorentzVector>> allSolutions = m_sonn.solve(lp, b, t.M(), Wp.M(), lm, bbar, tbar.M(), Wm.M(), met_ex, met_ey);

    std::cout << "EVENT" << std::endl;
    unsigned int counter = 0;
    for (const auto& possible : allSolutions) {
        std::cout << "Possible solution number " << counter << "\n";
        std::cout << "    first neutrino : " << possible.first << "\n";
        std::cout << "    second neutrino: " << possible.second << "\n";
        ++counter;
    }

    return true;
}

}
