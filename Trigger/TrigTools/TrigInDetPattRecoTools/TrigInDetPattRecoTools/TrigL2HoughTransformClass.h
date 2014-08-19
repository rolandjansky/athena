/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_HOUGH_TRANSFORM_CLASS_H__
#define __TRIG_L2_HOUGH_TRANSFORM_CLASS_H__

#include <map>
#include <list>

#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigInDetPattRecoTools/HoughBin2D.h"
#include "TrigInDetPattRecoTools/HoughBinCluster.h"

class TrigL2HoughTransformClass {
 public:

  TrigL2HoughTransformClass(long maxY, bool doCheck = true, long coeff=1000) : 
    m_binCounter(0), m_layerThreshold(3.0), m_maxYbin(maxY), m_checkYwraparound(doCheck), m_hashKeyCoeff(coeff) {
    m_binMap = new HoughAccumulatorType;
  }

  TrigL2HoughTransformClass(const TrigL2HoughTransformClass& ht) : m_binCounter(ht.m_binCounter), 
    m_layerThreshold(ht.m_layerThreshold), 
    m_maxYbin(ht.m_maxYbin), m_checkYwraparound(ht.m_checkYwraparound), m_hashKeyCoeff(ht.m_hashKeyCoeff) {
    m_binMap = new HoughAccumulatorType;
  }

  TrigL2HoughTransformClass& operator = (const TrigL2HoughTransformClass& ht) {
    m_binCounter = ht.m_binCounter;
    m_layerThreshold = ht.m_layerThreshold; 
    m_maxYbin = ht.m_maxYbin;
    m_checkYwraparound = ht.m_checkYwraparound;
    m_hashKeyCoeff = ht.m_hashKeyCoeff;
    m_binMap = new HoughAccumulatorType(*ht.m_binMap);
    return *this;
  }

  ~TrigL2HoughTransformClass() {
    delete m_binMap; 
  }
  void clusterBins(std::list<HoughBinCluster>&, double);

  HoughBin2D& operator[](long key) {
    return (*m_binMap)[key];
  }

  long hashKeyCoeff() {
    return m_hashKeyCoeff;
  }

 protected:

  HoughAccumulatorType* m_binMap;
  long m_binCounter;
  void joinGroup(long, HoughBin2D&, std::list<HoughBinCluster>::iterator, std::list<HoughBinCluster>&);
  void neighborKeys(long, long*);
  double m_layerThreshold;
  long m_maxYbin;
  bool m_checkYwraparound;
  long m_hashKeyCoeff;

};



#endif
