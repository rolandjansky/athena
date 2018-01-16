/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_LocReco/AFP_SIDLocRecoTool.h"

#include "xAODForward/AFPTrackContainer.h"
#include "xAODForward/AFPTrackAuxContainer.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h" 

using namespace std;

AFP_SIDLocRecoTool::AFP_SIDLocRecoTool(const std::string &type,
				       const std::string &name,
				       const IInterface *parent)
  : AthAlgTool(type, name, parent),
    m_recoClusters("AFPSiClusterTool")
{
  ATH_MSG_DEBUG("begin AFP_SIDLocRecoTool::AFP_SIDLocRecoTool");

  declareInterface< IAFPSiDLocRecoTool >(this);

  declareProperty("recoTools", m_recoToolsHandles, "Vector of track reconstruction algorithms");
  declareProperty("clusterTool", m_recoClusters, "Tool to create clusters from pixel hits");

  ATH_MSG_DEBUG("end AFP_SIDLocRecoTool::AFP_SIDLocRecoTool");
}


StatusCode AFP_SIDLocRecoTool::initialize()
{
  ATH_MSG_DEBUG("begin AFP_SIDLocRecoTool::initialize()");

  StatusCode sc;

  // --- Prepare track reconstruction tools initialised in python steering cards ---
  if (m_recoToolsHandles.size() != 0) {
    // prepare size of the pairs of tools and output containers
    m_recoToolsAndContainers.resize (m_recoToolsHandles.size());
    int recoToolsAndContainersID = 0;
    
    // loop over tools
    for (ToolHandle<IAFPSiDLocRecoTrackAlgTool>& recoTool : m_recoToolsHandles) {
      // retrieve tools
      sc=recoTool.retrieve(); 
      if (sc.isFailure()) {
	ATH_MSG_ERROR("Failed to retrieve AlgTool " << m_recoToolsHandles);
	return sc;
      }

      // initialise vector of pairs of tools and output containers
      m_recoToolsAndContainers[recoToolsAndContainersID].first = &recoTool;
      m_recoToolsAndContainers[recoToolsAndContainersID].second = &( m_trackContainersWithNames[ recoTool->outputContainerName() ] ); // if the entry in map does not exist it is automatically created
      ++recoToolsAndContainersID;
    }
  }
  else {
    ATH_MSG_ERROR("No track reconstruction tools set. Aborting.");
    return StatusCode::FAILURE;
  }

  //  ATH_MSG_INFO(m_recoToolsHandles);

  // --- Prepare clustering tools ---
  sc=m_recoClusters.retrieve(); //Dot, not asterisk! This is a method of the ToolHandle, not of the tool it holds.
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve recoClusters " << m_recoClusters);
    return sc;
  }

  ATH_MSG_INFO(m_recoClusters);

  ATH_MSG_DEBUG("end AFP_SIDLocRecoTool::initialize()");
  return StatusCode::SUCCESS;
}

StatusCode AFP_SIDLocRecoTool::execute()
{
  ATH_MSG_DEBUG("begin AFP_SIDLocRecoTool::execute()");

  // reconstruct clusters
  if (m_recoClusters->clusterHits().isFailure() ) {
    ATH_MSG_WARNING ("Pixel clusters creation failed. Aborting track reconstruction.");
    return StatusCode::SUCCESS;
  }

  // make output containers
  if (makeNewTrackContainers().isFailure()) {
    ATH_MSG_WARNING ("Failed to create and save new track containers. Aborting track reconstruction.");
    return StatusCode::SUCCESS;
  }

  // run reconstruction tools
  for (std::pair<ToolHandle<IAFPSiDLocRecoTrackAlgTool>*, xAOD::AFPTrackContainer**>& toolAndContainer : m_recoToolsAndContainers) {
    ToolHandle<IAFPSiDLocRecoTrackAlgTool>& tool = *(toolAndContainer.first);
    xAOD::AFPTrackContainer* const container = *(toolAndContainer.second);

    if ( tool->reconstructTracks( container ).isFailure() )
      ATH_MSG_WARNING ("Failed to reconstruct tracks with algorithm="<<tool);
  }

  ATH_MSG_DEBUG("end AFP_SIDLocRecoTool::execute()");
  return StatusCode::SUCCESS;
}

StatusCode AFP_SIDLocRecoTool::finalize()
{
  return StatusCode::SUCCESS;
}


StatusCode AFP_SIDLocRecoTool::makeNewTrackContainers ()
{
  for (pair<const string, xAOD::AFPTrackContainer*>& containerWithName : m_trackContainersWithNames) {
    // get storeGate name of the track container
    const string& storeGateName = containerWithName.first;

    // create track container and aux storage and save to storeGate
    xAOD::AFPTrackContainer* container = new xAOD::AFPTrackContainer();
    if (evtStore()->record(container, storeGateName).isFailure() ) {
      ATH_MSG_WARNING("Failed to record track container.");
      return StatusCode::SUCCESS;
    }
    
    xAOD::AFPTrackAuxContainer* auxContainer = new xAOD::AFPTrackAuxContainer();
    if (evtStore()->record(auxContainer, storeGateName + "Aux.").isFailure() ) {
      ATH_MSG_WARNING("Failed to record track aux container.");
      return StatusCode::SUCCESS;
    }
    container->setStore(auxContainer);

    // assign new container to m_trackContainerswithnames
    containerWithName.second = container;
  }

  return StatusCode::SUCCESS;
}
