/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauCluster_p1
 *
 * @brief persistent partner for TrigTauCluster
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTauCluster_p1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTER_P1_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTER_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"

//need this for MAXSIZE, NUMEMSAMP, and NUMHADSAMP
#include "TrigCaloEvent/TrigTauCluster.h"

class TrigTauCluster_p1
{
 public:
  
  TrigTauCluster_p1() : m_EMenergy(0.), m_HADenergy(0.), m_eEMCalib(0.), m_eCalib(0.), m_Eta(-999.), m_Phi(-999.0), m_IsoFrac(0.), m_numStripCells(0), m_stripWidth(0.) {}
  friend class TrigTauClusterCnv_p1;
  
 private:
  
  float m_EMenergy;
  float m_HADenergy;
  float m_eEMCalib;
  float m_eCalib;
  float m_EnergyS[MAXSIZE];
  float m_Eta;
  float m_Phi;
  float m_EMRadius[NUMEMSAMP];
  float m_IsoFrac;
  float m_EMenergyWidth[NUMEMSAMP];
  float m_HADenergyWidth[NUMHADSAMP];
  int    m_numStripCells;
  float m_stripWidth;
  
  float m_EMenergyNor[NUMEMSAMP];
  float m_EMenergyWid[NUMEMSAMP];
  float m_EMenergyNar[NUMEMSAMP];
  
  float m_HADenergyNor[NUMHADSAMP];
  float m_HADenergyWid[NUMHADSAMP];
  float m_HADenergyNar[NUMHADSAMP];
  
  //the TrigCaloCluster base class
  TPObjRef m_trigCaloCluster;

};

#endif
