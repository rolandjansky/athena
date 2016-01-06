#include "GaudiKernel/DeclareFactoryEntries.h"

#include "ISF_FatrasToolsG4/G4ParticleDecayHelper.h"
#include "ISF_FatrasToolsG4/G4ParticlePropertyTool.h"
#include "ISF_FatrasToolsG4/G4HadIntProcessor.h"
#include "ISF_FatrasToolsG4/PDGToG4Particle.h"

DECLARE_NAMESPACE_TOOL_FACTORY( iFatras, G4ParticleDecayHelper   )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras, G4ParticlePropertyTool  )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras, G4HadIntProcessor       )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras, PDGToG4Particle         )

DECLARE_FACTORY_ENTRIES( ISF_FatrasToolsG4 )
{
  DECLARE_NAMESPACE_TOOL( iFatras, G4ParticleDecayHelper   )
  DECLARE_NAMESPACE_TOOL( iFatras, G4ParticlePropertyTool  )
  DECLARE_NAMESPACE_TOOL( iFatras, G4HadIntProcessor       )
  DECLARE_NAMESPACE_TOOL( iFatras, PDGToG4Particle         )
}

