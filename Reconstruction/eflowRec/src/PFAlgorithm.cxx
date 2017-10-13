#include "eflowRec/PFAlgorithm.h"

PFAlgorithm::PFAlgorithm(const std::string& name,  ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator), m_IPFSubtractionTools(this), m_IPFBaseTools(this)  
{
  declareProperty( "SubtractionToolList",  m_IPFSubtractionTools, "List of Private Subtraction IPFSubtractionTools" );
  declareProperty( "BaseToolList",  m_IPFBaseTools, "List of Private IPFBaseTools" );
}

StatusCode PFAlgorithm::initialize(){

  ATH_CHECK(m_IPFSubtractionTools.retrieve());
  ATH_CHECK( m_IPFBaseTools.retrieve());
  
  ATH_CHECK(m_eflowRecTracksReadHandleKey.initialize());
  ATH_CHECK(m_eflowRecClustersReadHandleKey.initialize());

  ATH_CHECK(m_eflowCaloObjectsWriteHandleKey.initialize());
  ATH_CHECK(m_caloClustersWriteHandleKey.initialize());

  printTools();

  return StatusCode::SUCCESS;
}

StatusCode PFAlgorithm::execute(){

  ATH_MSG_DEBUG("Executing");

  SG::WriteHandle<eflowCaloObjectContainer> eflowCaloObjectsWriteHandle(m_eflowCaloObjectsWriteHandleKey);
  ATH_CHECK(eflowCaloObjectsWriteHandle.record(std::make_unique<eflowCaloObjectContainer>()));
  eflowCaloObjectContainer* theElowCaloObjectContainer = eflowCaloObjectsWriteHandle.ptr();

  SG::WriteHandle<xAOD::CaloClusterContainer> caloClustersWriteHandle(m_caloClustersWriteHandleKey);
  ATH_CHECK(caloClustersWriteHandle.record(std::make_unique<xAOD::CaloClusterContainer>(),std::make_unique<xAOD::CaloClusterAuxContainer>()));
  ATH_MSG_DEBUG("CaloClusterWriteHandle has container of size" << caloClustersWriteHandle->size());

  SG::ReadHandle<eflowRecTrackContainer> eflowRecTracksReadHandle(m_eflowRecTracksReadHandleKey);
  eflowRecTrackContainer localEFlowRecTrackContainer(*eflowRecTracksReadHandle.ptr());
  SG::ReadHandle<eflowRecClusterContainer> eflowRecClustersReadHandle(m_eflowRecClustersReadHandleKey);
  eflowRecClusterContainer localEFlowRecClusterContainer(*eflowRecClustersReadHandle.ptr());
  
  /* Run the SubtractionTools */
  for (auto thisIPFSubtractionTool : m_IPFSubtractionTools){
    thisIPFSubtractionTool->execute(theElowCaloObjectContainer,&localEFlowRecTrackContainer,&localEFlowRecClusterContainer,*(caloClustersWriteHandle.ptr()));
  }

  for (auto thisEFTrack : localEFlowRecTrackContainer){
    ATH_MSG_DEBUG("This efRecTrack has E,pt,eta and phi of " << thisEFTrack->getTrack()->e() << ", " << thisEFTrack->getTrack()->pt() << ", " << thisEFTrack->getTrack()->eta() << " and " << thisEFTrack->getTrack()->phi());
  }

  for (auto thisEFCluster : localEFlowRecClusterContainer){
    ATH_MSG_DEBUG("This efRecCluster has E,pt,eta and phi of " << thisEFCluster->getCluster()->e() << "," << thisEFCluster->getCluster()->pt() << ", " << thisEFCluster->getCluster()->eta() << " and " << thisEFCluster->getCluster()->phi());
  }


  /* Run the other AglTools */
  for (auto thisIPFBaseTool :  m_IPFBaseTools){
    thisIPFBaseTool->execute(*theElowCaloObjectContainer,*(caloClustersWriteHandle.ptr()));
  }
    
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

