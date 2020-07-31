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
using TrigCompositeUtils::createAndStore;
using TrigCompositeUtils::roiString;

InputMakerForRoI:: InputMakerForRoI( const std::string& name, 
 ISvcLocator* pSvcLocator )    
: InputMakerBase( name, pSvcLocator ) {}


StatusCode  InputMakerForRoI::initialize() {
  ATH_MSG_DEBUG("Will produce output RoI collections: " << m_RoIs);
  ATH_CHECK( m_RoIs.initialize( SG::AllowEmpty ) );
  ATH_CHECK( m_roiTool.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode  InputMakerForRoI::execute( const EventContext& context ) const {
  ATH_MSG_DEBUG( "Executing " << name() << "..." );
  
  // create the output decisions from the input collections
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore( decisionOutputs(), context );
  ATH_CHECK(outputHandle.isValid());
  ATH_CHECK(decisionInputToOutput(context, outputHandle));
  ATH_MSG_DEBUG("Merging complete");

  if( outputHandle->size() == 0) {
    ATH_MSG_WARNING( "Have no decisions in output handle "<< outputHandle.key() << ". Handle is valid but container is empty. "
      << "Check why this InputMakerForRoI was unlocked by a Filter, if the Filter then gave it no inputs.");
  } else {
    ATH_MSG_DEBUG( "Have output " << outputHandle.key() << " with " << outputHandle->size() << " elements" );
  }

  // Find and link to the output Decision objects the ROIs to run over
  ATH_CHECK( m_roiTool->attachROILinks(*outputHandle, context) );

  if (m_RoIs.empty()) {
    ATH_MSG_DEBUG("No concrete output ROI collection required from this InputMaker.");
    return StatusCode::SUCCESS;
  }

  // Prepare Outputs
  std::unique_ptr<TrigRoiDescriptorCollection> outputRoIColl = std::make_unique<TrigRoiDescriptorCollection>();

  std::vector <ElementLink<TrigRoiDescriptorCollection> > RoIsFromDecision;  // used to check for duplicate RoIs 

  if (m_mergeIntoSuperRoI) {
    TrigRoiDescriptor* superRoI = new TrigRoiDescriptor();
    superRoI->setComposite(true);
    superRoI->manageConstituents(false);
    outputRoIColl->push_back(superRoI);
  }

  // loop over output decisions in container of outputHandle, collect RoIs to process
  for (const Decision* outputDecision : *outputHandle) { 

    if (!outputDecision->hasObjectLink(roiString(), ClassID_traits<TrigRoiDescriptorCollection>::ID())) {
      ATH_MSG_ERROR("No '" << roiString() << "'link was attached by the ROITool. Decision object dump:" << *outputDecision);
      return StatusCode::FAILURE;
    }
    const ElementLink<TrigRoiDescriptorCollection> roiEL = outputDecision->objectLink<TrigRoiDescriptorCollection>(roiString());
    ATH_CHECK(roiEL.isValid());

    // Avoid adding the same ROI multiple times: check if not in container, if not add it
    if ( find(RoIsFromDecision.begin(), RoIsFromDecision.end(), roiEL) == RoIsFromDecision.end() ) {
      RoIsFromDecision.push_back(roiEL); // just to keep track of which we have used 
      const TrigRoiDescriptor* roi = *roiEL;
      ATH_MSG_DEBUG("Found RoI:" <<*roi<<" FS="<<roi->isFullscan());

      if (m_mergeIntoSuperRoI) { // Append to the single superRoI
        outputRoIColl->back()->push_back( roi );
      } else { // Add individually
        TrigRoiDescriptor* newroi = new TrigRoiDescriptor(*roi); //use copy constructor
        outputRoIColl->push_back(newroi);
        ATH_MSG_DEBUG("Added RoI:" <<*newroi<<" FS="<<newroi->isFullscan());
      }
    } 

  } // loop over decisions      
  
  // Finally, record output
  ATH_MSG_DEBUG("Produced "<<outputRoIColl->size() <<" output RoIs");
  auto roi_outputHandle = SG::makeHandle(m_RoIs, context);
  ATH_CHECK( roi_outputHandle.record(std::move(outputRoIColl)) );


  return StatusCode::SUCCESS;
}
