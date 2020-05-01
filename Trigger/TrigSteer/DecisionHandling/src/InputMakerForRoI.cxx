/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "InputMakerForRoI.h"
#include <xAODTrigger/TrigCompositeAuxContainer.h>
#include <TrigSteeringEvent/TrigRoiDescriptorCollection.h>
#include <DecisionHandling/TrigCompositeUtils.h>
#include <vector>

using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::linkToPrevious;
using TrigCompositeUtils::getLinkToPrevious;
using TrigCompositeUtils::findLink;
using TrigCompositeUtils::createAndStore;

InputMakerForRoI:: InputMakerForRoI( const std::string& name, 
 ISvcLocator* pSvcLocator )    
: InputMakerBase( name, pSvcLocator ) {}


StatusCode  InputMakerForRoI::initialize() {
  ATH_MSG_DEBUG("Will produce output RoI collections: " << m_RoIs);
  CHECK( m_RoIs.initialize( SG::AllowEmpty ) );
  return StatusCode::SUCCESS;
}

StatusCode  InputMakerForRoI::execute( const EventContext& context ) const {
  ATH_MSG_DEBUG( "Executing " << name() << "..." );
  
  // create the output decisions from the input collections
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore( decisionOutputs(), context );
  ATH_CHECK(decisionInputToOutput(context, outputHandle));
  ATH_CHECK(outputHandle.isValid());
  
  // Prepare Outputs
  std::unique_ptr<TrigRoiDescriptorCollection> oneRoIColl = std::make_unique<TrigRoiDescriptorCollection>();

  // use also this:    ElementLinkVector<xAOD::MuonRoIContainer> getMuonRoILinks = obj->objectCollectionLinks<xAOD::MuonRoIContainer>("ManyMuonRoIs");
  std::vector <ElementLink<TrigRoiDescriptorCollection> > RoIsFromDecision;  // used to check for duplicate features linked to different inputHandles

  if( outputHandle->size() == 0) {
    ATH_MSG_WARNING( "Have no decisions in output handle "<< outputHandle.key() << ". Handle is valid but container is empty. "
      << "Check why this EventViewCreatorAlgorithm was unlocked by a Filter, if the Filter then gave it no inputs.");
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG( "Got output "<< outputHandle.key()<<" with " << outputHandle->size() << " elements" );
  // loop over output decisions in container of outputHandle, follow link to inputDecision
  for ( auto outputDecision : *outputHandle){ 
    ElementLinkVector<DecisionContainer> inputLinks = getLinkToPrevious(outputDecision);
    for (auto input: inputLinks){
      const Decision* inputDecision = *input;
      auto roiELInfo = findLink<TrigRoiDescriptorCollection>( inputDecision, m_roisLink.value() );
      auto roiEL = roiELInfo.link;
      ATH_CHECK( roiEL.isValid() );

      // avoid adding the same feature multiple times: check if not in container, if not add it
      if ( find(RoIsFromDecision.begin(), RoIsFromDecision.end(), roiEL) == RoIsFromDecision.end() ) {
        RoIsFromDecision.push_back(roiEL); // just to keep track of which we have used 
        const TrigRoiDescriptor* roi = *roiEL;
        ATH_MSG_DEBUG("Found RoI:" <<*roi<<" FS="<<roi->isFullscan());
        //make a new one:
        TrigRoiDescriptor* newroi= new TrigRoiDescriptor(*roi); //use copy constructor
        oneRoIColl->push_back(newroi);
        ATH_MSG_DEBUG("Added RoI:" <<*newroi<<" FS="<<newroi->isFullscan());
      }
    } // loop over previous input links           
  } // loop over decisions      
  
  
  // Finally, record output
  if (m_RoIs.empty()) {
    ATH_MSG_DEBUG("No concrete output ROI collection required from this InputMaker.");
  } else {
    ATH_MSG_DEBUG("Produced "<<oneRoIColl->size() <<" output RoIs");
    auto roi_outputHandle = SG::makeHandle(m_RoIs, context);
    ATH_CHECK( roi_outputHandle.record(std::move(oneRoIColl)) );
  }

  // call base class helper method to print some debug messages summarising the content of the outputHandles.
  if (msgLvl(MSG::DEBUG)) {
    debugPrintOut(context, outputHandle);
  }

  return StatusCode::SUCCESS;
}
