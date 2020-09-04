/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOLOCALHADCALIB_CALOLOCALHADCOEFFHELPER_H
#define CALOLOCALHADCALIB_CALOLOCALHADCOEFFHELPER_H

#include <fstream>
#include <string>

#include "CaloConditions/CaloLocalHadCoeff.h"


class CaloLocalHadCoeffHelper {
  public:

    enum keys_dim_order { DIM_EMFRAC, DIM_SIDE, DIM_ETA, DIM_PHI, DIM_ENER, DIM_LAMBDA, DIM_UNKNOWN};

    CaloLocalHadCoeffHelper();
    virtual ~CaloLocalHadCoeffHelper();

    CaloLocalHadCoeff *InitDataFromFile(const char *fname);
    void PrintData(const CaloLocalHadCoeff *data, std::ostream &fout);
    void PrintData(const CaloLocalHadCoeff *data, const char *fname);

    const CaloLocalHadCoeff::LocalHadArea * getAreaFromName(const CaloLocalHadCoeff * coeff, const std::string& sname, int &indx) const;

  private:
    CaloLocalHadCoeff::LocalHadDimension *parse_dim(std::string &sLine);

};



#endif
