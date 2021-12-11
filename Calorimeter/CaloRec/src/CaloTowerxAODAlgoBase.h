// -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOTOWERXAODALGOBASE_H
#define CALOREC_CALOTOWERXAODALGOBASE_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaKernel/IOVSvcDefs.h"

#include "xAODCaloEvent/CaloTowerContainer.h"
#include "xAODCaloEvent/CaloTowerAuxContainer.h"
#include "StoreGate/WriteHandle.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "CxxUtils/CachedValue.h"

#include <string>
#include <vector>

class CaloTowerxAODAlgoBase : public AthReentrantAlgorithm
{
public:

  /// @brief Default algorithm constructor
  ///
  /// @param name        reference to non-modifiable name of algorithm instance
  /// @param pSvcLocator pointer to generic service locator 
  ///
  CaloTowerxAODAlgoBase(const std::string& name, ISvcLocator* pSvcLocator);

  /// @brief destructor
  ~CaloTowerxAODAlgoBase();

protected:

  /// @name Algorithm properties
  /// @{
  SG::WriteHandleKey<xAOD::CaloTowerContainer> m_caloTowerContainerKey;///< @brief Handle to @c xAOD::CaloTowerContainer
  float      m_minEta;                   ///< @brief Tower-grid: lower eta boundary
  float      m_maxEta;                   ///< @brief Tower-grid: upper eta boundary
  int        m_nEtaBins;                 ///< @brief Tower-grid: number of phi bins
  int        m_nPhiBins;                 ///< @brief Tower-grid: number of phi bins
  bool       m_doxCheck;                 ///< @brief Turn on internal checks (debugging)
  /// @}

  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
      , "CaloDetDescrManager"
      , "CaloDetDescrManager"
      , "SG Key for CaloDetDescrManager in the Condition Store" };


  /// @name Internal mapping of cells to towers
  /// @{
  struct cellToTower_t{   ///<@brief struct to store the tower index and the weight for each cell
    size_t m_towerIdx;
    float m_weight;
    cellToTower_t(size_t idx, float w=1.0) : m_towerIdx(idx), m_weight(w) {};
    cellToTower_t() : m_towerIdx(0),m_weight(0) {};
  };
  typedef std::vector<std::vector<cellToTower_t>> CellToTowerVec;
  /// @}

  ///< @brief Initialization of this base-class
  StatusCode initBase();

  ///< @brief Create @c xAOD::CaloTowerContainer and auxiliary store
  SG::WriteHandle<xAOD::CaloTowerContainer>
  makeContainer(const EventContext& ctx) const;

  ///< @brief Intialize m_cellToTower cache
  const CellToTowerVec& getIndexCache(const EventContext& ctx) const; 

private:
  CxxUtils::CachedValue<CellToTowerVec> m_cellToTower; ///< @brief map of cell indices to tower indices and weights

  StatusCode fillIndexCache(const EventContext& ctx, CellToTowerVec& cellToTower) const;
};

#endif
