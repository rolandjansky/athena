/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ***********************************************************
//                                                            
// CaloLayer - class to hold granularity info   
// for layers of the Calorimeter. To be used with             
// CaloCellVecMon class                                       
//                                                            
// ***********************************************************                                                                                                                                                   

#ifndef CALO_LAYER_H
#define CALO_LAYER_H

namespace CaloMonitoring {

  class CaloLayer {
  
  public:

    enum Partition { EMB, HEC, EMEC, FCAL, MAXPART };

    enum Side { A, C };  

    CaloLayer(const char* name,Partition partition, int layerNumber, Side side);
    ~CaloLayer();

    // inline functions
    const char* getName()        { return m_name;        };
    int         getNTotEtaBins() { return m_nTotEtaBins; };
    int         getNTotPhiBins() { return m_nTotPhiBins; };
    double*     getEtaBinArray() { return m_etaBinArray; };
    double*     getPhiBinArray() { return m_phiBinArray; };
    int         getLayerNumber() { return m_layerNumber; };
    Partition   getPartition()   { return m_partition;   };
    Side        getSide()        { return m_side;        };

    void        setProperties(Partition partition, int layerNumber, Side side);
  
    // Binning - creates variable sized bin arrays
    void        doEtaBinning(const int nEtaRegions, double* etaBreakPts, int* nEtaBins);
    void        doPhiBinning(const int nPhiRegions, double* phiBreakPts, int* nPhiBins);

    // Binning for phi for the common case with only one granularity region
    void        doPhiBinning(int nPhiBins);

    // Takes an existing "side A" CaloLayer and creates phi and eta bin arrays
    // for side C
    void        doSideCBinning(CaloLayer * lyr);

  private:
    const char* m_name;

    int         m_nTotEtaBins;
    double*     m_etaBinArray;
    int         m_nTotPhiBins;
    double*     m_phiBinArray;

    int         m_layerNumber;
    Partition   m_partition;
    Side        m_side;
  };
}

#endif // CALO_LAYER_H
