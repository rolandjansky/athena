// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
 /**
 * @file LArRecUtils/LArFCalTowerBuilderToolTestAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2013
 * @brief Regression test for LArFCalTowerBuilderTool.
 */


#ifndef LARRECUTILS_LARFCALTOWERBUILDERTOOLTESTALG_H
#define LARRECUTILS_LARFCALTOWERBUILDERTOOLTESTALG_H



#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloEvent/CaloTowerSeg.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/ReadCondHandleKey.h"
#include <vector>
class CaloCellContainer;
class CaloTowerContainer;
class ICaloTowerBuilderToolBase;


class LArFCalTowerBuilderToolTestAlg
  : public AthAlgorithm
{
public:
  /**
   * @brief Constructor.
   */
  LArFCalTowerBuilderToolTestAlg (const std::string& name,
                                  ISvcLocator* pSvcLocator);


  /** 
   * @brief Standard Gaudi initialize method.
   */
  virtual StatusCode initialize() override;


  /** 
   * @brief Standard Gaudi execute method.
   */
  virtual StatusCode execute() override;


private:
  CaloCellContainer* make_cells();
  StatusCode test_subseg (const EventContext& ctx,
                          const CaloTowerSeg::SubSeg& subseg,
                          const CaloCellContainer* cells,
                          const CaloTowerContainer* tow0);
  StatusCode test1();

  ToolHandle<ICaloTowerBuilderToolBase> m_builder;
  std::vector<CaloCell_ID::SUBCALO> m_calos;
  CaloTowerSeg m_seg;
  uint32_t m_seed;
  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
      , "CaloDetDescrManager"
      , "CaloDetDescrManager"
      , "SG Key for CaloDetDescrManager in the Condition Store" };
};




#endif // not LARRECUTILS_LARFCALTOWERBUILDERTOOLTESTALG_H
