/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauCluster_tlp1
 *
 * @brief "top level" persistent partner for TrigTauCluster
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTauCluster_tlp1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTER_TLP1_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTER_TLP1_H

#include "TrigCaloEventTPCnv/TrigTauCluster_p1.h"
#include "TrigCaloEventTPCnv/TrigTauCluster_p2.h"
#include "TrigCaloEventTPCnv/TrigCaloCluster_p1.h"

class TrigTauCluster_tlp1
{
 public:
  
  TrigTauCluster_tlp1() {}
  friend class TrigTauClusterCnv_tlp1; 
  
 private:
  
  std::vector< TrigTauCluster_p1 >    m_trigTauCluster;
  std::vector< TrigCaloCluster_p1 >   m_trigCaloCluster;
  std::vector< TrigTauCluster_p2 >    m_trigTauCluster_p2;

};

#endif
