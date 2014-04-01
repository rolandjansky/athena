/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigEMCluster_tlp1
 *
 * @brief "top level" persistent partner for TrigEMCluster
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEMCluster_tlp1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGEMCLUSTER_TLP1_H
#define TRIGCALOEVENTTPCNV_TRIGEMCLUSTER_TLP1_H

#include "TrigCaloEventTPCnv/TrigEMCluster_p1.h"
#include "TrigCaloEventTPCnv/TrigEMCluster_p2.h"
#include "TrigCaloEventTPCnv/TrigCaloCluster_p1.h"

class TrigEMCluster_tlp1
{
 public:
  
  TrigEMCluster_tlp1() {}
  friend class TrigEMClusterCnv_tlp1; 
  
 private:
  
  std::vector< TrigEMCluster_p1 >              m_trigEMCluster;
  std::vector< TrigCaloCluster_p1 >            m_trigCaloCluster;
  std::vector< TrigEMCluster_p2 >              m_trigEMCluster_p2;

};

#endif
