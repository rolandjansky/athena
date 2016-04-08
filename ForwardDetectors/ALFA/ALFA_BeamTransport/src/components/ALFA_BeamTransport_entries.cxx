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

#include "ALFA_BeamTransport/ALFA_BeamTransport.h"

DECLARE_ALGORITHM_FACTORY( ALFA_BeamTransport )

DECLARE_FACTORY_ENTRIES( ALFA_BeamTransport ) {
     DECLARE_ALGORITHM( ALFA_BeamTransport )
}
//DECLARE_ALGORITHM_FACTORY( THistSvc )
