/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigLArNoiseBurstAlg.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

using namespace TrigCompositeUtils;

TrigLArNoiseBurstAlg::TrigLArNoiseBurstAlg( const std::string& name, 
                      ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {}


StatusCode TrigLArNoiseBurstAlg::initialize() {

  ATH_CHECK( m_cellContainerKey.initialize() );
  ATH_CHECK( m_knownBadFEBsVecKey.initialize() );
  ATH_CHECK( m_knownMNBFEBsVecKey.initialize() );
  ATH_CHECK( m_hypoTools.retrieve() );
  
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
  std::vector<ITrigLArNoiseBurstHypoTool::CaloCellNoiseInfo> toolInput;

  ATH_CHECK( previousDecisionsHandle->size() == 1 );
  const auto previousDecision = previousDecisionsHandle->at(0);
    //get RoI  
    auto roiELInfo = findLink<TrigRoiDescriptorCollection>( previousDecision, initialRoIString() );
    
    ATH_CHECK( roiELInfo.isValid() );
    const TrigRoiDescriptor* roi = *(roiELInfo.link);

    // get cells
    SG::ReadHandle<CaloCellContainer> cellsHandle(m_cellContainerKey, context);
    ATH_CHECK( cellsHandle.isValid() );
    ATH_MSG_DEBUG ( "Cluster handle size: " << cellsHandle->size() << "..." );

    // necessary conditions
    std::set<unsigned int> bf;
    std::vector<HWIdentifier> MNBfeb;
    SG::ReadCondHandle<LArBadFebCont> badHdl(m_knownBadFEBsVecKey, context);
    const LArBadFebCont* badCont=*badHdl;
    if(badCont) {
       for(LArBadFebCont::BadChanVec::const_iterator i = badCont->begin(); i!=badCont->end(); i++) {
          bf.insert(i->first);
       }
    }
    
    SG::ReadCondHandle<LArBadFebCont> MNBHdl(m_knownMNBFEBsVecKey, context);
    const LArBadFebCont* MNBCont=*MNBHdl;
    if(MNBCont) {
       for(LArBadFebCont::BadChanVec::const_iterator i = MNBCont->begin(); i!=MNBCont->end(); i++) {
          MNBfeb.push_back(HWIdentifier(i->first));
       } 
    }

    // Get new output decision, child of previousDecision
    auto d = newDecisionIn (decisions, previousDecision, hypoAlgNodeName(), context );
    
    // create summary struct
    toolInput.emplace_back( d, roi, cellsHandle.cptr(), &bf, &MNBfeb, previousDecision );
    

    // link the cluster
    { 
      auto cell = ElementLink<CaloCellContainer>( *cellsHandle, 0 );
      ATH_CHECK( cell.isValid() );
      d->setObjectLink( featureString(),  cell );
    }
    
  ATH_MSG_DEBUG( "Found "<<toolInput.size()<<" inputs to tools");

   
  for ( auto& tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( toolInput ) );
  }
 
  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );

  return StatusCode::SUCCESS;
}
