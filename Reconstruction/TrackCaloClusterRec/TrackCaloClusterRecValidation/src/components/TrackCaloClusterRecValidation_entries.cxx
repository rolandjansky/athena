#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrackCaloClusterRecValidation/TrackCaloClusterRecValidationTool.h"
#include "TrackCaloClusterRecValidation/CalibrationNtupleMakerTool.h"

DECLARE_TOOL_FACTORY(TrackCaloClusterRecValidationTool)
DECLARE_ALGORITHM_FACTORY(CalibrationNtupleMakerTool)

DECLARE_FACTORY_ENTRIES(TrackCaloClusterRecValidation){
    DECLARE_TOOL(TrackCaloClusterRecValidationTool)
    DECLARE_ALGORITHM(CalibrationNtupleMakerTool)
}