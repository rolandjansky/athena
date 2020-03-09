///////// -*- c++ -*- /////////////////////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ParticleLinksCnv.h 
// Header file for class ParticlesLinksCnv
// Author: S. Protopopescu <serban@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTATHENAPOOL_PARTICLELINKSCNV_H 
#define PARTICLEEVENTATHENAPOOL_PARTICLELINKSCNV_H 1

// STL includes
 
 
// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IAlgTool.h"
#include "StoreGate/StoreGateSvc.h"

// ParticleLinks includes
#include "ParticleEvent/ParticleLinks.h"
#include "ParticleEventTPCnv/ParticleLinks_p1.h"
#include "ParticleEventTPCnv/ParticleLinksCnv_p1.h"


template <class Container>
using ParticleLinksCnv = T_AthenaPoolTPCnvCnv< ParticleLinks<Container>,
                                               ParticleLinksCnv_p1<Container> >;


#endif //> !PARTICLEEVENTATHENAPOOL_PARTICLELINKSCNV_H 

