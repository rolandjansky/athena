
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
 * Name :       SuperCellBCIDAlg.h
 * PACKAGE :    Trigger/TrigL1Upgrade/L1CaloUpgrade/SuperCellBCIDAlg
 *
 * AUTHOR :     Denis Oliveira Damazio
 *
 * PURPOSE :    prepares SuperCellContainer to have the offset BCID-dependent correction
 *
 * **/

#ifndef TRIGL1CALOUPGRADE_SUPERCELLBCIDALG
#define TRIGL1CALOUPGRADE_SUPERCELLBCIDALG

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloInterface/ICaloLumiBCIDTool.h"
class CaloCellContainer;
class CaloCell;

class SuperCellBCIDAlg : public AthAlgorithm
{

public :
	SuperCellBCIDAlg( const std::string& name, ISvcLocator* pSvcLocator);
	virtual ~SuperCellBCIDAlg() { }
	StatusCode initialize();
	StatusCode finalize() { return StatusCode::SUCCESS;}
	StatusCode execute();

private:
	ToolHandle <ICaloLumiBCIDTool> m_tool;
	std::string m_sCellContainerIn; 
	std::string m_sCellContainerOut;
};
	
#endif
