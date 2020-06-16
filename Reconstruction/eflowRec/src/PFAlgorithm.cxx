/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "eflowRec/PFAlgorithm.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

PFAlgorithm::PFAlgorithm(const std::string& name,  ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator), m_IPFSubtractionTools(this), m_IPFBaseTools(this)  
{
  declareProperty( "SubtractionToolList",  m_IPFSubtractionTools, "List of Private Subtraction IPFSubtractionTools" );
  declareProperty( "BaseToolList",  m_IPFBaseTools, "List of Private IPFBaseTools" );
}

StatusCode PFAlgorithm::initialize(){

  ATH_CHECK(m_IPFClusterSelectorTool);
  ATH_CHECK(m_IPFSubtractionTools.retrieve());
  ATH_CHECK( m_IPFBaseTools.retrieve());
  
  ATH_CHECK(m_eflowRecTracksReadHandleKey.initialize());

  ATH_CHECK(m_eflowRecClustersWriteHandleKey.initialize());
  ATH_CHECK(m_eflowCaloObjectsWriteHandleKey.initialize());
  ATH_CHECK(m_caloClustersWriteHandleKey.initialize());

  if (!m_monTool.empty()) ATH_CHECK(m_monTool.retrieve());

  printTools();

  return StatusCode::SUCCESS;
}

StatusCode PFAlgorithm::execute(){
  // Define monitored quantities
  auto t_exec = Monitored::Timer<std::chrono::milliseconds>( "TIME_execute" );
  auto t_subtract = Monitored::Timer<std::chrono::milliseconds>( "TIME_subtract" );
  auto N_efrTracks = Monitored::Scalar( "N_efrTracks", 0 );
  auto N_efrClusters = Monitored::Scalar( "N_efrClusters", 0 );

  ATH_MSG_DEBUG("Executing");

  SG::WriteHandle<eflowCaloObjectContainer> eflowCaloObjectsWriteHandle(m_eflowCaloObjectsWriteHandleKey);
  ATH_CHECK(eflowCaloObjectsWriteHandle.record(std::make_unique<eflowCaloObjectContainer>()));
  eflowCaloObjectContainer* theElowCaloObjectContainer = eflowCaloObjectsWriteHandle.ptr();

  SG::WriteHandle<xAOD::CaloClusterContainer> caloClustersWriteHandle(m_caloClustersWriteHandleKey);
  ATH_CHECK(caloClustersWriteHandle.record(std::make_unique<xAOD::CaloClusterContainer>(),std::make_unique<xAOD::CaloClusterAuxContainer>()));
  ATH_MSG_DEBUG("CaloClusterWriteHandle has container of size" << caloClustersWriteHandle->size());

  SG::ReadHandle<eflowRecTrackContainer> eflowRecTracksReadHandle(m_eflowRecTracksReadHandleKey);
  eflowRecTrackContainer localEFlowRecTrackContainer(*eflowRecTracksReadHandle.ptr());

  /* Record the eflowRecCluster output container */
  SG::WriteHandle<eflowRecClusterContainer> eflowRecClustersWriteHandle(m_eflowRecClustersWriteHandleKey);
  ATH_CHECK(eflowRecClustersWriteHandle.record(std::make_unique<eflowRecClusterContainer>()));
  eflowRecClusterContainer& theEFlowRecClusterContainerReference = *(eflowRecClustersWriteHandle.ptr());
  
  xAOD::CaloClusterContainer& theCaloClusterContainerReference = *(caloClustersWriteHandle.ptr());
  ATH_CHECK(m_IPFClusterSelectorTool->execute(theEFlowRecClusterContainerReference,theCaloClusterContainerReference));

  // Explicitly start/stop the timer around the subtraction tool calls
  t_subtract.start();
  /* Run the SubtractionTools */
  for (auto thisIPFSubtractionTool : m_IPFSubtractionTools){
    thisIPFSubtractionTool->execute(theElowCaloObjectContainer,&localEFlowRecTrackContainer,&theEFlowRecClusterContainerReference);
  }
  t_subtract.stop();

  if(msgLvl(MSG::DEBUG)) {
    for (auto thisEFTrack : localEFlowRecTrackContainer){
      msg() << "This efRecTrack has E,pt,eta and phi of " << thisEFTrack->getTrack()->e() << ", " << thisEFTrack->getTrack()->pt() << ", " << thisEFTrack->getTrack()->eta() << " and " << thisEFTrack->getTrack()->phi() << endmsg;
    }

    for (auto thisEFCluster : *(eflowRecClustersWriteHandle.ptr()) ){
      msg() << "This efRecCluster has E,pt,eta and phi of " << thisEFCluster->getCluster()->e() << "," << thisEFCluster->getCluster()->pt() << ", " << thisEFCluster->getCluster()->eta() << " and " << thisEFCluster->getCluster()->phi() << endmsg;
    }
  }

  N_efrTracks = localEFlowRecTrackContainer.size();
  N_efrClusters = theEFlowRecClusterContainerReference.size();

  /* Run the other AglTools */
  for (auto thisIPFBaseTool :  m_IPFBaseTools){
    thisIPFBaseTool->execute(*theElowCaloObjectContainer);
  }

  auto mon = Monitored::Group(m_monTool, t_exec, t_subtract, N_efrTracks, N_efrClusters);
  return StatusCode::SUCCESS;
}

StatusCode PFAlgorithm::finalize(){ return StatusCode::SUCCESS;}

void PFAlgorithm::printTools() {
  ATH_MSG_VERBOSE(" ");
  ATH_MSG_VERBOSE("List of IPFSubtraction tools in execution sequence:");
  ATH_MSG_VERBOSE("------------------------------------");
  ATH_MSG_VERBOSE(" ");
  unsigned int subtractionToolCtr = 0;
  for (auto thisIPFSubtractionTool : m_IPFSubtractionTools){
    subtractionToolCtr++;
    ATH_MSG_VERBOSE(std::setw(2) << std::setiosflags(std::ios_base::right) << subtractionToolCtr << ".) "
		    << std::resetiosflags(std::ios_base::right) << std::setw(36) << std::setfill('.')
		    << std::setiosflags(std::ios_base::left) << thisIPFSubtractionTool->type() << std::setfill('.')
                    << thisIPFSubtractionTool->name() << std::setfill(' '));
  }  
  ATH_MSG_VERBOSE(" ");
  ATH_MSG_VERBOSE("------------------------------------");

  ATH_MSG_VERBOSE(" ");
  ATH_MSG_VERBOSE("List of IPFBase tools in execution sequence:");
  ATH_MSG_VERBOSE("------------------------------------");
  ATH_MSG_VERBOSE(" ");
  unsigned int baseToolCtr = 0;
  for (auto thisIPFBaseTool : m_IPFBaseTools){
    baseToolCtr++;
    ATH_MSG_VERBOSE(std::setw(2) << std::setiosflags(std::ios_base::right) << baseToolCtr << ".) "
		    << std::resetiosflags(std::ios_base::right) << std::setw(36) << std::setfill('.')
		    << std::setiosflags(std::ios_base::left) << thisIPFBaseTool->type() << std::setfill('.')
                    << thisIPFBaseTool->name() << std::setfill(' '));
  }  
  ATH_MSG_VERBOSE(" ");
  ATH_MSG_VERBOSE("------------------------------------");
  
  
}

