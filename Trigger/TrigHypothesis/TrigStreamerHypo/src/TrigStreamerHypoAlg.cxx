/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigStreamerHypoAlg.h"

#include "Gaudi/Property.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

using namespace TrigCompositeUtils;

TrigStreamerHypoAlg::TrigStreamerHypoAlg( const std::string& name, 
				      ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {}


StatusCode TrigStreamerHypoAlg::initialize() {

  CHECK( m_hypoTools.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode TrigStreamerHypoAlg::execute( const EventContext& context ) const {  

  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto h_prevDecisions = SG::makeHandle( decisionInput(), context );
  ATH_CHECK( h_prevDecisions.isValid() );
  ATH_MSG_DEBUG( "Running with "<< h_prevDecisions->size() <<" previous decisions");

  // Make a new Decisions container which will contain the new Decision object created by this hypo.
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  DecisionContainer* newDecisions = outputHandle.ptr();

  // Struct to pass info on to the HypoTools
  std::vector< ITrigStreamerHypoTool::HypoInfo > hypoInfo;

  for (const Decision* previousDecision : *h_prevDecisions ) {
    // Create output Decision object, link it to prevDecision.
    Decision* newDecision = newDecisionIn(newDecisions, previousDecision, hypoAlgNodeName(), context);

    // Obligatory link to feature. Re-using the initial ROI.
    // If this happens to be a FullScan ROI, then the following ComboHypo will also pass this leg through without cuts
    //LinkInfo<TrigRoiDescriptorCollection> featureLinkInfo = findLink<TrigRoiDescriptorCollection>(previousDecision, initialRoIString());
    //newDecision->setObjectLink(featureString(), featureLinkInfo.link);
    if (m_featureIsROI) {
       // Obligatory link to feature. Re-using the initial ROI.
       // If this happens to be a FullScan ROI, then the following ComboHypo will also pass this leg through without cuts
       LinkInfo<TrigRoiDescriptorCollection> featureLinkInfo = findLink<TrigRoiDescriptorCollection>(previousDecision, initialRoIString());
       newDecision->setObjectLink(featureString(), featureLinkInfo.link);
     } else {
       // Obligatory link to feature. Re-use previous Step's feature.
       uint32_t key = 0;
       uint32_t clid = 0;
       uint16_t index = 0;
       const Decision* source = nullptr; // Unused
       ATH_CHECK( typelessFindLink(previousDecision, featureString(), key, clid, index, source) );
       newDecision->typelessSetObjectLink(featureString(), key, clid, index);
     }

    hypoInfo.emplace_back(newDecision, previousDecision);
  }

  for (const auto& tool: m_hypoTools) {
    ATH_MSG_DEBUG( "About to decide for " << tool->name() );
    ATH_CHECK(tool->decide(hypoInfo)); // The StreamerHypoTool only contains logical-flow checks
  }

  // Common debug printing
  ATH_CHECK(hypoBaseOutputProcessing(outputHandle));

  return StatusCode::SUCCESS;
}
