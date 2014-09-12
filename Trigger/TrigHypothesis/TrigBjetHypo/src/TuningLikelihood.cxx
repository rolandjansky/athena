/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TuningLikelihood.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#include "TrigBjetHypo/TuningLikelihood.h"



//** ----------------------------------------------------------------------------------------------------------------- **//


TuningLikelihood::TuningLikelihood(const float *sizeLikelihood, const float *bLikelihood,
				   const float *uLikelihood, const int dimensionLikelihood) :
  m_bLikelihood(&bLikelihood[0]),
  m_uLikelihood(&uLikelihood[0]),
  m_dimensionLikelihood(dimensionLikelihood)
{
  
  m_numXLikelihood = &sizeLikelihood[0];
  m_minXLikelihood = &sizeLikelihood[1];
  m_maxXLikelihood = &sizeLikelihood[2];
  
  if (dimensionLikelihood >= 6) {
    m_numYLikelihood = &sizeLikelihood[3];
    m_minYLikelihood = &sizeLikelihood[4];
    m_maxYLikelihood = &sizeLikelihood[5];
  } 

  if (dimensionLikelihood >= 9) {
    m_numZLikelihood = &sizeLikelihood[6];
    m_minZLikelihood = &sizeLikelihood[7];
    m_maxZLikelihood = &sizeLikelihood[8];
  }
} 


//** ----------------------------------------------------------------------------------------------------------------- **//


TuningLikelihood::~TuningLikelihood() {}
