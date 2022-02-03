/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "sTGC_HitsTruthRelink.h"


sTGC_HitsTruthRelink::sTGC_HitsTruthRelink(const std::string &name, ISvcLocator *pSvcLocator)
  : HitsTruthRelinkBase(name, pSvcLocator)
{
}


StatusCode sTGC_HitsTruthRelink::initialize()
{
  ATH_CHECK(HitsTruthRelinkBase::initialize());

  // Check and initialize keys
  ATH_CHECK( m_inputHitsKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_inputHitsKey);
  ATH_CHECK( m_outputHitsKey.initialize() );
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputHitsKey);
  return StatusCode::SUCCESS;
}


StatusCode sTGC_HitsTruthRelink::execute(const EventContext &ctx) const
{
  ATH_MSG_DEBUG("Doing truth relinking");

  SG::ReadHandle<sTGCSimHitCollection> inputCollection(m_inputHitsKey, ctx);
  if (!inputCollection.isValid()) {
    ATH_MSG_ERROR("Could not get input hits collection " << inputCollection.name() << " from store " << inputCollection.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found input hits collection " << inputCollection.name() << " in store " << inputCollection.store());

  SG::WriteHandle<sTGCSimHitCollection> outputCollection(m_outputHitsKey, ctx);
  ATH_CHECK(outputCollection.record(std::make_unique<sTGCSimHitCollection>()));
  if (!outputCollection.isValid()) {
    ATH_MSG_ERROR("Could not record output hits collection " << outputCollection.name() << " to store " << outputCollection.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output hits collection " << outputCollection.name() << " in store " << outputCollection.store());

  // Do relinking
  int referenceBarcode{};
  ATH_CHECK(getReferenceBarcode(ctx, &referenceBarcode));

  for (const sTGCSimHit &hit : *inputCollection) {
    const HepMcParticleLink oldLink = hit.particleLink();

    int currentBarcode{};
    if (oldLink.barcode() != 0) {
      currentBarcode = referenceBarcode;
    }

    HepMcParticleLink particleLink(currentBarcode, oldLink.eventIndex(), oldLink.getEventCollection(), HepMcParticleLink::IS_INDEX, ctx);

    int           id            = hit.sTGCId();
    double        time          = hit.globalTime();
    Amg::Vector3D position      = hit.globalPosition();
    int           pdgID         = hit.particleEncoding();
    Amg::Vector3D direction     = hit.globalDirection();
    double        energyDeposit = hit.depositEnergy();
    double        kineticEnergy = hit.kineticEnergy();
    Amg::Vector3D preposition   = hit.globalPrePosition();

    outputCollection->Emplace(id, time, position, pdgID, direction, energyDeposit, particleLink, kineticEnergy, preposition);
  }

  return StatusCode::SUCCESS;
}
