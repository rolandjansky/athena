/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOLOCALHADCALIB_CALOHADDMCOEFFHELPER_H
#define CALOLOCALHADCALIB_CALOHADDMCOEFFHELPER_H

/**
 * @class CaloHadDMCoeffHelper
 * @version \$Id: CaloHadDMCoeffHelper.h,v 1.1 2009-03-03 17:30:21 pospelov Exp $
 * @author Gennady Pospelov <guennadi.pospelov@cern.ch>
 * @date 23-Januar-2009
 * @brief Helper class to read dead material correction coeffitients from ASCII file
 *
 * */

#include <string>

#include "CaloConditions/CaloHadDMCoeff2.h"


class CaloHadDMCoeffHelper {
  public:
    enum key_area_type { kAREA_PROF, kAREA_LOOKUP, kAREA_SMPW};

    CaloHadDMCoeffHelper();
    virtual ~CaloHadDMCoeffHelper();

    CaloHadDMCoeff2 *InitDataFromFile(std::string &hadDMCoeffFileName);

    const CaloHadDMCoeff2::HadDMArea * getHadDMAreaFromBin(CaloHadDMCoeff2 * m_coeff, int iBin) const;
    int getBin(CaloHadDMCoeff2 * m_coeff, const int n_area, int ifrac, int iener, int ilambda, int ieta) const;
    const CaloHadDMCoeff2::HadDMArea * getHadDMAreaFromName(CaloHadDMCoeff2 * m_coeff, const std::string& sname) const;

  private:
    int parse_dim(std::string &sLine, CaloHadDMCoeff2::HadDMDimension &dim);
};



#endif
