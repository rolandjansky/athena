/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRec/MuonSegmentExtractor.h"
#include "MuonSegmentPseudoJetAlgorithm.h"

//**********************************************************************

StatusCode MuonSegmentPseudoJetAlgorithm::initialize() {
  ATH_MSG_INFO("Initializing " << name() << "...");

  print();

  if(m_incoll.key().empty() || m_outcoll.key().empty()) {
    ATH_MSG_ERROR("Either input or output collection is empty!");
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_incoll.initialize() );
  ATH_CHECK( m_outcoll.initialize() );

  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode MuonSegmentPseudoJetAlgorithm::execute(const EventContext& ctx) const {
  ATH_MSG_VERBOSE("Executing " << name() << "...");

  auto incoll = SG::makeHandle<xAOD::MuonSegmentContainer>(m_incoll,ctx);
  if( !incoll.isValid() ) {
    // Return SUCCESS to avoid crashing T0 jobs
    ATH_MSG_WARNING("Failed to retrieve " << m_incoll.key() << " for PseudoJet creation!" );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("Retrieved muon segment container " << m_incoll.key() );

  std::vector<fastjet::PseudoJet> vpj = createPseudoJets(*incoll);
    
  // create an extractor (MuonSegmentExtractors are always ghost extractors)
  auto extractor = std::make_unique<MuonSegmentExtractor>(incoll.cptr(), m_label);
  ATH_MSG_DEBUG("Created extractor: "  << extractor->toString(0));

  // Put the PseudoJetContainer together
  auto pjcont = std::make_unique<PseudoJetContainer>(extractor.release(), vpj);
  
  auto outcoll = SG::makeHandle<PseudoJetContainer>(m_outcoll,ctx);
  ATH_MSG_DEBUG("New PseudoJetContainer size " << pjcont->size());
  ATH_CHECK(outcoll.record(std::move(pjcont)));

  return StatusCode::SUCCESS;
}

//**********************************************************************

std::vector<fastjet::PseudoJet> 
MuonSegmentPseudoJetAlgorithm::createPseudoJets(const xAOD::MuonSegmentContainer& ms) const {
  
  std::vector<fastjet::PseudoJet> vpj;
  int index=0;
  for(const xAOD::MuonSegment* part: ms) {
    double pt = m_pt;
    double x = part->x();
    double y = part->y();
    double z = part->z();
    double xy = sqrt(x*x + y*y);
    double r = sqrt(xy*xy + z*z);
    double pfac = pt/xy;
    double px = pfac*x;
    double py = pfac*y;
    double pz = pfac*z;
    double  e = pfac*r;
    fastjet::PseudoJet psj(px, py, pz, e);
    ATH_MSG_VERBOSE("Muon segment pseudojet y: " << psj.rap());
    vpj.push_back(psj);
    vpj.back().set_user_index(index); // Set the index !!
    index++;
  }

  return vpj;
}

//**********************************************************************

void MuonSegmentPseudoJetAlgorithm::print() const {
  ATH_MSG_INFO("Properties for MuonSegmentPseudoJetGetter " << name());
  ATH_MSG_INFO("             Label: " << m_label);
  ATH_MSG_INFO("   Input container: " << m_incoll.key());
  ATH_MSG_INFO("  Output container: " << m_outcoll.key());
  ATH_MSG_INFO("      Pseudojet pT: " << m_pt);
}

//**********************************************************************
