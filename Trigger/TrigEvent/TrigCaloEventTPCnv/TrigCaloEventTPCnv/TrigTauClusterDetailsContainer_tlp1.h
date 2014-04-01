/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauClusterDetailsContainer_tlp1
 *
 * @brief "top level" persistent partner for TrigTauClusterDetailsContainer
 *
 * @author M. Pilar Casado  <casado@ifae.es>  - IFAE-Barcelona
**********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERDETAILSCONTAINER_TLP1_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERDETAILSCONTAINER_TLP1_H

#include "TrigCaloEventTPCnv/TrigTauClusterDetailsContainer_p1.h"
#include "TrigCaloEventTPCnv/TrigTauClusterDetails_p1.h"

class TrigTauClusterDetailsContainer_tlp1 
{
   
 public:
   
  TrigTauClusterDetailsContainer_tlp1(){}
  friend class TrigTauClusterDetailsContainerCnv_tlp1;

 private:

  std::vector<TrigTauClusterDetailsContainer_p1>     m_TrigTauClusterDetailsContainers ;
  std::vector<TrigTauClusterDetails_p1>              m_TauClusterDetails               ;

};//end of class definitions
 
#endif
