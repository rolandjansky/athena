/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauClusterDetailsContainer_p1
 *
 * @brief persistent partner for TrigTauClusterDetailsContainer
 *
 * @author M. Pilar Casado  <casado@ifae.es>  - IFAE-Barcelona
**********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERDETAILSCONTAINER_P1_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERDETAILSCONTAINER_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>
  
class TrigTauClusterDetailsContainer_p1 : public std::vector<TPObjRef>
{
 public:
    
  TrigTauClusterDetailsContainer_p1(){}
     
};// end of class definitions
 
 
#endif
