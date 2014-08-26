/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/errorcheck.h"
#include "DataModel/ElementLink.h"

#include "GeneratorObjects/McEventCollection.h"
#include "GeneratorObjects/xAODTruthParticleLink.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "StoreGate/StoreGateSvc.h"

#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthEventAuxContainer.h"

#include "xAODTruth/TruthPileupEvent.h"
#include "xAODTruth/TruthPileupEventContainer.h"
#include "xAODTruth/TruthPileupEventAuxContainer.h"

#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODTruth/TruthVertexAuxContainer.h"

#include "xAODTruthCnvAlg.h"

using namespace std;

namespace xAODMaker {


  xAODTruthCnvAlg::xAODTruthCnvAlg( const string& name, ISvcLocator* svcLoc )
    : AthAlgorithm( name, svcLoc )
  {
    declareProperty("AODContainerName", m_aodContainerName="GEN_AOD" );
    declareProperty("xAODTruthEventContainerName", m_xaodTruthEventContainerName="TruthEvent" );
    declareProperty("xAODTruthPileupEventContainerName", m_xaodTruthPUEventContainerName="TruthPileupEvents" );
    /// @todo TruthParticle -> TruthParticles
    declareProperty("xAODTruthParticleContainerName", m_xaodTruthParticleContainerName="TruthParticle" );
    /// @todo TruthVertex -> TruthVertices
    declareProperty("xAODTruthVertexContainerName", m_xaodTruthVertexContainerName="TruthVertex" );
    declareProperty("TruthLinks", m_truthLinkContainerName="xAODTruthLinks" );
  }


  StatusCode xAODTruthCnvAlg::initialize() {
    ATH_MSG_DEBUG("Initializing; package version: " << PACKAGE_VERSION );
    ATH_MSG_DEBUG("AODContainerName = " << m_aodContainerName );
    ATH_MSG_DEBUG("xAOD TruthEventContainer name = " << m_xaodTruthEventContainerName );
    ATH_MSG_DEBUG("xAOD TruthPileupEventContainer name = " << m_xaodTruthPUEventContainerName );
    ATH_MSG_DEBUG("xAOD TruthParticleContainer name = " << m_xaodTruthParticleContainerName );
    ATH_MSG_DEBUG("xAOD TruthVertexContainer name = " << m_xaodTruthVertexContainerName );
    return StatusCode::SUCCESS;
  }


  StatusCode xAODTruthCnvAlg::execute() {

    // If the containers already exist then assume that nothing needs to be done
    /// @todo Should this check be AND rather than OR?
    if (evtStore()->contains< xAOD::TruthEventContainer >(m_xaodTruthEventContainerName) ||
        //evtStore()->contains< xAOD::TruthPileupEventContainer >(m_xaodTruthPUEventContainerName) ||
        evtStore()->contains< xAOD::TruthParticleContainer >(m_xaodTruthParticleContainerName) ||
        evtStore()->contains< xAOD::TruthVertexContainer >(m_xaodTruthVertexContainerName)) {
      ATH_MSG_WARNING("xAOD Truth seems to be already available in the event");
      return StatusCode::SUCCESS;
    }

    xAODTruthParticleLinkVector* truthLinkVec = new xAODTruthParticleLinkVector();
    CHECK( evtStore()->record( truthLinkVec, m_truthLinkContainerName ) );

    if (evtStore()->contains<McEventCollection>(m_aodContainerName)) {

      // Retrieve the HepMC truth:
      const McEventCollection* mcColl = 0;
      CHECK( evtStore()->retrieve( mcColl, m_aodContainerName ) );
      ATH_MSG_DEBUG( "Retrieved HepMC with key: " << m_aodContainerName );

      // **************************************************************
      // Create the xAOD containers and their auxiliary stores:
      // **************************************************************
      // Signal event
      xAOD::TruthEventContainer* xTruthEventContainer = new xAOD::TruthEventContainer();
      CHECK( evtStore()->record( xTruthEventContainer, m_xaodTruthEventContainerName ) );
      xAOD::TruthEventAuxContainer* xTruthEventAuxContainer = new xAOD::TruthEventAuxContainer();
      CHECK( evtStore()->record( xTruthEventAuxContainer, m_xaodTruthEventContainerName + "Aux." ) );
      xTruthEventContainer->setStore( xTruthEventAuxContainer );
      ATH_MSG_DEBUG( "Recorded TruthEventContainer with key: " << m_xaodTruthEventContainerName );
      // Pile-up events
      // xAOD::TruthPileupEventContainer* xTruthPileupEventContainer = new xAOD::TruthPileupEventContainer();
      // CHECK( evtStore()->record( xTruthPileupEventContainer, m_xaodTruthPUEventContainerName ) );
      // xAOD::TruthPileupEventAuxContainer* xTruthPileupEventAuxContainer = new xAOD::TruthPileupEventAuxContainer();
      // CHECK( evtStore()->record( xTruthPileupEventAuxContainer, m_xaodTruthPUEventContainerName + "Aux." ) );
      // xTruthEventContainer->setStore( xTruthPileupEventAuxContainer );
      // ATH_MSG_DEBUG( "Recorded TruthPileupEventContainer with key: " << m_xaodTruthPUEventContainerName );
      // Particles
      xAOD::TruthParticleContainer* xTruthParticleContainer = new xAOD::TruthParticleContainer();
      CHECK( evtStore()->record( xTruthParticleContainer, m_xaodTruthParticleContainerName ) );
      xAOD::TruthParticleAuxContainer* xTruthParticleAuxContainer = new xAOD::TruthParticleAuxContainer();
      CHECK( evtStore()->record( xTruthParticleAuxContainer, m_xaodTruthParticleContainerName + "Aux." ) );
      xTruthParticleContainer->setStore( xTruthParticleAuxContainer );
      ATH_MSG_DEBUG( "Recorded TruthParticleContainer with key: " << m_xaodTruthParticleContainerName );
      // Vertices
      xAOD::TruthVertexContainer* xTruthVertexContainer = new xAOD::TruthVertexContainer();
      CHECK( evtStore()->record( xTruthVertexContainer, m_xaodTruthVertexContainerName ) );
      xAOD::TruthVertexAuxContainer* xTruthVertexAuxContainer = new xAOD::TruthVertexAuxContainer();
      CHECK( evtStore()->record( xTruthVertexAuxContainer, m_xaodTruthVertexContainerName + "Aux." ) );
      xTruthVertexContainer->setStore( xTruthVertexAuxContainer );
      ATH_MSG_DEBUG( "Recorded TruthVertexContainer with key: " << m_xaodTruthVertexContainerName );

      // ***********************************************************************************
      // Create the xAOD objects
      // This consists of three parts:
      // (1)  For each Athena event, loop over the GenEvents and build TruthEvent collections
      //      In principle there can be more than one GenEvent per event
      // (2)  For each GenEvent, loop over the GenParticles. For each GenParticle:
      //      (a) Create a TruthParticle.
      //      (b) Call fillParticle
      //      (c) Add the TruthParticle to the TruthParticle container, and add
      //          an EL to this TruthParticle to the truthParticles in TruthEvent
      //          (call this EL eltp)
      //      (d) For the GenVertex * that's this particle's production vertex,
      //          (i) see if it is in tempMap. If not, add it.
      //          (ii) add a copy of eltp (the EL to this truth particle) to map[gv].second
      //      (e) For the GenVertex * that's this particle's decay vertex,
      //          (i) see if it is in tempMap. If not, add it.
      //          (ii) add a copy of eltp (the EL to this truth particle) to map[gv].first
      // (3) Iterate over tempMap. For each GenVertex *:
      //      (a) Create a TruthVertex
      //      (b) Call fillVertex
      //      (c) Add the TruthVertex to the TruthTruth container, and add an EL to this TruthVertex
      //          to the truthVertices in TruthEvent. (call this EL eltv)
      //      (d) call tv->setIncomingParticles(mapiter.second.first)  <- I think mapiter.second.first is the first of the pair
      //      (e) call tv->setOutgoingParticles(mapiter.second.second)
      //      (f) Iterate through the incomingParticles, and set the decay vertex EL as eltv.
      //      (g) Iterate through the outgoingParticles, and set the incoming vertex EL as eltv.
      //
      // Comment lines below follow this recipe
      // ************************************************************************************

      // (1) Build TruthEvents
      ATH_MSG_DEBUG("Number of GenEvents in this Athena event = " << mcColl->size());
      // JRC: temporary hack 6th June 2014
      // To avoid writing large numbers of pile-up events, for now we only write the first (signal)
      // event. Once a more intelligent way of handling pile-up is developed this will be updated.

      /// @todo Rewrite to fill the single signal event on cntr==0, and to copy one signal vtx (from
      /// signal_vertex, else from the end vtx of a beam particle, else first vtx) into each PU event
      for (unsigned int cntr = 0; cntr < mcColl->size(); ++cntr) {

        McEventCollection::const_iterator itr = mcColl->begin();
        const HepMC::GenEvent* genEvt = *itr;

        if (cntr == 0) {
          // Handle the signal (first) event specially

          xAOD::TruthEvent* xTruthEvent = new xAOD::TruthEvent();
          xTruthEventContainer->push_back( xTruthEvent );

          /// @todo Drop or re-enable these? Signal process can be set to DSID... preferably not to the gen-name code
          //xTruthEvent->setSignalProcessId(genEvt->signal_process_id());
          //xTruthEvent->setEventNumber(genEvt->event_number());

          // Cross-section
          const HepMC::GenCrossSection* const crossSection = genEvt->cross_section();
          xTruthEvent->setCrossSection(crossSection ? (float)crossSection->cross_section() : -1);
          xTruthEvent->setCrossSectionError(crossSection ? (float)crossSection->cross_section_error() : -1);

          // Event weights
          vector<float> weights;
          for (const double& w : genEvt->weights()) weights.push_back((float)(w));
          xTruthEvent->setWeights(weights);

          // Heavy ion info
          const HepMC::HeavyIon* const hiInfo = genEvt->heavy_ion();
          if (hiInfo) {
            xTruthEvent->setHeavyIonParameter(hiInfo->Ncoll_hard(), xAOD::TruthEvent::NCOLLHARD);
            xTruthEvent->setHeavyIonParameter(hiInfo->Npart_proj(), xAOD::TruthEvent::NPARTPROJ);
            xTruthEvent->setHeavyIonParameter(hiInfo->Npart_targ(), xAOD::TruthEvent::NPARTTARG);
            xTruthEvent->setHeavyIonParameter(hiInfo->Ncoll(), xAOD::TruthEvent::NCOLL);
            xTruthEvent->setHeavyIonParameter(hiInfo->spectator_neutrons(), xAOD::TruthEvent::SPECTATORNEUTRONS);
            xTruthEvent->setHeavyIonParameter(hiInfo->spectator_protons(), xAOD::TruthEvent::SPECTATORPROTONS);
            xTruthEvent->setHeavyIonParameter(hiInfo->N_Nwounded_collisions(), xAOD::TruthEvent::NNWOUNDEDCOLLISIONS);
            xTruthEvent->setHeavyIonParameter(hiInfo->Nwounded_N_collisions(), xAOD::TruthEvent::NWOUNDEDNCOLLISIONS);
            xTruthEvent->setHeavyIonParameter(hiInfo->Nwounded_Nwounded_collisions(), xAOD::TruthEvent::NWOUNDEDNWOUNDEDCOLLISIONS);
            xTruthEvent->setHeavyIonParameter(hiInfo->impact_parameter(), xAOD::TruthEvent::IMPACTPARAMETER);
            xTruthEvent->setHeavyIonParameter(hiInfo->event_plane_angle(), xAOD::TruthEvent::EVENTPLANEANGLE);
            xTruthEvent->setHeavyIonParameter(hiInfo->eccentricity(), xAOD::TruthEvent::ECCENTRICITY);
            xTruthEvent->setHeavyIonParameter(hiInfo->sigma_inel_NN(), xAOD::TruthEvent::SIGMAINELNN);
            // This doesn't yet exist in our version of HepMC
            // xTruthEvent->setHeavyIonParameter(hiInfo->centrality(),xAOD::TruthEvent::CENTRALITY);
          }

          // Parton density info
          // This will exist 99% of the time, except for e.g. cosmic or particle gun simulation
          const HepMC::PdfInfo* const pdfInfo = genEvt->pdf_info();
          if (pdfInfo) {
            xTruthEvent->setPdfParameter(pdfInfo->id1(), xAOD::TruthEvent::PDGID1);
            xTruthEvent->setPdfParameter(pdfInfo->id1(), xAOD::TruthEvent::PDGID2);
            xTruthEvent->setPdfParameter(pdfInfo->pdf_id1(), xAOD::TruthEvent::PDFID1);
            xTruthEvent->setPdfParameter(pdfInfo->pdf_id2(), xAOD::TruthEvent::PDFID2);

            xTruthEvent->setPdfParameter((float)pdfInfo->x1(), xAOD::TruthEvent::X1);
            xTruthEvent->setPdfParameter((float)pdfInfo->x2(), xAOD::TruthEvent::X2);
            xTruthEvent->setPdfParameter((float)pdfInfo->scalePDF(), xAOD::TruthEvent::Q);
            xTruthEvent->setPdfParameter((float)pdfInfo->pdf1(), xAOD::TruthEvent::XF1);
            xTruthEvent->setPdfParameter((float)pdfInfo->pdf2(), xAOD::TruthEvent::XF2);
          }


          // (2) Build particles and vertices
          // Map for building associations between particles and vertices
          // The pair in the map is the (incomingParticles . outgoingParticles) of the given vertex
          VertexMap vertexMap;
          VertexMap::iterator mapItr;
          vector<const HepMC::GenVertex*> vertices;

          // Loop over GenParticles
          unsigned int i(0);
          // Get the beam particles
          pair<HepMC::GenParticle*,HepMC::GenParticle*> beamParticles;
          if ( genEvt->valid_beam_particles() ) beamParticles = genEvt->beam_particles();
          for (HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin(); pitr!=genEvt->particles_end(); ++pitr, ++i) {
            // (a) create TruthParticle
            xAOD::TruthParticle* xTruthParticle = new xAOD::TruthParticle();
            xTruthParticleContainer->push_back( xTruthParticle );
            fillParticle(xTruthParticle, *pitr); // (b) Copy HepMC info into the new particle
            // (c) Put particle into container; Build Event<->Particle element link
            const ElementLink<xAOD::TruthParticleContainer> eltp(*xTruthParticleContainer, i);
            xTruthEvent->addTruthParticleLink(eltp);

            // Create link between HepMC and xAOD truth
            truthLinkVec->push_back(new xAODTruthParticleLink(HepMcParticleLink((*pitr),genEvt->event_number()), eltp));

            // Is this one of the beam particles?
            if (genEvt->valid_beam_particles()) {
              if (*pitr == beamParticles.first) xTruthEvent->setBeamParticle1Link(eltp);
              if (*pitr == beamParticles.second) xTruthEvent->setBeamParticle2Link(eltp);
            }
            // (d) Particle's production vertex
            HepMC::GenVertex* productionVertex = (*pitr)->production_vertex();
            if (productionVertex) {
              VertexParticles& parts = vertexMap[productionVertex];
              if (parts.incoming.empty() && parts.outgoing.empty())
                vertices.push_back (productionVertex);
              parts.outgoingEL.push_back(eltp);
              parts.outgoing.push_back(xTruthParticle);
            }
            //
            // else maybe want to keep track that this is the production vertex
            //
            // (e) Particle's decay vertex
            HepMC::GenVertex* decayVertex = (*pitr)->end_vertex();
            if (decayVertex) {
              VertexParticles& parts = vertexMap[decayVertex];
              if (parts.incoming.empty() && parts.outgoing.empty())
                vertices.push_back (decayVertex);
              parts.incomingEL.push_back(eltp);
              parts.incoming.push_back(xTruthParticle);
            }

          } // end of loop over particles

          // (3) Loop over the map
          HepMC::GenVertex* signalProcessVtx = genEvt->signal_process_vertex(); // Get the signal process vertex
          for (const HepMC::GenVertex* vertex : vertices) {
            const auto& parts = vertexMap[vertex];
            // (a) create TruthVertex
            xAOD::TruthVertex* xTruthVertex = new xAOD::TruthVertex();
            xTruthVertexContainer->push_back( xTruthVertex );
            fillVertex(xTruthVertex, vertex); // (b) Copy HepMC info into the new vertex
            // (c) Put particle into container; Build Event<->Vertex element link
            ElementLink<xAOD::TruthVertexContainer> eltv(*xTruthVertexContainer, xTruthVertexContainer->size()-1);
            // Mark if this is the signal process vertex
            if (vertex == signalProcessVtx) xTruthEvent->setSignalProcessVertexLink(eltv);
            xTruthEvent->addTruthVertexLink(eltv);
            // (d) Assign incoming particles to the vertex, from the map
            xTruthVertex->setIncomingParticleLinks( parts.incomingEL );
            // (e) Assign outgoing particles to the vertex, from the map
            xTruthVertex->setOutgoingParticleLinks( parts.outgoingEL );
            // (f) Set Particle<->Vertex links for incoming particles
            for (xAOD::TruthParticle* p : parts.incoming) p->setDecayVtxLink(eltv);
            // (g) Set Particle<->Vertex links for incoming particles
            for (xAOD::TruthParticle* p : parts.outgoing) p->setProdVtxLink(eltv);
          } //end of loop over vertices

          // end of signal (i.e. first) event handling

        } else {

          // Handle pile-up events... currently do nothing at all

          /// @todo Provide some options for either full or limited pile-up event conversion

        }
      } // end of loop over McEventCollection

    } else {

      // Retrieve the xAOD Truth and recreate the TruthLinks
      if ( !evtStore()->contains<xAOD::TruthEventContainer>(m_xaodTruthEventContainerName) ) {
        ATH_MSG_WARNING("TruthEventContainer " <<  m_xaodTruthEventContainerName << " not found");
        return StatusCode::SUCCESS;
      }
      xAOD::TruthEventContainer* xTruthEventContainer = 0;
      CHECK( evtStore()->retrieve( xTruthEventContainer, m_xaodTruthEventContainerName ) );

      // Loop over events and particles
      for (auto evt : *xTruthEventContainer) {
        for (const auto& par : evt->truthParticleLinks()) {
          if ( !par.isValid() ) {
            //ATH_MSG_WARNING("Found invalid particle element link in TruthEvent " << evt->eventNumber());
            ATH_MSG_WARNING("Found invalid particle element link in TruthEvent"); //< @todo Use HepMC evt number?
            continue;
          }
          // Create link between HepMC and xAOD truth
          //truthLinkVec->push_back(new xAODTruthParticleLink(HepMcParticleLink((*par)->barcode(), evt->eventNumber()), par));
          /// @todo AB: Truth particle links should only be made to the signal event... hence the 0. Right?
          truthLinkVec->push_back(new xAODTruthParticleLink(HepMcParticleLink((*par)->barcode(), 0), par));
        }
      }

    }

    std::stable_sort(truthLinkVec->begin(), truthLinkVec->end(), SortTruthParticleLink());
    ATH_MSG_VERBOSE("Summarizing truth link size: " << truthLinkVec->size() );

    return StatusCode::SUCCESS;
  }


  // A helper to set up a TruthVertex (without filling the ELs)
  void xAODTruthCnvAlg::fillVertex(xAOD::TruthVertex* tv, const HepMC::GenVertex* gv) {
    tv->setId(gv->id());
    tv->setBarcode(gv->barcode());

    // No vertex weights
    // vector<float> weights;
    // for (const double& w : gv->weights()) weights.push_back((float)w);
    // tv->setWeights(weights);

    tv->setX(gv->position().x());
    tv->setY(gv->position().y());
    tv->setZ(gv->position().z());
    tv->setT(gv->position().t());
  }


  // A helper to set up a TruthParticle (without filling the ELs)
  void xAODTruthCnvAlg::fillParticle(xAOD::TruthParticle* tp, const HepMC::GenParticle* gp) {
    tp->setPdgId(gp->pdg_id());
    tp->setBarcode(gp->barcode());
    tp->setStatus(gp->status());

    const HepMC::Polarization& pol = gp->polarization();
    if (pol.is_defined()) {
      tp->setPolarizationParameter(pol.theta(), xAOD::TruthParticle::THETA);
      tp->setPolarizationParameter(pol.phi(), xAOD::TruthParticle::PHI);
    }

    tp->setM(gp->generated_mass());
    tp->setPx(gp->momentum().px());
    tp->setPy(gp->momentum().py());
    tp->setPz(gp->momentum().pz());
    tp->setE(gp->momentum().e());
  }


} // namespace xAODMaker
