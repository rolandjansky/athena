/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CLASS DERIVED FROM HARVARD GROUP's CLASS //

#ifndef ETAPHI_BINNING_CLASS_H
#define ETAPHI_BINNING_CLASS_H

#include "TLorentzVector.h"
#include <iostream>


class EtaPhiBinning {

    public:
        EtaPhiBinning(){ };
        virtual ~EtaPhiBinning() {;};

        virtual int bin(const TLorentzVector & m) const {
            if(m.Eta() > 0)
                return this->symmetricBin(m) + nBINREGIONS;

            return nBINREGIONS - this->symmetricBin(m);
        }
        virtual int symmetricBin(const TLorentzVector & m) const;

        enum binregion{ binUNKNOWN=0, bin1BARRELLG=1, bin1BARRELSM=2, bin2BARREL=3, binFEET=4,
			binTRANSITION=5, binENDCAPLG=6, binENDCAPSM=7, binBEE=8, binFORWARDLG=9, 
			binFORWARDSM=10, binCRACKLG = 11,  binCRACKSM = 12, binCRACK = 13,
			nBINREGIONS // keep this at the end!
        };
        std::string GetRegionLabel(int region) const;

    private:
        int getCoarseNSector(double phi) const;
        int getSector(double phi) const;
        int getECSector(double phi) const;

};

#endif
