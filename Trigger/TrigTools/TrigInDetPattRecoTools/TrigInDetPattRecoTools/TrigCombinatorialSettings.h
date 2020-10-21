/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_COMBINATORIAL_SETTINGS_H__
#define __TRIG_COMBINATORIAL_SETTINGS_H__

class IRoiDescriptor;
#include <vector>
#include "TrigInDetPattRecoEvent/TrigInDetSiLayer.h"

typedef struct TrigCombinatorialSettings {
public:
  TrigCombinatorialSettings() {//provides some defaults

    //default silicon geometry 

    m_maxBarrelPix = 3;
    m_minEndcapPix = 7;
    m_maxEndcapPix = 10;
    m_maxSiliconLayer = 19;

    m_doubletD0Max         = 5.0;

    m_tripletD0Max      = 4.0; 
    m_tripletD0_PPS_Max = 1.7; 
    m_tripletPtMin      = 2500.0;//was 1000.0
    m_tripletDoPSS      = false; // Allow Pixel SCT SCT seeds?
    m_doubletFilterRZ   = true;
    m_tripletDtCut      = 3.0;//in sigmas of mult.scattering for m_tripletPtMin track at eta=0
    m_magFieldZ = 2.0;//switch to configured value
    m_nMaxPhiSlice = 53;
    m_maxTripletBufferLength = 3;

    m_zvError = 10.0;
    m_LRTmode=false;
    m_layerGeometry.clear();
  }

  int m_maxBarrelPix, m_minEndcapPix, m_maxEndcapPix, m_maxSiliconLayer;

  float m_doubletD0Max;
  float m_doublet_dR_Max;
  float m_magFieldZ;
  float m_tripletD0Max;
  float m_tripletD0_PPS_Max;
  float m_tripletPtMin;
  float m_seedRadBinWidth;
  bool  m_tripletDoPSS;
  bool  m_doubletFilterRZ;
  float m_tripletDtCut;
  int m_nMaxPhiSlice;
  unsigned int m_maxTripletBufferLength;

  float m_zvError;
  bool m_LRTmode;

  std::vector<TRIG_INDET_SI_LAYER> m_layerGeometry;
} TRIG_COMBINATORIAL_SETTINGS;



#endif
