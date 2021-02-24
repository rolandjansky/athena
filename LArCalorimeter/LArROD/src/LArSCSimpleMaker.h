/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARROD_LARSCSIMPLEMAKER_H
#define LARROD_LARSCSIMPLEMAKER_H
// +======================================================================+
// +                                                                      +
// + Author ........: Denis Oliveira Damazio                              +
// + Institute ......: BNL                                                +
// + Creation date .: 17/06/2012                                          +
// + Move to MT in : 20/02/2021
// +                                                                      +
// +======================================================================+
//
// ....... include
//

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthAllocators/DataPool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloConditions/CaloNoiseSigmaDiff.h"
#include "CaloEvent/CaloBCIDAverage.h"


class CaloSuperCellDetDescrManager;
class ICaloSuperCellIDTool;
class CaloDetDescrElement;
class CaloCell;
class CaloIdManager;

/**
 * @brief Make a dummy supercell @c LArRawChannelContainer from a
 *        @c CaloCallContainer.
 */
class LArSCSimpleMaker : public AthReentrantAlgorithm
{
public:
  /// Standard Gaudi algorithm constructor.
  LArSCSimpleMaker(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArSCSimpleMaker() {};

  
  /// Standard Gaudi initialize method.
  StatusCode initialize(); 

  /// Algorithm execute method.
  StatusCode execute(const EventContext& context) const;

private:
  /// Property: Offline / supercell mapping tool.
  ToolHandle<ICaloSuperCellIDTool>     m_scidtool
	{ this, "SCIDTool", "CaloSuperCellIDTool" ,
		"Offline / supercell mapping tool."};

  /// Property: SG key for the input calorimeter cell container.
  SG::ReadHandleKey<CaloCellContainer> m_cellContainerKey
	{ this, "CellContainer", "AllCalo", 
		"SG key for the input calorimeter cell container"};

  /// Property: SG key for the output supercell LAr channel container.
  SG::WriteHandleKey<CaloCellContainer> m_sCellContainerKey
	{ this, "SCellContainer", "SCellContainer",
		"SG key for the output supercell LAr channel container"};

  /// Property SG Key for the Expected Noise Sigma diff in diff gains
  SG::ReadCondHandleKey<CaloNoiseSigmaDiff> m_noise_per_cell_Key
	{ this, "CaloNoiseSigmaDiff", "CaloNoiseSigmaDiff", "Sigma Diff for CaloNoise with High and Medium gain"}; 

  /// Property SG Key for the CaloLumiBCID
  SG::ReadHandleKey<CaloBCIDAverage> m_bcidAvgKey 
    {this, "BCIDAvgKey", "CaloBCIDAverage", "" };

  // compensate for Noise (superCell from cells with high gain
  Gaudi::Property<bool> m_compNoise{this,"CompNoise",true,"Compensate for Noise difference in LAr between high and medium gain"};
  // compensate for BCID effect
  Gaudi::Property<bool> m_addBCID{this,"addBCID",true,"Add BCID compensation back to cells prior to sum them up"};

  /// Geometry manager.
  const CaloSuperCellDetDescrManager* m_sem_mgr;

  /// Entry point for calorimeter ID helpers.
  const CaloIdManager* m_calo_id_manager;


};


#endif // not LARROD_LARSCSIMPLEMAKER_H
