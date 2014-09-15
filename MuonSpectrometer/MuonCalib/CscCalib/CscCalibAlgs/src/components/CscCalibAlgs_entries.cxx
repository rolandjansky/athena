#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../CscCalcPed.h"
#include "../CscCalcSlope.h"

using namespace MuonCalib;

DECLARE_ALGORITHM_FACTORY( CscCalcPed )
DECLARE_ALGORITHM_FACTORY( CscCalcSlope )
    
DECLARE_FACTORY_ENTRIES( CscCalibAlgs ) 
{
    DECLARE_ALGORITHM( CscCalcPed )
    DECLARE_ALGORITHM( CscCalcSlope )
}
