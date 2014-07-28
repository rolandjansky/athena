/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     HistoVertexHelper.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigT2HistoPrmVtx
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#include "TrigT2HistoPrmVtx/HistoVertexHelper.h"



//** ----------------------------------------------------------------------------------------------------------------- **//


HistoVertexHelper::HistoVertexHelper() :
  m_N(8000),
  m_min(-200),
  m_max(200)
{ m_zCoord = new int[8001]; reset();}


//** ----------------------------------------------------------------------------------------------------------------- **//


HistoVertexHelper::HistoVertexHelper(int N, int min, int max) :
  m_N(N),
  m_min(min),
  m_max(max)
{ m_zCoord = new int[N+1]; reset();}


//** ----------------------------------------------------------------------------------------------------------------- **//


HistoVertexHelper::~HistoVertexHelper() {

  delete [] m_zCoord;

}


//** ----------------------------------------------------------------------------------------------------------------- **//


bool HistoVertexHelper::fill(float z) {

  if (z < m_min)
    z = m_min;
  if (z > m_max)
    z = m_max;

  int i;
  i = (int)(((z - m_min)/(m_max - m_min))*m_N);
  m_zCoord[i] ++;

  return true;

}


//** ----------------------------------------------------------------------------------------------------------------- **//


int HistoVertexHelper::getN(int i) {

  return m_zCoord[i];

}


//** ----------------------------------------------------------------------------------------------------------------- **//


int HistoVertexHelper::getBin(float z) {

  return (int)(((z - m_min)/(m_max - m_min))*m_N);

}


//** ----------------------------------------------------------------------------------------------------------------- **//


float HistoVertexHelper::getZ(float i) {

  return (((float)(m_max - m_min)/m_N)*(i+0.5) + m_min); 

}


//** ----------------------------------------------------------------------------------------------------------------- **//


int HistoVertexHelper::getIntegral(int min, int max) {

  int integral = 0;

  for (int i = (min+1); i <= (min+max+1); i++)
    integral += m_zCoord[i];

  return integral;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


bool HistoVertexHelper::reset() {

  for (int i = 0; i < m_N; i++)
    m_zCoord[i] = 0;

  return true;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


bool HistoVertexHelper::reset(int min, int max) {

  for (int i = (min+1); i <= (min+max+1); i++)
    m_zCoord[i] = 0;

  return true;

}
