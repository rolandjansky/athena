/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "HitsTruthRelinkBase.h"


HitsTruthRelinkBase::HitsTruthRelinkBase(const std::string &name, ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{
}


StatusCode HitsTruthRelinkBase::initialize()
{
  // Check and initialize keys
  ATH_CHECK( m_inputTruthCollectionKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_inputTruthCollectionKey);
  return StatusCode::SUCCESS;
}


StatusCode HitsTruthRelinkBase::getReferenceBarcode(const EventContext &ctx, int *barcode) const
{
  SG::ReadHandle<McEventCollection> inputCollection(m_inputTruthCollectionKey, ctx);
  if (!inputCollection.isValid()) {
    ATH_MSG_ERROR("Could not get input truth collection " << inputCollection.name() << " from store " << inputCollection.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found input truth collection " << inputCollection.name() << " in store " << inputCollection.store());

  const HepMC::GenEvent *genEvt = *(inputCollection->begin());
#ifdef HEPMC3
  size_t nVertices = genEvt->vertices().size();
#else
  size_t nVertices = genEvt->vertices_size();
#endif
  if (nVertices == 0) {
    ATH_MSG_ERROR("Truth collection should have at least one vertex!");
    return StatusCode::FAILURE;
  }

  const HepMC::ConstGenVertexPtr genVtx = HepMC::barcode_to_vertex(genEvt, -nVertices);
#ifdef HEPMC3
  size_t nParticles = genVtx->particles_out().size();
#else
  size_t nParticles = genVtx->particles_out_size();
#endif
  if (nParticles == 0) {
    ATH_MSG_ERROR("Truth vertex should have at least one particle!");
    return StatusCode::FAILURE;
  }

#ifdef HEPMC3
  *barcode = HepMC::barcode(genVtx->particles_out().back());
#else
  *barcode = HepMC::barcode(*(genVtx->particles_out_const_begin()));
#endif

  ATH_MSG_DEBUG("Reference barcode: " << *barcode);

  return StatusCode::SUCCESS;

}
