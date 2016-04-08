/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NEWETAPHI_BINNING_CLASS_H
#define NEWETAPHI_BINNING_CLASS_H

#include "TLorentzVector.h"
#include <iostream>

/// defines a fine eta-phi binning
/// in 22 eta and 16 phi sectors
/// 352 bins in total
/// goblirsc@cern.ch 2013-05-24


class fineEtaPhiBinning {

public:
    fineEtaPhiBinning(){};
    ~fineEtaPhiBinning() {};

    // This method returns the global bin used in the scale factor input files
    static int bin(const TLorentzVector & m) {
        return getPhiSector(m) + 16 * getEtaBin(m);
    }

    // returns the number of |eta| bins. 11 by default
    static int nabsetabins () {
        return 1 + getAbsEtaBin(9.5);   // we access this using the highest reachable bin...
    }
    // returns the number of eta bins. 22 by default
    static int netabins ()  {
        return 2 *nabsetabins();
    }
    // returns the overall number of bins
    static int nbins ()  {
        return netabins() * 16;
    }
    // returns the number of the first bin. default -175
    static int firstbin ()  {
        return  1 - (16 * nabsetabins());
    }
    // returns the number of the last bin. default +176
    static int lastbin ()  {
        return  16 + (16 * (nabsetabins()-1));
    }



private:

    // used internally
    //  get the phi sector
    static int getPhiSector(const TLorentzVector & m) {
        return (fabs(m.Eta()) < 1.19 ? getSector(m.Phi()) : getECSector(m.Phi()));
    }
    //  get the eta bin
    static int getEtaBin (const TLorentzVector & mst)   {
        double eta = mst.Eta();
        int etabin = getAbsEtaBin(eta);
        double signeta = eta >= 0 ? 1 : -1;
        int addeta = signeta < 0 ? 0 : -1;
        return (etabin + 1) * signeta + addeta;
    }
    static int getAbsEtaBin (double eta) {

        double abseta = fabs(eta);

        if (abseta < 0.21) return 0;
        else if (abseta < 0.42) return 1;
        else if (abseta < 0.63) return 2;
        else if (abseta < 0.84) return 3;
        else if (abseta < 1.05) return 4;
        else if (abseta < 1.37) return 5;
        else if (abseta < 1.52) return 6;
        else if (abseta < 1.74) return 7;
        else if (abseta < 1.95) return 8;
        else if (abseta < 2.18) return 9;
        else return 10;
    }

    // called by GetPhiSector
    static int getECSector(double phi) {

        int sector = -9;
        if (phi>3.011 || phi<=-3.011) sector = 9;
        else if (phi>2.487 && phi<=3.011) sector = 8;
        else if (phi>2.225 && phi<=2.487) sector = 7;
        else if (phi>1.702 && phi<=2.225) sector = 6;
        else if (phi>1.440 && phi<=1.702) sector = 5;
        else if (phi>0.916 && phi<=1.440) sector = 4;
        else if (phi>0.655 && phi<=0.916) sector = 3;
        else if (phi>0.131 && phi<=0.655) sector = 2;
        else if (phi>-0.131 && phi<=0.131) sector = 1;
        else if (phi>-0.655 && phi<=-0.131) sector = 16;
        else if (phi>-0.916 && phi<=-0.655) sector = 15;
        else if (phi>-1.440 && phi<=-0.916) sector = 14;
        else if (phi>-1.702 && phi<=-1.440) sector = 13;
        else if (phi>-2.225 && phi<=-1.702) sector = 12;
        else if (phi>-2.487 && phi<=-2.225) sector = 11;
        else if (phi>-3.011 && phi<=-2.487) sector = 10;
        return sector;

    }
    static int getSector(double phi)  {

        int sector = -9;
        if (phi>2.905 || phi<=-2.905) sector = 9;
        else if (phi>2.59 && phi<=2.905) sector = 8;
        else if (phi>2.12 && phi<=2.59) sector = 7;
        else if (phi>1.805 && phi<=2.12) sector = 6;
        else if (phi>1.335 && phi<=1.805) sector = 5;
        else if (phi>1.02 && phi<=1.335) sector = 4;
        else if (phi>0.55 && phi<=1.02) sector = 3;
        else if (phi>0.235 && phi<=0.55) sector = 2;
        else if (phi>-0.235 && phi<=0.235) sector = 1;
        else if (phi>-0.55 && phi<=-0.235) sector = 16;
        else if (phi>-1.02 && phi<=-0.55) sector = 15;
        else if (phi>-1.335 && phi<=-1.02) sector = 14;
        else if (phi>-1.805 && phi<=-1.335) sector = 13;
        else if (phi>-2.12 && phi<=-1.805) sector = 12;
        else if (phi>-2.59 && phi<=-2.12) sector = 11;
        else if (phi>-2.905 && phi<=-2.59) sector = 10;
        return sector;

    }

};
#endif
