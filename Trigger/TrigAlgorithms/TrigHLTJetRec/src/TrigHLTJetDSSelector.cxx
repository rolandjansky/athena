/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigHLTJetDSSelector:Trigger algorithm that reads a jet collection from
// an incoming trigger element, selects them and attaches a stripped 
// jet collection to the outgoing trigger element.

#include <vector>
#include <string>
#include "TrigHLTJetRec/TrigHLTJetDSSelector.h"
#include "JetRec/JetRecTool.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetTrigAuxContainer.h"


///Helpers

//this function also exists in TrigEFJetHypo.cxx
struct DescendingEt:std::binary_function<const xAOD::Jet*, const xAOD::Jet*, bool> {

  bool operator()(const xAOD::Jet* l, const xAOD::Jet* r)  const {
    return l->p4().Et() > r->p4().Et();
  }

};

//question: deprecated in C++11?
struct HasPtAboveThreshold {

  HasPtAboveThreshold(double thresholdPt) : thresholdPt(thresholdPt) {}
  double thresholdPt;

  bool operator()(const xAOD::Jet* myjet)  const {
    return myjet->p4().Pt() > thresholdPt;
  }

};


///Class member functions

TrigHLTJetDSSelector::TrigHLTJetDSSelector(const std::string& name, 
    ISvcLocator* pSvcLocator ):
  HLT::FexAlgo( name, pSvcLocator ) {
    declareProperty( "jetCollectionName", m_jetCollectionName);
    declareProperty( "jetPtThreshold", m_jetPtThreshold);
    declareProperty( "maxNJets", m_maxNJets);
  }


TrigHLTJetDSSelector::~TrigHLTJetDSSelector() { }


HLT::ErrorCode TrigHLTJetDSSelector::hltInitialize() {

  /* Nothing to retrieve for now
  */

  ATH_MSG_INFO("Initializing " << name() << "...");

  ATH_MSG_DEBUG("Name of output jet collection: " << m_jetCollectionName );
  ATH_MSG_DEBUG("pT threshold for output jet collection: " << m_jetCollectionName );
  ATH_MSG_DEBUG("Maximum number of jets kept " << m_maxNJets );


  return HLT::OK;
}


HLT::ErrorCode TrigHLTJetDSSelector::hltFinalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return HLT::OK;
}

HLT::ErrorCode TrigHLTJetDSSelector::hltExecute(const HLT::TriggerElement* inputTE,
    HLT::TriggerElement* outputTE) {  

  ATH_MSG_VERBOSE("Executing " << name() << "...");
  ATH_MSG_DEBUG("inputTE->getId(): " << inputTE->getId());
  ATH_MSG_DEBUG("outputTE->getId(): " << outputTE->getId());


  // get JetCollections from the trigger element:
  //--------------------------------------------------

  // xAOD conversion
  const xAOD::JetContainer* outJets(0);
  HLT::ErrorCode ec = getFeature(outputTE, outJets);

  if(ec!=HLT::OK) {
    ATH_MSG_WARNING("Failed to get JetCollection");
    return ec;
  } else {
    ATH_MSG_DEBUG("Obtained JetContainer");
  }

  // check the pointer is valid
  if(outJets == 0){
    ATH_MSG_WARNING("Jet collection pointer is null");
    return HLT::ERROR;
  }

  // make a local copy of the input jets from the TE for easier iteration
  std::vector<const xAOD::Jet*> originalJets(outJets->begin(), outJets->end());

  /*ATH_MSG_INFO("List of original jets, before sorting");
  for ( unsigned int iJet=0; iJet < originalJets.size(); iJet++ ) {
    const xAOD::Jet* thisJet = originalJets[iJet];
    ATH_MSG_INFO( "jet pT #1 = " << thisJet->pt() );
  }*/
  
  std::vector<const xAOD::Jet*>::iterator it_maxJetBound;
  
  //make sure we don't go out of bounds in the following vectors
  if (m_maxNJets > 0) it_maxJetBound = m_maxNJets < int(originalJets.size()) ? originalJets.begin()+m_maxNJets : originalJets.end();
  //sanity check
  else if (m_maxNJets == 0) {
    ATH_MSG_DEBUG("This algorithm will keep no jets, so it should not be running at all.");
    outputTE->setActiveState(false);
    return HLT::OK;
  }
  //if we got here, we do not have a constraint on a number of jets: -1 means keep all jets
  else it_maxJetBound = originalJets.end();

  // check the sort order of the input container is ok
  // use a partial sort to save some time
  std::partial_sort (originalJets.begin(), it_maxJetBound, originalJets.end(), DescendingEt());
  
  // use std::partition to select the jets above threshold
  std::vector<const xAOD::Jet*>::iterator ptThresholdBound;
  ptThresholdBound = std::partition(originalJets.begin(), it_maxJetBound, HasPtAboveThreshold(m_jetPtThreshold)); 

  xAOD::JetContainer* outputJets = new xAOD::JetContainer();
  xAOD::JetTrigAuxContainer outputJetsAux;
  outputJets->setStore(&outputJetsAux);

  for(std::vector<const xAOD::Jet*>::iterator K=originalJets.begin(); K!=ptThresholdBound && K!=it_maxJetBound; ++K){
	ATH_MSG_DEBUG("Initial jet pT = " << (*K)->pt());
	xAOD::Jet* newJet = new xAOD::Jet(*(*K));
  	outputJets->push_back(newJet);
  }

  std::size_t njets = outputJets->size();

  // check the collection contains jets  
  if(njets < 2){
    ATH_MSG_DEBUG("JetCollection contains <2 jets, not running algorithm");
    //set trigger element to not active so that chain doesn't go any further
    outputTE->setActiveState(false);
    // cleaning up the initial vector of jets
    delete outputJets;
    return HLT::OK;
  } else {
    ATH_MSG_DEBUG("JetCollection contains " << njets << "jets");
  } 

  /*ATH_MSG_INFO("List of output jets");
  for ( unsigned int iJet=0; iJet < outputJets->size(); iJet++ ) {
    const xAOD::Jet* thisJet = outputJets->at(iJet);
    ATH_MSG_INFO("jet pT #3 = " << thisJet->pt());
  }*/

  ATH_MSG_DEBUG(outputJets->size() << " jets kept");
  ec = attachJetCollection(outputTE, outputJets, m_jetCollectionName);
  
  return ec;

  }


  HLT::ErrorCode
    TrigHLTJetDSSelector::attachJetCollection(HLT::TriggerElement* outputTE,
        const xAOD::JetContainer* j_container, std::string j_label){

      std::string ignored_key; // ignored output parameter

      HLT::ErrorCode hltStatus = 
        recordAndAttachFeature(outputTE, j_container, ignored_key, j_label);

      // cleanup
      if (hltStatus != HLT::OK) {
        // this is unrecoverable. report error, and do not try to clean up.
        ATH_MSG_ERROR("Failed to attach xAOD::JetContainer into outputTE- status"
            << hltStatus);
      } else {
        ATH_MSG_DEBUG("Attached xAOD::JetContainer into outputTE");
      }
      return hltStatus;
    }

