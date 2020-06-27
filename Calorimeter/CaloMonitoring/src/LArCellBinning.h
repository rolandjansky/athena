/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


// ***********************************************************
//                                                            
// CaloCellbinning - class to hold granularity info   
// for cellbinnings of the Calorimeter. To be used with             
// CaloCellVecMon class                                       
//                                                            
// ***********************************************************                                                                                                                                                   

#ifndef LAR_CELLBINNING_H
#define LAR_CELLBINNING_H

#include <vector>

namespace CaloMonitoring {

  class LArCellBinning {
  
  public:
    LArCellBinning() {};
    ~LArCellBinning() = default;

    //Getters
    int         getNTotEtaBins() const { return  m_etaBinArray.size()-1; };
    int         getNTotPhiBins() const { return m_phiBinArray.size()-1; };
    const double*     getEtaBinArray() const { return m_etaBinArray.data(); };
    const double*     getPhiBinArray() const { return m_phiBinArray.data(); };
  
    // Binning - creates variable sized bin arrays
    void        doEtaBinning(const int nEtaRegions, const double* etaBreakPts, const int* nEtaBins);
    void        doPhiBinning(const int nPhiRegions, const double* phiBreakPts, const int* nPhiBins);

    // Binning for phi for the common case with only one granularity region
    void        doPhiBinning(int nPhiBins);

    
    LArCellBinning etaMirror() const;

  private:
    std::vector<double> m_etaBinArray;
    std::vector<double> m_phiBinArray;
  };
}

#endif // LAr_CELLBINNING_H
