#include "TrkExTools/Extrapolator.h"
#include "TrkExTools/Navigator.h"
#include "TrkExTools/MaterialEffectsUpdator.h"
#include "TrkExTools/DummyMaterialEffectsUpdator.h"
#include "TrkExTools/EnergyLossUpdator.h"
#include "TrkExTools/MultipleScatteringUpdator.h"
#include "TrkExTools/TimedExtrapolator.h"
#include "TrkExTools/NIMatEffUpdator.h"

using namespace Trk;

DECLARE_COMPONENT( Extrapolator )
DECLARE_COMPONENT( TimedExtrapolator )
DECLARE_COMPONENT( Navigator )
DECLARE_COMPONENT( MaterialEffectsUpdator )
DECLARE_COMPONENT( DummyMaterialEffectsUpdator )
DECLARE_COMPONENT( EnergyLossUpdator )
DECLARE_COMPONENT( MultipleScatteringUpdator )
DECLARE_COMPONENT( NIMatEffUpdator )

