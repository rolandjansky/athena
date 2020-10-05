/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TruthPVCollectionMaker.cxx
// Makes a small collection of 'primary' vertices, one per event
// A bit like a collection of 'reconstructable' vertices
// Future option: try to add b-decay vertices, or LLP vertices?

#include "DerivationFrameworkMCTruth/TruthPVCollectionMaker.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODTruth/TruthVertexAuxContainer.h"
// STL includes
#include <string>

// Constructor
DerivationFramework::TruthPVCollectionMaker::TruthPVCollectionMaker(const std::string& t,
                                                                    const std::string& n,
                                                                    const IInterface* p)
  : AthAlgTool(t,n,p)
  , m_eventsKey("TruthEvents")
  , m_collectionName("")
{
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("EventsKey", m_eventsKey);
    declareProperty("NewCollectionName", m_collectionName);
}

// Destructor
DerivationFramework::TruthPVCollectionMaker::~TruthPVCollectionMaker() {
}

// Athena initialize
StatusCode DerivationFramework::TruthPVCollectionMaker::initialize()
{
    ATH_MSG_VERBOSE("initialize() ...");

    // Check configuration, print errors, warning, and information for the user
    if (m_eventsKey=="") {
        ATH_MSG_FATAL("No truth event collection provided to use as a basis for new collections");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Using " << m_eventsKey << " as the source collections for new truth collections");}
    if (m_collectionName=="") {
        ATH_MSG_FATAL("No key provided for the new truth vertex collections");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("New truth vertex collection key: " << m_collectionName );}

    return StatusCode::SUCCESS;
}


// Selection and collection creation
StatusCode DerivationFramework::TruthPVCollectionMaker::addBranches() const
{
    // Retrieve truth collections
    const xAOD::TruthEventContainer* importedTruthEvents(nullptr);
    if (evtStore()->retrieve(importedTruthEvents,m_eventsKey).isFailure()) {
        ATH_MSG_ERROR("No TruthEvent collection with name " << m_eventsKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }

    // Create the new vertex containers
    xAOD::TruthVertexContainer* newVertexCollection = new xAOD::TruthVertexContainer();
    CHECK( evtStore()->record( newVertexCollection, m_collectionName ) );
    xAOD::TruthVertexAuxContainer* newVertexAuxCollection = new xAOD::TruthVertexAuxContainer();
    CHECK( evtStore()->record( newVertexAuxCollection, m_collectionName + "Aux." ) );
    newVertexCollection->setStore( newVertexAuxCollection );
    ATH_MSG_DEBUG( "Recorded new TruthVertexContainer with key: " << m_collectionName);

    // Go through the events, add one vertex for each event
    for (auto * event : *importedTruthEvents){
        // Just in case there is a place-holder
        if (!event) continue;
        // Try with the signal process vertex
        const xAOD::TruthVertex* old_vert(event->signalProcessVertex());
        // If that's not there, grab the first vertex
        if (!old_vert && event->nTruthVertices()>0){
            old_vert = event->truthVertex(0);
        }
        if (old_vert){
            // Hit -- copy it!
            // Make a new vertex and add it to the container
            xAOD::TruthVertex* xTruthVertex = new xAOD::TruthVertex();
            newVertexCollection->push_back( xTruthVertex );
            // Set properties
            xTruthVertex->setId(old_vert->id());
            xTruthVertex->setBarcode(old_vert->barcode());
            xTruthVertex->setX(old_vert->x());
            xTruthVertex->setY(old_vert->y());
            xTruthVertex->setZ(old_vert->z());
            xTruthVertex->setT(old_vert->t());
        } else {
            ATH_MSG_WARNING("No signal vertex or vertices associated to an input event!");
        }
    }
    return StatusCode::SUCCESS;
}
