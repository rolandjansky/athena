/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

#include "egammaInterfaces/IegammaCaloClusterSelector.h"
#include "InDetRecToolInterfaces/ICaloClusterROI_Builder.h"

#include "xAODCaloEvent/CaloClusterFwd.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/ReadCondHandleKey.h"

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
  StatusCode initialize() override;
  /** @brief finalize method*/
  StatusCode finalize() override;
  /** @brief execute method*/
  StatusCode execute(const EventContext& ctx) const override;

 private:

  /** @brief Name of the cluster intput collection*/
  SG::ReadHandleKey<xAOD::CaloClusterContainer>   m_inputClusterContainerName {this,
      "InputClusterContainerName", "egammaTopoCluster",
      "Input cluster for egamma objects"};

  /** @brief Name of the cluster output collection*/
  SG::WriteHandleKey<CaloClusterROI_Collection>  m_outputClusterContainerName {this,
      "OutputClusterContainerName", "CaloClusterROIs", "Output cluster for egamma objects"};

  /**
   * @brief Name of the CaloDetDescrManager condition object
   */
  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
      , "CaloDetDescrManager"
      , "CaloDetDescrManager"
      , "SG Key for CaloDetDescrManager in the Condition Store" };

  //
  // The tools
  //
  /** @brief Tool to filter the calo clusters */
  ToolHandle<IegammaCaloClusterSelector> m_egammaCaloClusterSelector {this, 
      "egammaCaloClusterSelector", "egammaCaloClusterSelector",
      "Tool that makes the cluster selection"};

  /** @brief Tool to build ROI*/
  ToolHandle<InDet::ICaloClusterROI_Builder>   m_caloClusterROI_Builder {this,
      "CaloClusterROIBuilder", "","Handle of the CaloClusterROI_Builder Tool"};

  // use atomics
  mutable std::atomic_uint m_allClusters{0};
  mutable std::atomic_uint m_selectedClusters{0};

};

} //End namespace 
#endif
