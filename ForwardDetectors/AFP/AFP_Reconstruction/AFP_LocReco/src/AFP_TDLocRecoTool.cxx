/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_LocReco/AFP_TDLocRecoTool.h"

#include "xAODForward/AFPToFTrackContainer.h"
#include "xAODForward/AFPToFTrackAuxContainer.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h" 

using namespace std;

AFP_TDLocRecoTool::AFP_TDLocRecoTool(const std::string &type,
				     const std::string &name,
				     const IInterface *parent)
  : AthAlgTool(type, name, parent)
{
  ATH_MSG_DEBUG("begin AFP_TDLocRecoTool::AFP_TDLocRecoTool");

  declareInterface< IAFPTDLocRecoTool >(this);

  declareProperty("recoTools", m_recoToolsHandles, "Vector of track reconstruction algorithms");

  ATH_MSG_DEBUG("end AFP_TDLocRecoTool::AFP_TDLocRecoTool");
}


StatusCode AFP_TDLocRecoTool::initialize()
{
  ATH_MSG_DEBUG("begin AFP_TDLocRecoTool::initialize()");

  StatusCode sc;

  // --- Prepare track reconstruction tools initialised in python steering cards ---
  if (m_recoToolsHandles.size() != 0) {
    // prepare size of the pairs of tools and output containers
    m_recoToolsAndContainers.resize (m_recoToolsHandles.size());
    int recoToolsAndContainersID = 0;
    
    // loop over tools
    for (ToolHandle<IAFPTDLocRecoTrackAlgTool>& recoTool : m_recoToolsHandles) {
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


  ATH_MSG_DEBUG("end AFP_TDLocRecoTool::initialize()");
  return StatusCode::SUCCESS;
}

StatusCode AFP_TDLocRecoTool::execute()
{
  ATH_MSG_DEBUG("begin AFP_TDLocRecoTool::execute()");

  // make output containers
  if (makeNewTrackContainers().isFailure()) {
    ATH_MSG_WARNING ("Failed to create and save new track containers. Aborting track reconstruction.");
    return StatusCode::SUCCESS;
  }

  // run reconstruction tools
  for (std::pair<ToolHandle<IAFPTDLocRecoTrackAlgTool>*, xAOD::AFPToFTrackContainer**>& toolAndContainer : m_recoToolsAndContainers) {
    ToolHandle<IAFPTDLocRecoTrackAlgTool>& tool = *(toolAndContainer.first);
    xAOD::AFPToFTrackContainer* const container = *(toolAndContainer.second);

    if ( tool->reconstructTracks( container ).isFailure() )
      ATH_MSG_WARNING ("Failed to reconstruct ToF tracks with algorithm="<<tool);
  }

  ATH_MSG_DEBUG("end AFP_TDLocRecoTool::execute()");
  return StatusCode::SUCCESS;
}

StatusCode AFP_TDLocRecoTool::finalize()
{
  return StatusCode::SUCCESS;
}


StatusCode AFP_TDLocRecoTool::makeNewTrackContainers ()
{
  for (pair<const string, xAOD::AFPToFTrackContainer*>& containerWithName : m_trackContainersWithNames) {
    // get storeGate name of the track container
    const string& storeGateName = containerWithName.first;

    // create track container and aux storage and save to storeGate
    xAOD::AFPToFTrackContainer* container = new xAOD::AFPToFTrackContainer();
    if (evtStore()->record(container, storeGateName).isFailure() ) {
      ATH_MSG_WARNING("Failed to record track container.");
      return StatusCode::SUCCESS;
    }
    
    xAOD::AFPToFTrackAuxContainer* auxContainer = new xAOD::AFPToFTrackAuxContainer();
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
