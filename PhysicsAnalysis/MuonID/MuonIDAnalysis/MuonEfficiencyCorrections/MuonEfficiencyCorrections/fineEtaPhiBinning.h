/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef NEWETAPHI_BINNING_CLASS_H
#define NEWETAPHI_BINNING_CLASS_H

#include "TLorentzVector.h"
#include "TH2Poly.h"
#include <iostream>
#include <map>

/// defines a fine eta-phi binning
/// in 22 eta and 16 phi sectors
/// 352 bins in total
/// goblirsc@cern.ch 2013-05-24

class fineEtaPhiBinning {

    public:
        fineEtaPhiBinning();
        ~fineEtaPhiBinning() {
        }
        ;

        // This method returns the global bin used in the scale factor input files
        int bin(const TLorentzVector & m) const {
            return getPhiSector(m) + nphibins() * getEtaBin(m);
        }
        // returns the number of |eta| bins. 11 by default
        int nabsetabins() const {
            return m_abs_eta_bins.size();
        }
        // returns the number of eta bins. 22 by default
        int netabins() const {
            return 2 * nabsetabins() - 1;
        }
        // returns the number of phi bins. 16 by default
        int nphibins() const {
            return m_phi_upper_BAR.size();
        }
        // returns the overall number of bins
        int nbins() const {
            return netabins() * nphibins();
        }
        // returns the number of the first bin. default -175
        int firstbin() const {
            return 1 - (nphibins() * (nabsetabins() - 1));
        }
        // returns the number of the last bin. default +176
        int lastbin() const {
            return nphibins() + (nphibins() * (nabsetabins() - 1));
        }
        // :::::::::::::::::::::::::::::::::::::::::::::::
        // some method useful when plotting this beast
        // :::::::::::::::::::::::::::::::::::::::::::::::

        // generate an empty histogram with the fine eta-phi binning
        // caller is responsible for deleting the pointer!
        TH2Poly* getPolyHist(const std::string & name, const std::string & title) const;
        // get the TH2Poly-internal bin number corresponding the the fine eta phi bin number
        int getPolyBin(const int & binnumber) const;

    private:

        // used internally
        //  get the phi sector
        int getPhiSector(const TLorentzVector & m) const;
        //  get the eta bin
        int getEtaBin(const TLorentzVector & mst) const;
        // utility, used by getEtaBin
        int getAbsEtaBin(double eta) const;

        // called by GetPhiSector - for endcaps
        int getECSector(double phi) const;
        // called by GetPhiSector - for barrel
        int getSector(double phi) const;

        float m_barrel_endcap_transition; // location in |eta| where the endcaps start

        std::vector<float> m_abs_eta_bins; // bins in |eta|

        std::vector<float> m_phi_upper_BAR; // bin edges in the barrel
        std::vector<float> m_phi_lower_BAR;

        std::vector<float> m_phi_upper_EC; // bin edges in the endcaps
        std::vector<float> m_phi_lower_EC;

};
#endif
