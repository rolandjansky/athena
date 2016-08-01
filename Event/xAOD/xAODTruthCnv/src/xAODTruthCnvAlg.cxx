/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/errorcheck.h"
#include "DataModel/ElementLink.h"




#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/PhysicalConstants.h"

#include "EventInfo/EventStreamInfo.h"

#include "xAODTruth/TruthPileupEvent.h"
#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthMetaDataAuxContainer.h"
#include "xAODTruth/TruthMetaData.h"

#include "xAODTruthCnvAlg.h"
#include "CxxUtils/make_unique.h"

using namespace std;
using namespace xAOD;
namespace xAODMaker {


xAODTruthCnvAlg::xAODTruthCnvAlg( const string& name, ISvcLocator* svcLoc )
    : AthAlgorithm( name, svcLoc ),
      m_mcColl("GEN_AOD"),  m_xTruthEventContainer("TruthEvents"), m_xTruthPileupEventContainer("TruthPileupEvents"),
       m_xTruthParticleContainer("TruthParticles"), m_xTruthVertexContainer("TruthVertices"),  m_truthLinkVec("xAODTruthLinks"), 
        m_metaStore( "MetaDataStore", name ), inputMetaStore( "StoreGateSvc/InputMetaDataStore",name)
{
    declareProperty("AODContainerName", m_mcColl );
    declareProperty("xAODTruthEventContainerName", m_xTruthEventContainer );
    declareProperty("xAODTruthPileupEventContainerName", m_xTruthPileupEventContainer );
    /// @todo TruthParticle -> TruthParticles
    declareProperty("xAODTruthParticleContainerName", m_xTruthParticleContainer );
    /// @todo TruthVertex -> TruthVertices
    declareProperty("xAODTruthVertexContainerName", m_xTruthVertexContainer );
    declareProperty("TruthLinks", m_truthLinkVec);
    declareProperty( "WriteAllPileUpTruth", m_doAllPileUp = false);
    declareProperty( "WriteInTimePileUpTruth", m_doInTimePileUp = false);
    declareProperty( "MetaObjectName", m_metaName = "TruthMetaData" );
    declareProperty( "MetaDataStore", m_metaStore );
    declareProperty( "ForceRerun", m_forceRerun = false);
}


StatusCode xAODTruthCnvAlg::initialize() {
    ATH_MSG_DEBUG("Initializing; package version: " << PACKAGE_VERSION );
    ATH_MSG_DEBUG("AODContainerName = " << m_mcColl.name() );
    ATH_MSG_DEBUG("xAOD TruthEventContainer name = " << m_xTruthEventContainer.name() );
    ATH_MSG_DEBUG("xAOD TruthPileupEventContainer name = " << m_xTruthPileupEventContainer.name() );
    ATH_MSG_DEBUG("xAOD TruthParticleContainer name = " << m_xTruthParticleContainer.name()  );
    ATH_MSG_DEBUG("xAOD TruthVertexContainer name = " << m_xTruthVertexContainer.name() );
    if (m_doAllPileUp && m_doInTimePileUp) {
        ATH_MSG_FATAL( "Contradictory xAOD truth pile-up setting: all pile-up AND in-time alone requested simultaneously. Check settings." );
        return StatusCode::FAILURE;
    }
    if (m_doAllPileUp) ATH_MSG_INFO( "All pile-up truth (including out-of-time) will be written" );
    if (m_doInTimePileUp) ATH_MSG_INFO( "In-time pile-up truth (but not out-of-time) will be written" );
    if (!m_doAllPileUp && !m_doInTimePileUp) ATH_MSG_INFO( "No pile-up truth will be written" );

    CHECK( m_metaStore.retrieve() );

    // Create an empty truth meta data container:
    xAOD::TruthMetaDataAuxContainer* aux = new xAOD::TruthMetaDataAuxContainer();
    m_tmd = new xAOD::TruthMetaDataContainer();
    m_tmd->setStore( aux );

    // Record the trigger configuration metadata into it:
    CHECK( m_metaStore->record( aux, m_metaName + "Aux." ) );
    CHECK( m_metaStore->record( m_tmd, m_metaName ) );

    m_xTruthEventContainerAux = SG::WriteHandle<TruthEventAuxContainer>(m_xTruthEventContainer.name() + "Aux.");
    ATH_CHECK( m_xTruthEventContainerAux.initialize() );

    m_xTruthPileupEventAuxContainer = SG::WriteHandle<TruthPileupEventAuxContainer>(m_xTruthPileupEventContainer.name() + "Aux.");
    ATH_CHECK( m_xTruthPileupEventAuxContainer.initialize() );

    m_xTruthParticleAuxContainer = SG::WriteHandle<xAOD::TruthParticleAuxContainer>(m_xTruthParticleContainer.name() + "Aux.");
    ATH_CHECK( m_xTruthParticleAuxContainer.initialize() );
    
    m_xTruthVertexContainerAUX = SG::WriteHandle<xAOD::TruthVertexAuxContainer>(m_xTruthVertexContainer.name() + "Aux.");
    ATH_CHECK( m_xTruthVertexContainerAUX.initialize() );
    
    return StatusCode::SUCCESS;
}


StatusCode xAODTruthCnvAlg::execute() {



    m_truthLinkVec = CxxUtils::make_unique<xAODTruthParticleLinkVector>();

    if (!m_mcColl.name().empty()) {

        // Retrieve the HepMC truth:
        if(!m_mcColl.isValid()) {
            ATH_MSG_ERROR( "Cannot retrieve " << m_mcColl.name() );
            return StatusCode::FAILURE;
        }
        ATH_MSG_DEBUG( "Retrieved HepMC with key: " << m_mcColl.name() );

        // **************************************************************
        // Create the xAOD containers and their auxiliary stores:
        // **************************************************************
        // Signal event

        m_xTruthEventContainer = CxxUtils::make_unique<TruthEventContainer>();
        m_xTruthEventContainerAux = CxxUtils::make_unique<TruthEventAuxContainer>();


        m_xTruthEventContainer->setStore( m_xTruthEventContainerAux.ptr() );


        ATH_MSG_DEBUG( "Recorded TruthEventContainer with key: " << m_xTruthEventContainer.name() );
        // Pile-up events
        if (m_doAllPileUp || m_doInTimePileUp) {
            m_xTruthPileupEventContainer = CxxUtils::make_unique<TruthPileupEventContainer>();
            m_xTruthPileupEventAuxContainer = CxxUtils::make_unique<TruthPileupEventAuxContainer>();
            m_xTruthPileupEventContainer->setStore( m_xTruthPileupEventAuxContainer.ptr() );
            ATH_MSG_DEBUG( "Recorded TruthPileupEventContainer with key: " << m_xTruthPileupEventContainer.name() );
        }
        // Particles
        m_xTruthParticleContainer = CxxUtils::make_unique<xAOD::TruthParticleContainer>();
        m_xTruthParticleAuxContainer = CxxUtils::make_unique<xAOD::TruthParticleAuxContainer>();
        
        m_xTruthParticleContainer->setStore( m_xTruthParticleAuxContainer.ptr() );
        ATH_MSG_DEBUG( "Recorded TruthParticleContainer with key: " << m_xTruthParticleContainer.name() );

        // Vertices
        m_xTruthVertexContainer = CxxUtils::make_unique<xAOD::TruthVertexContainer>();
        m_xTruthVertexContainerAUX = CxxUtils::make_unique<xAOD::TruthVertexAuxContainer>();
        m_xTruthVertexContainer->setStore( m_xTruthVertexContainerAUX.ptr() );
        ATH_MSG_DEBUG( "Recorded TruthVertexContainer with key: " << m_xTruthVertexContainer.name() );
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
        ATH_MSG_DEBUG("Number of GenEvents in this Athena event = " << m_mcColl->size());
        for (unsigned int cntr = 0; cntr < m_mcColl->size(); ++cntr) {
            const HepMC::GenEvent* genEvt = (*m_mcColl)[cntr];
            bool isSignalProcess(false);
            if (cntr==0) isSignalProcess=true;
            if (cntr>0) {
                // Handle pile-up events
                // If in-time pileup only is requested, loop stops when
                // the separator between out-of-time and in-time is reached
                // Separator defined by pid==0 and eventNumber==-1 as per
                // https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/PileupDigitization#Arrangement_of_Truth_Information
                if (!m_doInTimePileUp && !m_doAllPileUp) break;
                isSignalProcess=false;
                int pid = genEvt->signal_process_id();
                int eventNumber = genEvt->event_number();
                if (m_doInTimePileUp && pid==0 && eventNumber==-1) break; // stop at the separator
            }

            xAOD::TruthEvent* xTruthEvent = new xAOD::TruthEvent();
            xAOD::TruthPileupEvent* xTruthPileupEvent = new xAOD::TruthPileupEvent();

            /// @todo Drop or re-enable these? Signal process can be set to DSID... preferably not to the gen-name code
            //xTruthEvent->setSignalProcessId(genEvt->signal_process_id());
            //xTruthEvent->setEventNumber(genEvt->event_number());

            if (isSignalProcess) {
                m_xTruthEventContainer->push_back( xTruthEvent );
                // Cross-section
                const HepMC::GenCrossSection* const crossSection = genEvt->cross_section();
                xTruthEvent->setCrossSection(crossSection ? (float)crossSection->cross_section() : -1);
                xTruthEvent->setCrossSectionError(crossSection ? (float)crossSection->cross_section_error() : -1);

                //The mcChannelNumber is used as a unique identifier for which truth meta data belongs to
                const EventStreamInfo* esi = nullptr;
                CHECK( inputMetaStore->retrieve(esi));
                uint32_t mcChannelNumber = esi->getEventTypes().begin()->mc_channel_number();

                //Inserting in a (unordered_)set returns an <iterator, boolean> pair, where the boolean
                //is used to check if the key already exists (returns false in the case it exists)
                if( m_existingMetaDataChan.insert(mcChannelNumber).second ) {
                    xAOD::TruthMetaData* md = new xAOD::TruthMetaData();
                    m_tmd->push_back( md );

                        const auto &weightNameMap = genEvt->m_weights.m_names;
                        std::vector<std::string> orderedWeightNameVec;
                        orderedWeightNameVec.reserve( weightNameMap.size() );
                        for (const auto& entry: weightNameMap) {
                                orderedWeightNameVec.push_back(entry.first);
                        }

                    //The map from the HepMC record pairs the weight names with a corresponding index,
                    //it is not guaranteed that the indices are ascending when iterating over the map
                    std::sort(orderedWeightNameVec.begin(), orderedWeightNameVec.end(),
                    [&](const std::string &i, const std::string &j) {
                        return weightNameMap.at(i) < weightNameMap.at(j);
                    });

                    md->setMcChannelNumber(mcChannelNumber);
                    md->setWeightNames( std::move(orderedWeightNameVec) );
                }

                // Event weights
                vector<float> weights;
                weights.reserve(genEvt->weights().size());
                for (const double& w : genEvt->weights()) weights.push_back((float)(w));
                xTruthEvent->setWeights(std::move(weights));

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
                    xTruthEvent->setPdfInfoParameter(pdfInfo->id1(), xAOD::TruthEvent::PDGID1);
                    xTruthEvent->setPdfInfoParameter(pdfInfo->id2(), xAOD::TruthEvent::PDGID2);
                    xTruthEvent->setPdfInfoParameter(pdfInfo->pdf_id1(), xAOD::TruthEvent::PDFID1);
                    xTruthEvent->setPdfInfoParameter(pdfInfo->pdf_id2(), xAOD::TruthEvent::PDFID2);

                    xTruthEvent->setPdfInfoParameter((float)pdfInfo->x1(), xAOD::TruthEvent::X1);
                    xTruthEvent->setPdfInfoParameter((float)pdfInfo->x2(), xAOD::TruthEvent::X2);
                    xTruthEvent->setPdfInfoParameter((float)pdfInfo->scalePDF(), xAOD::TruthEvent::Q);
                    xTruthEvent->setPdfInfoParameter((float)pdfInfo->pdf1(), xAOD::TruthEvent::XF1);
                    xTruthEvent->setPdfInfoParameter((float)pdfInfo->pdf2(), xAOD::TruthEvent::XF2);
                }
            }
            if (!isSignalProcess) m_xTruthPileupEventContainer->push_back( xTruthPileupEvent );

            // (2) Build particles and vertices
            // Map for building associations between particles and vertices
            // The pair in the map is the (incomingParticles . outgoingParticles) of the given vertex
            // If signal process vertex is a disconnected vertex (no incoming/outgoing particles), add it manually
            VertexMap vertexMap;
            VertexMap::iterator mapItr;
            vector<const HepMC::GenVertex*> vertices;

            // Check signal process vertex
            // If this is a disconnected vertex, add it manually or won't be added from the loop over particles below.
            HepMC::GenVertex* disconnectedSignalProcessVtx = genEvt->signal_process_vertex(); // Get the signal process vertex
            if (disconnectedSignalProcessVtx) {
                if (disconnectedSignalProcessVtx->particles_in_size() == 0 &&
                        disconnectedSignalProcessVtx->particles_out_size() == 0 ) {
                    //This is a disconnected vertex, add it manually
                    vertices.push_back (disconnectedSignalProcessVtx);
                }
            } else {
                ATH_MSG_WARNING("Signal process vertex pointer not valid in HepMC Collection for GenEvent #" << cntr << " / " << m_mcColl->size());
            }

            // Get the beam particles
            pair<HepMC::GenParticle*,HepMC::GenParticle*> beamParticles;
            if ( genEvt->valid_beam_particles() ) beamParticles = genEvt->beam_particles();
            for (HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin(); pitr!=genEvt->particles_end(); ++pitr) {
                // (a) create TruthParticle
                xAOD::TruthParticle* xTruthParticle = new xAOD::TruthParticle();
                m_xTruthParticleContainer->push_back( xTruthParticle );
                fillParticle(xTruthParticle, *pitr); // (b) Copy HepMC info into the new particle
                // (c) Put particle into container; Build Event<->Particle element link
                const ElementLink<xAOD::TruthParticleContainer> eltp(*m_xTruthParticleContainer, m_xTruthParticleContainer->size()-1);
                if (isSignalProcess) xTruthEvent->addTruthParticleLink(eltp);
                if (!isSignalProcess) xTruthPileupEvent->addTruthParticleLink(eltp);

                // Create link between HepMC and xAOD truth
                if (isSignalProcess) m_truthLinkVec->push_back(new xAODTruthParticleLink(HepMcParticleLink((*pitr),0), eltp));
                if (!isSignalProcess) m_truthLinkVec->push_back(new xAODTruthParticleLink(HepMcParticleLink((*pitr),genEvt->event_number()), eltp));

                // Is this one of the beam particles?
                if (genEvt->valid_beam_particles()) {
                    if (isSignalProcess) {
                        if (*pitr == beamParticles.first) xTruthEvent->setBeamParticle1Link(eltp);
                        if (*pitr == beamParticles.second) xTruthEvent->setBeamParticle2Link(eltp);
                    }
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
            m_xTruthVertexContainer->reserve(vertices.size());
            for (const HepMC::GenVertex* vertex : vertices) {
                const auto& parts = vertexMap[vertex];
                // (a) create TruthVertex
                xAOD::TruthVertex* xTruthVertex = new xAOD::TruthVertex();
                m_xTruthVertexContainer->push_back( xTruthVertex );
                fillVertex(xTruthVertex, vertex); // (b) Copy HepMC info into the new vertex
                // (c) Put particle into container; Build Event<->Vertex element link
                ElementLink<xAOD::TruthVertexContainer> eltv(*m_xTruthVertexContainer, m_xTruthVertexContainer->size()-1);
                // Mark if this is the signal process vertex
                if (vertex == signalProcessVtx && isSignalProcess) xTruthEvent->setSignalProcessVertexLink(eltv);
                if (isSignalProcess) xTruthEvent->addTruthVertexLink(eltv);
                if (!isSignalProcess) xTruthPileupEvent->addTruthVertexLink(eltv);
                // (d) Assign incoming particles to the vertex, from the map
                xTruthVertex->setIncomingParticleLinks( parts.incomingEL );
                // (e) Assign outgoing particles to the vertex, from the map
                xTruthVertex->setOutgoingParticleLinks( parts.outgoingEL );
                // (f) Set Particle<->Vertex links for incoming particles
                for (xAOD::TruthParticle* p : parts.incoming) p->setDecayVtxLink(eltv);
                // (g) Set Particle<->Vertex links for incoming particles
                for (xAOD::TruthParticle* p : parts.outgoing) p->setProdVtxLink(eltv);
            } //end of loop over vertices

            // Delete the event that wasn't used
            if (isSignalProcess) delete xTruthPileupEvent;
            if (!isSignalProcess) delete xTruthEvent;

        } // end of loop over McEventCollection

    }

    std::stable_sort(m_truthLinkVec->begin(), m_truthLinkVec->end(), SortTruthParticleLink());
    ATH_MSG_VERBOSE("Summarizing truth link size: " << m_truthLinkVec->size() );

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
        tp->setPolarizationParameter(pol.theta(), xAOD::TruthParticle::polarizationTheta);
        tp->setPolarizationParameter(pol.phi(), xAOD::TruthParticle::polarizationPhi);
    }

    tp->setM(gp->generated_mass());
    tp->setPx(gp->momentum().px());
    tp->setPy(gp->momentum().py());
    tp->setPz(gp->momentum().pz());
    tp->setE(gp->momentum().e());
}


} // namespace xAODMaker
