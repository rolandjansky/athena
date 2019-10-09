/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// HardScatterCollectionMaker.cxx
// Keeps the first N generations of particles from the hard scatter
// Based on TruthDecayCollectionMaker (but simpler)

#include "HardScatterCollectionMaker.h"
#include "CollectionMakerHelpers.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODTruth/TruthVertexAuxContainer.h"

// STL includes
#include <vector>
#include <string>

// Constructor
DerivationFramework::HardScatterCollectionMaker::HardScatterCollectionMaker(const std::string& t,
                                                                          const std::string& n,
                                                                          const IInterface* p)
  : AthAlgTool(t,n,p)
  , m_eventsKey("TruthEvents")
  , m_collectionName("")
{
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("TruthEventKey", m_eventsKey);
    declareProperty("NewCollectionName", m_collectionName);
    declareProperty("Generations", m_generations=1, "Number of generations after the particle in question to keep (-1 for all)");
}

// Destructor
DerivationFramework::HardScatterCollectionMaker::~HardScatterCollectionMaker() {
}

// Athena initialize
StatusCode DerivationFramework::HardScatterCollectionMaker::initialize()
{
    ATH_MSG_VERBOSE("initialize() ...");

    if (m_eventsKey=="") {
        ATH_MSG_FATAL("No truth event collection provided to use as a basis for new collections");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Using " << m_eventsKey << " as the source collections for new truth collections");}

    if (m_collectionName=="") {
        ATH_MSG_FATAL("No key provided for the new truth particle collections");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("New truth particle collection key: " << m_collectionName );}

    return StatusCode::SUCCESS;
}


// Selection and collection creation
StatusCode DerivationFramework::HardScatterCollectionMaker::addBranches() const
{
    // Retrieve truth collections
    const xAOD::TruthEventContainer* importedTruthEvents(nullptr);
    if (evtStore()->retrieve(importedTruthEvents,m_eventsKey).isFailure()) {
        ATH_MSG_ERROR("No TruthEvent collection with name " << m_eventsKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    // We only care about the first event
    if (importedTruthEvents->size()==0){
        ATH_MSG_ERROR("TruthEvent collection with name " << m_eventsKey << " is empty!");
        return StatusCode::FAILURE;
    }
    // Check that it has a hard scatter process defined
    const xAOD::TruthVertex* my_tv = importedTruthEvents->at(0)->signalProcessVertex();
    if (my_tv==nullptr){
        static bool warn_once=false;
        if (!warn_once){
            ATH_MSG_WARNING("TruthEvent collection with name " << m_eventsKey << " has a null signal process vertex!");
            warn_once=true;
        }
        my_tv = importedTruthEvents->at(0)->truthVertex(0);
        if (my_tv==nullptr){
            ATH_MSG_ERROR("TruthEvent collection had no vertices at all? Something is wrong with your truth record!");
            return StatusCode::FAILURE;
        }
    }

    // Create the new particle containers
    xAOD::TruthParticleContainer* newParticleCollection = new xAOD::TruthParticleContainer();
    CHECK( evtStore()->record( newParticleCollection, m_collectionName + "Particles" ) );
    xAOD::TruthParticleAuxContainer* newParticleAuxCollection = new xAOD::TruthParticleAuxContainer();
    CHECK( evtStore()->record( newParticleAuxCollection, m_collectionName + "ParticlesAux." ) );
    newParticleCollection->setStore( newParticleAuxCollection );
    ATH_MSG_DEBUG( "Recorded new TruthParticleContainer with key: " << (m_collectionName+"Particles"));
    // Create the new vertex containers
    xAOD::TruthVertexContainer* newVertexCollection = new xAOD::TruthVertexContainer();
    CHECK( evtStore()->record( newVertexCollection, m_collectionName + "Vertices" ) );
    xAOD::TruthVertexAuxContainer* newVertexAuxCollection = new xAOD::TruthVertexAuxContainer();
    CHECK( evtStore()->record( newVertexAuxCollection, m_collectionName + "VerticesAux." ) );
    newVertexCollection->setStore( newVertexAuxCollection );
    ATH_MSG_DEBUG( "Recorded new TruthVertexContainer with key: " << (m_collectionName+"Vertices"));

    // Get the signal process vertex.  Get the incoming particles and outgoing particles and 
    // make a mini truth collection based on those
    std::vector<int> seen_particles; // Loop protection
    // Let's assume a reasonable case...
    CollectionMakerHelpers::addTruthParticle( *(my_tv->incomingParticle(0)), newParticleCollection, newVertexCollection, seen_particles, m_generations );
    // Are there any other incoming particles we need to add?
    for (size_t i=1;i<my_tv->nIncomingParticles();++i){
        // Set up decorators
        const static SG::AuxElement::Decorator< unsigned int > originDecorator("classifierParticleOrigin");
        const static SG::AuxElement::Decorator< unsigned int > typeDecorator("classifierParticleType");
        const static SG::AuxElement::Decorator< unsigned int > outcomeDecorator("classifierParticleOutCome");
        const static SG::AuxElement::Decorator< int > motherIDDecorator("motherID");
        const static SG::AuxElement::Decorator< int > daughterIDDecorator("daughterID");
        // Make a truth particle and add it to the container
        xAOD::TruthParticle* xTruthParticle = new xAOD::TruthParticle();
        newParticleCollection->push_back( xTruthParticle );
        // Make a link to this particle
        int my_index = newParticleCollection->size()-1;
        ElementLink<xAOD::TruthParticleContainer> eltp(*newParticleCollection, my_index);
        // Decay vertex link
        ElementLink<xAOD::TruthVertexContainer> eltv( *newVertexCollection, 0 ); // First one we added in
        xTruthParticle->setDecayVtxLink( eltv );
        (*newVertexCollection)[0]->addIncomingParticleLink( eltp );
        // Fill with numerical content
        xTruthParticle->setPdgId(my_tv->incomingParticle(i)->pdgId());
        xTruthParticle->setBarcode(my_tv->incomingParticle(i)->barcode());
        xTruthParticle->setStatus(my_tv->incomingParticle(i)->status());
        xTruthParticle->setM(my_tv->incomingParticle(i)->m());
        xTruthParticle->setPx(my_tv->incomingParticle(i)->px());
        xTruthParticle->setPy(my_tv->incomingParticle(i)->py());
        xTruthParticle->setPz(my_tv->incomingParticle(i)->pz());
        xTruthParticle->setE(my_tv->incomingParticle(i)->e());
        // Copy over the polarization information if it's there
        if (my_tv->incomingParticle(i)->polarization().valid()){
            xTruthParticle->setPolarizationParameter( my_tv->incomingParticle(i)->polarizationParameter( xAOD::TruthParticle::polarizationPhi ) , xAOD::TruthParticle::polarizationPhi );
            xTruthParticle->setPolarizationParameter( my_tv->incomingParticle(i)->polarizationParameter( xAOD::TruthParticle::polarizationTheta ) , xAOD::TruthParticle::polarizationTheta );
        }
        // Copy over the decorations if they are available
        if (my_tv->incomingParticle(i)->isAvailable<unsigned int>("classifierParticleType")) {
            typeDecorator(*xTruthParticle) = my_tv->incomingParticle(i)->auxdata< unsigned int >( "classifierParticleType" );
        } else {typeDecorator(*xTruthParticle) = 0;}
        if (my_tv->incomingParticle(i)->isAvailable<unsigned int>("classifierParticleOrigin")) {
            originDecorator(*xTruthParticle) = my_tv->incomingParticle(i)->auxdata< unsigned int >( "classifierParticleOrigin" );
        } else {originDecorator(*xTruthParticle) = 0;}
        if (my_tv->incomingParticle(i)->isAvailable<unsigned int>("classifierParticleOutCome")) {
            outcomeDecorator(*xTruthParticle) = my_tv->incomingParticle(i)->auxdata< unsigned int >( "classifierParticleOutCome" );
        } else {outcomeDecorator(*xTruthParticle) = 0;}
    } // Done adding incoming particles -- all finished!

    return StatusCode::SUCCESS;
}
