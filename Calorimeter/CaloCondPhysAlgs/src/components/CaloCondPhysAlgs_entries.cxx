#include "CaloCondPhysAlgs/CaloCellNoiseAlg.h"
#include "CaloCondPhysAlgs/CaloNoise2Ntuple.h"
#include "CaloCondPhysAlgs/CaloRescaleNoise.h"
#include "CaloCondPhysAlgs/CaloFillCellPositionShift.h"
#include "CaloCondPhysAlgs/CaloCellPosition2Ntuple.h"
#include "CaloCondPhysAlgs/CaloAddCellPedShift.h"
#include "CaloCondPhysAlgs/FCAL_HV_Energy_Rescale.h"
#include "CaloCondPhysAlgs/CaloCellCalcEnergyCorr.h"
#include "CaloCondPhysAlgs/CaloCellEnergyCorr2Ntuple.h"
#include "CaloCondPhysAlgs/LArMinBiasAlg.h"

DECLARE_ALGORITHM_FACTORY( CaloCellNoiseAlg )
DECLARE_ALGORITHM_FACTORY( CaloNoise2Ntuple )
DECLARE_ALGORITHM_FACTORY( CaloFillCellPositionShift )
DECLARE_ALGORITHM_FACTORY( CaloCellPosition2Ntuple )
DECLARE_ALGORITHM_FACTORY( CaloRescaleNoise )
DECLARE_ALGORITHM_FACTORY( CaloAddCellPedShift )
DECLARE_ALGORITHM_FACTORY( FCAL_HV_Energy_Rescale )
DECLARE_ALGORITHM_FACTORY( CaloCellCalcEnergyCorr )
DECLARE_ALGORITHM_FACTORY( CaloCellEnergyCorr2Ntuple )
DECLARE_ALGORITHM_FACTORY( LArMinBiasAlg)
  

