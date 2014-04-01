/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauClusterContainer_tlp1
 *
 * @brief "top level" persistent partner for TrigTauClusterContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTauClusterContainer_tlp1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCONTAINER_TLP1_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCONTAINER_TLP1_H

#include "TrigCaloEventTPCnv/TrigTauClusterContainer_p1.h"
#include "TrigCaloEventTPCnv/TrigTauCluster_p1.h"
#include "TrigCaloEventTPCnv/TrigTauCluster_p2.h"
#include "TrigCaloEventTPCnv/TrigCaloCluster_p1.h"
#include "TrigCaloEventTPCnv/TrigCaloCluster_p2.h"

class TrigTauClusterContainer_tlp1 
{
   
 public:
   
  TrigTauClusterContainer_tlp1(){}
  friend class TrigTauClusterContainerCnv_tlp1;

 private:

  std::vector<TrigTauClusterContainer_p1>     m_TrigTauClusterContainers ;
  std::vector<TrigTauCluster_p1>              m_TauCluster               ;
  std::vector<TrigCaloCluster_p1>             m_CaloCluster          ;
  std::vector<TrigTauCluster_p2>              m_TauCluster_p2               ;
  std::vector<TrigCaloCluster_p2>             m_CaloCluster_p2       ;

};//end of class definitions
 
#endif
