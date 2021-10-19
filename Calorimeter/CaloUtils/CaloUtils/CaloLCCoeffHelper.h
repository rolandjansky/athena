/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOUTILS_CALOLCCOEFFHELPER_H
#define CALOUTILS_CALOLCCOEFFHELPER_H

#include <fstream>
#include <string>

#include "CaloConditions/CaloLocalHadCoeff.h"
#include "CaloConditions/CaloLocalHadDefs.h"
//#include "CaloLocalHadCoeff.h"


class CaloLCCoeffHelper {
  public:

    CaloLCCoeffHelper();
    virtual ~CaloLCCoeffHelper();

    static CaloLocalHadCoeff *InitDataFromFile(const char *fname);
    static void PrintData(CaloLocalHadCoeff *m_data, std::ostream &fout);
    static void PrintData(CaloLocalHadCoeff *m_data, const char *fname);

    static const CaloLocalHadCoeff::LocalHadArea * getAreaFromName(const CaloLocalHadCoeff * m_coeff, const std::string& sname, int &m_indx) ;

    // Interpolate m_data, n_area in phase space point x, over dimensions dim (all if 0)
    // return in pars
    static bool Interpolate(const CaloLocalHadCoeff *m_data, const unsigned int n_area, std::vector<float> &x, 
                     CaloLocalHadCoeff::LocalHadCoeff &pars, const std::vector<int> &dim, double xfit=0.);

    static CaloLocalHadDefs::LocalHadDimensionId getDimensionId(const std::string&  dimensionName)
    {
      if(dimensionName == "DIMC_SIDE") {
        return CaloLocalHadDefs::DIMC_SIDE;
      }else if(dimensionName == "DIMC_ETA") {
        return CaloLocalHadDefs::DIMC_ETA;
      }else if(dimensionName == "DIMC_PHI") {
        return CaloLocalHadDefs::DIMC_PHI;
      }else if(dimensionName == "DIMC_ENER") {
        return CaloLocalHadDefs::DIMC_ENER;
      }else if(dimensionName == "DIMC_EDENS") {
        return CaloLocalHadDefs::DIMC_EDENS;
      }else if(dimensionName == "DIMC_LAMBDA") {
        return CaloLocalHadDefs::DIMC_LAMBDA;

      }else if(dimensionName == "DIMW_SIDE") {
        return CaloLocalHadDefs::DIMW_SIDE;
      }else if(dimensionName == "DIMW_ETA") {
        return CaloLocalHadDefs::DIMW_ETA;
      }else if(dimensionName == "DIMW_PHI") {
        return CaloLocalHadDefs::DIMW_PHI;
      }else if(dimensionName == "DIMW_ENER") {
        return CaloLocalHadDefs::DIMW_ENER;
      }else if(dimensionName == "DIMW_EDENS") {
        return CaloLocalHadDefs::DIMW_EDENS;

      }else if(dimensionName == "DIMO_SIDE") {
        return CaloLocalHadDefs::DIMO_SIDE;
      }else if(dimensionName == "DIMO_PHI") {
        return CaloLocalHadDefs::DIMO_PHI;
      }else if(dimensionName == "DIMO_ENER") {
        return CaloLocalHadDefs::DIMO_ENER;
      }else if(dimensionName == "DIMO_ETA") {
        return CaloLocalHadDefs::DIMO_ETA;
      }else if(dimensionName == "DIMO_LAMBDA") {
        return CaloLocalHadDefs::DIMO_LAMBDA;

      }else if(dimensionName == "DIMD_EMFRAC") {
        return CaloLocalHadDefs::DIMD_EMFRAC;
      }else if(dimensionName == "DIMD_SIDE") {
        return CaloLocalHadDefs::DIMD_SIDE;
      }else if(dimensionName == "DIMD_ETA") {
        return CaloLocalHadDefs::DIMD_ETA;
      }else if(dimensionName == "DIMD_PHI") {
        return CaloLocalHadDefs::DIMD_PHI;
      }else if(dimensionName == "DIMD_ENER") {
        return CaloLocalHadDefs::DIMD_ENER;
      }else if(dimensionName == "DIMD_LAMBDA") {
        return CaloLocalHadDefs::DIMD_LAMBDA;

      }else{
        //std::cout << " CaloUtils/CaloLocalHadCoeffHelper/getDimensionId() -> Error! Unknown dimension '" << dimensionName << "'" << std::endl;
        return CaloLocalHadDefs::DIMU_UNKNOWN;
      }
    }

  private:
    static CaloLocalHadCoeff::LocalHadDimension *parse_dim(std::string &sLine);

};



#endif
