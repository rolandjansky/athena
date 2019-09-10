///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// CaloClusterCellLinksUpdater.h 
// Header file for class CaloClusterCellLinksUpdater
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef CALOREC_CALOCLUSTERCELLLINKSUPDATER_H
#define CALOREC_CALOCLUSTERCELLLINKSUPDATER_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadHandleKeyArray.h"

class CaloClusterCellLinksUpdater
  : public ::AthReentrantAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Constructor with parameters: 
  using AthReentrantAlgorithm::AthReentrantAlgorithm;

  /// Destructor: 
  virtual ~CaloClusterCellLinksUpdater() = default;

  // Athena algorithm's Hooks
  /// Standard algorithm initialize
  virtual StatusCode  initialize() override;
  /// Standard algorithm execute
  virtual StatusCode  execute(const EventContext& ctx) const override;

 private: 

  /// Name of the CaloCellContainer the links should point to(jobProperty) 
  SG::ReadHandleKey<CaloCellContainer> m_caloCellContainerKey{this,
      "NewCaloCellName", "AODCellContainer", "Name of the CaloCellContainer the links should point to"};

  /// CaloClusters to be considered (jobProperty)
  SG::ReadHandleKeyArray<xAOD::CaloClusterContainer> m_clusterKeys{this,
      "CaloClusterNames", {}, "CaloClusters to be considered"};

}; 

#endif //> !CALOREC_CALOCLUSTERCELLLINKSUPDATER_H
