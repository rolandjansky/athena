/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATOROBJECTSTPCNV_INITMCEVENTCOLLECTION_H
# define GENERATOROBJECTSTPCNV_INITMCEVENTCOLLECTION_H
/** @file initMcEventCollection.h
 * @brief  minimal gaudi initialization and record an McEventCollection in StoreGate
 *
 **/

#include <string>
#include <vector>
#include "AtlasHepMC/GenParticle.h"

#undef NDEBUG

class ISvcLocator;
#include "AtlasHepMC/GenEvent_fwd.h"

namespace Athena_test {
  /** @fn bool initGaudi(ISvcLocator*& pSvcLoc, HepMC::GenParticle*& pGenParticle)
   *  @brief minimal gaudi initialization and record an McEventCollection in StoreGate
   *  @param pSvcLoc returns a pointer to the Gaudi ServiceLocator
   *  @param genPartVector returns a vector of pointers to GenParticle objects for use in HepMcParticleLink creation
   *  @param initGaudi default is true. If you already did Athena_test::initGaudi and want to skip it, set this false.
   */
  bool initMcEventCollection(ISvcLocator*& pSvcLoc, std::vector<HepMC::GenParticlePtr>& genPartVector, const bool initGaudi=true);
  /** @fn HepMC::GenParticle* populateGenEvent(HepMC::GenEvent & ge)
   *  @brief fills a HepMC::GenEvent with some dummy GenParticle and GenVertex objects
   *  @param pSvcLoc returns a pointer to the Gaudi ServiceLocator
   *  @param genPartVector returns a vector of pointers to GenParticle objects for use in HepMcParticleLink creation
   */
  void populateGenEvent(HepMC::GenEvent & ge, int pdgid1, int pdgid2, std::vector<HepMC::GenParticlePtr>& genPartVector);
}
#endif // GENERATOROBJECTSTPCNV_INITMCEVENTCOLLECTION_H
