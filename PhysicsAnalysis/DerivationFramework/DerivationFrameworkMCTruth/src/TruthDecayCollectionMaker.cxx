/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TruthDecayCollectionMaker.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Removes all truth particles/vertices which do not pass a user-defined cut
// Based on TruthCollectionMaker (but simpler)

#include "DerivationFrameworkMCTruth/TruthDecayCollectionMaker.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODTruth/TruthVertexAuxContainer.h"
// STL includes
#include <vector>
#include <string>
// For a find in the vector
#include <algorithm>

// Constructor
DerivationFramework::TruthDecayCollectionMaker::TruthDecayCollectionMaker(const std::string& t,
                                                                          const std::string& n,
                                                                          const IInterface* p)
  : AthAlgTool(t,n,p)
  , m_particlesKey("TruthParticles")
  , m_collectionName("")
{
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("ParticlesKey", m_particlesKey);
    declareProperty("NewCollectionName", m_collectionName);
    declareProperty("PDGIDsToKeep", m_pdgIdsToKeep={}, "PDG IDs of particles to build the collection from");
    declareProperty("KeepBHadrons", m_keepBHadrons=false, "Keep b-hadrons (easier than by PDG ID)");
    declareProperty("KeepCHadrons", m_keepCHadrons=false, "Keep c-hadrons (easier than by PDG ID)");
    declareProperty("KeepBSM", m_keepBSM=false, "Keep BSM particles (easier than by PDG ID)");
    declareProperty("Generations", m_generations=-1, "Number of generations after the particle in question to keep (-1 for all)");
}

// Destructor
DerivationFramework::TruthDecayCollectionMaker::~TruthDecayCollectionMaker() {
}

// Athena initialize
StatusCode DerivationFramework::TruthDecayCollectionMaker::initialize()
{
    ATH_MSG_VERBOSE("initialize() ...");

    if (m_particlesKey=="") {
        ATH_MSG_FATAL("No truth particle collection provided to use as a basis for new collections");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Using " << m_particlesKey << " as the source collections for new truth collections");}

    if (m_collectionName=="") {
        ATH_MSG_FATAL("No key provided for the new truth particle collections");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("New truth particle collection key: " << m_collectionName );}

    if (m_pdgIdsToKeep.size()==0 && !m_keepBHadrons && !m_keepCHadrons && !m_keepBSM) {
        ATH_MSG_FATAL("No PDG IDs provided, not keeping b- or c-hadrons or BSM particles -- what do you want?");
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}


// Selection and collection creation
StatusCode DerivationFramework::TruthDecayCollectionMaker::addBranches() const
{
    // Retrieve truth collections
    const xAOD::TruthParticleContainer* importedTruthParticles(nullptr);
    if (evtStore()->retrieve(importedTruthParticles,m_particlesKey).isFailure()) {
        ATH_MSG_ERROR("No TruthParticle collection with name " << m_particlesKey << " found in StoreGate!");
        return StatusCode::FAILURE;
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

    // List of barcodes for particles in our collection already.  Because of the way we recurse,
    // adding more particles as we go, there should be no need to add (or help from adding) the
    // barcodes of particles that we are *not* going to keep
    std::vector<int> seen_particles;
    // Go through that list of particles!
    for (auto * part : *importedTruthParticles){
        for (int id : m_pdgIdsToKeep){
            if (part->absPdgId()==id){
                addTruthParticle( *part, newParticleCollection, newVertexCollection, seen_particles, m_generations );
            } // Found a particle of interest!
        } // Loop over the PDG IDs we want to keep
        if ((m_keepBHadrons && part->isBottomHadron()) ||
            (m_keepCHadrons && part->isCharmHadron()) ||
            (m_keepBSM && part->isBSM()) ){
            addTruthParticle( *part, newParticleCollection, newVertexCollection, seen_particles , m_generations );
        }
    } // Loop over the initial truth particle collection
    return StatusCode::SUCCESS;
}

int DerivationFramework::TruthDecayCollectionMaker::addTruthParticle( const xAOD::TruthParticle& old_part, xAOD::TruthParticleContainer* part_cont, 
                                                                      xAOD::TruthVertexContainer* vert_cont, std::vector<int>& seen_particles,
                                                                      const int generations) const {
    // See if we've seen it - note, could also do this with a unary function on the container itself
    if (std::find(seen_particles.begin(),seen_particles.end(),old_part.barcode())!=seen_particles.end()){
      for (size_t p=0;p<part_cont->size();++p){
        // Was it a hit?
        if ((*part_cont)[p]->barcode()==old_part.barcode()) return p;
      } // Look through the old container
    } // Found it in the old container
    // Now we have seen it
    seen_particles.push_back(old_part.barcode());
    // Set up decorators
    const static SG::AuxElement::Decorator< unsigned int > originDecorator("classifierParticleOrigin");
    const static SG::AuxElement::Decorator< unsigned int > typeDecorator("classifierParticleType");
    const static SG::AuxElement::Decorator< unsigned int > outcomeDecorator("classifierParticleOutCome");
    const static SG::AuxElement::Decorator< int > motherIDDecorator("motherID");
    const static SG::AuxElement::Decorator< int > daughterIDDecorator("daughterID");
    // Make a truth particle and add it to the container
    xAOD::TruthParticle* xTruthParticle = new xAOD::TruthParticle();
    part_cont->push_back( xTruthParticle );
    // Make a link to this particle
    int my_index = part_cont->size()-1;
    ElementLink<xAOD::TruthParticleContainer> eltp(*part_cont, my_index);
    // Decay vertex information
    if (old_part.hasDecayVtx()) {
        int vert_index = addTruthVertex( *old_part.decayVtx(), part_cont, vert_cont, seen_particles, generations);
        ElementLink<xAOD::TruthVertexContainer> eltv( *vert_cont, vert_index );
        xTruthParticle->setDecayVtxLink( eltv );
        (*vert_cont)[vert_index]->addIncomingParticleLink( eltp );
    }
    // Fill with numerical content
    xTruthParticle->setPdgId(old_part.pdgId());
    xTruthParticle->setBarcode(old_part.barcode());
    xTruthParticle->setStatus(old_part.status());
    xTruthParticle->setM(old_part.m());
    xTruthParticle->setPx(old_part.px());
    xTruthParticle->setPy(old_part.py());
    xTruthParticle->setPz(old_part.pz());
    xTruthParticle->setE(old_part.e());
    // Copy over the polarization information if it's there
    if (old_part.polarization().valid()){
        xTruthParticle->setPolarizationParameter( old_part.polarizationParameter( xAOD::TruthParticle::polarizationPhi ) , xAOD::TruthParticle::polarizationPhi );
        xTruthParticle->setPolarizationParameter( old_part.polarizationParameter( xAOD::TruthParticle::polarizationTheta ) , xAOD::TruthParticle::polarizationTheta );
    }
    // Copy over the decorations if they are available
    if (old_part.isAvailable<unsigned int>("classifierParticleType")) {
        typeDecorator(*xTruthParticle) = old_part.auxdata< unsigned int >( "classifierParticleType" );
    } else {typeDecorator(*xTruthParticle) = 0;}
    if (old_part.isAvailable<unsigned int>("classifierParticleOrigin")) {
        originDecorator(*xTruthParticle) = old_part.auxdata< unsigned int >( "classifierParticleOrigin" );
    } else {originDecorator(*xTruthParticle) = 0;}
    if (old_part.isAvailable<unsigned int>("classifierParticleOutCome")) {
        outcomeDecorator(*xTruthParticle) = old_part.auxdata< unsigned int >( "classifierParticleOutCome" );
    } else {outcomeDecorator(*xTruthParticle) = 0;}
    // Return a link to this particle
    return my_index;
}

int DerivationFramework::TruthDecayCollectionMaker::addTruthVertex( const xAOD::TruthVertex& old_vert, xAOD::TruthParticleContainer* part_cont, 
                                                                    xAOD::TruthVertexContainer* vert_cont, std::vector<int>& seen_particles,
                                                                    const int generations) const {
    // Make a new vertex and add it to the container
    xAOD::TruthVertex* xTruthVertex = new xAOD::TruthVertex();
    vert_cont->push_back( xTruthVertex );
    // Get a link to this vertex -- will be used to set production vertices on all the next particles
    int my_index = vert_cont->size()-1;
    ElementLink<xAOD::TruthVertexContainer> eltv(*vert_cont, my_index);
    // Set properties
    xTruthVertex->setId(old_vert.id());
    xTruthVertex->setBarcode(old_vert.barcode());
    xTruthVertex->setX(old_vert.x());
    xTruthVertex->setY(old_vert.y());
    xTruthVertex->setZ(old_vert.z());
    xTruthVertex->setT(old_vert.t());
    // If we are done, then stop here
    if (generations==0) return my_index;
    // Add all the outgoing particles
    for (size_t n=0;n<old_vert.nOutgoingParticles();++n){
        if (!old_vert.outgoingParticle(n)) continue; // Just in case we removed some truth particles, e.g. G4 decays
        int part_index = addTruthParticle( *old_vert.outgoingParticle(n), part_cont, vert_cont, seen_particles, generations-1);
        ElementLink<xAOD::TruthParticleContainer> eltp( *part_cont, part_index);
        xTruthVertex->addOutgoingParticleLink( eltp );
        (*part_cont)[part_index]->setProdVtxLink( eltv );
    }
    // Return a link to this vertex
    return my_index;
}
