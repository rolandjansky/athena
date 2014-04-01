/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigEMCluster_p1
 *
 * @brief persistent partner for TrigEMCluster
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEMCluster_p1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGEMCLUSTER_P1_H
#define TRIGCALOEVENTTPCNV_TRIGEMCLUSTER_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"

//need this for MAXSIZE, NUMEMSAMP, and NUMHADSAMP
#include "TrigCaloEvent/TrigEMCluster.h"

// needed to fix a Gaudi problem in SLC3 (should be removed eventually):
#include <stdint.h>

class TrigEMCluster_p1 {
  friend class TrigEMClusterCnv;
  template <class T>
  friend class TrigEMClusterConverterBase;
    
 public:
    
  // default constructor
  TrigEMCluster_p1() { }

  // default destructor
  ~TrigEMCluster_p1() { }


 private:

  float m_Energy;
  float m_Et;
  float m_EnergyS[MAXSIZE];
  float m_Eta;
  float m_Phi;
  float m_e237;
  float m_e277;
  float m_fracs1;
  float m_weta2;
  float m_ehad1;
  float m_Eta1;
  float m_emaxs1;
  float m_e2tsts1;

  //the TrigCaloCluster base class
  TPObjRef m_trigCaloCluster;

};

#endif
