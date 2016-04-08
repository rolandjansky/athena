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
 * $Id: TrigCaloCluster_p2.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGCALOCLUSTER_P2_H
#define TRIGCALOEVENTTPCNV_TRIGCALOCLUSTER_P2_H


class TrigCaloCluster_p2
{
 public:
  static const int MAXSIZE_P = 25;

  TrigCaloCluster_p2() : m_rawEnergy(0.), m_rawEt(0.), m_rawEta(-99.), m_rawPhi(-99.), m_roiWord(0), m_numberUsedCells(0), m_quality(0) {}
  friend class TrigCaloClusterCnv_p2;
  
 private:

  float        m_rawEnergy;
  float        m_rawEt;
  float        m_rawEnergyS[MAXSIZE_P];
  float        m_rawEta;
  float        m_rawPhi;

  long         m_roiWord;
  int          m_numberUsedCells;
  unsigned int m_quality;

};

#endif
