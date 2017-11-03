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
  
  /** ReadHandleKey for the eflowRecTrackContainer to be read in */
  SG::ReadHandleKey<eflowRecTrackContainer> m_eflowRecTracksReadHandleKey{this,"eflowRecTracksInputName","eflowRecTracks","ReadHandleKey for the eflowRecTrackContainer to be read in"};

  /** ReadHandleKey for the eflowRecClusterContainer to be read in */
  SG::ReadHandleKey<eflowRecClusterContainer> m_eflowRecClustersReadHandleKey{this,"eflowRecClustersInputName","eflowRecClusters","ReadHandleKey for the eflowRecClusterContainer to be read in"};

  /** WriteHandleKey for CaloClusterContainer to be written out */
  SG::WriteHandleKey<xAOD::CaloClusterContainer> m_caloClustersWriteHandleKey{this,"PFCaloClustersOutputName","PFCaloCluster","WriteHandleKey for CaloClusterContainer to be written out"};

  /** WriteHandleKey for eflowCaloObjectContainer to be written out */
  SG::WriteHandleKey<eflowCaloObjectContainer> m_eflowCaloObjectsWriteHandleKey{this,"eflowCaloObjectsOutputName","eflowCaloObjects","WriteHandleKey for eflowCaloObjectContainer to be written out"};
  
  /** Funciton to print out list of tools if in VERBOSE mode */
  void printTools();
  
};
#endif
