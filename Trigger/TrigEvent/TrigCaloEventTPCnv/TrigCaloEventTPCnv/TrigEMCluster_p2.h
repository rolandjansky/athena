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
 * $Id: TrigEMCluster_p2.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGEMCLUSTER_P2_H
#define TRIGCALOEVENTTPCNV_TRIGEMCLUSTER_P2_H

#include "AthenaPoolUtilities/TPObjRef.h"

//need this for MAXSIZE, NUMEMSAMP, and NUMHADSAMP
#include "TrigCaloEvent/TrigEMCluster.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"

class TrigEMCluster_p2 {
  friend class TrigEMClusterCnv;
  template <class T>
  friend class TrigEMClusterConverterBase;
    
 public:
    
  // default constructor
  TrigEMCluster_p2() : m_Energy(0.), m_Et(0.), m_Eta(-99.), m_Phi(-99.), m_e237(0.), m_e277(999999.), m_fracs1(0.), m_weta2(0.), m_ehad1(0.), m_Eta1(-99.), m_emaxs1(0.), m_e2tsts1(0.)  { }

  // default destructor
  ~TrigEMCluster_p2() { }

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

  ElementLinkInt_p1 m_rings;

};

#endif
