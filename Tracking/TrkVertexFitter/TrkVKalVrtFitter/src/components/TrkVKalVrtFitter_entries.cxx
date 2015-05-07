#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace Trk;

DECLARE_TOOL_FACTORY( TrkVKalVrtFitter )
DECLARE_FACTORY_ENTRIES(TrkVKalVrtFitter) {
    DECLARE_TOOL( TrkVKalVrtFitter )
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
