#include "CaloCellCorrection/CaloCellPedestalCorr.h"
#include "CaloCellCorrection/CaloCellMBAverageCorr.h"
#include "CaloCellCorrection/CaloCellNeighborsAverageCorr.h"
#include "CaloCellCorrection/CaloCellRandomizer.h"
#include "CaloCellCorrection/CaloCellRescaler.h"
#include "CaloCellCorrection/CaloCellEnergyRescaler.h"
#include "CaloCellCorrection/CaloCellTimeCorrTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( CaloCellPedestalCorr )
DECLARE_TOOL_FACTORY( CaloCellMBAverageCorr )
DECLARE_TOOL_FACTORY( CaloCellNeighborsAverageCorr )
DECLARE_TOOL_FACTORY( CaloCellRandomizer )
DECLARE_TOOL_FACTORY( CaloCellRescaler )
DECLARE_TOOL_FACTORY( CaloCellEnergyRescaler )
DECLARE_TOOL_FACTORY ( CaloCellTimeCorrTool )


DECLARE_FACTORY_ENTRIES(CaloCellCorrection) {
    DECLARE_TOOL( CaloCellPedestalCorr ) 
    DECLARE_TOOL( CaloCellMBAverageCorr )
    DECLARE_TOOL( CaloCellNeighborsAverageCorr )
    DECLARE_TOOL( CalloCellRandomizer )
    DECLARE_TOOL( CaloCellRescaler )
    DECLARE_TOOL( CaloCellEnergyRescaler )
    DECLARE_TOOL ( CaloCellTimeCorrTool )
}
