/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventReconstructionTools/PTMaxReco.h"

#include "TopEvent/Event.h"

namespace top {

PTMaxReco::PTMaxReco() : m_wmass(80400.), m_topmass(172500.) {
}

PTMaxReco::~PTMaxReco() {
}

bool PTMaxReco::apply(const top::Event& event) const {
    //get the hadronic top

    //jets that make-up the hadronic top quark
    const xAOD::Jet* jet1 = nullptr;
    const xAOD::Jet* jet2 = nullptr;
    const xAOD::Jet* jet3 = nullptr;

    double maxptsq = 0.;
    for (xAOD::JetContainer::const_iterator j1 = event.m_jets.begin(); j1 != event.m_jets.end(); ++j1) {
        for (xAOD::JetContainer::const_iterator j2 = j1 + 1; j2 != event.m_jets.end(); ++j2) {
            for (xAOD::JetContainer::const_iterator j3 = j2 + 1; j3 != event.m_jets.end(); ++j3) {
                const double px = (*j1)->px() + (*j2)->px() + (*j3)->px();
                const double py = (*j1)->py() + (*j2)->py() + (*j3)->py();

                //pt2 to avoid the slow sqrt
                const double ptsq = px * px + py * py;

                if (ptsq > maxptsq) {
                    jet1 = (*j1);
                    jet2 = (*j2);
                    jet3 = (*j3);
                }
            }
        }
    }

    //get the leptonic top

    //e+jets
    TLorentzVector lepton;
    if (event.m_electrons.size() == 1)
        lepton = event.m_electrons.at(0)->p4();

    //mu+jets
    if (event.m_muons.size() == 1)
        lepton = event.m_muons.at(0)->p4();

    TLorentzVector nu = neutrinoCandidate(lepton, *event.m_met, true);

    const xAOD::Jet* lepb = nullptr;
    double topdiff = 1000000.;
    for (xAOD::JetContainer::const_iterator j1 = event.m_jets.begin(); j1 != event.m_jets.end(); ++j1) {
        //ignore the three jets that make-up the hadronic top quark
        if (*j1 == jet1 || *j1 == jet2 || *j1 == jet3)
            continue;

        //pick the jet which makes a leptonic top closest to the true top mass
        const double tempdiff = fabs((lepton + nu + (*j1)->p4()).M());

        if ((tempdiff - m_topmass) < topdiff) {
            lepb = *j1;
            topdiff = tempdiff;
        }
    }

    std::cout << lepb->pt() << std::endl;

    return true;
}

TLorentzVector PTMaxReco::neutrinoCandidate(const TLorentzVector& lep, const xAOD::MissingET& met, bool dealWithNegative_nu) const {
    const double px = met.mpx();
    const double py = met.mpy();

    // solve quadratic to find neutrino four vector
    double alpha = pow(m_wmass,2) + pow((px+lep.Px()),2)+pow((py+lep.Py()),2) -pow(lep.E(),2);
    double beta = 0.5 * ( alpha-pow(met.met(),2) + pow(lep.Pz(), 2) );
    double gamma = -( beta*beta - ( pow(lep.E(),2)*pow(met.met(), 2) ) ) / ( pow(lep.E(),2)-pow(lep.Pz(), 2) );
    double lambda = 2. * beta * lep.Pz() / (pow(lep.E(), 2)-pow(lep.Pz(), 2));
    double delta = pow(lambda,2) - 4 * gamma;

    if (delta < 0) { // ignore non real solutions
        if (dealWithNegative_nu)
            delta = 0;
        else
            return TLorentzVector();
    }

    delta = sqrt(delta);

    double pz_pos = (lambda+delta) / 2.;
    double pz_neg = (lambda-delta) / 2.;

    double pz = 0.0;

    if (fabs(pz_pos) > fabs(pz_neg))
        pz = pz_neg;
    else
        pz = pz_pos;

    double e = sqrt(px*px + py*py + pz*pz);

    TLorentzVector neutrino;
    neutrino.SetPxPyPzE(px, py, pz, e);
    return neutrino;
}

}
