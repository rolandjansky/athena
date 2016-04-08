#include "GaudiKernel/DeclareFactoryEntries.h"

#include "DerivationFrameworkSUSY/Truth3CollectionMaker.h"
#include "DerivationFrameworkSUSY/SUSYSignalTagger.h"
#include "DerivationFrameworkSUSY/SUSYGenFilterTool.h"
#include "DerivationFrameworkSUSY/TrackParticleCaloCellDecorator.h"
#include "DerivationFrameworkSUSY/trackIsolationDecorator.h"

using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( Truth3CollectionMaker )
DECLARE_TOOL_FACTORY( SUSYSignalTagger )
DECLARE_TOOL_FACTORY( SUSYGenFilterTool )
DECLARE_TOOL_FACTORY( TrackParticleCaloCellDecorator )
DECLARE_TOOL_FACTORY( trackIsolationDecorator )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkSUSY ) {
   DECLARE_TOOL( Truth3CollectionMaker )
   DECLARE_TOOL( SUSYSignalTagger )
   DECLARE_TOOL( SUSYGenFilterTool )
   DECLARE_TOOL( TrackParticleCaloCellDecorator )
   DECLARE_TOOL( trackIsolationDecorator )
}
