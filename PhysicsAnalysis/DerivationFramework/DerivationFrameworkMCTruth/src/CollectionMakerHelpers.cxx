/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// CollectionMakerHelpers.cxx
// Helper functions used in a couple of collection makers to add
// particles and vertices to light collections

#include "CollectionMakerHelpers.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODTruth/TruthVertexAuxContainer.h"

// STL includes
#include <vector>
// For a find in the vector
#include <algorithm>

int DerivationFramework::CollectionMakerHelpers::addTruthVertex( const xAOD::TruthVertex& oldVert, xAOD::TruthParticleContainer* partCont,
                                                                 xAOD::TruthVertexContainer* vertCont, std::vector<int>& seenParticles,
                                                                 const int generations) {
    // Make a new vertex and add it to the container
    xAOD::TruthVertex* xTruthVertex = new xAOD::TruthVertex();
    vertCont->push_back( xTruthVertex );
    // Get a link to this vertex -- will be used to set production vertices on all the next particles
    int myIndex = vertCont->size()-1;
    ElementLink<xAOD::TruthVertexContainer> eltv(*vertCont, myIndex);
    // Set properties
    xTruthVertex->setId(oldVert.id());
    xTruthVertex->setBarcode(oldVert.barcode());
    xTruthVertex->setX(oldVert.x());
    xTruthVertex->setY(oldVert.y());
    xTruthVertex->setZ(oldVert.z());
    xTruthVertex->setT(oldVert.t());
    // If we are done, then stop here
    if (generations==0) return myIndex;
    // Add all the outgoing particles
    for (size_t n=0;n<oldVert.nOutgoingParticles();++n){
        if (!oldVert.outgoingParticle(n)) continue; // Just in case we removed some truth particles, e.g. G4 decays
        int partIndex = CollectionMakerHelpers::addTruthParticle( *oldVert.outgoingParticle(n), partCont, vertCont, seenParticles, generations-1);
        ElementLink<xAOD::TruthParticleContainer> eltp( *partCont, partIndex);
        xTruthVertex->addOutgoingParticleLink( eltp );
        (*partCont)[partIndex]->setProdVtxLink( eltv );
    }
    // Return a link to this vertex
    return myIndex;
}

int DerivationFramework::CollectionMakerHelpers::addTruthParticle( const xAOD::TruthParticle& oldPart, xAOD::TruthParticleContainer* partCont,
                                                                   xAOD::TruthVertexContainer* vertCont, std::vector<int>& seenParticles,
                                                                   const int generations, bool includeVertex) {
    // See if we've seen it - note, could also do this with a unary function on the container itself
    if (std::find(seenParticles.begin(),seenParticles.end(),oldPart.barcode())!=seenParticles.end()){
      for (size_t p=0;p<partCont->size();++p){
        // Safety check
        if (!(*partCont)[p]) continue;
        // Was it a hit?
        if ((*partCont)[p]->barcode()==oldPart.barcode()) return p;
      } // Look through the old container
    } // Found it in the old container
    // Now we have seen it
    seenParticles.push_back(oldPart.barcode());
    // Make a nw truth particle
    xAOD::TruthParticle* xTruthParticle = setupTruthParticle(oldPart,partCont);
    // Make a link to this particle
    int myIndex = partCont->size()-1;
    ElementLink<xAOD::TruthParticleContainer> eltp(*partCont, myIndex);
    // Decay vertex information
    if (oldPart.hasDecayVtx() && includeVertex) {
        int vertIndex = CollectionMakerHelpers::addTruthVertex( *oldPart.decayVtx(), partCont, vertCont, seenParticles, generations);
        ElementLink<xAOD::TruthVertexContainer> eltv( *vertCont, vertIndex );
        xTruthParticle->setDecayVtxLink( eltv );
        (*vertCont)[vertIndex]->addIncomingParticleLink( eltp );
    }
    // Return a link to this particle
    return myIndex;
}

xAOD::TruthParticle* DerivationFramework::CollectionMakerHelpers::setupTruthParticle(const xAOD::TruthParticle& oldPart, xAOD::TruthParticleContainer* partCont){
    // Set up decorators
    const static SG::AuxElement::Decorator< unsigned int > originDecorator("classifierParticleOrigin");
    const static SG::AuxElement::Decorator< unsigned int > typeDecorator("classifierParticleType");
    const static SG::AuxElement::Decorator< unsigned int > outcomeDecorator("classifierParticleOutCome");
    const static SG::AuxElement::Decorator< int > motherIDDecorator("motherID");
    const static SG::AuxElement::Decorator< int > daughterIDDecorator("daughterID");
    // Make a truth particle and add it to the container
    xAOD::TruthParticle* xTruthParticle = new xAOD::TruthParticle();
    partCont->push_back( xTruthParticle );
    // Fill with numerical content
    xTruthParticle->setPdgId(oldPart.pdgId());
    xTruthParticle->setBarcode(oldPart.barcode());
    xTruthParticle->setStatus(oldPart.status());
    xTruthParticle->setM(oldPart.m());
    xTruthParticle->setPx(oldPart.px());
    xTruthParticle->setPy(oldPart.py());
    xTruthParticle->setPz(oldPart.pz());
    xTruthParticle->setE(oldPart.e());
    // Copy over the polarization information if it's there
    if (oldPart.polarization().valid()){
        xTruthParticle->setPolarizationParameter( oldPart.polarizationParameter( xAOD::TruthParticle::polarizationPhi ) , xAOD::TruthParticle::polarizationPhi );
        xTruthParticle->setPolarizationParameter( oldPart.polarizationParameter( xAOD::TruthParticle::polarizationTheta ) , xAOD::TruthParticle::polarizationTheta );
    }
    // Copy over the decorations if they are available
    if (oldPart.isAvailable<unsigned int>("classifierParticleType")) {
        typeDecorator(*xTruthParticle) = oldPart.auxdata< unsigned int >( "classifierParticleType" );
    } else {typeDecorator(*xTruthParticle) = 0;}
    if (oldPart.isAvailable<unsigned int>("classifierParticleOrigin")) {
        originDecorator(*xTruthParticle) = oldPart.auxdata< unsigned int >( "classifierParticleOrigin" );
    } else {originDecorator(*xTruthParticle) = 0;}
    if (oldPart.isAvailable<unsigned int>("classifierParticleOutCome")) {
        outcomeDecorator(*xTruthParticle) = oldPart.auxdata< unsigned int >( "classifierParticleOutCome" );
    } else {outcomeDecorator(*xTruthParticle) = 0;}
    return xTruthParticle;
}
