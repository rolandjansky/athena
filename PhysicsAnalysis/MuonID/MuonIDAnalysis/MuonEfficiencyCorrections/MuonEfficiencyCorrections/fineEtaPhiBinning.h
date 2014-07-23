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
        fineEtaPhiBinning();
        virtual ~fineEtaPhiBinning() {;};

        // This method returns the global bin used in the scale factor input files 
        virtual int bin(const TLorentzVector & m) const;
        
        // returns the number of |eta| bins. 11 by default
        int nabsetabins (void) const{
            return absetabins.size() - 1;
        }
        // returns the number of eta bins. 22 by default
        int netabins (void) const {
            return 2 *nabsetabins();
        }
        // returns the overall number of bins
        int nbins (void) const {
            return netabins() * 16;
        }
        // returns the number of the first bin. default -175
        int firstbin (void) const {
            return  1 - (16 * nabsetabins());
        }
        // returns the number of the last bin. default +176
        int lastbin (void) const {
            return  16 + (16 * (nabsetabins()-1));
        }
        
      

    private:

        // used internally 
        //  get the phi sector
        int getPhiSector(const TLorentzVector & m) const;
        //  get the eta bin
        int getEtaBin (const TLorentzVector & m) const;
        
        // called by GetPhiSector
        int getSector(double phi) const;
        int getECSector(double phi) const;
        std::vector<double> absetabins;

};

#endif
