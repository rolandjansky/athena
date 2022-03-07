/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// TrackCaloClusterAlgs includes
#include "TrackCaloClusterRecAlgs/TrackCaloClusterAlg.h"

#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "xAODPFlow/TrackCaloClusterAuxContainer.h"
#include "xAODPFlow/FlowElementContainer.h"
#include "xAODPFlow/FlowElementAuxContainer.h"

#include "StoreGate/ReadDecorHandle.h"
#include "StoreGate/WriteDecorHandle.h"



TrackCaloClusterAlg::TrackCaloClusterAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ) { }
  
  
// *****************************************************
StatusCode TrackCaloClusterAlg::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");    
  ATH_CHECK(m_tccTools.retrieve());
  ATH_MSG_VERBOSE("TCCAlg::init got tcc tools");
  ATH_CHECK(m_tccInfoHandle.initialize());
  ATH_MSG_VERBOSE("TCCAlg::init got tcc info handle");
  ATH_CHECK(m_outputTCCHandle.initialize());
  ATH_MSG_VERBOSE("TCCAlg::init got output tcc handle");
  ATH_CHECK(m_copiedTCCHandle.initialize(! m_copiedTCCHandle.empty()) );
  ATH_MSG_VERBOSE("TCCAlg::init got copied tcc handle");
  return StatusCode::SUCCESS;
}

// *****************************************************
StatusCode TrackCaloClusterAlg::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  SG::ReadHandle<TrackCaloClusterInfo> tccInfo(m_tccInfoHandle);
  
  // Create containers
  SG::WriteHandle<xAOD::FlowElementContainer> tccContainer(m_outputTCCHandle) ;
  ATH_CHECK( tccContainer.record (std::make_unique<xAOD::FlowElementContainer>(),
				  std::make_unique<xAOD::FlowElementAuxContainer>()) );
  ATH_MSG_DEBUG( "Recorded TrackCaloClusterContainer with key: " << m_outputTCCHandle.key()  );
  
  // Optionnaly recopy an existing container.
  // This option is mainly to enable functionalities of the original alg. 
  if(! m_copiedTCCHandle.empty() ){
    SG::ReadHandle<xAOD::FlowElementContainer> tccToCopy(m_copiedTCCHandle) ;
    for(const xAOD::FlowElement* tobecopied: *tccToCopy){
      xAOD::FlowElement* tcc = new xAOD::FlowElement;
      tccContainer->push_back(tcc);
      *tcc = *tobecopied;
    }
  }


  // Loop over each TCC tool  ...
  for(const ToolHandle<ITrackCaloClusterTool> & tool : m_tccTools){
    // .. and have it fill  tccContainer
    ATH_MSG_DEBUG("executing "<<tool.name() );
    StatusCode sc=tool->fillTCC(tccContainer.ptr(), *tccInfo);
    if(sc.isFailure()){
      ATH_MSG_ERROR(" TCC tool "<< tool->name() << "  failed. "); return sc;
    }
    ATH_MSG_DEBUG("executed "<<tool.name() << " num object= "<< tccContainer->size() );
  }
         
  return StatusCode::SUCCESS;
}
