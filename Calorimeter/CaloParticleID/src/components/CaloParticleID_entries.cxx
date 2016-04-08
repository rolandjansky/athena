#include "GaudiKernel/DeclareFactoryEntries.h"

#include "CaloParticleID/CaloParticleIDTool.h"

DECLARE_TOOL_FACTORY( CaloParticleIDTool )

DECLARE_FACTORY_ENTRIES( CaloParticleID ) {
  DECLARE_TOOL( CaloParticleIDTool )
}
