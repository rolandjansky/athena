/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Muon Calib Identifiers
 * -----------------------------------------
 *
 * Author       : Martin Woudstra
 * Creation Date: 03 May 2005
 * Last Update  : 09 May 2005
 ***************************************************************************/

/** A set of simple MDT hashes, which is just a wrapped unsigned int for
    function overloading. */

#ifndef MUONCALIBIDENTIFIER_MDTFIXEDHASHES_H
# define MUONCALIBIDENTIFIER_MDTFIXEDHASHES_H
// std
#include <limits.h>
// other packages

// this package
#include "MuonCalibIdentifier/HashMaker.h"

CREATE_HASH_CLASS(MdtChamberHash)

CREATE_HASH_CLASS(MdtMultilayerHash)

CREATE_HASH_CLASS(MdtTubeHash)

#endif // MUONCALIBIDENTIFIER_MDTFIXEDHASHES_H
