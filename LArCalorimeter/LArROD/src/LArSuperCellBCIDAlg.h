/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


/**
 * Name :       LArSuperCellBCIDAlg.h
 * PACKAGE :    LArCalorimeter/LArROD
 *
 * AUTHOR :     Denis Oliveira Damazio
 *
 * PURPOSE :    prepares SuperCellContainer to have the offset BCID-dependent correction
 *
 * **/

#ifndef LARROD_LARSUPERCELLBCIDALG
#define LARROD_LARSUPERCELLBCIDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloBCIDAverage.h"

class LArOnline_SuperCellID;
class LArOnOffIdMapping;

class LArSuperCellBCIDAlg : public AthReentrantAlgorithm
{

public :
        LArSuperCellBCIDAlg( const std::string& name, ISvcLocator* pSvcLocator);
        virtual ~LArSuperCellBCIDAlg() { }
        StatusCode initialize();
        StatusCode finalize() { return StatusCode::SUCCESS;}
        StatusCode execute(const EventContext& context) const;

private:

  /// Property: SG key for the input calorimeter cell container.
  SG::ReadHandleKey<CaloCellContainer> m_sCellContainerInKey
        { this, "SCellContainerIn", "SCellContainer",
                "SG key for the input supercell LAr channel container"};

  /// Property: SG key for the output supercell LAr channel container.
  SG::WriteHandleKey<CaloCellContainer> m_sCellContainerOutKey
        { this, "SCellContainerOut", "SCell",
                "SG key for the output supercell LAr channel container"};

  /// Property SG Key for the CaloLumiBCID
  SG::ReadHandleKey<CaloBCIDAverage> m_bcidAvgKey
    {this, "BCIDAvgKey", "CaloBCIDAverageSC", "" };

  /// Property SG Key for the OnOffMapping
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey
    {this,"CablingKey","LArOnOffIdMapSC","SG Key of LArOnOffIdMapping object"};

  //Identifier helper
  const LArOnline_SuperCellID* m_laronline_id;

};

#endif
