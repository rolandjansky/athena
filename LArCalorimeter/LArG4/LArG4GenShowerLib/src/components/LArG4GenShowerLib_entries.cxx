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

#include "LArG4GenShowerLib/LArG4GenShowerLib.h"
#include "LArG4GenShowerLib/TestActionShowerLibTool.h"

DECLARE_ALGORITHM_FACTORY( LArG4GenShowerLib )
DECLARE_TOOL_FACTORY( G4UA::TestActionShowerLibTool )

DECLARE_FACTORY_ENTRIES( LArG4GenShowerLib ) {
  DECLARE_TOOL( G4UA::TestActionShowerLibTool )
  DECLARE_ALGORITHM( LArG4GenShowerLib )
}
