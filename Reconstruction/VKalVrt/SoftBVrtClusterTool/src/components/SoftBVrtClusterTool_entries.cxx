#include "SoftBVrtClusterTool/SoftBVrtClusterTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( SoftBVrt, SoftBVrtClusterTool )
DECLARE_FACTORY_ENTRIES( SoftBVrtClusterTool) {
  DECLARE_NAMESPACE_ALGORITHM( SoftBVrt, SoftBVrtClusterTool )
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
