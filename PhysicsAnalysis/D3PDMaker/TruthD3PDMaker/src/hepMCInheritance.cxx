/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TruthD3PDMaker/src/hepMCInheritance.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Declare inheritance relationships for HepMC classes.
 *
 * Eventually, these should be moved to the EDM classes.
 */

#include "McParticleEvent/TruthEtIsolationsContainer.h"
#include "McParticleEvent/TruthParticle.h"
#include "GeneratorObjects/McEventCollection.h"
#include "Navigation/IAthenaBarCode.h"
#include "AtlasHepMC/GenEvent.h"
#include "AthenaKernel/BaseInfo.h"

SG_ADD_BASE (McEventCollection, DataVector<HepMC::GenEvent>);
SG_ADD_BASE (TruthEtIsolationsContainer, DataVector<TruthEtIsolations>);
SG_ADD_BASE (TruthParticle, SG_VIRTUAL(IAthenaBarCode));
