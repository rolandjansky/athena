/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file caloInheritance.cxx
 * @author Marco van Woerden <mvanwoer@cern.ch>
 * @date September 2012
 * @brief Declare inheritance relationships for calorimeter classes.
 */

#include "TileEvent/TileContainer.h"
#include "CaloEvent/CaloCell.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "SGTools/BaseInfo.h"

SG_ADD_BASE (TileContainer<TileCell>, DataVector<TileCell>);
SG_ADD_BASE (CaloCell, INavigable4Momentum);
SG_ADD_BASE (LArDigitContainer, DataVector<LArDigit> );
