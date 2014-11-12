///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// File: McParticleEventTPCnv/McParticleEventTPCnvDict.h
/// Dictionary file for persistent representation(s) of McParticleEvent classes
/// Author: Sebastien Binet <binet@cern.ch>
/// Date:   July 2007

#ifndef MCPARTICLEEVENTTPCNV_MCPARTICLEEVENTTPCNVDICT_H
#define MCPARTICLEEVENTTPCNV_MCPARTICLEEVENTTPCNVDICT_H

#include "McParticleEventTPCnv/TruthParticleContainer_p1.h"
#include "McParticleEventTPCnv/TruthParticleContainer_p2.h"
#include "McParticleEventTPCnv/TruthParticleContainer_p3.h"
#include "McParticleEventTPCnv/TruthParticleContainer_p4.h"
#include "McParticleEventTPCnv/TruthParticleContainer_p5.h"
#include "McParticleEventTPCnv/TruthParticleContainer_p6.h"

#include "McParticleEventTPCnv/TruthEtIsolationsContainer_p1.h"


namespace McParticleEventTPCnv_dict {

  struct tmp {
    TruthEtIsolations_p1::EtIsolMap_t m_etIsolMap_p1;
    TruthEtIsolations_p1::EtIsolBc_t  m_etIsolBc_p1;
  };

} //> namespace McParticleEventTPCnv_dict

#endif //MCPARTICLEEVENTTPCNV_MCPARTICLEEVENTTPCNVDICT_H
