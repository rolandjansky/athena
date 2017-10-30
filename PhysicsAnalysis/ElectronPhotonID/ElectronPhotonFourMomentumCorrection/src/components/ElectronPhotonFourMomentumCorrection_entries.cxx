#include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"
#include "../testAthenaEgammaCalibTool.h"
#include "../CalibratedEgammaProvider.h"

DECLARE_NAMESPACE_TOOL_FACTORY( CP, EgammaCalibrationAndSmearingTool )
DECLARE_ALGORITHM_FACTORY( testAthenaEgammaCalibTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY(CP,CalibratedEgammaProvider)



#include "../dumpAllSystematics.h"
DECLARE_ALGORITHM_FACTORY( DumpAllSystematics )

