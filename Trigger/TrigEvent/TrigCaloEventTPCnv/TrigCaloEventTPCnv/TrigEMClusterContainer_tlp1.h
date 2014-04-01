/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigEMClusterContainer_tlp1
 *
 * @brief "top level" persistent partner for TrigEMClusterContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEMClusterContainer_tlp1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGEMCLUSTERCONTAINER_TLP1_H
#define TRIGCALOEVENTTPCNV_TRIGEMCLUSTERCONTAINER_TLP1_H

#include "TrigCaloEventTPCnv/TrigEMClusterContainer_p1.h"
#include "TrigCaloEventTPCnv/TrigEMCluster_p1.h"
#include "TrigCaloEventTPCnv/TrigEMCluster_p2.h"
#include "TrigCaloEventTPCnv/TrigCaloCluster_p1.h"
#include "TrigCaloEventTPCnv/TrigCaloCluster_p2.h"

class TrigEMClusterContainer_tlp1 
{
   
 public:
   
  TrigEMClusterContainer_tlp1(){}
  friend class TrigEMClusterContainerCnv_tlp1;

 private:

  std::vector<TrigEMClusterContainer_p1>       m_TrigEMClusterContainers ;
  std::vector<TrigEMCluster_p1>                m_EMCluster               ;
  std::vector<TrigCaloCluster_p1>           m_CaloCluster          ;
  std::vector<TrigEMCluster_p2>                m_EMCluster_p2               ;
  std::vector<TrigCaloCluster_p2>           m_CaloCluster_p2       ;

};//end of class definitions
 
#endif
