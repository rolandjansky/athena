/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////
// Oleg.Fedin@cern.ch, August 2010
//////////////////////////////////////////////////////////////////////////
#include "McEventCollectionFilter.h"
//
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/Flow.h"
#include "AtlasHepMC/Polarization.h"
//
#include "InDetSimEvent/SiHit.h"
#include "MuonSimEvent/TGCSimHit.h"
#include "MuonSimEvent/CSCSimHit.h"
#include "MuonSimEvent/sTGCSimHit.h"
#include "MuonSimEvent/MMSimHit.h"
// CLHEP
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
//
#include "CLHEP/Geometry/Point3D.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include <climits>


McEventCollectionFilter::McEventCollectionFilter(const std::string &name, ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{
}


StatusCode McEventCollectionFilter::initialize()
{
  // Check and initialize keys
  ATH_CHECK( m_inputTruthCollectionKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_inputTruthCollectionKey);
  ATH_CHECK( m_outputTruthCollectionKey.initialize() );
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputTruthCollectionKey);

  ATH_CHECK( m_inputTRTHitsKey.initialize(!m_inputTRTHitsKey.empty() && m_keepElectronsLinkedToTRTHits) );
  if (m_keepElectronsLinkedToTRTHits) {
    ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_inputTRTHitsKey);
  } else {
    ATH_MSG_DEBUG("Not keeping electrons from TRT hits");
  }

  return StatusCode::SUCCESS;
}


StatusCode McEventCollectionFilter::execute(const EventContext &ctx) const
{
  ATH_MSG_DEBUG("Filtering McEventCollection...");

  SG::ReadHandle<McEventCollection> inputCollection(m_inputTruthCollectionKey, ctx);
  if (!inputCollection.isValid()) {
    ATH_MSG_ERROR("Could not get input truth collection " << inputCollection.name() << " from store " << inputCollection.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found input truth collection " << inputCollection.name() << " in store " << inputCollection.store());

  SG::WriteHandle<McEventCollection> outputCollection(m_outputTruthCollectionKey, ctx);
  ATH_CHECK(outputCollection.record(std::make_unique<McEventCollection>()));
  if (!outputCollection.isValid()) {
    ATH_MSG_ERROR("Could not record output truth collection " << outputCollection.name() << " to store " << outputCollection.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output truth collection " << outputCollection.name() << " in store " << outputCollection.store());

  //.......Create new particle (geantino) to link  hits from pileup
  HepMC::GenParticlePtr genPart=HepMC::newGenParticlePtr();
  genPart->set_pdg_id(m_pileUpParticlePDGID); //Geantino
  genPart->set_status(1); //!< set decay status
  HepMC::suggest_barcode(genPart, std::numeric_limits<int32_t>::max() );

  HepMC::GenVertexPtr genVertex = HepMC::newGenVertexPtr();
  genVertex->add_particle_out(genPart);

  const HepMC::GenEvent* genEvt = *(inputCollection->begin());
  //......copy GenEvent to the new one and remove all vertex
  HepMC::GenEvent* evt = HepMC::copyemptyGenEvent(genEvt);
#ifdef HEPMC3
  for (const auto &bp : evt->beams()) {
    evt->add_beam_particle(bp);
  }
  if (genEvt->cross_section()) {
    auto cs = std::make_shared<HepMC3::GenCrossSection>(*genEvt->cross_section().get());
    evt->set_cross_section(cs);
  }
#else
  evt->set_beam_particles(genEvt->beam_particles());
  if (genEvt->cross_section()) {
    evt->set_cross_section(*genEvt->cross_section());
  }
#endif

  // to set geantino vertex as a truth primary vertex
  HepMC::ConstGenVertexPtr hScatVx = HepMC::barcode_to_vertex(genEvt,-3);
  if (hScatVx != nullptr) {
    HepMC::FourVector pmvxpos=hScatVx->position();
    genVertex->set_position(pmvxpos);
    // to set geantino kinematic phi=eta=0, E=p=E_hard_scat
#ifdef HEPMC3 
    auto itrp = hScatVx->particles_in().begin();
    if (hScatVx->particles_in().size()==2) {
#else
    HepMC::GenVertex::particles_in_const_iterator itrp =hScatVx->particles_in_const_begin();
    if (hScatVx->particles_in_size()==2) {
#endif
      HepMC::FourVector mom1=(*itrp)->momentum();
      HepMC::FourVector mom2=(*(++itrp))->momentum();
      HepMC::FourVector vxmom;
      vxmom.setPx(mom1.e()+mom2.e());
      vxmom.setPy(0.);
      vxmom.setPz(0.);
      vxmom.setE(mom1.e()+mom2.e());

      genPart->set_momentum(vxmom);
    }
  }

  // electrons from TRT hits
  if (m_keepElectronsLinkedToTRTHits) {
    std::vector<int> barcodes;
    ATH_CHECK(findElectronsLinkedToTRTHits(ctx, &barcodes));

    for (int barcode : barcodes) {
      HepMC::ConstGenParticlePtr particle = HepMC::barcode_to_particle(genEvt, barcode);
      if (particle == nullptr) {
        ATH_MSG_DEBUG("Could not find particle for barcode " << barcode);
        continue;
      }
      HepMC::ConstGenVertexPtr vx = particle->production_vertex();
      HepMC::GenParticlePtr newParticle = HepMC::newGenParticlePtr(particle->momentum(),
                                                                   particle->pdg_id(),
                                                                   particle->status());
      HepMC::suggest_barcode(newParticle, barcode);

      const HepMC::FourVector &position = vx->position();
      HepMC::GenVertexPtr newVertex = HepMC::newGenVertexPtr(position);
      newVertex->add_particle_out(newParticle);
      evt->add_vertex(newVertex);
    }
  }

  //.....add new vertex with geantino
  evt->add_vertex(genVertex);
  
  int referenceBarcode = HepMC::barcode(genPart);
  ATH_MSG_DEBUG("Reference barcode: " << referenceBarcode);

  outputCollection->push_back(evt);

  return StatusCode::SUCCESS;
}


StatusCode McEventCollectionFilter::findElectronsLinkedToTRTHits(const EventContext &ctx, std::vector<int> *barcodes) const
{
  SG::ReadHandle<TRTUncompressedHitCollection> inputCollection(m_inputTRTHitsKey, ctx);
  if (!inputCollection.isValid()) {
    ATH_MSG_ERROR("Could not get input hits collection " << inputCollection.name() << " from store " << inputCollection.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found input hits collection " << inputCollection.name() << " in store " << inputCollection.store());

  std::set<int> barcodeSet;
  for (const TRTUncompressedHit &hit : *inputCollection) {
    const HepMcParticleLink link = hit.particleLink();
    int pdgID = hit.GetParticleEncoding();
    if (std::abs(pdgID) == 11 && link.barcode() != 0) {
      barcodeSet.insert(link.barcode());
    }
  }
  barcodes->assign(barcodeSet.begin(), barcodeSet.end());

  return StatusCode::SUCCESS;
}
