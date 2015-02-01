/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __HICALOHELPER_H__
#define __HICALOHELPER_H__

#include <string>
#include "AthContainers/DataVector.h"
#include "xAODHIEvent/HIEventShapeContainer.h"


class CaloCell;


namespace HICaloHelper{

  float GetEtDensity(const CaloCell* theCell, float geoWeight);
  float GetAreaEtaPhi(const CaloCell* theCell);

  void UpdateShape(xAOD::HIEventShapeContainer* shape, const CaloCell* theCell, float geoWeight, float eta0, float phi0, bool isNeg=false);  

  std::string DumpCell(const CaloCell* theCell);
  
  float GetRho(const xAOD::HIEventShapeContainer* shape, float eta, unsigned int sampling);
  float GetHarmonicModulation(const xAOD::HIEventShapeContainer* shape, unsigned int nn, float eta, float phi, unsigned int sampling);
  inline float GetVN(const xAOD::HIEventShapeContainer*, unsigned int, float, unsigned int) {return 0.;}
  inline float GetPsiN(const xAOD::HIEventShapeContainer*, unsigned int, float, unsigned int) {return 0.;}


  extern unsigned int NumEtaBins;
  extern unsigned int NumPhiBins;
  extern float EtaMin;
  extern float EtaMax;
  extern float PhiMin;
  extern float PhiMax;
  inline float GetBinSizeEta() {return (EtaMax-EtaMin)/((float)NumEtaBins);}
  inline float GetBinSizePhi() {return (PhiMax-PhiMin)/((float)NumPhiBins);}

  inline float GetBinArea() {return GetBinSizeEta()*GetBinSizePhi();}
  inline float GetTotalArea() {return (EtaMax-EtaMin)*(PhiMax-PhiMin);}
  inline unsigned int GetNumBins() {return NumEtaBins*NumPhiBins;}

  inline float GetBinLowEdgeEta(unsigned int eb) {return EtaMin+eb*GetBinSizeEta();}
  inline float GetBinUpEdgeEta(unsigned int eb) {return EtaMin+(eb+1)*GetBinSizeEta();}
  inline float GetBinCenterEta(unsigned int eb) {return EtaMin+(eb+0.5)*GetBinSizeEta();}

  inline float GetBinLowEdgePhi(unsigned int pb) {return PhiMin+pb*GetBinSizePhi();}
  inline float GetBinUpEdgePhi(unsigned int pb) {return PhiMin+(pb+1)*GetBinSizePhi();}
  inline float GetBinCenterPhi(unsigned int pb) {return PhiMin+(pb+0.5)*GetBinSizePhi();}

  inline unsigned int FindBinEta(float eta) {return std::floor((eta-EtaMin)/GetBinSizeEta());}
  inline unsigned int FindBinPhi(float phi) 
  {
    int pb=std::floor((phi-PhiMin) /GetBinSizePhi());
    return pb % NumPhiBins;
  }
  inline unsigned int FindEtaPhiBin(float eta, float phi) { return NumPhiBins*FindBinEta(eta)+FindBinPhi(phi);}

  inline unsigned int FindEtaLayerBin(float eta, unsigned int layer) {return FindBinEta(eta)*24+layer;}
}


#endif

