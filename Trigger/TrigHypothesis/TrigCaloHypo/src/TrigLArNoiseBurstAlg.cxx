/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigLArNoiseBurstAlg.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "hltinterface/IInfoRegister.h"
#include "hltinterface/ContainerFactory.h"

#include <boost/property_tree/ptree.hpp>

using namespace TrigCompositeUtils;

TrigLArNoiseBurstAlg::TrigLArNoiseBurstAlg( const std::string& name, 
                      ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ){}


StatusCode TrigLArNoiseBurstAlg::initialize() {

  ATH_CHECK( m_hypoTools.retrieve() );
  ATH_CHECK( m_noiseBurstDetected.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode TrigLArNoiseBurstAlg::execute( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );

  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  ATH_CHECK( previousDecisionsHandle.isValid() );
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions");

  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto decisions = outputHandle.ptr();

  // input for decision
  std::vector<ITrigLArNoiseBurstHypoTool::FlagNoiseInfo> toolInput;

  ATH_CHECK( previousDecisionsHandle->size() == 1 );
  const auto previousDecision = previousDecisionsHandle->at(0);
  //get RoI  
  auto roiELInfo = findLink<TrigRoiDescriptorCollection>( previousDecision, initialRoIString() );
    
  ATH_CHECK( roiELInfo.isValid() );
  const TrigRoiDescriptor* roi = *(roiELInfo.link);

  SG::ReadHandle<bool> flagHandle(m_noiseBurstDetected, context);
  ATH_CHECK( flagHandle.isValid() );

  // Get new output decision, child of previousDecision
  auto d = newDecisionIn (decisions, previousDecision, hypoAlgNodeName(), context );
    
  unsigned int flag(*flagHandle); // to get information outside
  // create summary struct
  toolInput.emplace_back( d, flag, roi, previousDecision );
  
  ATH_MSG_DEBUG( "Found "<<toolInput.size()<<" inputs to tools");
  for ( auto& tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( toolInput ) );
  }
  
  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );

  return StatusCode::SUCCESS;
}
