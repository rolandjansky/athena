/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEIDENTIFIER_TILETRIGTYPE_H
#define TILEIDENTIFIER_TILETRIGTYPE_H

#include <inttypes.h>

/** 
 * @class TileTrigType
 * @brief Definition of different trigger types (calibration and physics)
 * @author Luca Fiorini
 *
 *
 * TileTrigType defined 4 different trigger types
 * which we can have in the bytestream file
 *
 */

namespace TileTrigType  
{
  typedef uint32_t TYPE;
  const TYPE Invalid = 0;
  const TYPE Physics = 1;
  const TYPE Laser = 2;
  const TYPE Pedestal = 4;
  const TYPE CIS = 8;
}

#endif // TILEIDENTIFIER_TILETRIGTYPE_H
