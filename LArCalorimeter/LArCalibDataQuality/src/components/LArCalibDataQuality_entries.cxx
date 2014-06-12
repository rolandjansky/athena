#include "LArCalibDataQuality/LArRampValidationAlg.h"
#include "LArCalibDataQuality/LArPedestalValidationAlg.h"
#include "LArCalibDataQuality/LArCaliWaveValidationAlg.h"
#include "LArCalibDataQuality/LArAutoCorrValidationAlg.h"
#include "LArCalibDataQuality/LArBadChannelHunter.h"
#include "LArCalibDataQuality/LArBadEventCatcher.h"

#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_ALGORITHM_FACTORY( LArRampValidationAlg )
DECLARE_ALGORITHM_FACTORY( LArPedestalValidationAlg )
DECLARE_ALGORITHM_FACTORY( LArCaliWaveValidationAlg )
DECLARE_ALGORITHM_FACTORY( LArAutoCorrValidationAlg )
DECLARE_ALGORITHM_FACTORY( LArBadChannelHunter )
DECLARE_ALGORITHM_FACTORY( LArBadEventCatcher )

DECLARE_FACTORY_ENTRIES(LArCalibDataQuality) {
DECLARE_ALGORITHM( LArRampValidationAlg )
DECLARE_ALGORITHM( LArPedestalValidationAlg )
DECLARE_ALGORITHM( LArAutoCorrValidationAlg ) 
DECLARE_ALGORITHM( LArCaliWaveDataQuality ) 
DECLARE_ALGORITHM( LArBadChannelHunter ) 
DECLARE_ALGORITHM (LArBadEventCatcher)
}
