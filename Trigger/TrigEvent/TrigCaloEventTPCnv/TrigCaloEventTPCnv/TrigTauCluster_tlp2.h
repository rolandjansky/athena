/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauCluster_tlp2
 *
 * @brief "top level" persistent partner for TrigTauCluster
 *
 * @author M. Pilar Casado  <casado@ifae.es>  - IFAE-Barcelona
 *
 * File and Version Information:
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTER_TLP2_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTER_TLP2_H

#include "TrigCaloEventTPCnv/TrigTauCluster_p2.h"
#include "TrigCaloEventTPCnv/TrigCaloCluster_p1.h"

class TrigTauCluster_tlp2
{
 public:
  
  TrigTauCluster_tlp2() {}
  friend class TrigTauClusterCnv_tlp2; 
  
 private:
  
  std::vector< TrigTauCluster_p2 >    m_trigTauCluster;
  std::vector< TrigCaloCluster_p1 >   m_trigCaloCluster;

};

#endif
