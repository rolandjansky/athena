// $Id: ElectronIsolationSelection_entries.cxx 630001 2014-11-20 23:20:08Z christos $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "ElectronIsolationSelection/ElectronIsolationSelectionTool.h"
#include "../TestElectronIsoAthena.h"


DECLARE_NAMESPACE_TOOL_FACTORY( CP, ElectronIsolationSelectionTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( CP , TestElectronIsoAthena )

DECLARE_FACTORY_ENTRIES( ElectronIsolationSelection ) {

   DECLARE_NAMESPACE_TOOL( CP, ElectronIsolationSelectionTool )
   DECLARE_NAMESPACE_ALGORITHM( CP , TestElectronIsoAthena )
}
