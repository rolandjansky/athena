/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "MuonEfficiencyCorrections/fineEtaPhiBinning.h"

fineEtaPhiBinning::fineEtaPhiBinning() {

    m_barrel_endcap_transition = 1.19;

    // OUTDATED old binning, driven by coil geometry. 
//    m_abs_eta_bins = {0.21,0.42,0.63,0.84,1.05,1.37,1.52,1.74,1.95,2.18,2.75};
//     m_abs_eta_bins = {0.10,0.32,0.54,0.76,0.98,1.19,1.40,1.62,1.84,2.00,2.25,2.50};
// 
//     m_phi_lower_BAR = {-0.235,0.235,0.55,1.020,1.335,1.805,2.12,2.590, 2.905,-2.905,-2.59,-2.120,-1.805,-1.335,-1.020,-0.550 };
//     m_phi_upper_BAR = { 0.235,0.55 ,1.02,1.335,1.805,2.120,2.59,2.905,-2.905,-2.590,-2.12,-1.806,-1.335,-1.020,-0.550,-0.235 };

//     UPDATED, based on sector binning used by the MomentumCorrections. Overlaps accounted towards small sectors. 
    m_barrel_endcap_transition = 1.05;
    m_abs_eta_bins = {0.10,0.4,0.6,0.8,1.05,1.25,1.5,1.7,2,2.3,2.5,2.7};
    m_phi_lower_BAR = {-0.180,0.180,0.605,0.965,1.390,1.750,2.175,2.535, 2.960,-2.960,-2.535,-2.175,-1.750,-1.390,-0.965,-0.605};
    m_phi_upper_BAR = {0.180,0.605 ,0.965,1.390,1.750,2.175,2.535,2.960,-2.960,-2.535,-2.175,-1.750,-1.390,-0.965,-0.605,-0.180};

    m_phi_lower_EC = {-0.131,0.131,0.655,0.916,1.440,1.702,2.225,2.487, 3.011,-3.011,-2.487,-2.225,-1.702,-1.440,-0.916,-0.655};
    m_phi_upper_EC = {0.131,0.655,0.916,1.440,1.702,2.225,2.487,3.011,-3.011,-2.487,-2.225,-1.702,-1.440,-0.916,-0.655,-0.131};

}

int fineEtaPhiBinning::getECSector(double phi) const {
    float lower = 0;
    float upper = 0;
    for (size_t sector = 0; sector < m_phi_lower_EC.size(); ++sector) {
        upper = m_phi_upper_EC.at(sector);
        lower = m_phi_lower_EC.at(sector);
        if (upper < lower) { // for closing around pi
            if (phi > lower || phi <= upper) return sector + 1;
        }
        if (phi > lower && phi <= upper) return sector + 1;
    }
    return -9;
}

TH2Poly* fineEtaPhiBinning::getPolyHist(const std::string& name, const std::string& title) const {
    TH2Poly* h = new TH2Poly();
    h->SetName(name.c_str());
    h->SetTitle(title.c_str());

    // collect all eta bins
    std::vector<float> etabins;
    for (int p = m_abs_eta_bins.size() - 1; p >= 0; --p) {
        etabins.push_back(-1. * m_abs_eta_bins.at(p));
    }
//     etabins.push_back(0.);
    etabins.insert(etabins.end(), m_abs_eta_bins.begin(), m_abs_eta_bins.end());
    //:::::::::::::::::::::::::::
    // Create the poly bins
    //:::: Careful: Ugly! :::::::

    /*
     *
     *  we go like this when adding the bins:
     *
     *
     *
     *    ^  ^  ^
     *    |  | |
     *    x------->
     *
     *
     */

    // This version has the correct behavior for the bin around pi (one common bin at +pi and -pi instead of 2!)
    for (unsigned int k = 0; k < etabins.size() - 1; k++) {
        double eta = etabins.at(k);
        double etanext = etabins.at(k + 1);

        for (unsigned int l = 0; l < m_phi_lower_BAR.size(); l++) {
            double phiB = m_phi_lower_BAR.at(l);
            double phiBnext = m_phi_upper_BAR.at(l);
            double phiE = m_phi_lower_EC.at(l);
            double phiEnext = m_phi_upper_EC.at(l);

            if (phiB > phiBnext) { // the -pi / pi bin!
                double phiedgePlus = ((eta < -m_barrel_endcap_transition || eta >= m_barrel_endcap_transition) ? phiE : phiB);
                double phiedgerPlus = ((etanext <= -m_barrel_endcap_transition || etanext > m_barrel_endcap_transition) ? phiE : phiB);
                double phiedgeMinus = ((eta < -m_barrel_endcap_transition || eta >= m_barrel_endcap_transition) ? phiEnext : phiBnext);
                double phiedgerMinus = ((etanext <= -m_barrel_endcap_transition || etanext > m_barrel_endcap_transition) ? phiEnext : phiBnext);

                /*
                 *      here, we fill like this:
                 *      and of course if we have no threshold
                 *      we don't have 6,7,10,11
                 *
                 *        4.........................3     <- +pi
                 *        '                         '
                 *        '                         '
                 *        5.......6                 '
                 *                7................8'  8,9 are actually *on* the vertical double line of course!
                 *                                 ''  and exactly along the line 2-3.
                 *                                 ''
                 *                                 ''
                 *                                 ''  this line has 0 width and is aligned with the other bin
                 *                                 ''  edges, so we don't see it on the histo!
                 *                                 ''  drawn twice here just to clarify that we go along that
                 *                                 ''  way twice
                 *                10...............9'
                 *        12......11                '
                 *        '                         '
                 *        '                         '
                 *        1.........................2      <- -pi
                 */

                // if we pass the threshold, different treatment
                if (eta < -m_barrel_endcap_transition && etanext > -m_barrel_endcap_transition) {

                    double xe[12] = { eta, etanext, etanext, eta, eta, -m_barrel_endcap_transition, -m_barrel_endcap_transition, etanext, etanext, -m_barrel_endcap_transition, -m_barrel_endcap_transition, eta };

                    double ye[12] = { -3.15, -3.15, 3.15, 3.15, phiedgePlus, phiedgePlus, phiedgerPlus, phiedgerPlus, phiedgerMinus, phiedgerMinus, phiedgeMinus, phiedgeMinus };
                    h->AddBin(12, xe, ye);

                }

                else if (eta < m_barrel_endcap_transition && etanext > m_barrel_endcap_transition) {

                    double xe[12] = { eta, etanext, etanext, eta, eta, m_barrel_endcap_transition, m_barrel_endcap_transition, etanext, etanext, m_barrel_endcap_transition, m_barrel_endcap_transition, eta };

                    double ye[12] = { -3.15, -3.15, 3.15, 3.15, phiedgePlus, phiedgePlus, phiedgerPlus, phiedgerPlus, phiedgerMinus, phiedgerMinus, phiedgeMinus, -phiedgeMinus };
                    h->AddBin(12, xe, ye);

                }

                else {
                    double xe5[8] = { eta, etanext, etanext, eta, eta, etanext, etanext, eta };

                    double ye5[8] = { -3.15, -3.15, 3.15, 3.15, phiedgePlus, phiedgerPlus, phiedgerMinus, phiedgeMinus };

                    h->AddBin(8, xe5, ye5);
                }
            } else { // normal case
                // if we pass the threshold, different treatment
                if (eta < -m_barrel_endcap_transition && etanext > -m_barrel_endcap_transition) { //            first threshold passage
                /*
                 *
                 *      here, we do:
                 *           |m_barrel_endcap_transition
                 *       1   2
                 *           3    4
                 *           6    5
                 *      8   |7
                 */

                    double x[] = { eta, -m_barrel_endcap_transition, -m_barrel_endcap_transition, etanext, etanext, -m_barrel_endcap_transition, -m_barrel_endcap_transition, eta };
                    double y[] = { phiEnext, phiEnext, phiBnext, phiBnext, phiB, phiB, phiE, phiE };
                    h->AddBin(8, x, y);
                } else if (eta < m_barrel_endcap_transition && etanext > m_barrel_endcap_transition) { //            second threshold passage
                /*
                 *
                 *      here, we do:
                 *           |m_barrel_endcap_transition
                 *           |3    4
                 *       1   2
                 *       8   7
                 *          |6     5
                 */
                    double x[] = { eta, m_barrel_endcap_transition, m_barrel_endcap_transition, etanext, etanext, m_barrel_endcap_transition, m_barrel_endcap_transition, eta };
                    double y[] = { phiBnext, phiBnext, phiEnext, phiEnext, phiE, phiE, phiB, phiB };
                    h->AddBin(8, x, y);
                } else { // normal case
                    /*
                     *
                     *      here, we do:
                     *
                     *        1     2
                     *
                     *        4     3
                     */

                    double phi = ((etanext <= -m_barrel_endcap_transition || eta >= m_barrel_endcap_transition) ? phiE : phiB); //EC
                    double phinext = ((etanext <= -m_barrel_endcap_transition || eta >= m_barrel_endcap_transition) ? phiEnext : phiBnext); //Barrel
                    double x[] = { eta, etanext, etanext, eta };
                    double y[] = { phinext, phinext, phi, phi };
                    h->AddBin(4, x, y);
                }
            }
        }

    }
    return h;
}
int fineEtaPhiBinning::getPolyBin(const int& binnumber) const {
    return binnumber + nbins() * 0.5;
}

int fineEtaPhiBinning::getSector(double phi) const {
    float lower = 0;
    float upper = 0;
    for (size_t sector = 0; sector < m_phi_lower_BAR.size(); ++sector) {
        upper = m_phi_upper_BAR.at(sector);
        lower = m_phi_lower_BAR.at(sector);
        if (upper < lower) { // for closing around pi
            if (phi > lower || phi <= upper) return sector + 1;
        }
        if (phi > lower && phi <= upper) return sector + 1;
    }
    return -9;
}

int fineEtaPhiBinning::getAbsEtaBin(double eta) const {

    double abseta = fabs(eta);
    for (size_t bin = 0; bin < m_abs_eta_bins.size(); ++bin) {
        if (abseta < m_abs_eta_bins.at(bin)) return bin;
    }
    // the last bin collects all overflows
    return m_abs_eta_bins.size() - 1;
}
int fineEtaPhiBinning::getEtaBin(const TLorentzVector & mst) const {
    double eta = mst.Eta();
    int etabin = getAbsEtaBin(eta);
    double signeta = eta >= 0 ? 1 : -1;
//     int addeta = signeta < 0 ? 0 : -1;
//     return (etabin + 1) * signeta + addeta;
    return etabin * signeta;
}
int fineEtaPhiBinning::getPhiSector(const TLorentzVector & m) const {
    return (fabs(m.Eta()) < m_barrel_endcap_transition ? getSector(m.Phi()) : getECSector(m.Phi()));
}
