/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArIdentifierDict.h
 *
 * @brief This file includes the class for dictionary definitions
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: LArIdentifierDict.h,v 1.5 2007-03-05 09:14:06 fledroit Exp $
 */
#ifndef LARIDENTIFIER_LARIDENTIFIERDICT_H
# define LARIDENTIFIER_LARIDENTIFIERDICT_H

#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "LArIdentifier/LArHVLineID.h"
#include "LArIdentifier/LArElectrodeID.h"
#include "StoreGate/StoreGateSvc.h" 

namespace LArIdentifierDict
{
 
   std::vector<Identifier> vIdentifier;
   std::vector<Identifier>::const_iterator Identifier_iter;
   std::vector<HWIdentifier> vHWIdentifier;

}

#endif // LARIDENTIFIER_LARIDENTIFIERDICT_H
