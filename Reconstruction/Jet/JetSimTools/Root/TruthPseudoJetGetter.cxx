// TruthPseudoJetGetter.cxx

#include "JetSimTools/TruthPseudoJetGetter.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "JetRec/PseudoJetContainer.h"
#include "JetSimTools/TruthParticleExtractor.h"


using fastjet::PseudoJet;


//**********************************************************************

TruthPseudoJetGetter::TruthPseudoJetGetter(const std::string &name)
: PseudoJetGetter(name),
  m_selector("JetTruthParticleSelectorTool") {
  declareProperty("TruthSelector", m_selector);
  declareProperty("InputContainer", m_incolltruth);
  declareProperty("OutputContainer", m_outcoll);
}

//**********************************************************************

StatusCode TruthPseudoJetGetter::initialize() {
  
  StatusCode sc= m_selector.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Unable to retrieve TruthSelector.");
    return sc;
  }
  m_incolltruth = m_incoll.key();
  ATH_CHECK( m_incolltruth.initialize() );
  ATH_CHECK( m_outcoll.initialize() );

  return StatusCode::SUCCESS;

}

//**********************************************************************

const PseudoJetContainer* TruthPseudoJetGetter::getC() const {
  ATH_MSG_DEBUG("Getting TruthPseudoJetContainer...");
  std::vector<PseudoJet> vpj;
  const PseudoJetContainer * pjcont;

  // build and record the container
  // retrieve the input.
  const xAOD::TruthParticleContainer* cont;
  auto handle_in = SG::makeHandle(m_incolltruth);
  if ( handle_in.isValid() ) {
    ATH_MSG_DEBUG("Retrieving xAOD container " << m_incolltruth.key());
    // retrieve the input.
    cont = handle_in.cptr();
  } else {
    ATH_MSG_ERROR("Unable to find input collection: " << m_incolltruth.key());
    ATH_MSG_ERROR("Error creating pseudojets.");
    return nullptr;
  }

  // create PseudoJets
  int index=0;
  for(const xAOD::TruthParticle* part: *cont) {
    vpj.push_back( PseudoJet(part->p4()));
    vpj.back().set_user_index(index); // Set the index !!
    index++;
  }


  // "Ghost" in outout collection name? If so is a ghost collection.
  bool isGhost = (m_outcoll.key()).find("Ghost") != std::string::npos;

  // create an extractor
  TruthParticleExtractor* extractor = new TruthParticleExtractor(cont,
                                                                 m_label, 
                                                                 isGhost);

  // ghostify the pseudojets if necessary
  if(isGhost){
    for(PseudoJet& pj : vpj) {pj *= 1e-40;}
  }

  // Put the PseudoJetContainer together :
  pjcont = new PseudoJetContainer(extractor, vpj);
  std::unique_ptr<const PseudoJetContainer> pjcont_ptr(pjcont);
  
  // record
  SG::WriteHandle<PseudoJetContainer> handle_out(m_outcoll);
  ATH_MSG_DEBUG("New PseudoJetContainer in event store with extractor: " 
                << extractor->toString(0));
  if ( ! handle_out.put(std::move(pjcont_ptr))) {
    ATH_MSG_ERROR("Unable to write new PseudoJetContainer to event store collection: " << m_outcoll.key());
  } else {
    ATH_MSG_DEBUG("Created new PseudoJetContainer in event store collection: " << m_outcoll.key());
  }

  return pjcont;

}


StatusCode TruthPseudoJetGetter::createAndRecord() const {
  // Use const pointer for now, but can change to unique pointer when
  // we move to DataVector and when MR 2431 is complete:
  // https://gitlab.cern.ch/atlas/athena/merge_requests/2431

  // should rename getC createAndRecord once get() is removed.
  const PseudoJetContainer* pjc = this->getC();
  if (!pjc) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}


const PseudoJetVector* TruthPseudoJetGetter::get() const {
  // Kept for backward compatibity
  ATH_MSG_DEBUG("Getting TruthPseudoJetContainer as PseudoJetVector ...");
  const PseudoJetContainer* cont = this->getC();
  const PseudoJetVector* vpj = cont->casVectorPseudoJet(); 
  return vpj;
}
