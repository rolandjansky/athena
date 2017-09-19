#ifndef PFALGORITHM_H
#define PFALGORITHM_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"

#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowRecTrack.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "eflowRec/IPFBaseTool.h"
#include "eflowRec/IPFSubtractionTool.h"

class PFAlgorithm : public AthAlgorithm {

public:
  PFAlgorithm(const std::string& name, ISvcLocator* pSvcLocator);
  ~PFAlgorithm() {};

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  /** List of IPFSubtractionTool, which will be executed by this algorithm */
  ToolHandleArray<IPFSubtractionTool> m_IPFSubtractionTools;

  /** List of PFBaseAlgTool, which will be executed by this algorithm */
  ToolHandleArray<IPFBaseTool> m_IPFBaseTools;
  
  /** ReadHandle for the eflowRecTrackContainer to be read in */
  SG::ReadHandle<eflowRecTrackContainer> m_eflowRecTracksReadHandle;

  /** ReadHandle for the eflowRecClusterContainer to be read in */
  SG::ReadHandle<eflowRecClusterContainer> m_eflowRecClustersReadHandle;

  /** WriteHandle for CaloClusterContainer to be written out */
  SG::WriteHandle<xAOD::CaloClusterContainer> m_caloClustersWriteHandle;

  /** WriteHandle for eflowCaloObjectContainer to be written out */
  SG::WriteHandle<eflowCaloObjectContainer> m_eflowCaloObjectsWriteHandle;
  
  /** Funciton to print out list of tools if in VERBOSE mode */
  void printTools();
  
};
#endif
