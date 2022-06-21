#include "FastCaloSim/FastShowerCellBuilderTool.h"
#include "../EmptyCellBuilderTool.h"
#include "../AddNoiseCellBuilderTool.h"
#include "FastCaloSim/FSStoregateClean.h"
#include "FastCaloSim/CaloCellContainerFCSFinalizerTool.h"
#include "../CellInfoContainerCondAlg.h"

DECLARE_COMPONENT( FastCaloSim::FSStoregateClean )
DECLARE_COMPONENT( FastShowerCellBuilderTool )
DECLARE_COMPONENT( EmptyCellBuilderTool )
DECLARE_COMPONENT( AddNoiseCellBuilderTool )
DECLARE_COMPONENT( CaloCellContainerFCSFinalizerTool )
DECLARE_COMPONENT( CellInfoContainerCondAlg )
