/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class TTToSCTool
 * @author Walter Hopkins whopkins@uoregon.edu
 * @brief Tile TT to SC conversion
 *
 * 
 *   */


#ifndef CALOREC_TTTOSCTOOL_H
#define CALOREC_TTTOSCTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloRec/ITTToSCTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "StoreGate/StoreGateSvc.h"

#include "CaloIdentifier/CaloGain.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TileConditions/TileCablingSvc.h"
#include <string>

class HWIdentifier;
class Identifier;

class CaloSuperCellDetDescrManager;
class CaloDetDescrManager;
class CaloCellContainer;

class TTToSCTool : public AthAlgTool, virtual public ITTToSCTool
{
 public:
  
	TTToSCTool (const std::string& type,
	            const std::string& name,
	            const IInterface* parent);
	virtual ~TTToSCTool();
	static const InterfaceID& interfaceID() {return ITTToSCTool::interfaceID();}

	StatusCode initialize();
	StatusCode buildTileSCs(CaloCellContainer* caloCell);
 private:
	const TileCablingService* m_tileCablingService;
	const CaloSuperCellDetDescrManager* m_sem_mgr;
	const CaloDetDescrManager* m_em_mgr;
	ToolHandle<LVL1::IL1TriggerTowerTool> m_ttTool;
	ToolHandle<ICaloSuperCellIDTool>     m_scidtool;
	const CaloLVL1_ID *m_lvl1Helper;
	std::map<Identifier,Identifier> m_ttToSC;
};

#endif
