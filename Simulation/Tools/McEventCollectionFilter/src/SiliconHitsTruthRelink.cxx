/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "SiliconHitsTruthRelink.h"


SiliconHitsTruthRelink::SiliconHitsTruthRelink(const std::string &name, ISvcLocator *pSvcLocator)
  : HitsTruthRelinkBase(name, pSvcLocator)
{
}


StatusCode SiliconHitsTruthRelink::initialize()
{
  ATH_CHECK(HitsTruthRelinkBase::initialize());

  // Check and initialize keys
  ATH_CHECK( m_inputHitsKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_inputHitsKey);
  ATH_CHECK( m_outputHitsKey.initialize() );
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputHitsKey);
  return StatusCode::SUCCESS;
}


StatusCode SiliconHitsTruthRelink::execute(const EventContext &ctx) const
{
  ATH_MSG_DEBUG("Doing truth relinking");

  SG::ReadHandle<SiHitCollection> inputCollection(m_inputHitsKey, ctx);
  if (!inputCollection.isValid()) {
    ATH_MSG_ERROR("Could not get input hits collection " << inputCollection.name() << " from store " << inputCollection.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found input hits collection " << inputCollection.name() << " in store " << inputCollection.store());

  SG::WriteHandle<SiHitCollection> outputCollection(m_outputHitsKey, ctx);
  ATH_CHECK(outputCollection.record(std::make_unique<SiHitCollection>()));
  if (!outputCollection.isValid()) {
    ATH_MSG_ERROR("Could not record output hits collection " << outputCollection.name() << " to store " << outputCollection.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output hits collection " << outputCollection.name() << " in store " << outputCollection.store());

  // Do relinking
  int referenceBarcode{};
  ATH_CHECK(getReferenceBarcode(ctx, &referenceBarcode));

  for (const SiHit &hit : *inputCollection) {
    const HepMcParticleLink oldLink = hit.particleLink();

    HepGeom::Point3D<double> lP1 = hit.localStartPosition();
    HepGeom::Point3D<double> lP2 = hit.localEndPosition();
    double       energyLoss = hit.energyLoss();
    double       meanTime   = hit.meanTime();
    unsigned int id         = hit.identify();

    int currentBarcode{};
    if (oldLink.barcode() != 0) {
      currentBarcode = referenceBarcode;
    }
    HepMcParticleLink particleLink(currentBarcode, oldLink.eventIndex(), oldLink.getEventCollection(), HepMcParticleLink::IS_INDEX, ctx);
    outputCollection->Emplace(lP1, lP2, energyLoss, meanTime, currentBarcode, id);
  }

  return StatusCode::SUCCESS;
}
