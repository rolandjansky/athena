/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARL1SIM_LARSCSIMPLEMAKER_H
#define LARL1SIM_LARSCSIMPLEMAKER_H
// +======================================================================+
// +                                                                      +
// + Author ........: Denis Oliveira Damazio                              +
// + Institute ......: BNL                                                +
// + Creation date .: 17/06/2012                                          +
// +                                                                      +
// +======================================================================+
//
// ....... include
//

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthAllocators/DataPool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "CaloInterface/ICaloNoiseTool.h"
#include "CaloInterface/ICaloLumiBCIDTool.h"


/**
 * @brief Make a dummy supercell @c LArRawChannelContainer from a
 *        @c CaloCallContainer.
 */
class LArSCSimpleMaker : public AthAlgorithm
{
public:
  /// Standard Gaudi algorithm constructor.
  LArSCSimpleMaker(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArSCSimpleMaker() { m_noise_per_cell.clear(); }

  
  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();

  /// Algorithm execute method.
  virtual StatusCode execute();


private:
  /// Property: Offline / supercell mapping tool.
  ToolHandle<ICaloSuperCellIDTool>     m_scidtool;

  /// Property: SG key for the input calorimeter cell container.
  std::string m_cellContainer;

  /// Property: SG key for the output supercell LAr channel container.
  std::string m_sCellContainer;

  /// Geometry manager.
  const CaloSuperCellDetDescrManager* m_sem_mgr;

  /// Entry point for calorimeter ID helpers.
  const CaloIdManager* m_calo_id_manager;

  /// Calo Noise tool for unfolding the noise
  ToolHandle<ICaloNoiseTool> m_noisetool;

  /// CaloLumiBCIDTool to unfold correction
  ToolHandle<ICaloLumiBCIDTool> m_caloLumiBCIDTool;

  /// accumulate noise information
  std::vector<float> m_noise_per_cell;

  // in first event we will have to work a bit further (not good for MT)
  bool m_first;

  // compensate for Noise (superCell from cells with high gain
  bool m_compNoise;

  // compensate for BCID effect
  bool m_addBCID;

};


#endif // not LARL1SIM_LARSCSIMPLEMAKER_H
