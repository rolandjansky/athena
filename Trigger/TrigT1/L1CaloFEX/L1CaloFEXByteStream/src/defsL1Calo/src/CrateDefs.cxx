//==============================================================================
// CrateDefs.cxx
//==============================================================================

#include <stdlib.h>

#include "defsL1Calo/CrateDefs.h"

/*!
 *  \class CrateDefs
 *  This class provides static methods to return some hard coded
 *  constants for various L1Calo crates (legacy VME, phase1 ATCA).
 */

//==============================================================================
// getSystemName
//==============================================================================
/*!
 *  Return the ATCA logical slot corresponding to a physical slot.
 *  NB this is specific to the kind of crates used in ATLAS.
 *  \return ATCA logical slot number
 */
unsigned int
CrateDefs::atcaLogicalSlot(int physicalSlot)
{
  if (physicalSlot <= 7) {
    return 1 + (7 - physicalSlot) * 2;
  }
  else {
    return 1 + (physicalSlot - 8) * 2;
  }
}
