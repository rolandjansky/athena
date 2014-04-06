/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 *
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigEFBjetContainer_p2
 *
 * @brief persistent partner for TrigEFBjetContainer
 *
 * @author Andrea Coccaro  <Andrea.Coccaro@ge.infn.it>
 *
 **********************************************************************************/

#ifndef TRIGPARTICLETPCNV_TRIGEFBJETCONTAINER_P2_H
#define TRIGPARTICLETPCNV_TRIGEFBJETCONTAINER_P2_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>
  
class TrigEFBjetContainer_p2 : public std::vector<TPObjRef> {

 public:
    
  TrigEFBjetContainer_p2(){}
     
};

#endif
