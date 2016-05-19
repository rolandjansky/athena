// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOTOWERXAODFROMCELLS_H
#define CALOREC_CALOTOWERXAODFROMCELLS_H

#include "CaloRec/CaloTowerxAODAlgoBase.h"


#include <string>
#include <vector>


class CaloCellContainer;

class CaloTowerxAODFromCells : public CaloTowerxAODAlgoBase
{
public:

  /// @brief Default algorithm constructor
  ///
  /// @param name        reference to non-modifiable name of algorithm instance
  /// @param pSvcLocator pointer to generic service locator 
  ///
  CaloTowerxAODFromCells(const std::string& name, ISvcLocator* pSvcLocator);

  /// @brief Baseclass destructor
  ~CaloTowerxAODFromCells();

  /// @name @c AthAlgorithm interface implementation
  /// @{
  virtual StatusCode initialize();     ///< @brief Initialize algorithm
  virtual StatusCode execute();        ///< @brief Execute algorithm
  virtual StatusCode finalize();       ///< @brief Finalize algorithm
  /// @}

private:

  /// @name Algorithm properties
  /// @{
  //SG::ReadHandle<CaloCellContainer> m_inputCellContainer;
  std::string m_inputCellContainerKey;///< @brief SG key of the input calo-cell container
  double      m_cellThresholdE;       ///< @brief Cell energy threshold (only for @b FilteredCell mode)
  //bool        m_useThresholdE;        ///< @brief @c true if tower (@b AllCell ) or cell (@b FilteredCell ) thresholds are to be used  
  /// @}

  bool  m_filterCells;

};

#endif
