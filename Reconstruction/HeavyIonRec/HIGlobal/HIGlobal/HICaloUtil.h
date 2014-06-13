/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HICaloData.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef HICALOUTIL_H
#define HICALOUTIL_H

#include <string>
#include <vector> 

/** @class HICaloUtil
 *   @author  Adam Trzupek <adam.Trzupek@ifj.edu.pl>
 *   @author  Dominik Derendarz <Dominik.Derendarz@ifj.edu.pl>
 *   @date 26 June 2009. 
 *   @brief Utility class for HIFlow and HIGlobal algorithms.
 */  

class HICaloUtil 
  {
  public:
    // order of detectors is important 
    enum CaloDetectors {
      PreSamplerBN=0, EMBN1, EMBN2, EMBN3,   // LAr barrel N (negative eta) side
      PreSamplerEN, EMEN1, EMEN2, EMEN3,     // LAr EM endcap N side
      HECN0, HECN1, HECN2, HECN3,            // Hadronic end cap cal. N side
      TileBarN0, TileBarN1, TileBarN2,       // Tile CLHEP::bar (ITC & scint) N side
      TileGapN1, TileGapN2, TileGapN3,       // Tile gap (ITC & scint) N side
      TileExtN0, TileExtN1, TileExtN2,       // Tile extended barrel   N side
      FCALN0, FCALN1, FCALN2,                // Forward EM endcap      N side 
      PreSamplerBP, EMBP1, EMBP2, EMBP3,     // LAr barrel  P side
      PreSamplerEP, EMEP1, EMEP2, EMEP3,     // LAr EM endcap P side
      HECP0, HECP1, HECP2, HECP3,            // Hadronic end cap cal. P side
      TileBarP0, TileBarP1, TileBarP2,       // Tile CLHEP::bar (ITC & scint) P side
      TileGapP1, TileGapP2, TileGapP3,       // Tile gap (ITC & scint) P side
      TileExtP0, TileExtP1, TileExtP2,       // Tile extended barrel   P side
      FCALP0, FCALP1, FCALP2,                // Forward EM endcap      P side     
      Unknown
    };

    enum CaloSample {
     PreSamplerB=0, EMB1, EMB2, EMB3,              // LAr barrel
     PreSamplerE, EME1, EME2, EME3,                // LAr EM endcap
     HEC0, HEC1, HEC2, HEC3,                       // Hadronic end cap cal.
     TileBar0, TileBar1, TileBar2,                 // Tile barrel
     TileGap1, TileGap2, TileGap3,                 // Tile gap (ITC & scint)
     TileExt0, TileExt1, TileExt2,                 // Tile extended barrel
     FCAL0, FCAL1, FCAL2,                          // Forward EM endcap
     End
    };
   
    HICaloUtil();
     ~HICaloUtil();
 
    static std::string CaloSampleImage(int inum);
    static std::string CaloDetImage(int inum);
    void    SetToZero(std::vector<float> *x);
    void    MappingDetectors(); 
    int*    GetMap(){return m_map;}
    int*    GetMapN(){return m_mapN;}
    int*    GetMapP(){return m_mapP;}
    void    InitBinSizeEtaRange(const bool CaloTag[HICaloUtil::Unknown]);
    void    BinSize();
    void    EtaRange(const bool CaloTag[HICaloUtil::Unknown]);
    static bool isB(int i);
    static bool isN(int i);
    static bool isP(int i);
    std::vector<float>* GetEtaRange(){return m_EtaRange;}
    float* GetBinSize(){return m_BinSize;}
    /** C_Correction is the fuction calculating reconstructed transverse energy in eta bins 0.2, which is stored in m_Et_final  */
    float  C_Correction(float eta);     
    /** same as abowe but for pp data   */
    float C_Correction_pp(float eta); 
    
  private:
    
    int* m_map; 
    int* m_mapN;
    int* m_mapP;
    float m_BinSize[HICaloUtil::Unknown];
    std::vector<float> m_EtaRange[HICaloUtil::Unknown];    
  }; 

#endif 
