/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigTauContainer_p3
 *
 * @brief persistent partner for TrigTauContainer
 *
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGTAUCONTAINER_P3_H
#define TRIGPARTICLETPCNV_TRIGTAUCONTAINER_P3_H

#include "TrigParticleTPCnv/TrigTau_p3.h"
  
class TrigTauContainer_p3 : public std::vector<TrigTau_p3> { };
    
#endif
