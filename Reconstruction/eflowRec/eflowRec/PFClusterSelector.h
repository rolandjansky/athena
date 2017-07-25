#ifndef PFCLUSTERSELECTOR_H
#define PFCLUSTERSELECTOR_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include <map>

class eflowRecClusterContainer;

class PFClusterSelector : public AthAlgorithm {

public:
  /** Default constructor */
  PFClusterSelector(const std::string& name, ISvcLocator* pSvcLocator);
  /** Default destructor */
  ~PFClusterSelector() {};

  /** Gaudi AthAlgorithm hooks */
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  /** for EM mode, LC weight for cells are retrieved before doing any subtraction; they will be used after subtraction */
  void retrieveLCCalCellWeight(const double& energy, const unsigned& index, std::map<IdentifierHash,double>& cellsWeight);
  
  /** ReadHandle for the CaloClusterContainer to be used as input */
  SG::ReadHandle<xAOD::CaloClusterContainer> m_caloClustersReadHandle;

  /** ReadHandle for the CaloClusterContainer, at LC scale, to be used as input */
  SG::ReadHandle<xAOD::CaloClusterContainer> m_caloCalClustersReadHandle;

  /** WriteHandle for the eflowRecClusterContainer to write out: */
  SG::WriteHandle<eflowRecClusterContainer> m_eflowRecClustersWriteHandle;
  

  
};
#endif
