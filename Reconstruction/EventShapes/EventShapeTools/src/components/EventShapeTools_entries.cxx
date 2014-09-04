#include "EventShapeTools/Thrust.h"
#include "EventShapeTools/JetMasses.h"
#include "EventShapeTools/JetBroadening.h"
#include "EventShapeTools/FoxWolfram.h"
#include "EventShapeTools/ForwardTerm.h"
#include "EventShapeTools/YFlipValues.h"
#include "EventShapeTools/TransverseSphericity.h"
#include "EventShapeTools/CParameter.h"
#include "EventShapeTools/EEC.h"
#include "EventShapeTools/EventEtDensityClusterTool.h"

#include "EventShapeTools/EventDensityTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( EventEtDensityClusterTool )
DECLARE_TOOL_FACTORY( Thrust )
DECLARE_TOOL_FACTORY( JetMasses )
DECLARE_TOOL_FACTORY( JetBroadening )
DECLARE_TOOL_FACTORY( FoxWolfram )
DECLARE_TOOL_FACTORY( ForwardTerm )
DECLARE_TOOL_FACTORY( YFlipValues )
DECLARE_TOOL_FACTORY( TransverseSphericity )
DECLARE_TOOL_FACTORY( CParameter )
DECLARE_TOOL_FACTORY( EEC )
DECLARE_TOOL_FACTORY( EventDensityTool )


DECLARE_FACTORY_ENTRIES( EventShapeTools ) {
  DECLARE_TOOL( Thrust );
  DECLARE_TOOL( JetMasses );
  DECLARE_TOOL( JetBroadening );
  DECLARE_TOOL( FoxWolfram );
  DECLARE_TOOL( ForwardTerm );
  DECLARE_TOOL( YFlipValues );
  DECLARE_TOOL( TransverseSphericity );
  DECLARE_TOOL( CParameter );
  DECLARE_TOOL( EEC );
  DECLARE_TOOL( EventEtDensityClusterTool );
  DECLARE_TOOL( EventDensityTool );
  
}
