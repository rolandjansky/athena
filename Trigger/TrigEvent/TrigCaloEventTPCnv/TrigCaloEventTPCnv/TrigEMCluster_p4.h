/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigEMCluster_p4
 *
 * @brief persistent partner for TrigEMCluster
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEMCluster_p4.h,v 1.2 2011-05-31 22:01:37 damazio Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGEMCLUSTER_P4_H
#define TRIGCALOEVENTTPCNV_TRIGEMCLUSTER_P4_H

//need this for MAXSIZE, NUMEMSAMP, and NUMHADSAMP
#include "TrigCaloEvent/TrigEMCluster.h"
#include "DataModelAthenaPool/ElementLink_p3.h"
//#include "AthenaPoolUtilities/TPObjRef.h"
#include "TrigCaloEventTPCnv/TrigCaloCluster_p2.h"

class TrigEMCluster_p4 {
  friend class TrigEMClusterCnv_p4;
    
 public:
    
  // default constructor
  TrigEMCluster_p4() { }

  // default destructor
  ~TrigEMCluster_p4() { }

 private:

/*
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
*/
  float m_allFloats[14+MAXSIZE];

  //the TrigCaloCluster base class
  TrigCaloCluster_p2 m_trigCaloCluster;
//  TPObjRef m_trigCaloCluster;

  ElementLinkInt_p3 m_rings;

};

#endif
