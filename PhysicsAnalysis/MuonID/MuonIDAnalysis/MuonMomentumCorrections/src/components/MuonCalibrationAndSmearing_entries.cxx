// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "MuonMomentumCorrections/MuonCalibrationAndSmearingTool.h"

#include "../TestMCASTTool.h"
#include "../CalibratedMuonsProvider.h"

DECLARE_NAMESPACE_TOOL_FACTORY( CP, MuonCalibrationAndSmearingTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( CP, TestMCASTTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( CP, CalibratedMuonsProvider )

DECLARE_FACTORY_ENTRIES( MuonCalibrationAndSmearing ) {
  DECLARE_NAMESPACE_TOOL( CP, MuonCalibrationAndSmearingTool )

  DECLARE_NAMESPACE_ALGORITHM( CP, TestMCASTTool )
  DECLARE_NAMESPACE_ALGORITHM( CP, CalibratedMuonsProvider )
}
