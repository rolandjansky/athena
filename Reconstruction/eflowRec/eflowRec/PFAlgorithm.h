#ifndef PFALGORITHM_H
#define PFALGORITHM_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"

#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowRecTrack.h"

#include "xAODCaloEvent/CaloClusterContainer.h"

class PFISubtractionTool;

class PFAlgorithm : public AthAlgorithm {

public:
  PFAlgorithm(const std::string& name, ISvcLocator* pSvcLocator);
  ~PFAlgorithm() {};

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  /** List of PFISubtractionTool, which will be executed by this algorithm */
  ToolHandleArray<PFISubtractionTool> m_tools;

  /** ReadHandle for the eflowRecTrackContainer to be read in */
  SG::ReadHandle<eflowRecTrackContainer> m_eflowRecTracksReadHandle;

  /** ReadHandle for the eflowRecClusterContainer to be read in */
  SG::ReadHandle<eflowRecClusterContainer> m_eflowRecClustersReadHandle;

  /** WriteHandle for CaloClusterContainer to be written out */
  SG::WriteHandle<xAOD::CaloClusterContainer> m_caloClustersWriteHandle;
  
  /** Funciton to print out list of tools if in VERBOSE mode */
  void printTools();
  
};
#endif
