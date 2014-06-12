#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TileMuId/TileLookForMuAlg.h"
#include "TileMuId/TileMuToNtuple.h"

DECLARE_ALGORITHM_FACTORY( TileLookForMuAlg )
DECLARE_ALGORITHM_FACTORY( TileMuToNtuple )

DECLARE_FACTORY_ENTRIES(TileMuId) {
  DECLARE_ALGORITHM( TileLookForMuAlg )
  DECLARE_ALGORITHM( TileMuToNtuple )
}
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

 
