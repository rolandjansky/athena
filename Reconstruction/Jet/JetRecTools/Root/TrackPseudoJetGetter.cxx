/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRecTools/TrackPseudoJetGetter.h"
#include "JetEDM/VertexIndexedConstituentUserInfo.h"

#include "JetEDM/TrackVertexAssociation.h"

#include "JetRec/PseudoJetContainer.h"
#include "JetRecTools/TrackExtractor.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "fastjet/PseudoJet.hh"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"


using fastjet::PseudoJet;


TrackPseudoJetGetter::TrackPseudoJetGetter(const std::string &name) 
  : PseudoJetGetter(name) , m_inTVA("JetTrackVtxAssoc") {
  declareProperty("TrackVertexAssociation", m_inTVA, "SG key for the TrackVertexAssociation object");
}

//**********************************************************************
StatusCode TrackPseudoJetGetter::initialize() {
  ATH_MSG_DEBUG("Initializing...");

  ATH_CHECK( PseudoJetGetter::initialize() );
  ATH_CHECK( m_inTVA.initialize() );
  m_incolltrk = m_incoll.key();
  ATH_CHECK( m_incolltrk.initialize() );
  print();
  
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode TrackPseudoJetGetter::createAndRecord() const {
  // Use const pointer for now, but can change to unique pointer when
  // we move to DataVector and when MR 2431 is complete:
  // https://gitlab.cern.ch/atlas/athena/merge_requests/2431
  const PseudoJetContainer* pjc = this->getC();
  if (!pjc) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

//**********************************************************************
const PseudoJetContainer* TrackPseudoJetGetter::getC() const {

  ATH_MSG_DEBUG("Getting PseudoJetContainer in TrackPseudoJetGetter ...");
  
  // build and record the container
  const xAOD::TrackParticleContainer* cont;
  auto handle_in = SG::makeHandle(m_incolltrk);
  if ( handle_in.isValid() ) {
    ATH_MSG_DEBUG("Retrieving xAOD container " << m_incolltrk.key() << ", ghost scale=" << m_ghostscale  <<  ", isGhost=" << bool(m_ghostscale));
    // retrieve the input.
    cont = handle_in.cptr();
  } else {
    ATH_MSG_ERROR("Unable to find input collection: " << m_incolltrk.key());
    ATH_MSG_ERROR("Error creating pseudojets.");
    return nullptr;
  }

  SG::ReadHandle<jet::TrackVertexAssociation> h_tva(m_inTVA);

  std::vector<PseudoJet> vpj = createPseudoJets(cont);

  // "Ghost" in outout collection name? If so is a ghost collection.
  bool isGhost = (m_outcoll.key()).find("Ghost") != std::string::npos;

  // create an extractor
  TrackExtractor* extractor = new TrackExtractor(cont, m_label, isGhost);

  // ghostify the pseudojets if necessary
  if(isGhost){
    for(PseudoJet& pj : vpj) {pj *= 1e-40;}
  }

  // record
  // to satisfy legacy code, need to return a bare pointer as well
  // as write to storegate. so no calls to  make_unique...

  SG::WriteHandle<PseudoJetContainer> handle_out(m_outcoll);
  PseudoJetContainer*  pjcont = new PseudoJetContainer(extractor, vpj);
  
  if(!handle_out.record(std::unique_ptr<PseudoJetContainer>(pjcont))){
    ATH_MSG_DEBUG("Error storing PseudoJetContainer in event "
                  << " at key " << m_outcoll.key());
  } else {
    //ATH_MSG_DEBUG("PseudoJetContainer in event store with extractor: " 
    //               << extractor->toString(0)
    //             << " at key " << m_outcoll.key());
  }

  return pjcont;

}

std::vector<PseudoJet> 
TrackPseudoJetGetter::createPseudoJets(const xAOD::TrackParticleContainer* ips) const {

  std::vector<PseudoJet> vpj;
  // create PseudoJets
  int index=-1;
  for(const xAOD::TrackParticle* part: *ips) {
    index++;
    
    if ( part == 0 || (m_skipNegativeEnergy && part->e() <= 0.0) ) {
      if ( part == 0 ) ATH_MSG_DEBUG("NUll object!");
      else ATH_MSG_VERBOSE("Skipping cluster with E = " << part->e());
      //++nskip;
      continue;
    }
    
    // Take momentum from TrackParticle.
    fastjet::PseudoJet psj(part->p4());
    
    // const xAOD::Vertex* vtx = h_tva->associatedVertex(part);
    //jet::IConstituentUserInfo* pcui = this->buildCUI(trk, vtx, labidx, pli);
    //psj.set_user_info(pcui);
    
    vpj.push_back( psj );
    vpj.back().set_user_index(index); // Set the index !!
  }

  return vpj;
}


