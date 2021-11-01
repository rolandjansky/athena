/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALODETDESCRUTILS_CALOSUPERCELLUTILS_H
#define CALODETDESCRUTILS_CALOSUPERCELLUTILS_H

/**
 *  @file CaloSuperCellUtils.h
 *
 *  @brief This is a collection of helper functions for building Calo Super Cell 
 *         detector manager, and for updating it with alignment corrections.
 *         The functions previously existed in two packages: 
 *         CaloSuperCellMgrDetDescrCnv and CaloDetDescr.
 *         They have been moved over to the CaloDetDescrUtils so that now they
 *         can also be used by the CaloSuperCellAlignCondAlg.
 */

#include "GaudiKernel/StatusCode.h"
#include "CxxUtils/checker_macros.h"

class CaloSuperCellDetDescrManager;
class CaloDetDescrManager;
class ICaloSuperCellIDTool;

void createDescriptors(CaloSuperCellDetDescrManager* mgr);

void createElements(CaloSuperCellDetDescrManager* mgr);

StatusCode updateElements(CaloSuperCellDetDescrManager* mgr
			  , const CaloDetDescrManager* cellmgr
			  , const ICaloSuperCellIDTool* scidTool);

void updateDescriptors ATLAS_NOT_CONST_THREAD_SAFE (CaloSuperCellDetDescrManager* mgr
		       , const CaloDetDescrManager* cellmgr
		       , const ICaloSuperCellIDTool* scidTool);

#endif
