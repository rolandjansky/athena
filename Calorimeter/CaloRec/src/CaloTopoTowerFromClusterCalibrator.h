// -*- c++ -*- 
/* Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration */
#ifndef CALOREC_CALOTOPOTOWERFROMCLUSTERCALIBRATOR_H
#define CALOREC_CALOTOPOTOWERFROMCLUSTERCALIBRATOR_H

#include "StoreGate/ReadHandleKey.h"

#include "AthenaBaseComps/AthAlgTool.h" 

#include "CaloRec/CaloTowerCollectionProcessor.h"

#include <string>

class CaloCellClusterWeights;

class CaloTopoTowerFromClusterCalibrator : public AthAlgTool, virtual public CaloTowerCollectionProcessor
{
public:

  /// @brief Tool constructor
  CaloTopoTowerFromClusterCalibrator(const std::string& type,const std::string& name,const IInterface* pParent);
 
  /// @brief Tool execution
  
  virtual StatusCode execute(const EventContext& ctx,
                             xAOD::CaloClusterContainer* pClusCont,
			     CaloCellClusterWeights* cellWeights) const override;
  //  virtual StatusCode initialize() override;
private:

  /// @name Tool properties
  /// @{
  bool m_orderByPt = { false };            ///< Turn on pT ordering if @c true
  /// @}
};
/// @class CaloTopoTowerFromClusterCalibrator
///
/// @brief A cluster builder tool to calibrate topo-clusters formed from (EM) calorimeter towers to the LCW scale. 
///
/// This module applies LCW weights to cells contributing to towers represented by @c xAOD::CaloCluster objects. 
/// The overall energy contribution of a given cell contributing to a given tower is then @f$ w_{\rm geo} \times w_{\rm LCW} \times E_{\rm cell} @f$,
/// where @f$ w_{\rm geo} @f$ is the geometrical weight, @f$ w_{\rm LCW} @f$ is
/// the calibration weight the cell received from the LCW calibration in the context of the @c xAOD::CaloCluster objects it contributes to (at most two),
/// and @f$ E_{\rm cell} @f$ is the cell energy on EM scale. More details on the weights are given on 
/// <a href="https://twiki.cern.ch/twiki/bin/view/AtlasSandboxProtected/CaloTowerPerformance" title="https://twiki.cern.ch/twiki/bin/view/AtlasSandboxProtected/CaloTowerPerformance">this page</a>.
///  
/// @author Peter Loch <loch@physics.arizona.edu>      
#endif
