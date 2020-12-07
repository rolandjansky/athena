
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
 * Name :       SuperCellBCIDEmAlg.h
 * PACKAGE :    Trigger/TrigL1Upgrade/L1CaloUpgrade/SuperCellBCIDEmAlg
 *
 * AUTHOR :     Denis Oliveira Damazio
 *
 * PURPOSE :    prepares SuperCellContainer to have the offset BCID-dependent correction
 *
 * **/

#ifndef TRIGL1CALOUPGRADE_SUPERCELLBCIDEMALG
#define TRIGL1CALOUPGRADE_SUPERCELLBCIDEMALG

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloInterface/ICaloLumiBCIDTool.h"
class CaloCellContainer;
class CaloCell;
class ICaloSuperCellIDTool;

class SuperCellBCIDEmAlg : public AthAlgorithm
{

public :
	SuperCellBCIDEmAlg( const std::string& name, ISvcLocator* pSvcLocator);
	virtual ~SuperCellBCIDEmAlg() { }
	StatusCode initialize();
	StatusCode finalize() { return StatusCode::SUCCESS;}
	StatusCode execute();

private:
	ToolHandle <ICaloLumiBCIDTool> m_tool;
	ToolHandle <ICaloSuperCellIDTool> m_scidtool;
	std::string m_sCellContainerIn; 
	std::string m_sCellContainerOut;
};
	
#endif
