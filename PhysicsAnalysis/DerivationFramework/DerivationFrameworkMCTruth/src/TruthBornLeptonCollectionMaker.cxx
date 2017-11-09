/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TruthBornLeptonCollectionMaker.cxx
// Makes a special collection of Born leptons

// My own header file
#include "TruthBornLeptonCollectionMaker.h"
// EDM includes for the particles we need
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
// To look up which generator is being used
#include "xAODTruth/TruthMetaDataContainer.h"
// STL includes
#include <string>

// Constructor
DerivationFramework::TruthBornLeptonCollectionMaker::TruthBornLeptonCollectionMaker(const std::string& t,
                                const std::string& n,
                                const IInterface* p)
  : AthAlgTool(t,n,p)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("ParticlesKey", m_particlesKey="TruthParticles");
  declareProperty("NewCollectionName", m_collectionName="");
}

// Destructor
DerivationFramework::TruthBornLeptonCollectionMaker::~TruthBornLeptonCollectionMaker() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::TruthBornLeptonCollectionMaker::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");

  if (m_particlesKey=="" /*|| m_verticesKey==""*/) {
    ATH_MSG_FATAL("No truth particle collection provided to use as a basis for new collections");
    return StatusCode::FAILURE;
  } else {ATH_MSG_INFO("Using " << m_particlesKey << " as the source collections for new truth collections");}
  
  if (m_collectionName=="") {
    ATH_MSG_FATAL("No key provided for the new truth particle collection");
    return StatusCode::FAILURE;
  } else {ATH_MSG_INFO("New truth particle collection key: " << m_collectionName );}

  return StatusCode::SUCCESS;
}

// Selection and collection creation
StatusCode DerivationFramework::TruthBornLeptonCollectionMaker::addBranches() const
{
  // Set up for some metadata handling
  bool is_sherpa = false;
  static bool warn_once=false;
  if (evtStore()->contains<xAOD::TruthMetaDataContainer>("TruthMetaData")){
    // Note that I'd like to get this out of metadata in general, but it seems that the
    // metadata isn't fully available in initialize, and since this is a const function
    // I can only do the retrieve every event, rather than lazy-initializing, since this
    // metadata ought not change during a run
    const DataHandle<xAOD::TruthMetaDataContainer> truthMetaData(nullptr);
    // Shamelessly stolen from the file meta data tool
    ATH_CHECK( evtStore()->retrieve(truthMetaData) );
  
    if (truthMetaData->size()>0){
      // Let's just be super sure...
      const std::string gens = truthMetaData->at(0)->generators();
      is_sherpa = gens.find("sherpa")==std::string::npos &&
             gens.find("Sherpa")==std::string::npos &&
             gens.find("SHERPA")==std::string::npos;
    } // Seems to be the only sure way...
  } else if (!warn_once){
    ATH_MSG_WARNING("Could not find metadata container in storegate; assuming NOT sherpa");
    warn_once=true;
  }

  // Retrieve truth collections
  const DataHandle<xAOD::TruthParticleContainer> importedTruthParticles(nullptr);
  if (evtStore()->retrieve(importedTruthParticles,m_particlesKey).isFailure()) {
    ATH_MSG_ERROR("No TruthParticle collection with name " << m_particlesKey << " found in StoreGate!");
    return StatusCode::FAILURE;
  }

  // Create the new containers
  xAOD::TruthParticleContainer* newParticleCollection = new xAOD::TruthParticleContainer();
  CHECK( evtStore()->record( newParticleCollection, m_collectionName ) );
  xAOD::TruthParticleAuxContainer* newParticleAuxCollection = new xAOD::TruthParticleAuxContainer();
  CHECK( evtStore()->record( newParticleAuxCollection, m_collectionName + "Aux." ) );
  newParticleCollection->setStore( newParticleAuxCollection );
  ATH_MSG_DEBUG( "Recorded new TruthParticleContainer with key: " <<  m_collectionName);

  // Set up decorators
  const static SG::AuxElement::Decorator< unsigned int > originDecorator("classifierParticleOrigin");
  const static SG::AuxElement::Decorator< unsigned int > typeDecorator("classifierParticleType");
  const static SG::AuxElement::Decorator< unsigned int > outcomeDecorator("classifierParticleOutCome");

  // add relevant particles to new collection
  for (unsigned int i=0; i<importedTruthParticles->size(); ++i) {
    // Grab the particle
    const xAOD::TruthParticle* theParticle = (*importedTruthParticles)[i];
    if (!theParticle) continue; // Protection against null pointers
    if (!theParticle->isLepton()) continue; // Only include leptons!

    if (is_sherpa && theParticle->status()!=11){
      // If Sherpa, take leptons with status 11
      continue;
    } else if (!is_sherpa) {
      // Some generators, look for leptons with status 3 coming from vertices with other leptons
      bool has_status_n3=false, has_status_3=false, has_V=false;
      if (theParticle->status()==3){
        // Look for other leptons in the production vertex... carefully
        if (theParticle->hasProdVtx()){
          const xAOD::TruthVertex * prod = theParticle->prodVtx();
          for (size_t p=0;p<prod->nOutgoingParticles();++p){
            if (prod->outgoingParticle(p) &&
              prod->outgoingParticle(p)->isLepton()){
              has_status_n3 = has_status_n3 || prod->outgoingParticle(p)->status()!=3;
              has_status_3 = has_status_3 || prod->outgoingParticle(p)->status()==3;
            }
          } // Loop over particles from the same production vertex
          for (size_t p=0;p<prod->nIncomingParticles();++p){
            // See if there was a boson going *into* the vertex
            if (prod->incomingParticle(p) &&
              (prod->incomingParticle(p)->isZ() || prod->incomingParticle(p)->isW() || prod->incomingParticle(p)->isHiggs()) ){
              has_V=true;
              break;
            } // Found a vector boson
          } // Loop over particles going into the same production vertex
        } // Doesn't have a production vertex
      } // Had status 3

      // Now we have all the information for the special case of V->l(born) l(bare) l(born) l(bare)
      if ( !(has_status_3 && has_status_n3 && has_V && theParticle->status()==3) &&
         theParticle->status()!=23){
        // If not a special case, deal with the standard: has a boson parent, is a lepton, and has a descendent that is a bare lepton
        if (!theParticle->parent()) continue;
        if (!theParticle->parent()->isZ() && !theParticle->parent()->isW() && !theParticle->parent()->isHiggs()) continue;
        if (!hasBareDescendent( theParticle ) ) continue;
      }
    } // End of treatment for generators that are not Sherpa

    // Add this particle to the new collection
    xAOD::TruthParticle* xTruthParticle = new xAOD::TruthParticle();
    newParticleCollection->push_back( xTruthParticle );
    // Fill with numerical content
    xTruthParticle->setPdgId(theParticle->pdgId());
    xTruthParticle->setBarcode(theParticle->barcode());
    xTruthParticle->setStatus(theParticle->status());
    xTruthParticle->setM(theParticle->m());
    xTruthParticle->setPx(theParticle->px());
    xTruthParticle->setPy(theParticle->py());
    xTruthParticle->setPz(theParticle->pz());
    xTruthParticle->setE(theParticle->e());
    // Copy over the decorations if they are available
    if (theParticle->isAvailable<unsigned int>("classifierParticleType")) {
      typeDecorator(*xTruthParticle) = theParticle->auxdata< unsigned int >( "classifierParticleType" );
    } else {typeDecorator(*xTruthParticle) = 0;}
    if (theParticle->isAvailable<unsigned int>("classifierParticleOrigin")) {
      originDecorator(*xTruthParticle) = theParticle->auxdata< unsigned int >( "classifierParticleOrigin" );
    } else {originDecorator(*xTruthParticle) = 0;}
    if (theParticle->isAvailable<unsigned int>("classifierParticleOutCome")) {
      outcomeDecorator(*xTruthParticle) = theParticle->auxdata< unsigned int >( "classifierParticleOutCome" );
    } else {outcomeDecorator(*xTruthParticle) = 0;}
  } // Loop over alll particles

  return StatusCode::SUCCESS;
}

// Find out if a particle has a bare descendent
bool DerivationFramework::TruthBornLeptonCollectionMaker::hasBareDescendent( const xAOD::TruthParticle* p ) const
{
  // Null pointer check
  if (!p) return false;
  // If we hit a bare descendent, then we're a winnner
  if (p->isLepton() && p->status()==1) return true;
  // Otherwise look through all the children
  for (size_t c=0;c<p->nChildren();++c){
    if (!p->child(c)) continue; // Null pointer protection
    if (p->pdgId()!=p->child(c)->pdgId()) continue; // Different particle child
    if (hasBareDescendent( p->child(c) )) return true;
  }
  // No luck -- this branch is a dead end
  return false;
}

