/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 *
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigL2BjetContainer_p2
 *
 * @brief persistent partner for TrigL2BjetContainer
 *
 * @author Andrea Coccaro  <Andrea.Coccaro@ge.infn.it>
 *
 **********************************************************************************/

#ifndef TRIGPARTICLETPCNV_TRIGL2BJETCONTAINER_P2_H
#define TRIGPARTICLETPCNV_TRIGL2BJETCONTAINER_P2_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>
  
class TrigL2BjetContainer_p2 : public std::vector<TPObjRef> {

 public:
    
  TrigL2BjetContainer_p2(){}
     
};
 
#endif
