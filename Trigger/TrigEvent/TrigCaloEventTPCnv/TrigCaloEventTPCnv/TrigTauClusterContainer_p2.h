/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauClusterContainer_p2
 *
 * @brief persistent partner for TrigTauClusterContainer
 *
  * @author M. Pilar Casado  <casado@ifae.es>  - IFAE-Barcelona
 *
 * File and Version Information:
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCONTAINER_P2_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCONTAINER_P2_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>
  
class TrigTauClusterContainer_p2 : public std::vector<TPObjRef>
{
 public:
    
  TrigTauClusterContainer_p2(){}
     
};// end of class definitions
 
 
#endif
