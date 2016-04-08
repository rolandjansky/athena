/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigCaloCluster_p2
 *
 * @brief persistent partner for TrigCaloCluster
 *
 * @author Denis Oliveira Damazio <Denis.Oliveira.Damazio@cern.ch>
 *
 * File and Version Information:
 * $Id: TrigCaloCluster_p3.h 697601 2015-09-30 13:34:01Z damazio $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGCALOCLUSTER_P3_H
#define TRIGCALOEVENTTPCNV_TRIGCALOCLUSTER_P3_H

class TrigCaloCluster_p3
{
 public:
  static const int MAXSIZE_P = 25;
  
  TrigCaloCluster_p3() : m_roiWord(0), m_numberUsedCells(0), m_quality(0) {}
  friend class TrigCaloClusterCnv_p3;
  
 private:

/*
  float        m_rawEnergy;
  float        m_rawEt;
  float        m_rawEnergyS[MAXSIZE];
  float        m_rawEta;
  float        m_rawPhi;
*/
  float m_allFloats[4+MAXSIZE_P];

  long         m_roiWord;
  int          m_numberUsedCells;
  unsigned int m_quality;

};

#endif
