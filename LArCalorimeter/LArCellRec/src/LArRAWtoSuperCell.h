/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


/**
 * Name :       LArRAWtoSuperCell.h
 * PACKAGE :    LArCalorimeter/LArROD
 *
 * AUTHOR :     Denis Oliveira Damazio
 *
 * PURPOSE :    prepares SuperCellContainer in CaloCellContainer type from RAW SC type
 *
 * **/

#ifndef LARCELL_LARRAWTOSUPERCELL
#define LARCELL_LARRAWTOSUPERCELL

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "CaloEvent/CaloCellContainer.h"
#include "LArRawEvent/LArRawSCContainer.h"


class LArOnline_SuperCellID;
class LArOnOffIdMapping;
class CaloSuperCellDetDescrManager;

class LArRAWtoSuperCell : public AthReentrantAlgorithm
{

public :
        LArRAWtoSuperCell( const std::string& name, ISvcLocator* pSvcLocator);
        virtual ~LArRAWtoSuperCell() { }
        StatusCode initialize();
        StatusCode finalize() { return StatusCode::SUCCESS;}
        StatusCode execute(const EventContext& context) const;

private:

  /// Property: SG key for the input calorimeter cell container.
  SG::ReadHandleKey<LArRawSCContainer> m_sCellContainerInKey
        { this, "SCellContainerIn", "SC_ET",
                "SG key for the input supercell LAr channel container"};

  /// Property: SG key for the output supercell LAr channel container.
  SG::WriteHandleKey<CaloCellContainer> m_sCellContainerOutKey
        { this, "SCellContainerOut", "SCell_ET",
                "SG key for the output supercell LAr channel container"};

  /// Geometry manager.
  SG::ReadCondHandleKey<CaloSuperCellDetDescrManager> m_caloMgrKey
        {this, "CaloSuperCellDetDescrManager", "CaloSuperCellDetDescrManager", "SG Key for CaloDetDescrManager in the Condition Store" };

  //Identifier helper
  const LArOnline_SuperCellID* m_laronline_id = nullptr;

};

#endif
