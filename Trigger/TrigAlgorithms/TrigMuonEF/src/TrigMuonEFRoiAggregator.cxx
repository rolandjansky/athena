/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cstdlib>
#include <cmath>
#include <algorithm>

#include "TrigMuonEFRoiAggregator.h"

#include "TrigNavigation/TriggerElement.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODMuon/Muon.h"

/**
 * Constructor - set up the algorithm
 */
TrigMuonEFRoiAggregator::TrigMuonEFRoiAggregator(const std::string & name, ISvcLocator* pSvcLocator) :
  HLT::AllTEAlgo(name, pSvcLocator)
{
  declareMonitoredVariable("nRoiIn", m_nRoiIn, IMonitoredAlgo::AutoClear);
  declareMonitoredVariable("nTrkIn", m_nTrkIn, IMonitoredAlgo::AutoClear);
}

/**
 * Destructor - clean up when done
 */
TrigMuonEFRoiAggregator::~TrigMuonEFRoiAggregator() {

}

/**
 * Initialize the algorithm
 */
HLT::ErrorCode TrigMuonEFRoiAggregator::hltInitialize() {

  return HLT::OK;
}//hltInitialize

/**
 * Finalize the algorithm
 */
HLT::ErrorCode TrigMuonEFRoiAggregator::hltFinalize() {

  return HLT::OK;
}//hltFinalize

HLT::ErrorCode TrigMuonEFRoiAggregator::hltExecute(std::vector< std::vector<HLT::TriggerElement*> >& inputTEtypes, unsigned int output) {  

  // init monitor variables "by hand" because this is an allTE algo
  beforeExecMonitors().ignore();

  // we run after FS instance - so should only be one inputTE
  if (inputTEtypes.size() != 1) {
    ATH_MSG_WARNING("Got more than one inputTE collections");
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }
  
  std::vector<HLT::TriggerElement*>& inputTEs = inputTEtypes.at(0);
  ATH_MSG_DEBUG( "Number of input TEs: " << inputTEs.size() );

  std::vector<HLT::TriggerElement*>::iterator itTE = inputTEs.begin();

  // would be better to use a view container or ConstDataVector
  // here to avoid the deep copy - but need to figure out how to
  // put this to the navigation
  xAOD::MuonContainer* outMuons = new xAOD::MuonContainer();
  xAOD::MuonAuxContainer muonAux;
  outMuons->setStore( &muonAux );
  
  m_nRoiIn = inputTEs.size();
  m_nTrkIn = 0;

  // loop over all ROIs (aka TEs)
  for ( ; itTE != inputTEs.end(); ++itTE ) {
    HLT::TriggerElement* inputTE = *itTE;
    // Some debug output:
    std::string label;
    TrigConf::HLTTriggerElement::getLabel (inputTE->getId(), label );
    ATH_MSG_DEBUG( "inputTE: \"" << label << "\" with ID " << inputTE->getId() );

    // Get xAOD Muon container linked to input TE
    const xAOD::MuonContainer* inputMuonCont=0;
    if(getFeature(inputTE, inputMuonCont)!=HLT::OK || !inputMuonCont) {
      ATH_MSG_DEBUG("no MuonContainer Feature found in TE " << inputTE->getId() );
      return HLT::MISSING_FEATURE;
    }

    ATH_MSG_DEBUG( "MuonContainer found with size " << inputMuonCont->size() );

    // loop on the muons within the RoI
    for(auto muon : *inputMuonCont) {
      ATH_MSG_DEBUG( "Looking at next muon, primaryTrackPartile = " << muon->primaryTrackParticle());
      ++m_nTrkIn;
      // deep copy the muon
      xAOD::Muon* muoncopy = new xAOD::Muon();
      outMuons->push_back( muoncopy );
      *muoncopy = *muon;

    } // loop over muons within the RoI
    
  } //loop over inputTEs

  // make output TE   
  HLT::TriggerElement* outputTE = addRoI(output);
  
  if ( outMuons->size() > 0 ) {
    outputTE->setActiveState(true);
    HLT::ErrorCode hltStatus = attachFeature(outputTE, outMuons, "MuonEFInfo");
    if( hltStatus != HLT::OK ) {
      std::string label;
      TrigConf::HLTTriggerElement::getLabel( outputTE->getId(), label );
      ATH_MSG_WARNING( "Failed to attach output muons to output TE \"" << label << "\" ID=" << outputTE->getId() );
      delete outMuons;
      return hltStatus;
    }
    ATH_MSG_DEBUG( "attached muon container with size: " << outMuons->size() );
  } else {
    outputTE->setActiveState(false);
    delete outMuons;
    outMuons = 0;
  }

  // fill monitor variables "by hand" because this is an allTE algo
  afterExecMonitors().ignore();
  
  return HLT::OK;  
}//hltExecute
