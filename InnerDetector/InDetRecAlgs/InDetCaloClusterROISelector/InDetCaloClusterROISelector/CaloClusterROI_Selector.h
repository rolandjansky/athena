/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAREC_CALOCLUSTERROI_SELECTOR_H
#define EGAMMAREC_CALOCLUSTERROI_SELECTOR_H
/**
  @class CaloClusterROI_Selector
          Algorithm which creates an CaloClusterROICollection. 
          It retrieves data objects from TDS 
*/

// INCLUDE HEADER FILES:
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "TrkCaloClusterROI/CaloClusterROI_Collection.h"

#include "egammaInterfaces/IegammaCheckEnergyDepositTool.h"
#include "InDetRecToolInterfaces/ICaloClusterROI_Builder.h"

#include "xAODCaloEvent/CaloClusterFwd.h"

#include <atomic>

namespace InDet {

class ICaloClusterROI_Builder;


class CaloClusterROI_Selector : public AthReentrantAlgorithm
{
 public:

  /** @brief Default constructor*/
  CaloClusterROI_Selector(const std::string& name, ISvcLocator* pSvcLocator);

  /** @brief Destructor*/
  ~CaloClusterROI_Selector();
	
  /** @brief initialize method*/
  StatusCode initialize();
  /** @brief finalize method*/
  StatusCode finalize();
  /** @brief execute method*/
  StatusCode execute_r(const EventContext& ctx) const;

 private:
  bool PassClusterSelection(const xAOD::CaloCluster* cluster) const;

  /** @brief Name of the cluster intput collection*/
  SG::ReadHandleKey<xAOD::CaloClusterContainer>   m_inputClusterContainerName {this,
      "InputClusterContainerName", "egammaTopoCluster",
      "Input cluster for egamma objects"};

  /** @brief Name of the cluster output collection*/
  SG::WriteHandleKey<CaloClusterROI_Collection>  m_outputClusterContainerName {this,
      "OutputClusterContainerName", "CaloClusterROIs", "Output cluster for egamma objects"};

  //
  // The tools
  //
  /** @brief Pointer to the egammaCheckEnergyDepositTool*/
  ToolHandle<IegammaCheckEnergyDepositTool> m_egammaCheckEnergyDepositTool {this, 
      "egammaCheckEnergyDepositTool", "",
      "Optional tool that performs basic checks of viability of cluster"};
  /** @brief Tool to build ROI*/
  ToolHandle<InDet::ICaloClusterROI_Builder>   m_caloClusterROI_Builder {this,
      "CaloClusterROIBuilder", "","Handle of the CaloClusterROI_Builder Tool"};

  Gaudi::Property<double>  m_ClusterEtCut {this,
      "ClusterEtCut", 0.0, "Cut on cluster EM Et"};

  Gaudi::Property<double>  m_ClusterEMFCut {this,
      "ClusterEMFCut", 0.0, "Cut on cluster EM fraction"};

  Gaudi::Property<double>  m_ClusterLateralCut {this,
      "ClusterLateralCut", 1.0,
      "Cut on cluster LATERAL, i.e., the second transverse moment normalized"};

  Gaudi::Property<double>  m_ClusterEMEtCut {this,
      "ClusterEMEtCut", 0.0, "Cut on cluster Et"};

  // use atomics
  mutable std::atomic_uint m_allClusters{0};
  mutable std::atomic_uint m_selectedClusters{0};

  // others:
  IChronoStatSvc* m_timingProfile;

};

} //End namespace 
#endif
