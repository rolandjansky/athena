// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOTOWERXAODFROMCLUSTERS_H
#define CALOREC_CALOTOWERXAODFROMCLUSTERS_H

#include "CaloRec/CaloTowerxAODAlgoBase.h"

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
  ~CaloTowerxAODFromClusters();

  /// @name @c AthAlgorithm interface implementation
  /// @{
  virtual StatusCode initialize();     ///< @brief Initialize algorithm
  virtual StatusCode execute();        ///< @brief Execute algorithm
  virtual StatusCode finalize();       ///< @brief Finalize algorithm
  /// @}

private:

  /// @name Algorithm properties
  /// @{
  std::string m_inputClusterContainerKey;
  /// @}

};

#endif
