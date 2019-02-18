// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloRec/CaloConstCellMaker.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2014
 * @brief Create a @c CaloConstCellContainer.
 */


#ifndef CALOREC_CALOCONSTCELLMAKER_H
#define CALOREC_CALOCONSTCELLMAKER_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloEvent/CaloConstCellContainer.h"
class IChronoStatSvc;
class ICaloConstCellMakerTool;


/**
 * @brief Create a @c CaloConstCellContainer.
 *
 * This is similar to @c CaloCellMaker, except that it works on const
 * cell pointers.  This algorithm is thus appropriate for copying
 * cell pointers between containers.
 */
class CaloConstCellMaker
  : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name Algorithm name.
   * @param pSvcLocator Gaudi service locator.
   */
  CaloConstCellMaker(const std::string& name, ISvcLocator* pSvcLocator);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize() override;

  /// Standard Gaudi execute method.
  virtual StatusCode execute(const EventContext& ctx) const override;


private:
  /// Property: Will the new CellContainer will own its cells (default no)?
  int m_ownPolicy;

  /// Property: List of tools to run.
  ToolHandleArray<ICaloConstCellMakerTool> m_caloCellMakerTools;
  
  /// Property: Output container key.
  SG::WriteHandleKey<CaloConstCellContainer> m_caloCellsOutputKey;

  /// For timekeeping.
  ServiceHandle<IChronoStatSvc> m_chrono;
};


#endif // CALOREC_CALOCONSTCELLMAKER_H
