/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventReconstructionTools/NeutrinoWeighting.h"

#include "TopEvent/Event.h"

namespace top {

NeutrinoWeighting::NeutrinoWeighting() : sigmax(10), sigmay(10) {
    double etaStep = 0.2;

    //construct the array of eta values to use (cosh and sinh them)
    int index = 0;
    for (double eta = -5.0; eta < 5.0001; eta += etaStep) {
        neutrinos[index][0] = eta;
        neutrinos[index][1] = sinh(eta);
        neutrinos[index][2] = cosh(eta);
        ++index;
    }

    etaSize = index;

    if (etaSize > 2000) {
        std::cout << "ERROR TOO MANY SAMPLING POINT FOR neutrinos ARRAY" << std::endl;
        exit(1);
    }
}

NeutrinoWeighting::~NeutrinoWeighting() {
}

bool NeutrinoWeighting::apply(const top::Event& event) const {
    TLorentzVector l1;
    TLorentzVector l2;
    TLorentzVector b1;
    TLorentzVector b2;
    double topMass = 172500.;

    const double met_ex = event.m_met->mpx();
    const double met_ey = event.m_met->mpy();

    for (int i = 0; i < etaSize; ++i) {
         for (int j = 0; j < etaSize; ++j) {
             NWSolution ans1 = solveForNeutrinoEta(l1, b1, topMass, i);
             NWSolution ans2 = solveForNeutrinoEta(l2, b2, topMass, j);

             if (ans1.getNumSolutions() > 0 && ans2.getNumSolutions() > 0) {
                 const double sol1 = neutrino_weight(ans1.getv1(), ans2.getv1(), met_ex, met_ey);
                 const double sol2 = neutrino_weight(ans1.getv1(), ans2.getv2(), met_ex, met_ey);
                 const double sol3 = neutrino_weight(ans1.getv2(), ans2.getv1(), met_ex, met_ey);
                 const double sol4 = neutrino_weight(ans1.getv2(), ans2.getv2(), met_ex, met_ey);

                 if (isnan(sol1) || isnan(sol2) || isnan(sol3) || isnan(sol4))
                     std::cout << "One of the quadratic ans is NaN!" << std::endl;
             }

             ans1 = solveForNeutrinoEta(l1, b2, topMass, i);
             ans2 = solveForNeutrinoEta(l2, b1, topMass, j);

             if (ans1.getNumSolutions() > 0 && ans2.getNumSolutions() > 0) {
                 const double sol1 = neutrino_weight(ans1.getv1(), ans2.getv1(), met_ex, met_ey);
                 const double sol2 = neutrino_weight(ans1.getv1(), ans2.getv2(), met_ex, met_ey);
                 const double sol3 = neutrino_weight(ans1.getv2(), ans2.getv1(), met_ex, met_ey);
                 const double sol4 = neutrino_weight(ans1.getv2(), ans2.getv2(), met_ex, met_ey);

                 if (isnan(sol1) || isnan(sol2) || isnan(sol3) || isnan(sol4))
                     std::cout << "One of the quadratic ans is NaN!" << std::endl;
             }
         }
     }

    //normalise?

    return true;
}

NWSolution NeutrinoWeighting::solveForNeutrinoEta(const TLorentzVector& lepton, const TLorentzVector& bJet, double topMass, int index) const {
    double Wmass2 = m_wmass * m_wmass;
    double bmass = m_bmass;

    double Elprime = lepton.E() * neutrinos[index][2] - lepton.Pz() * neutrinos[index][1];
    double Ebprime = bJet.E() * neutrinos[index][2] - bJet.Pz() * neutrinos[index][1];

    double A = (lepton.Py() * Ebprime - bJet.Py() * Elprime) / (bJet.Px() * Elprime - lepton.Px() * Ebprime);
    double B = (Elprime * (topMass * topMass - Wmass2 - bmass * bmass - 2. * lepton * bJet) - Ebprime * Wmass2) / (2. * (lepton.Px() * Ebprime - bJet.Px() * Elprime));

    double par1 = (lepton.Px() * A + lepton.Py()) / Elprime;
    double C = A * A + 1. - par1 * par1;
    double par2 = (Wmass2 / 2. + lepton.Px() * B) / Elprime;
    double D = 2. * (A * B - par2 * par1);
    double F = B * B - par2 * par2;
    double det = D * D - 4. * C * F;

    NWSolution sol;
    sol.setSolutions(0);

    if (det > 0.) {
        double tmp = sqrt(det) / (2. * C);
        double py1 = -D / (2. * C) + tmp;
        double py2 = -D / (2. * C) - tmp;
        double px1 = A * py1 + B;
        double px2 = A * py2 + B;
        double pT2_1 = px1 * px1 + py1 * py1;
        double pT2_2 = px2 * px2 + py2 * py2;
        double pz1 = sqrt(pT2_1) * neutrinos[index][1];
        double pz2 = sqrt(pT2_2) * neutrinos[index][1];

        TLorentzVector a1(px1, py1, pz1, sqrt(pT2_1 + pz1 * pz1));
        TLorentzVector a2(px2, py2, pz2, sqrt(pT2_2 + pz2 * pz2));

        sol.setSolutions(2, a1, a2);
    }

    return sol;
}

double NeutrinoWeighting::neutrino_weight(const TLorentzVector& neutrino1, const TLorentzVector& neutrino2, double met_ex, double met_ey) const {
    const double dx = met_ex - neutrino1.Px() - neutrino2.Px();
    const double dy = met_ey - neutrino1.Py() - neutrino2.Py();

    return exp(- dx * dx / (2. * sigmax * sigmax) - dy * dy / (2. * sigmay * sigmay));
}

}
