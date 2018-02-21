#ifndef TRIGT1CAL_RODHeaderCOLLECTION_H
#define TRIGT1CAL_RODHeaderCOLLECTION_H

#include "AthContainers/DataVector.h"
#include "TrigT1CaloEvent/RODHeader.h"

/** Container class for RODHeaders.
    RODHeader contains the ROD header and status information
    from the raw data ROD fragments. */

using namespace LVL1;
typedef DataVector<RODHeader> RODHeaderCollection;

#endif
