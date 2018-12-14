// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOTOWERXAODFROMCELLS_H
#define CALOREC_CALOTOWERXAODFROMCELLS_H

#include "CaloTowerxAODAlgoBase.h"


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
  virtual ~CaloTowerxAODFromCells();

  /// @name @c AthAlgorithm interface implementation
  /// @{
  virtual StatusCode initialize() override;     ///< @brief Initialize algorithm
  virtual StatusCode execute(const EventContext& ctx) const override;        ///< @brief Execute algorithm
  virtual StatusCode finalize() override;       ///< @brief Finalize algorithm
  /// @}

private:

  /// @name Algorithm properties
  /// @{
  SG::ReadHandleKey<CaloCellContainer> m_inputCellContainerKey;
  double      m_cellThresholdE;       ///< @brief Cell energy threshold (only for @b FilteredCell mode)
  /// @}

  bool  m_filterCells;

};

#endif
