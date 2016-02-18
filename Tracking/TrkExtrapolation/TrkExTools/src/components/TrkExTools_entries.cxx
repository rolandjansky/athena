#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkExTools/Extrapolator.h"
#include "TrkExTools/Navigator.h"
#include "TrkExTools/MaterialEffectsUpdator.h"
#include "TrkExTools/DummyMaterialEffectsUpdator.h"
#include "TrkExTools/EnergyLossUpdator.h"
#include "TrkExTools/MultipleScatteringUpdator.h"
#include "TrkExTools/TimedExtrapolator.h"
#include "TrkExTools/NIMatEffUpdator.h"

using namespace Trk;

DECLARE_TOOL_FACTORY( Extrapolator )
DECLARE_TOOL_FACTORY( TimedExtrapolator )
DECLARE_TOOL_FACTORY( Navigator )
DECLARE_TOOL_FACTORY( MaterialEffectsUpdator )
DECLARE_TOOL_FACTORY( DummyMaterialEffectsUpdator )
DECLARE_TOOL_FACTORY( EnergyLossUpdator )
DECLARE_TOOL_FACTORY( MultipleScatteringUpdator )
DECLARE_TOOL_FACTORY( NIMatEffUpdator )

/** factory entries need to have the name of the package */

DECLARE_FACTORY_ENTRIES( TrkExTools )
{
    DECLARE_TOOL( Extrapolator )
    DECLARE_TOOL( TimedExtrapolator )
    DECLARE_TOOL( Navigator )  
    DECLARE_TOOL( MaterialEffectsUpdator )
    DECLARE_TOOL( DummyMaterialEffectsUpdator )
    DECLARE_TOOL( EnergyLossUpdator )
    DECLARE_TOOL( MultipleScatteringUpdator )
    DECLARE_TOOL( NIMatEffUpdator )
}


