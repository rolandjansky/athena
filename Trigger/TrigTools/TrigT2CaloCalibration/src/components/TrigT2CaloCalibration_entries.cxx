#include "TrigT2CaloCalibration/T2JESCalibTool.h"
#include "TrigT2CaloCalibration/T2GSCalibTool.h"
#include "TrigT2CaloCalibration/T2SampCalibTool.h"
#include "TrigT2CaloCalibration/EgammaHitsCalibration.h"
#include "TrigT2CaloCalibration/EgammaLWCalibration.h"
#include "TrigT2CaloCalibration/EgammaGapCalibration.h"
#include "TrigT2CaloCalibration/EgammaSshapeCalibration.h"
#include "TrigT2CaloCalibration/EgammaTransitionRegions.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( T2JESCalibTool )
DECLARE_TOOL_FACTORY( T2GSCalibTool )
DECLARE_TOOL_FACTORY( T2SampCalibTool )
DECLARE_TOOL_FACTORY( EgammaHitsCalibration )
DECLARE_TOOL_FACTORY( EgammaLWCalibration )
DECLARE_TOOL_FACTORY( EgammaGapCalibration )
DECLARE_TOOL_FACTORY( EgammaSshapeCalibration )
DECLARE_TOOL_FACTORY( EgammaTransitionRegions )

DECLARE_FACTORY_ENTRIES( TrigT2CaloCalibration ) {
    DECLARE_TOOL( T2JESCalibTool );
    DECLARE_TOOL( T2GSCalibTool );
    DECLARE_TOOL( T2SampCalibTool );
    DECLARE_TOOL( EgammaLWCalibration );
    DECLARE_TOOL( EgammaHitsCalibration );
    DECLARE_TOOL( EgammaGapCalibration );
    DECLARE_TOOL( EgammaSshapeCalibration );
    DECLARE_TOOL( EgammaTransitionRegions );
}
