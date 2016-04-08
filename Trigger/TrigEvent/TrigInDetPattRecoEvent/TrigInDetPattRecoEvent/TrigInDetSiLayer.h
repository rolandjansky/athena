/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_INDET_SI_LAYER_H__
#define __TRIG_INDET_SI_LAYER_H__

typedef struct TrigInDetSiLayer {
public:
  int m_subdet;//1 : Pixel, 2 : SCT
  int m_type;//0: barrel, +/-n : endcap
  float m_refCoord;
  float m_minBound, m_maxBound;
} TRIG_INDET_SI_LAYER;

#endif
