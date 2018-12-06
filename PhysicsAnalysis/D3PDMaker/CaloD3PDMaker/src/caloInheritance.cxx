/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloD3PDMaker/src/caloInheritance.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Declare inheritance relationships for calorimeter classes.
 *
 * Eventually, these should be moved to the EDM classes.
 */

#include "TileEvent/TileContainer.h"
#include "CaloEvent/CaloCell.h"
#include "AthenaKernel/BaseInfo.h"

SG_ADD_BASE (TileContainer<TileCell>, DataVector<TileCell>);
SG_ADD_BASE (CaloCell, INavigable4Momentum);
