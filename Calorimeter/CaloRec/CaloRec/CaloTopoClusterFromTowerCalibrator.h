// -*- c++ -*- 
/* Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration */
#ifndef CALOREC_CALOTOPOCLUSTERFROMTOWERCALIBRATOR_H
#define CALOREC_CALOTOPOCLUSTERFROMTOWERCALIBRATOR_H

#include "AthenaBaseComps/AthAlgTool.h" 

#include "CaloRec/CaloClusterCollectionProcessor.h"

#include <string>

class CaloCellClusterWeights;

class CaloTopoClusterFromTowerCalibrator : public AthAlgTool, virtual public CaloClusterCollectionProcessor
{
public:

  /// @brief Tool constructor
  CaloTopoClusterFromTowerCalibrator(const std::string& type,const std::string& name,const IInterface* pParent);

  /// @brief Tool execution
  StatusCode execute(xAOD::CaloClusterContainer* pClusCont);

private:

  /// @name Tool properties
  /// @{
  std::string m_cellClusterWeightsKey; ///> @brief Key for cell-cluster weight look up object
  bool        m_orderByPt;             ///> @brief Turn on pT ordering
  /// @}

  /// @brief Default container key
  static std::string m_defaultKey;
};
/// @class CaloTopoClusterFromTowerCalibrator
///
/// @brief A cluster builder tool calibrated topo-clusters formed from calorimeter towers to the LCW scale. 
///
/// @author Peter Loch <loch@physics.arizona.edu>      
#endif
