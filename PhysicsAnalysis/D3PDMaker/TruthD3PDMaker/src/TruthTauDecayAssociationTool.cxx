/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TruthD3PDMaker/src/TruthTauDecayAssociationTool.cxx
 * @author Zach Marshall <zach.marshall@cern.ch>
 * @date June, 2013
 * @brief Associate from a true hadronic tau to its decay products.
 */

#include "TruthTauDecayAssociationTool.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "AthenaKernel/errorcheck.h"
#include "barcodeOrder.h"
#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "GeneratorObjects/McEventCollection.h"


namespace D3PD {

/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TruthTauDecayAssociationTool::TruthTauDecayAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
    , m_i (0)
{}

/**
 * @brief Start the iteration for a new association.
 * @param p The object from which to associate.
 */
StatusCode
TruthTauDecayAssociationTool::reset (const TruthParticle& p)
{
  m_i = 0;
  m_tau_prods.clear();
  m_tau_prod_barcodes.clear();

  // Only receive the last tau in the chain through the filter
  // Just add the daughters in... but have to find it in the full record first
  const DataHandle<McEventCollection> mcec;
  if (evtStore()->retrieve<McEventCollection>(mcec,"GEN_EVENT").isSuccess()){
    // Loop over GenEvent's.
    for (const HepMC::GenEvent* ev_in : *mcec) {
      if (!ev_in) continue;
      for (HepMC::GenEvent::particle_const_iterator itrPart = ev_in->particles_begin();
           itrPart!=ev_in->particles_end();++itrPart){
        if ( (*itrPart) && (*itrPart)->barcode()==p.barcode() ){
          // Found it!
          addStableDaughters( (*itrPart) );
          break;
        }
      } // Loop over particles
    } // Loop over events
  } // Successful retrieve

  const DataHandle<TruthParticleContainer> tpc;
  if (evtStore()->retrieve<TruthParticleContainer>(tpc,"D3PDTruth").isSuccess()){
    for (unsigned int i=0;i<tpc->size();++i){
      if (! (*tpc)[i] ) continue;
      for (unsigned int j=0;j<m_tau_prod_barcodes.size();++j){
        if ( (*tpc)[i]->barcode() == m_tau_prod_barcodes[j] ){
          m_tau_prods.push_back( (*tpc)[i] );
          break;
        } // End matching barcode
      } // End loop over barcodes
    } // End loop over particles
  } // End successful retrieve

  return StatusCode::SUCCESS;
}

void TruthTauDecayAssociationTool::addStableDaughters(const HepMC::GenParticle* part) {
  // Sanity check
  if (!part) return;

  HepMC::GenVertex* endvx = part->end_vertex();
  if(!endvx){ // no children
    if ( part && part->status()==1 ) m_tau_prod_barcodes.push_back( part->barcode() );
    return;
  }

  // Loop over the parents of this particle.
  HepMC::GenVertex::particle_iterator itrChild = endvx->particles_begin(HepMC::children);
  HepMC::GenVertex::particle_iterator endChild = endvx->particles_end(HepMC::children);
  for(;itrChild!=endChild; ++itrChild){
    if ( (*itrChild) && (*itrChild)->status()==1 ){
      // Found a stable child!
      m_tau_prod_barcodes.push_back( (*itrChild)->barcode() );
    } else if ( (*itrChild) ){
      addStableDaughters( (*itrChild) );
    }
  } // End loop over children

}

/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const TruthParticle* TruthTauDecayAssociationTool::next()
{
  if (m_i < m_tau_prods.size())
    return m_tau_prods[m_i++];
  return 0;
}

} // namespace D3PD
