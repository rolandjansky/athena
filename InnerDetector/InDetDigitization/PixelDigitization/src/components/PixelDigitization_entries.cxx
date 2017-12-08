#include "src/PixelDigitization.h"
#include "src/PixelDigitizationTool.h"
#include "src/PixelNoisyCellGenerator.h"
#include "src/PixelChargeSmearer.h"
#include "src/PixelDiodeCrossTalkGenerator.h"
#include "src/PixelRandomDisabledCellGenerator.h"
#include "src/SensorSimPlanarTool.h"
#include "src/SensorSim3DTool.h"
#include "src/ChargeCollProbSvc.h"
#include "src/EnergyDepositionTool.h"
#include "src/RD53SimTool.h"
#include "src/FEI4SimTool.h"
#include "src/FEI3SimTool.h"

DECLARE_COMPONENT( PixelDigitization )
DECLARE_COMPONENT( EnergyDepositionTool )
DECLARE_COMPONENT( PixelDigitizationTool )
DECLARE_COMPONENT( PixelNoisyCellGenerator )
DECLARE_COMPONENT( PixelChargeSmearer )
DECLARE_COMPONENT( PixelDiodeCrossTalkGenerator )
DECLARE_COMPONENT( PixelRandomDisabledCellGenerator )
DECLARE_COMPONENT( SensorSimPlanarTool )
DECLARE_COMPONENT( SensorSim3DTool )
DECLARE_COMPONENT( RD53SimTool )
DECLARE_COMPONENT( FEI4SimTool )
DECLARE_COMPONENT( FEI3SimTool )
DECLARE_COMPONENT( ChargeCollProbSvc )

