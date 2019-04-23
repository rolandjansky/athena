//Notes:
//
//1. The argument to the DECLARE_FACTORY_ENTRIES(XXX) is the name of the
//   component library (libXXX.so).
//2. Each Algorithm that is contained in the library should be declared
//   by the DECLARE_ALGORITHM() statement.
//3. Each Service that is contained in the library should be declared
//   by the DECLARE_SERVICE() statement.
//
// See Athena User Guide for more information

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigT1CTP/CTPSimulation.h"
#include "TrigT1CTP/CTPDataDumper.h"
#include "../CTPEmulation.h"

using LVL1CTP::CTPSimulation;
using LVL1CTP::CTPDataDumper;

DECLARE_ALGORITHM_FACTORY( CTPSimulation )
DECLARE_ALGORITHM_FACTORY( CTPDataDumper )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( LVL1CTP, CTPEmulation )


DECLARE_FACTORY_ENTRIES( TrigT1CTP ) {
  DECLARE_ALGORITHM( CTPSimulation )
  DECLARE_ALGORITHM( CTPDataDumper )
  DECLARE_NAMESPACE_ALGORITHM( LVL1CTP, CTPEmulation )
}
