/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TruthNavigationDecorator.cxx
// Add navigation information to small truth collections

#include "TruthNavigationDecorator.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include <string>
#include <vector>
#include <map>

// Constructor
DerivationFramework::TruthNavigationDecorator::TruthNavigationDecorator(const std::string& t,
        const std::string& n,
        const IInterface* p ) :
    AthAlgTool(t,n,p)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty ("InputCollections", m_inputKeys = {});
}

// Destructor
DerivationFramework::TruthNavigationDecorator::~TruthNavigationDecorator() {
}

// Function to do dressing, implements interface in IAugmentationTool
StatusCode DerivationFramework::TruthNavigationDecorator::addBranches() const
{
  // Retrieve the truth collections
  const DataHandle<xAOD::TruthEventContainer> truthEvents(nullptr);
  CHECK(evtStore()->retrieve(truthEvents, "TruthEvents"));

  // Retrieve all the individual particle collections
  // Either use DataHandles or reserve the memory; these are just pointers, so the reserve is not such a big deal?
  std::vector< DataHandle<xAOD::TruthParticleContainer> > inputParticles;
  for (size_t k=0;k<m_inputKeys.size();++k){
    inputParticles.push_back(nullptr);
    CHECK(evtStore()->retrieve(inputParticles[k],m_inputKeys[k]));
  }

  // Build a dictionary of barcodes and element links
  std::map<int,ElementLink<xAOD::TruthParticleContainer> > linkMap;
  for (auto coll : inputParticles){
    for (size_t p=0;p<coll->size();++p){
      if (!coll->at(p)) continue; // Protection against null ptrs
      if (linkMap.find(coll->at(p)->barcode())!=linkMap.end()) continue; // Particle in multiple collections
      linkMap[coll->at(p)->barcode()] = ElementLink<xAOD::TruthParticleContainer>(*coll,p);
    } // Loop over particles in the collection
  } // Loop over collections

  // Naming to be consistent with xAOD::TruthParticle
  SG::AuxElement::Decorator< std::vector<ElementLink<xAOD::TruthParticleContainer> > > parent_decorator("parentLinks");
  SG::AuxElement::Decorator< std::vector<ElementLink<xAOD::TruthParticleContainer> > > child_decorator("childLinks");

  // Now loop over the collections and for each one decorate children and parents
  // The list of particles we keep is small-ish, and the list of particles in the
  // original truth record is large-ish, so I think it will be more efficient to
  // do a loop (O(N)) over the big record and a search (O(Nlog(N))) over the small
  // container.  Future performance optimization is welcome...

  // Keep maps, do the decoration last.  This ensures that duplicates all get decorated.
  std::map< int , std::vector<ElementLink<xAOD::TruthParticleContainer> > > parentMap;
  std::map< int , std::vector<ElementLink<xAOD::TruthParticleContainer> > > childMap;

  // Loop protection
  std::vector<int> seen_particles(20);
  // As usual, only consider the first truth event
  const xAOD::TruthEvent * event = truthEvents->at(0);
  for (size_t p=0;p<event->nTruthParticles();++p){
    if (!event->truthParticle(p)) continue; // Protection against null ptrs
    if (linkMap.find(event->truthParticle(p)->barcode())==linkMap.end()) continue; // Not a particle we are interested in
    // Make parent and child lists
    std::vector<ElementLink<xAOD::TruthParticleContainer> > parents;
    std::vector<ElementLink<xAOD::TruthParticleContainer> > children;
    // Populate the lists - include loop protection
    seen_particles.clear();
    find_parents( event->truthParticle(p) , parents , linkMap , seen_particles );
    seen_particles.clear();
    find_children( event->truthParticle(p) , children , linkMap , seen_particles );
    // Set the maps, so that we can decorate later
    parentMap[event->truthParticle(p)->barcode()] = parents;
    childMap[event->truthParticle(p)->barcode()] = children;
  } // Loop over truth particles in the big truth collection

  // Now final loop over the collections and setting all the decorators
  for (auto coll : inputParticles){
    for (size_t p=0;p<coll->size();++p){
      if (!coll->at(p)) continue; // Protection against null ptrs
      parent_decorator(*coll->at(p)) = parentMap[ coll->at(p)->barcode() ];
      child_decorator(*coll->at(p)) = childMap[ coll->at(p)->barcode() ];
    } // Loop over the particles in each collection
  } // Loop over the collections

  return StatusCode::SUCCESS;
}

void DerivationFramework::TruthNavigationDecorator::find_parents( const xAOD::TruthParticle* part ,
                                std::vector<ElementLink<xAOD::TruthParticleContainer> >& parents ,
                                std::map<int,ElementLink<xAOD::TruthParticleContainer> >& linkMap ,
                                std::vector<int>& seen_particles ) const {
  // Null pointer protection
  if (!part) return;
  // Check if we've seen the particle before, otherwise add it to our list
  if (std::find(seen_particles.begin(),seen_particles.end(),part->barcode())!=seen_particles.end()) return;
  seen_particles.push_back(part->barcode());
  // Loop through the parents and see if we know about them; otherwise iterate through the list
  for (size_t parent=0;parent<part->nParents();++parent){
    if (!part->parent(parent)) continue; // Null pointer check
    if (linkMap.find(part->parent(parent)->barcode())!=linkMap.end()){
      // Hit!  Add it to the list
      parents.push_back( linkMap[part->parent(parent)->barcode()] );
    } else {
      // Not a hit yet, keep iterating
      find_parents( part->parent(parent) , parents , linkMap , seen_particles );
    }
  } // Loop over parents
}

void DerivationFramework::TruthNavigationDecorator::find_children( const xAOD::TruthParticle* part ,
                                std::vector<ElementLink<xAOD::TruthParticleContainer> >& children ,
                                std::map<int,ElementLink<xAOD::TruthParticleContainer> >& linkMap ,
                                std::vector<int>& seen_particles ) const {
  // Null pointer protection
  if (!part) return;
  // Check if we've seen the particle before, otherwise add it to our list
  if (std::find(seen_particles.begin(),seen_particles.end(),part->barcode())!=seen_particles.end()) return;
  seen_particles.push_back(part->barcode());
  // Look through the children and see if we know about them; otherwise iterate through the list
  for (size_t child=0;child<part->nChildren();++child){
    if (!part->child(child)) continue; // Null pointer check
    if (linkMap.find(part->child(child)->barcode())!=linkMap.end()){
      // Hit!  Add it to the list
      children.push_back( linkMap[part->child(child)->barcode()] );
    } else {
      // Not a hit yet, keep iterating
      find_children( part->child(child) , children , linkMap , seen_particles );
    }
  } // Loop over parents
}
