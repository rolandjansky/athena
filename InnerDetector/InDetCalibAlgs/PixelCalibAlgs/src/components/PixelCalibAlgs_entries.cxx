#include "PixelCalibAlgs/PixelBarrelSurvey.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "PixelCalibAlgs/NoiseMapBuilder.h"
#include "PixelCalibAlgs/PixMapDBWriter.h"
#include "PixelCalibAlgs/PixMapOverlayWriter.h"
#include "PixelCalibAlgs/PixelChargeToTConversion.h"
#include "PixelCalibAlgs/CompareModulesTDAQandBytestream.h"

DECLARE_ALGORITHM_FACTORY( PixelBarrelSurvey )
DECLARE_ALGORITHM_FACTORY( NoiseMapBuilder )
DECLARE_ALGORITHM_FACTORY( PixMapDBWriter )
DECLARE_ALGORITHM_FACTORY( PixMapOverlayWriter )
DECLARE_ALGORITHM_FACTORY( PixelChargeToTConversion )
DECLARE_ALGORITHM_FACTORY( CompareModulesTDAQandBytestream )

DECLARE_FACTORY_ENTRIES( PixelCalibAlgs ) {
  DECLARE_ALGORITHM( PixelBarrelSurvey )
  DECLARE_ALGORITHM( NoiseMapBuilder )
  DECLARE_ALGORITHM( PixMapDBWriter )
  DECLARE_ALGORITHM( PixMapOverlayWriter )
  DECLARE_ALGORITHM( PixelChargeToTConversion )
  DECLARE_ALGORITHM( CompareModulesTDAQandBytestream )
}
