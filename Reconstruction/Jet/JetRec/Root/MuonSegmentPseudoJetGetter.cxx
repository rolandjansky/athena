/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonSegmentPseudoJetGetter.cxx

#include "JetRec/MuonSegmentPseudoJetGetter.h"
#ifdef USE_BOOST_AUTO
#include <boost/typeof/typeof.hpp>
#endif
#include "JetEDM/PseudoJetVector.h"
#include "JetEDM/IndexedTConstituentUserInfo.h"

#include "JetRec/PseudoJetContainer.h"
#include "JetRec/MuonSegmentExtractor.h"
#include "JetRec/PseudoJetContainer.h"

using std::string;
using jet::PseudoJetVector;
using fastjet::PseudoJet;

//**********************************************************************

MuonSegmentPseudoJetGetter::MuonSegmentPseudoJetGetter(const std::string& name)
: AsgTool(name) {
  declareProperty("InputContainer", m_incoll);
  declareProperty("OutputContainer", m_outcoll);
  declareProperty("Label", m_label);
  declareProperty("Pt", m_pt =1.e-10);
}

//**********************************************************************

StatusCode MuonSegmentPseudoJetGetter::initialize() {
  ATH_MSG_DEBUG("Initializing...");
  print();

  ATH_CHECK( m_incoll.initialize() );
  ATH_CHECK( m_outcoll.initialize() );

  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode MuonSegmentPseudoJetGetter::createAndRecord() const {
  // Use const pointer for now, but can change to unique pointer when
  // we move to DataVector and when MR 2431 is complete:
  // https://gitlab.cern.ch/atlas/athena/merge_requests/2431

  // should rename getC createAndRecord once get() is removed.

  const PseudoJetContainer* pjc = this->getC();
  if(!pjc) return StatusCode::FAILURE; 

  return StatusCode::SUCCESS;
}

//**********************************************************************

const PseudoJetContainer* MuonSegmentPseudoJetGetter::getC() const {
  ATH_MSG_DEBUG("Getting MuonSegmentPseudoJetContainer...");
  const PseudoJetContainer * pjcont;

  // build and record the container
  const xAOD::MuonSegmentContainer* cont;
  auto handle_in = SG::makeHandle(m_incoll);
  if ( handle_in.isValid() ) {
    ATH_MSG_DEBUG("Retrieving xAOD container " << m_incoll.key() );
    // retrieve the input.
    cont = handle_in.cptr();
  } else {
    ATH_MSG_ERROR("Unable to find input collection: " << m_incoll.key());
    ATH_MSG_ERROR("Error creating pseudojets.");
    return nullptr;
  }

  std::vector<PseudoJet> vpj = createPseudoJets(cont);
    
  // create an extractor (MuonSegmentExtractors are always ghost extractors)
  MuonSegmentExtractor* extractor = new MuonSegmentExtractor(cont, m_label);

  // Put the PseudoJetContainer together :
  pjcont = new PseudoJetContainer(extractor, vpj);
  std::unique_ptr<const PseudoJetContainer> pjcont_ptr(pjcont);
  
  // record
  SG::WriteHandle<PseudoJetContainer> handle_out(m_outcoll);
  //ATH_MSG_DEBUG("New PseudoJetContainer in event store with extractor: " 
  //              << extractor->toString(0));

  // notify
  if ( ! handle_out.put(std::move(pjcont_ptr))) {
    ATH_MSG_ERROR("Unable to write new PseudoJetContainer to event store: " 
                  << m_outcoll.key());
  } else {
    ATH_MSG_DEBUG("Created new PseudoJetContainer in event store: " 
                  << m_outcoll.key());
  }

  return pjcont;
}


std::vector<PseudoJet> 
MuonSegmentPseudoJetGetter::createPseudoJets(const xAOD::MuonSegmentContainer* ms) const {
  
  std::vector<PseudoJet> vpj;
  int index=0;
  for(const xAOD::MuonSegment* part: *ms) {
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
    ATH_MSG_VERBOSE("Muon segment pseuojet y: " << psj.rap());
    vpj.push_back(psj);
    //vpj.push_back( PseudoJet(part->px(),part->py(),part->pz(),part->t0()) );
    vpj.back().set_user_index(index); // Set the index !!
    index++;
  }

  return vpj;
}
  
//**********************************************************************

std::string MuonSegmentPseudoJetGetter::label() const{
  return m_label;
}

//**********************************************************************

void MuonSegmentPseudoJetGetter::print() const {
  ATH_MSG_INFO("Properties for PseudoJetGetter " << name());
  ATH_MSG_INFO("             Label: " << m_label);
  ATH_MSG_INFO("   Input container: " << m_incoll.key());
  ATH_MSG_INFO("  Output container: " << m_outcoll.key());
  ATH_MSG_INFO("      Pseudojet pT: " << m_pt);

}

//**********************************************************************
