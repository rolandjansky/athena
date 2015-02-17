#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../ParticleHelper.h"
#include "../MemoryMonitoringTool.h"
#include "../GenericBarcodeFilter.h"
#include "../EntryLayerFilter.h"
#include "../KinematicParticleFilter.h"
#include "../CosmicEventFilterTool.h"
#include "../GenericParticleOrderingTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( ISF , ParticleHelper                    )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , MemoryMonitoringTool              )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenericBarcodeFilter              )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , EntryLayerFilter                  )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , KinematicParticleFilter           )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , CosmicEventFilterTool             )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenericParticleOrderingTool       )

DECLARE_FACTORY_ENTRIES( ISF_Tools ) {
  DECLARE_NAMESPACE_TOOL( ISF ,  ParticleHelper                    )
  DECLARE_NAMESPACE_TOOL( ISF ,  MemoryMonitoringTool              )
  DECLARE_NAMESPACE_TOOL( ISF ,  GenericBarcodeFilter              )
  DECLARE_NAMESPACE_TOOL( ISF ,  EntryLayerFilter                  )
  DECLARE_NAMESPACE_TOOL( ISF ,  KineamticParticleFilter           )
  DECLARE_NAMESPACE_TOOL( ISF ,  CosmicEventFilterTool             )
  DECLARE_NAMESPACE_TOOL( ISF ,  GenericParticleOrderingTool       )
}
