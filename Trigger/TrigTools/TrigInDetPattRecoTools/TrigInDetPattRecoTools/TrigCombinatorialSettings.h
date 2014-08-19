/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_COMBINATORIAL_SETTINGS_H__
#define __TRIG_COMBINATORIAL_SETTINGS_H__

class IRoiDescriptor;

typedef struct TrigCombinatorialSettings {
public:
  TrigCombinatorialSettings() {//provides some defaults

    //default silicon geometry 

    m_maxBarrelPix = 3;
    m_minEndcapPix = 7;
    m_maxEndcapPix = 10;
    m_maxSiliconLayer = 19;

    m_doubletD0Max         = 5.0;
    m_doubletDphiOverDrMax = 0.001;
    m_doubletZ0Max         = 230.0;

    m_tripletRZMax      = 5.0;//was 8.0
    m_tripletRZMaxPixel = 1.0;//was 5.0
    m_tripletRPhiMax    = 0.0035;
    m_tripletD0Max      = 4.0; 
    m_tripletPtMin      = 2500.0;//was 1000.0
    m_tripletChi2Max    = 100.0;
    m_tripletCutRoiEta = false;
    m_tripletCutRoiPhi = false;
    m_tripletFilter         = true;
    m_tripletFilterPhiScale = 0.0006;
    m_tripletFilterEtaScale = 0.0022;
    m_tripletFilterDRMax    = 9.0;
    
    m_tripletNBest      = 2;//retain only N  best triplets per cluster
    m_useLikelihood = false;
    m_tripletMinLLR     = -20.0;
    m_tripletMaxLayers = 100;//Maximum number of layers for triplets 
    m_tripletDoMaxLayers = true;
    m_minTriplets      = 1;
    m_magFieldZ = 2.0;//switch to configured value
    m_tripletCloneRemoveThreshold = 0.6;
    m_prefitTripletClusters = false;
  }

  int m_maxBarrelPix, m_minEndcapPix, m_maxEndcapPix, m_maxSiliconLayer;

  float m_doubletD0Max;
  float m_doubletDphiOverDrMax;
  float m_doubletZ0Max;
  float m_tripletRZMax;  
  float m_magFieldZ;
  float m_tripletRZMaxPixel;
  float m_tripletRPhiMax;
  float m_tripletD0Max;
  float m_tripletPtMin;
  float m_tripletChi2Max;
  bool  m_tripletCutRoiEta;
  bool  m_tripletCutRoiPhi;
  bool  m_tripletFilter;
  float m_tripletFilterPhiScale;
  float m_tripletFilterEtaScale;
  float m_tripletFilterDRMax;
  unsigned int m_tripletNBest;
  bool m_useLikelihood;
  float m_tripletMinLLR;
  float m_tripletCloneRemoveThreshold;
  bool  m_tripletDoMaxLayers;
  int m_tripletMaxLayers;//Maximum number of layers for triplets
  int m_minTriplets;
  bool m_prefitTripletClusters;
  const IRoiDescriptor* roiDescriptor;
} TRIG_COMBINATORIAL_SETTINGS;



#endif
