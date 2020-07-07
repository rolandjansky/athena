#include "TrkExAlgs/ExtrapolatorTest.h"
#include "TrkExAlgs/ExtrapolatorComparisonTest.h"
#include "TrkExAlgs/CombinedExtrapolatorTest.h"
#include "TrkExAlgs/CETmaterial.h"
#include "TrkExAlgs/ExtrapolationValidation.h"
#include "TrkExAlgs/EnergyLossExtrapolationValidation.h"
#include "TrkExAlgs/RiddersAlgorithm.h"
#include "TrkExAlgs/PropResultRootWriterSvc.h"


using namespace Trk;

DECLARE_COMPONENT( RiddersAlgorithm )
DECLARE_COMPONENT( ExtrapolatorTest )
DECLARE_COMPONENT( ExtrapolatorComparisonTest )
DECLARE_COMPONENT( ExtrapolationValidation )
DECLARE_COMPONENT( EnergyLossExtrapolationValidation )
DECLARE_COMPONENT( CombinedExtrapolatorTest )
DECLARE_COMPONENT( CETmaterial )
DECLARE_COMPONENT( PropResultRootWriterSvc )
