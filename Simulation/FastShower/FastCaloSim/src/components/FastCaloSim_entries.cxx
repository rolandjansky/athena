//====================================================================
//  FastCaloSim_entries.cxx
//--------------------------------------------------------------------
//
//  Package    : Simulation/FastShower/FastCaloSim
//
//  Description: Implementation of <Package>_load routine.
//               This routine is needed for forcing the linker
//               to load all the components of the library. 
//
//====================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "FastCaloSim/FastShowerCellBuilderTool.h"
#include "FastCaloSim/EmptyCellBuilderTool.h"
#include "FastCaloSim/AddNoiseCellBuilderTool.h"
#include "FastCaloSim/FSStoregateClean.h"
#include "FastCaloSim/CaloCellContainerFCSFinalizerTool.h"
//#include "FastCaloSim/FSRedoCBNT.h"
//#include "FastCaloSim/CBNTAA_DetailedCellInfo.h"


DECLARE_NAMESPACE_ALGORITHM_FACTORY( FastCaloSim, FSStoregateClean )
//DECLARE_NAMESPACE_ALGORITHM_FACTORY( FastCaloSim, FSRedoCBNT )
DECLARE_TOOL_FACTORY( FastShowerCellBuilderTool )
DECLARE_TOOL_FACTORY( EmptyCellBuilderTool )
DECLARE_TOOL_FACTORY( AddNoiseCellBuilderTool )
DECLARE_TOOL_FACTORY( CaloCellContainerFCSFinalizerTool )
//DECLARE_ALGORITHM_FACTORY( CBNTAA_DetailedCellInfo )

DECLARE_FACTORY_ENTRIES(FastCaloSim) {
    DECLARE_NAMESPACE_ALGORITHM( FastCaloSim, FSStoregateClean );
    //DECLARE_NAMESPACE_ALGORITHM( FastCaloSim, FSRedoCBNT );
    DECLARE_TOOL( FastShowerCellBuilderTool );
    DECLARE_TOOL( EmptyCellBuilderTool );
    DECLARE_TOOL( AddNoiseCellBuilderTool );
    DECLARE_TOOL( CaloCellContainerFCSFinalizerTool )
}

