/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGL0GEPPERF_CUSTOMCALOCELL_H
#define TRIGL0GEPPERF_CUSTOMCALOCELL_H

#include <vector>
#include <iostream>

namespace Gep{
  class CustomCaloCell
  {
    
  public:
    CustomCaloCell() {}
    ~CustomCaloCell() {}
    
    float e;
    float et;
    float time;
    unsigned int quality;
    unsigned int provenance;
    float totalNoise;
    float electronicNoise;
    float sigma;
    bool isBad;
    float eta;
    float phi;
    float etaMin;
    float etaMax;
    float phiMin;
    float phiMax;
    float etaGranularity;
    float phiGranularity;
    float sinTh;
    float cosTh;
    float sinPhi;
    float cosPhi;
    float cotTh;
    float x;
    float y;
    float z;
    int layer;
    bool isEM;
    bool isEM_barrel;
    bool isEM_endCap;
    bool isEM_barrelPos;
    bool isEM_barrelNeg;
    bool isFCAL;
    bool isHEC;
    bool isTile;
    unsigned int sampling;
    unsigned int id;
    std::string detName;
    std::vector<unsigned int> neighbours;
    
    bool isBadCell() const {return isBad;}
    
    
  private:
    
  };
}

#endif //TRIGL0GEPPERF_CUSTOMCALOCELL_H
