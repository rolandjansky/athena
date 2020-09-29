/*
 *   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration  
 */

#include "TrigHLTJetRec/TrigHLTJetTLASelector.h"
#include "xAODBase/IParticleHelpers.h"


//Helper functions
struct DescendingEt{

  bool operator()(const xAOD::Jet* l, const xAOD::Jet* r)  const {
    return l->p4().Et() > r->p4().Et();
  }

};

struct HasPtAboveThreshold {

  HasPtAboveThreshold(double thresholdPt) : thresholdPt(thresholdPt) {}
  double thresholdPt;

  bool operator()(const xAOD::Jet* myjet)  const {
    return myjet->p4().Pt() > thresholdPt;
  }

};

//class members

  TrigHLTJetTLASelector::TrigHLTJetTLASelector (const std::string& name, ISvcLocator* svcLocator)
: AthReentrantAlgorithm (name, svcLocator)
{ 
   
}


StatusCode TrigHLTJetTLASelector::initialize()
{
  // This function will check that the properties were initialized properly
  // by job configuration.

  ATH_MSG_DEBUG("Initializing " << name() << "...");

  ATH_MSG_DEBUG("pT threshold for output jet collection: " << (float) m_jetPtThreshold );
  ATH_MSG_DEBUG("Maximum number of jets kept " << (int) m_maxNJets );

  //check that the handlers are correctly initialized
  CHECK( m_inputJetsKey.initialize() );
  CHECK( m_outputJetsKey.initialize() );

  if (m_maxNJets == 0) {
    ATH_MSG_ERROR("This algorithm will keep no jets (m_maxNJets property <=0), so it should not be running at all.");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


StatusCode TrigHLTJetTLASelector::execute (const EventContext& ctx) const
{

  ATH_MSG_DEBUG("Executing " << name() << ".");

  //get jets
  auto h_inJets = SG::makeHandle(m_inputJetsKey, ctx );
  auto h_outJets = SG::makeHandle(m_outputJetsKey, ctx );
  ATH_MSG_DEBUG("Retrieving jets from: " << h_inJets.key());
  ATH_MSG_DEBUG("Placing jets in: " << h_outJets.key());
  ATH_CHECK(h_inJets.isValid());
  const xAOD::JetContainer* inputJets = h_inJets.get();
  std::vector<const xAOD::Jet*> originalJets(inputJets->begin(), inputJets->end());

  // define the maximum number of jets we care about: either equivalent to m_maxNJets if smaller than size of vector, or keep all jets (in case of negative value)
  std::vector<const xAOD::Jet*>::iterator it_maxJetBound;

  if ((int)m_maxNJets > 0) it_maxJetBound = m_maxNJets < int(originalJets.size()) ? originalJets.begin()+m_maxNJets : originalJets.end();
  else it_maxJetBound = originalJets.end();

  // check the sort order of the input container is ok
  // use a partial sort to save some time
  std::partial_sort (originalJets.begin(), it_maxJetBound, originalJets.end(), DescendingEt());

  // get an iterator to the last element above the pT threshold (because we ordered the jets, this is the last jet we want)
  std::vector<const xAOD::Jet*>::iterator it_ptThresholdBound;
  it_ptThresholdBound = std::partition(originalJets.begin(), it_maxJetBound, HasPtAboveThreshold((float)m_jetPtThreshold)); 

  //make the output jet container
  ATH_CHECK( h_outJets.record (std::make_unique<xAOD::JetContainer>(),
        std::make_unique<xAOD::JetAuxContainer>() ) );


  //loop on all the jets from the beginning to the last jet we want, and put them the output jet collection
  //also, set a link to their parent jet
  for( auto it_jet=originalJets.begin(); it_jet!=it_ptThresholdBound; ++it_jet ) {
    xAOD::Jet* copiedJet = new xAOD::Jet(*(*it_jet));
    
    ATH_CHECK(setOriginalObjectLink(*(*it_jet),*copiedJet));

    h_outJets->push_back(copiedJet);
  }

  for (const xAOD::Jet* jet : *h_outJets) {
    ATH_MSG_DEBUG("Jet pT: " << jet->pt()); 
  }

  return StatusCode::SUCCESS;
}
