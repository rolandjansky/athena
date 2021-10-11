/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PileUpTruthDecoration.h"
//
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"
#include "StoreGate/WriteDecorHandle.h"

PileUpTruthDecoration::PileUpTruthDecoration(const std::string &name, ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{
}


StatusCode PileUpTruthDecoration::initialize()
{
  // Check and initialize keys
  ATH_CHECK( m_inputTruthCollectionKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_inputTruthCollectionKey);
  ATH_CHECK(m_particleKey.initialize());
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_particleKey);
  ATH_CHECK(m_decKey.initialize());
  ATH_MSG_VERBOSE("Initialized DecorHandleKey: " << m_decKey);
  return StatusCode::SUCCESS;
}


StatusCode PileUpTruthDecoration::execute(const EventContext &ctx) const
{
  ATH_MSG_VERBOSE("PileUpTruthDecoration::execute()");
  SG::ReadHandle<McEventCollection> inputCollection(m_inputTruthCollectionKey, ctx);
  if (!inputCollection.isValid()) {
    ATH_MSG_ERROR("Could not get input truth collection " << inputCollection.name() << " from store " << inputCollection.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found input truth collection " << inputCollection.name() << " in store " << inputCollection.store());
  const float refPVtxZ = getPVtxZ(inputCollection);

  SG::WriteDecorHandle<xAOD::IParticleContainer, float> decHandle(m_decKey, ctx);
  SG::ReadHandle<xAOD::IParticleContainer> particles(m_particleKey, ctx);
  if( !particles.isValid() ) {
    ATH_MSG_WARNING ("Couldn't retrieve container with key: " << m_particleKey.key() );
    return StatusCode::FAILURE;
  }
  for( const xAOD::IParticle* truthParticle : *particles ) {
    decHandle( *truthParticle ) = refPVtxZ;
  }
  return StatusCode::SUCCESS;
}

float PileUpTruthDecoration::getPVtxZ(SG::ReadHandle<McEventCollection>& inputCollection) const
{
  const HepMC::GenEvent* genEvt = *(inputCollection->begin());
  HepMC::ConstGenVertexPtr hScatVx = HepMC::barcode_to_vertex(genEvt,-3);
  if (hScatVx != nullptr) {
    HepMC::FourVector pmvxpos=hScatVx->position();
    return static_cast<float>(pmvxpos.z());
  }
  return 0.0f;
}
