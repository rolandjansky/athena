#include "eflowRec/PFAlgorithm.h"

PFAlgorithm::PFAlgorithm(const std::string& name,  ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator), m_IPFSubtractionTools(this), m_IPFBaseTools(this), m_eflowRecTracksReadHandle("eflowRecTracks"), m_eflowRecClustersReadHandle("eflowRecClusters"),  m_caloClustersWriteHandle("PFCaloCluster"), m_eflowCaloObjectsWriteHandle("eflowCaloObjects")   
{
  declareProperty( "SubtractionToolList",  m_IPFSubtractionTools, "List of Private Subtraction IPFSubtractionTools" );
  declareProperty( "BaseToolList",  m_IPFBaseTools, "List of Private IPFBaseTools" );
  declareProperty("eflowRecTracksInputName",  m_eflowRecTracksReadHandle);
  declareProperty("eflowRecClustersInputName",  m_eflowRecClustersReadHandle);
  declareProperty("PFCaloClustersOutputName", m_caloClustersWriteHandle);
}

StatusCode PFAlgorithm::initialize(){

  ATH_CHECK(m_IPFSubtractionTools.retrieve());
  ATH_CHECK( m_IPFBaseTools.retrieve());
  
  ATH_CHECK(m_eflowRecTracksReadHandle.initialize());
  ATH_CHECK(m_eflowRecClustersReadHandle.initialize());

  ATH_CHECK(m_eflowCaloObjectsWriteHandle.initialize());
  ATH_CHECK(m_caloClustersWriteHandle.initialize());

  printTools();

  return StatusCode::SUCCESS;
}

StatusCode PFAlgorithm::execute(){

  ATH_MSG_DEBUG("Executing");

  ATH_CHECK(m_eflowCaloObjectsWriteHandle.record(std::make_unique<eflowCaloObjectContainer>()));
  eflowCaloObjectContainer* theElowCaloObjectContainer = m_eflowCaloObjectsWriteHandle.ptr();

  ATH_CHECK(m_caloClustersWriteHandle.record(std::make_unique<xAOD::CaloClusterContainer>(),std::make_unique<xAOD::CaloClusterAuxContainer>()));
  ATH_MSG_DEBUG("CaloClusterWriteHandle has container of size" << m_caloClustersWriteHandle->size());

  eflowRecTrackContainer localEFlowRecTrackContainer(*m_eflowRecTracksReadHandle.ptr());
  eflowRecClusterContainer localEFlowRecClusterContainer(*m_eflowRecClustersReadHandle.ptr());
  
  /* Run the SubtractionTools */
  for (auto thisIPFSubtractionTool : m_IPFSubtractionTools){
    thisIPFSubtractionTool->execute(theElowCaloObjectContainer,&localEFlowRecTrackContainer,&localEFlowRecClusterContainer,*(m_caloClustersWriteHandle.ptr()));
  }

  /* Run the other AglTools */
  for (auto thisIPFBaseTool :  m_IPFBaseTools){
    thisIPFBaseTool->execute(*theElowCaloObjectContainer,*(m_caloClustersWriteHandle.ptr()));
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

