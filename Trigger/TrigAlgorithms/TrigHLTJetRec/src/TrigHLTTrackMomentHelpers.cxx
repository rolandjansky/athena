/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigHLTTrackMomentHelpers:Trigger algorithm that reads FTK primary vertices and tracks from
// an incoming trigger element and gives them fixed SG key aliases that the track vertex association (tva) and track moment tools are configured with.
// The tva tool is executed here. 

#include "TrigHLTJetRec/TrigHLTTrackMomentHelpers.h"
#include <string>
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
//#include "TrackVertexAssociationTool/TightTrackVertexAssociationTool.h"
#include "JetRecTools/TrackVertexAssociationTool.h"
#include "xAODCore/ShallowCopy.h"

TrigHLTTrackMomentHelpers::TrigHLTTrackMomentHelpers(const std::string& name, 
                                           ISvcLocator* pSvcLocator):
  HLT::FexAlgo(name, pSvcLocator) {
  declareProperty("tvassocTool", m_tvassocTool);
  declareProperty("trackSGkey", m_trackSGKey);
  declareProperty("primVtxSGkey", m_primVtxSGKey);
}


TrigHLTTrackMomentHelpers::~TrigHLTTrackMomentHelpers() { }


HLT::ErrorCode TrigHLTTrackMomentHelpers::hltInitialize() {
  /* Retrieve the tva tool. The python configuration code ensures that ToolSvc
     can access this tool. */

 ATH_MSG_INFO("Initializing " << name() << "...");

 ATH_MSG_INFO("Retrieving tools...");
 auto sc = m_tvassocTool.retrieve();

 if (sc.isSuccess()) {
   ATH_MSG_INFO("Retrieved  the tvassocTool "
                <<  m_tvassocTool->name());
  }else{
   ATH_MSG_ERROR("Unable to retrieve the tvassocTool.");
   return HLT::ERROR;
 }

  ATH_MSG_INFO("Tool retrieval completed.");
  
  ATH_MSG_INFO("    " << m_tvassocTool.name());
  m_tvassocTool->print();
  
  ATH_MSG_INFO("  FTK Track storegate key: " << m_trackSGKey);
  ATH_MSG_INFO("  FTK Primary Vertex storegate key: " << m_primVtxSGKey);
  
  return HLT::OK;
}


HLT::ErrorCode TrigHLTTrackMomentHelpers::hltFinalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return HLT::OK;
}

HLT::ErrorCode
TrigHLTTrackMomentHelpers::hltExecute(const HLT::TriggerElement* inputTE,
                                 HLT::TriggerElement* outputTE) {  

  ATH_MSG_VERBOSE("Executing " << name() << "...");
  
  ATH_MSG_DEBUG("outputTE->getId(): " << outputTE->getId());
  ATH_MSG_DEBUG("inputTE->getId(): " << inputTE->getId());


  /*** retrieving FTK primary vertices from incoming TE ****/
  const xAOD::VertexContainer* vertexContainer = nullptr;
  
  auto hltStatus = getFeature(outputTE, vertexContainer, "HLT_PrimVertexFTK"); 
  if (hltStatus == HLT::OK) {
        ATH_MSG_DEBUG("Retrieved the FTK primary vertex container at address " << vertexContainer); 
  }
  else {
	ATH_MSG_ERROR("Failed to retrieve the input container");
	return HLT::ERROR;
  }
 
  /*** FTK Primary Vertices given a fixed alias, to be used by jet modifier and tva tools ***/
  if ( evtStore()->setAlias(vertexContainer, m_primVtxSGKey).isFailure()) {
    ATH_MSG_ERROR("Could not set alias for FTK vertexContainer with key" <<
                 m_primVtxSGKey);
    return HLT::ERROR;
  }
 
  /*** retrieving FTK tracks from incoming TE, only used by tva tool here. ****/
  const xAOD::TrackParticleContainer* trackContainer = nullptr;
  
  hltStatus = getFeature(outputTE, trackContainer); 
  if (hltStatus == HLT::OK) {
        ATH_MSG_DEBUG("Retrieved the FTK track container at address " << trackContainer); 
  }
  else {
	ATH_MSG_ERROR("Failed to retrieve the track container.");
	return HLT::ERROR;
  }

  /*** FTK tracks given SG key alias that tva tool configured with. ***/
  if ( evtStore()->setAlias(trackContainer, m_trackSGKey).isFailure()) {
    ATH_MSG_ERROR("Could not set alias for FTK trackContainer with key" <<
                 m_trackSGKey);
    return HLT::ERROR;
  }

  /*** now do track vertex association ***/
  ATH_MSG_DEBUG("Executing tool " << m_tvassocTool->name());
  
  int tvstatus = m_tvassocTool->execute();  

  ATH_MSG_DEBUG("tvstatus is "<<tvstatus);
  if (tvstatus !=0) return HLT::ERROR;
 
  return HLT::OK;
}

