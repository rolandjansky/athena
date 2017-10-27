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

