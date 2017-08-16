#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/IPFSubtractionTool.h"
#include "eflowRec/PFAlgorithm.h"

PFAlgorithm::PFAlgorithm(const std::string& name,  ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator), m_tools(this), m_eflowRecTracksReadHandle("eflowRecTracks"), m_eflowRecClustersReadHandle("eflowRecClusters"),  m_caloClustersWriteHandle("PFCaloCluster")   
{
  declareProperty( "PrivateToolList",  m_tools, "List of Private Subtraction eflowISubtractionAlgTools" );
  declareProperty("eflowRecTracksInputName",  m_eflowRecTracksReadHandle);
  declareProperty("eflowRecClustersInputName",  m_eflowRecClustersReadHandle);
  declareProperty("PFCaloClustersOutputName", m_caloClustersWriteHandle);
}

StatusCode PFAlgorithm::initialize(){

  ATH_CHECK(m_tools.retrieve());

  ATH_CHECK(m_eflowRecTracksReadHandle.initialize());
  ATH_CHECK(m_eflowRecClustersReadHandle.initialize());

  ATH_CHECK(m_caloClustersWriteHandle.initialize());

  printTools();

  return StatusCode::SUCCESS;
}

StatusCode PFAlgorithm::execute(){

  ATH_MSG_DEBUG("Executing");

  eflowCaloObjectContainer theElowCaloObjectContainer;

  ATH_CHECK(m_caloClustersWriteHandle.record(std::make_unique<xAOD::CaloClusterContainer>(),std::make_unique<xAOD::CaloClusterAuxContainer>()));
  ATH_MSG_DEBUG("CaloClusterWriteHandle has container of size" << m_caloClustersWriteHandle->size());
  
   /* Run the SubtractionTools
   * --> CellLevelSubtractionTool, RecoverSplitShowersTool */
  for (auto thisPFISubtractionTool : m_tools){
    thisPFISubtractionTool->execute(&theElowCaloObjectContainer,const_cast<eflowRecTrackContainer*>(m_eflowRecTracksReadHandle.ptr()),const_cast<eflowRecClusterContainer*>(m_eflowRecClustersReadHandle.ptr()),*(m_caloClustersWriteHandle.ptr()));
  }
  
  return StatusCode::SUCCESS;
}

StatusCode PFAlgorithm::finalize(){ return StatusCode::SUCCESS;}

void PFAlgorithm::printTools() {
  ATH_MSG_VERBOSE(" ");
  ATH_MSG_VERBOSE("List of tools in execution sequence:");
  ATH_MSG_VERBOSE("------------------------------------");
  ATH_MSG_VERBOSE(" ");
  unsigned int toolCtr = 0;
  for (auto thisPFISubtractionTool : m_tools){
    toolCtr++;
    ATH_MSG_VERBOSE(std::setw(2) << std::setiosflags(std::ios_base::right) << toolCtr << ".) "
		    << std::resetiosflags(std::ios_base::right) << std::setw(36) << std::setfill('.')
		    << std::setiosflags(std::ios_base::left) << thisPFISubtractionTool->type() << std::setfill('.')
                    << thisPFISubtractionTool->name() << std::setfill(' '));
  }  
  ATH_MSG_VERBOSE(" ");
  ATH_MSG_VERBOSE("------------------------------------");
}

