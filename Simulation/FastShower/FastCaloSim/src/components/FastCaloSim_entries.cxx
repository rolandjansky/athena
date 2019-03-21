#include "FastCaloSim/FastShowerCellBuilderTool.h"
#include "FastCaloSim/EmptyCellBuilderTool.h"
#include "FastCaloSim/AddNoiseCellBuilderTool.h"
#include "FastCaloSim/FSStoregateClean.h"
#include "FastCaloSim/CaloCellContainerFCSFinalizerTool.h"
//#include "FastCaloSim/FSRedoCBNT.h"
//#include "FastCaloSim/CBNTAA_DetailedCellInfo.h"


DECLARE_COMPONENT( FastCaloSim::FSStoregateClean )
//DECLARE_COMPONENT( FastCaloSim::FSRedoCBNT )
DECLARE_COMPONENT( FastShowerCellBuilderTool )
DECLARE_COMPONENT( EmptyCellBuilderTool )
DECLARE_COMPONENT( AddNoiseCellBuilderTool )
DECLARE_COMPONENT( CaloCellContainerFCSFinalizerTool )
//DECLARE_COMPONENT( CBNTAA_DetailedCellInfo )

