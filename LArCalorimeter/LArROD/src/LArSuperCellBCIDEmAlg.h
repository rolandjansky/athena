/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


/**
 * Name :       LArSuperCellBCIDEmAlg.h
 * PACKAGE :    LArCalorimeter/LArROD
 *
 * AUTHOR :     Denis Oliveira Damazio
 *
 * PURPOSE :    prepares SuperCellContainer to have the offset BCID-dependent correction
 *
 * **/

#ifndef LARROD_LARSUPERCELLBCIDEMALG
#define LARROD_LARSUPERCELLBCIDEMALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloBCIDAverage.h"
class ICaloSuperCellIDTool;

class LArSuperCellBCIDEmAlg : public AthReentrantAlgorithm
{

public :
        LArSuperCellBCIDEmAlg( const std::string& name, ISvcLocator* pSvcLocator);
        virtual ~LArSuperCellBCIDEmAlg() { }
        StatusCode initialize();
        StatusCode finalize() { return StatusCode::SUCCESS;}
        StatusCode execute(const EventContext& context) const;

private:

  /// Property: Offline / supercell mapping tool.
  ToolHandle<ICaloSuperCellIDTool>     m_scidtool
	{ this, "SCIDTool", "CaloSuperCellIDTool" ,
                "Offline / supercell mapping tool."};

  /// Property: SG key for the input calorimeter cell container.
  SG::ReadHandleKey<CaloCellContainer> m_sCellContainerInKey
        { this, "SCellContainerIn", "SCellContainer",
                "SG key for the input supercell LAr channel container"};

  /// Property: SG key for the output supercell LAr channel container.
  SG::WriteHandleKey<CaloCellContainer> m_sCellContainerOutKey
        { this, "SCellContainerOut", "SCellEm",
                "SG key for the output supercell LAr channel container"};

  /// Property SG Key for the CaloLumiBCID
  SG::ReadHandleKey<CaloBCIDAverage> m_bcidAvgKey
    {this, "BCIDAvgKey", "CaloBCIDAverage", "" };

};

#endif
