// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOTOWERXAODFROMCLUSTERS_H
#define CALOREC_CALOTOWERXAODFROMCLUSTERS_H

#include "CaloTowerxAODAlgoBase.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

class CaloTowerxAODFromClusters : public CaloTowerxAODAlgoBase
{
public:

  /// @brief Default algorithm constructor
  ///
  /// @param name        reference to non-modifiable name of algorithm instance
  /// @param pSvcLocator pointer to generic service locator 
  ///
  CaloTowerxAODFromClusters(const std::string& name, ISvcLocator* pSvcLocator);

  /// @brief Baseclass destructor
  virtual ~CaloTowerxAODFromClusters();

  /// @name @c AthAlgorithm interface implementation
  /// @{
  virtual StatusCode initialize() override;     ///< @brief Initialize algorithm
  virtual StatusCode execute(const EventContext&) const override;        ///< @brief Execute algorithm
  virtual StatusCode finalize() override;       ///< @brief Finalize algorithm
  /// @}

private:

  /// @name Algorithm properties
  /// @{
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_inputClusterContainerKey;
  /// @}

};

#endif
