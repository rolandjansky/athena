// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file CaloTowerBuilderToolTestAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2013
 * @brief Regression test for CaloTowerBuilderTool.
 */


#ifndef CALOUTILS_CALOTOWERBUILDERTOOLTESTALG_H
#define CALOUTILS_CALOTOWERBUILDERTOOLTESTALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloUtils/CaloTowerStore.h"
#include "CaloEvent/CaloTowerSeg.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "GaudiKernel/ToolHandle.h"
#include <vector>
#include <cstdint>
class CaloCellContainer;
class CaloTowerContainer;
class ICaloTowerBuilderToolBase; 


class CaloTowerBuilderToolTestAlg
  : public AthAlgorithm
{
public:
  /**
   * @brief Constructor.
   */
  CaloTowerBuilderToolTestAlg (const std::string& name,
                               ISvcLocator* pSvcLocator);


  /** 
   * @brief Standard Gaudi initialize method.
   */
  virtual StatusCode initialize();


  /** 
   * @brief Standard Gaudi execute method.
   */
  virtual StatusCode execute();


private:
  CaloCellContainer* make_cells();
  StatusCode test_subseg (const CaloTowerSeg::SubSeg& subseg,
                          const CaloCellContainer* cells,
                          const CaloTowerContainer* tow0);
  StatusCode test1();

  ToolHandle<ICaloTowerBuilderToolBase> m_builder;
  std::vector<CaloCell_ID::SUBCALO> m_calos;
  CaloTowerSeg m_seg;

  uint32_t m_seed;
};




#endif // not CALOUTILS_CALOTOWERBUILDERTOOLTESTALG_H
