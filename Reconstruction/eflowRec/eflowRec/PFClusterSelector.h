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
  void retrieveLCCalCellWeight(const double& energy, const unsigned& index, std::map<IdentifierHash,double>& cellsWeight,const xAOD::CaloClusterContainer& caloCalClustersContainer);
  
  /** ReadHandleKey for the CaloClusterContainer to be used as input */
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_caloClustersReadHandleKey{this,"clustersName","CaloTopoCluster","ReadHandleKey for the CaloClusterContainer to be used as input"};

  /** ReadHandleKey for the CaloClusterContainer, at LC scale, to be used as input */
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_caloCalClustersReadHandleKey{this,"calClustersName","CaloCalTopoCluster","ReadHandleKey for the CaloClusterContainer, at LC scale, to be used as input"};

  /** WriteHandleKey for the eflowRecClusterContainer to write out */
  SG::WriteHandleKey<eflowRecClusterContainer> m_eflowRecClustersWriteHandleKey{this,"eflowRecClustersOutputName","eflowRecClusters","WriteHandleKey for the eflowRecClusterContainer to write out"};
  

  
};
#endif
