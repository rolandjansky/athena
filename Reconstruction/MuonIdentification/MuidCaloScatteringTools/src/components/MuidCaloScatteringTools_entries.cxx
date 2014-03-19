#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuidCaloScatteringTools/MuidCaloMaterialParam.h"
#include "MuidCaloScatteringTools/MuidCaloTrackStateOnSurface.h"
#include "MuidCaloScatteringTools/MuidMaterialEffectsOnTrackProvider.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Rec, MuidCaloMaterialParam )
DECLARE_NAMESPACE_TOOL_FACTORY( Rec, MuidCaloTrackStateOnSurface )
DECLARE_NAMESPACE_TOOL_FACTORY( Rec, MuidMaterialEffectsOnTrackProvider)

DECLARE_FACTORY_ENTRIES( MuidCaloScatteringTools )
{
  DECLARE_NAMESPACE_TOOL( Rec, MuidCaloMaterialParam )
  DECLARE_NAMESPACE_TOOL( Rec, MuidCaloTrackStateOnSurface )     
  DECLARE_NAMESPACE_TOOL( Rec, MuidMaterialEffectsOnTrackProvider)
}


