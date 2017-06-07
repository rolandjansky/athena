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

#include "TrigT1NSW/NSWL1Simulation.h"

using NSWL1::NSWL1Simulation;

DECLARE_ALGORITHM_FACTORY( NSWL1Simulation )

DECLARE_FACTORY_ENTRIES( TrigT1NSW ) {
  DECLARE_ALGORITHM( NSWL1Simulation )
}
