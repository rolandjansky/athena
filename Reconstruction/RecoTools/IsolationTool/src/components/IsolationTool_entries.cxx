// $Id: IsolationTool_entries.cxx 770003 2016-08-25 08:54:14Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "IsolationTool/TrackIsolationTool.h"
#include "IsolationTool/CaloIsolationTool.h"
#include "IsolationTool/IsolationHelper.h"
#include "../IsolationTool_AthTest.h"

DECLARE_NAMESPACE_TOOL_FACTORY( xAOD, TrackIsolationTool )
DECLARE_NAMESPACE_TOOL_FACTORY( xAOD, CaloIsolationTool )
DECLARE_NAMESPACE_TOOL_FACTORY( CP, IsolationHelper )
DECLARE_ALGORITHM_FACTORY( IsolationTool_AthTest )

DECLARE_FACTORY_ENTRIES( IsolationTool ) {

   DECLARE_NAMESPACE_TOOL( xAOD, TrackIsolationTool )
   DECLARE_NAMESPACE_TOOL( xAOD, CaloIsolationTool )
   DECLARE_NAMESPACE_TOOL( CP, IsolationHelper )
   DECLARE_ALGORITHM( IsolationTool_AthTest )
}
