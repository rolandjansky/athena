/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_HitsTruthRelink.h"


RPC_HitsTruthRelink::RPC_HitsTruthRelink(const std::string &name, ISvcLocator *pSvcLocator)
  : HitsTruthRelinkBase(name, pSvcLocator)
{
}


StatusCode RPC_HitsTruthRelink::initialize()
{
  ATH_CHECK(HitsTruthRelinkBase::initialize());

  // Check and initialize keys
  ATH_CHECK( m_inputHitsKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_inputHitsKey);
  ATH_CHECK( m_outputHitsKey.initialize() );
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputHitsKey);
  return StatusCode::SUCCESS;
}


StatusCode RPC_HitsTruthRelink::execute(const EventContext &ctx) const
{
  ATH_MSG_DEBUG("Doing truth relinking");

  SG::ReadHandle<RPCSimHitCollection> inputCollection(m_inputHitsKey, ctx);
  if (!inputCollection.isValid()) {
    ATH_MSG_ERROR("Could not get input hits collection " << inputCollection.name() << " from store " << inputCollection.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found input hits collection " << inputCollection.name() << " in store " << inputCollection.store());

  SG::WriteHandle<RPCSimHitCollection> outputCollection(m_outputHitsKey, ctx);
  ATH_CHECK(outputCollection.record(std::make_unique<RPCSimHitCollection>()));
  if (!outputCollection.isValid()) {
    ATH_MSG_ERROR("Could not record output hits collection " << outputCollection.name() << " to store " << outputCollection.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output hits collection " << outputCollection.name() << " in store " << outputCollection.store());

  // Do relinking
  int referenceBarcode{};
  ATH_CHECK(getReferenceBarcode(ctx, &referenceBarcode));

  for (const RPCSimHit & hit : *inputCollection) {
    const HepMcParticleLink oldLink = hit.particleLink();

    int currentBarcode{};
    if (oldLink.barcode() != 0) {
      currentBarcode = referenceBarcode;
    }

    HepMcParticleLink particleLink(currentBarcode, oldLink.eventIndex(), oldLink.getEventCollection(), HepMcParticleLink::IS_INDEX, ctx);
    int    id             = hit.RPCid();
    double time           = hit.globalTime();
    Amg::Vector3D prePos  = hit.preLocalPosition();
    Amg::Vector3D postPos = hit.postLocalPosition();
    double energyDeposit  = hit.energyDeposit();
    int    pdgID          = hit.particleEncoding();
    double kineticEnergy  = hit.kineticEnergy();
    double stepLength     = hit.stepLength();

    outputCollection->Emplace(id, time, prePos, particleLink, postPos, energyDeposit, stepLength, pdgID, kineticEnergy);
  }

  return StatusCode::SUCCESS;
}
