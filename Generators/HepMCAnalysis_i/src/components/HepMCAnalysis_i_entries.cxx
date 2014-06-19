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
//#include "GaudiKernel/ITHistSvc.h"

#include "HepMCAnalysis_i/HepMCAnalysis.h"

DECLARE_ALGORITHM_FACTORY( HepMCAnalysis )

DECLARE_FACTORY_ENTRIES( HepMCAnalysis_i ) {
  DECLARE_ALGORITHM( HepMCAnalysis )
}

//DECLARE_ALGORITHM_FACTORY( THistSvc )
