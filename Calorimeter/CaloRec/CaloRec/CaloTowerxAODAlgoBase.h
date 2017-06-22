// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOTOWERXAODALGOBASE_H
#define CALOREC_CALOTOWERXAODALGOBASE_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaKernel/IOVSvcDefs.h"

#include <string>
#include <vector>

#include "xAODCaloEvent/CaloTowerContainer.h"
#include "xAODCaloEvent/CaloTowerAuxContainer.h"


class CaloTowerxAODAlgoBase : public AthAlgorithm
{
public:

  /// @brief Default algorithm constructor
  ///
  /// @param name        reference to non-modifiable name of algorithm instance
  /// @param pSvcLocator pointer to generic service locator 
  ///
  CaloTowerxAODAlgoBase(const std::string& name, ISvcLocator* pSvcLocator);

  /// @brief destructor
  virtual ~CaloTowerxAODAlgoBase();
  virtual StatusCode execute() {return StatusCode::SUCCESS;}
 

  StatusCode geoInit(IOVSVC_CALLBACK_ARGS);

protected:

  /// @name Algorithm properties
  /// @{
  std::string  m_caloTowerContainerKey;   ///< @brief Handle to @c xAOD::CaloTowerContainer
  float      m_minEta;                   ///< @brief Tower-grid: lower eta boundary
  float      m_maxEta;                   ///< @brief Tower-grid: upper eta boundary
  int        m_nEtaBins;                 ///< @brief Tower-grid: number of phi bins
  int        m_nPhiBins;                 ///< @brief Tower-grid: number of phi bins
  bool       m_doxCheck;                 ///< @brief Turn on internal checks (debugging)
  bool       m_registerLinks;            ///< @brief Register cell-to-tower links in detector store

  /// @}

  

  /// @name Internal mapping of cells to towers
  /// @{
  struct cellToTower_t{   ///<@brief struct to store the tower index and the weight for each cell
    size_t m_towerIdx;
    float m_weight;
    cellToTower_t(size_t idx, float w=1.0) : m_towerIdx(idx), m_weight(w) {};
    cellToTower_t() : m_towerIdx(0),m_weight(0) {};
  };
  std::vector< std::vector<cellToTower_t> > m_cellToTower; ///< @brief map of cell indices to tower indices and weights
  /// @}


  StatusCode initBase();                               ///< @brief Initialization of this base-class                
  xAOD::CaloTowerContainer* makeContainer();          ///< @brief Create @c xAOD::CaloTowerContainer and auxiliary store

private:
  StatusCode fillIndexCache(); 

};

#endif
